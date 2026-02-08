//#include "AllFiles.h"
//
//bool f11_pressed(GLFWwindow* window)
//{
//	return glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
//}
//
//int main()
//{
//    KGR::_GLFW myWindow;
//    KGR::_GLFW::Init();
//    KGR::_GLFW::AddHint(GLFW_CLIENT_API, GLFW_NO_API);
//    KGR::_GLFW::AddHint(GLFW_RESIZABLE, GLFW_FALSE);
//    myWindow.CreateMyWindow({ 1280, 720 }, "GC goes Vulkan", nullptr, nullptr);
//
//    KGR::_Vulkan vulkan;
//    vulkan.Init(&myWindow);
//
//    uint32_t currentFrame = 0;
//
//    do
//    {
//        KGR::_GLFW::PollEvent();
//
//        auto& cb = vulkan.Begin();
//        auto& currentImage = vulkan.GetCurrentImage();
//
//        auto clearRange = vk::ImageSubresourceRange
//        {
//            .aspectMask = vk::ImageAspectFlagBits::eColor,
//            .levelCount = vk::RemainingMipLevels,
//            .layerCount = vk::RemainingArrayLayers
//        };
//
//        vulkan.TransitionToTransferDst(cb, currentImage);
//        cb.clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal,
//            vk::ClearColorValue(10.0f, 0.2f, 5.0f, 1.0f), clearRange);
//        vulkan.TransitionToPresent(cb, currentImage);
//
//        vulkan.End();
//
//    } while (!myWindow.ShouldClose());
//
//    vulkan.WaitIdle();
//    vulkan.Cleanup();
//
//    myWindow.DestroyMyWindow();
//    KGR::_GLFW::Destroy();
//
//    return 0;
//}


#define NOMINMAX
#define VK_USE_PLATFORM_WIN32_KHR
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>
#include <vma/vk_mem_alloc.h>

#define GLFW_INCLUDE_VULKAN
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <ranges>

const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};




namespace _Vulkan
{
	inline static vk::raii::Context gVkRaiiContext;
	struct AppInfo
	{
		const char* appName = "Basic_Api";
		const char* engineName = "None";
		std::uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
		std::uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
		std::uint32_t version = vk::ApiVersion14;
		vk::ApplicationInfo& GetInfo()
		{
			return m_Info;
		}
		void  Create()  
		{
			m_Info = vk::ApplicationInfo{
			.pApplicationName = appName,
			.applicationVersion = appVersion,
			.pEngineName = engineName,
			.engineVersion = engineVersion,
			.apiVersion = version
			};
		}
	private:
		vk::ApplicationInfo m_Info;
	};

	struct Instance
	{
		Instance( AppInfo&& info) : m_info(std::move(info)){}
		void AddLayer(const char* layer)
		{
			m_validationLayers.push_back(layer);
		}
		void Create()
		{
			std::vector<char const*> requiredLayers;
			requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());

			const auto layerProperties = gVkRaiiContext.enumerateInstanceLayerProperties();
			if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) { return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) { return strcmp(layerProperty.layerName, requiredLayer) == 0; }); }))
				throw std::runtime_error("One or more required layers are not supported !");

			uint32_t glfwExtensionCount = 0;
			auto     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
			auto     extensionProperties = gVkRaiiContext.enumerateInstanceExtensionProperties();

			std::vector<const char*> activeExtensions;
			for (uint32_t i = 0; i < glfwExtensionCount; ++i)
				activeExtensions.push_back(glfwExtensions[i]);

			for (uint32_t i = 0; i < glfwExtensionCount; ++i)
				if (std::ranges::none_of(extensionProperties, [glfwExtension = glfwExtensions[i]](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
					throw std::runtime_error("Required GLFW extensions not supported");

			m_info.Create();

			auto instanceCreateInfo = vk::InstanceCreateInfo{
				.pApplicationInfo = &m_info.GetInfo(),
				.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
				.ppEnabledLayerNames = requiredLayers.data(),
				.enabledExtensionCount = uint32_t(activeExtensions.size()),
				.ppEnabledExtensionNames = activeExtensions.data()
			};
			m_instance = std::make_unique<vk::raii::Instance>(gVkRaiiContext, instanceCreateInfo);
		}
		vk::raii::Instance& GetInstance()
		{
			return *m_instance;
		}
		const vk::raii::Instance& GetInstance() const
		{
			return *m_instance	;
		}
		void Clear()
		{
			m_instance->clear();
		}
	private :
		std::vector<char const*> m_validationLayers;
		AppInfo m_info;
		std::unique_ptr<vk::raii::Instance> m_instance;
	};

	struct PhysicDevice
	{
		
		
		void Create(Instance* instance)
		{
			auto devices = instance->GetInstance().enumeratePhysicalDevices();
			if (devices.empty())
				throw std::runtime_error("Unable to fetch physical device");

			vk::raii::PhysicalDevice* selectedDevice = nullptr;

			for (auto& d : devices)
			{
				auto dProperties = d.getProperties();

				if (dProperties.apiVersion < VK_API_VERSION_1_4)
					continue;

				if (dProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
				{
					selectedDevice = &d;
					break;
				}

				if (!selectedDevice)
					selectedDevice = &d;
			}

			if (!selectedDevice)
				throw std::runtime_error("No suitable phyisical device found");

			m_device = std::make_unique<vk::raii::PhysicalDevice>(std::move(*selectedDevice));



			std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_device->getQueueFamilyProperties();
			auto graphicsQueueFamilyProperty = std::find_if(
				queueFamilyProperties.begin(),
				queueFamilyProperties.end(),
				[](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; }
			);
			m_queueIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
		}
		vk::raii::PhysicalDevice& GetDevice()
		{
			return *m_device;
		}
		const vk::raii::PhysicalDevice& GetDevice() const
		{
			return *m_device;
		}
		void Clear()
		{
			m_device->clear();
		}
		std::uint32_t GraphicsQueueIndex() const
		{
			return m_queueIndex;
		}

	private:
		std::uint32_t m_queueIndex;
		std::unique_ptr<vk::raii::PhysicalDevice> m_device;
	};

	struct Surface
	{
		void Create(Instance* instance, GLFWwindow* window)
		{
			VkSurfaceKHR _surface;
			if (glfwCreateWindowSurface(*instance->GetInstance(), window, nullptr, &_surface) != VK_SUCCESS)
				throw std::runtime_error("Failed to create GLFW window surface");
			 m_surface = std::make_unique<vk::raii::SurfaceKHR>(instance->GetInstance(), _surface);
		}
		vk::raii::SurfaceKHR& GetSurface()
		{
			return *m_surface;
		}
		const vk::raii::SurfaceKHR& GetSurface() const
		{
			return *m_surface;
		}
		void Clear()
		{
			m_surface->clear();
		}
	private:
		std::unique_ptr<vk::raii::SurfaceKHR> m_surface;
	};

	struct Device
	{

		void Create(PhysicDevice* device,std::uint32_t count = 1)
		{
			float queuePriority = 0.0f;
			std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device->GetDevice().getQueueFamilyProperties();
			auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo{
				.queueFamilyIndex = device->GraphicsQueueIndex(),
				.queueCount = count,
				.pQueuePriorities = &queuePriority
			};

			vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features> featureChain = {
				{.features = {.samplerAnisotropy = true } },
				{.bufferDeviceAddress = true },
				{.synchronization2 = true, .dynamicRendering = true },
			};

			std::vector<const char*> deviceExtensions = {
				vk::KHRSwapchainExtensionName,
				vk::KHRSpirv14ExtensionName,
				vk::KHRSynchronization2ExtensionName
			};

			vk::DeviceCreateInfo deviceCreateInfo{
				.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
				.queueCreateInfoCount = 1,
				.pQueueCreateInfos = &deviceQueueCreateInfo,
				.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
				.ppEnabledExtensionNames = deviceExtensions.data()
			};

			m_device = std::make_unique< vk::raii::Device>(device->GetDevice(), deviceCreateInfo);
		}

		vk::raii::Device& GetDevice()
		{
			return *m_device;
		}

		const vk::raii::Device& GetDevice() const 
		{
			return *m_device;
		}

		void Clear()
		{
			m_device->clear();
		}
		void WaitIdle()
		{
			m_device->waitIdle();
		}
	private:
		std::unique_ptr<vk::raii::Device> m_device;
	};


	struct Queue
	{
	public:
		void Create(Device* device, PhysicDevice* phisicalD, std::uint32_t index = 0)
		{
			m_queue = std::make_unique<vk::raii::Queue>(device->GetDevice(),phisicalD->GraphicsQueueIndex(),index);
		}
		vk::raii::Queue& GetQueue()
		{
			return *m_queue;
		}
		const vk::raii::Queue& GetQueue() const
		{
			return *m_queue;
		}
		void Clear()
		{
			m_queue->clear();
		}
		
	private:
		std::unique_ptr<vk::raii::Queue> m_queue;
	};
}





vk::raii::SwapchainKHR CreateSwapchain(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device, vk::raii::SurfaceKHR& surface, GLFWwindow* window)
{
	auto surfaceCaps = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	auto availableFormats = physicalDevice.getSurfaceFormatsKHR(surface);
	auto availablePresentModes = physicalDevice.getSurfacePresentModesKHR(surface);

	auto swapSurfaceFormat = ([](const decltype(availableFormats)& formats) {
		for (const auto& availableFormat : formats)
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return availableFormat;
		return formats[0];
		})(availableFormats);

	auto swapExtent = ([&window](const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		return vk::Extent2D{
			std::clamp<uint32_t>(width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
		})(surfaceCaps);

	auto swapPresentMode = ([](const decltype(availablePresentModes)& modes) {
		for (const auto& availablePresentMode : modes)
			if (availablePresentMode == vk::PresentModeKHR::eFifoRelaxed)
				return availablePresentMode;
		return vk::PresentModeKHR::eFifo;
		})(availablePresentModes);

	auto minImageCount = std::clamp(3u, surfaceCaps.minImageCount, surfaceCaps.maxImageCount);

	auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR{
		.flags = vk::SwapchainCreateFlagsKHR(0),
		.surface = surface,
		.minImageCount = minImageCount,
		.imageFormat = swapSurfaceFormat.format,
		.imageColorSpace = swapSurfaceFormat.colorSpace,
		.imageExtent = swapExtent,
		.imageArrayLayers = 1,
		.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
		.imageSharingMode = vk::SharingMode::eExclusive,
		.preTransform = surfaceCaps.currentTransform,
		.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		.presentMode = swapPresentMode,
		.clipped = true,
		.oldSwapchain = nullptr
	};

	return vk::raii::SwapchainKHR(device, swapchainCreateInfo);
}

struct FrameData
{
	vk::raii::Semaphore     presentCompleteSemaphore = nullptr;
	vk::raii::Semaphore     renderFinishedSemaphore = nullptr;
	vk::raii::CommandBuffer commandBuffer = nullptr;
};

uint32_t AcquireNextImage(vk::raii::Device& device, vk::raii::SwapchainKHR& swapchain, vk::raii::Semaphore& semaphore, vk::raii::Fence& fence)
{
	auto fenceResult = device.waitForFences(*fence, true, UINT64_MAX);
	device.resetFences(*fence);

	auto [result, currentImageIndex] = swapchain.acquireNextImage(UINT64_MAX, semaphore, nullptr);
	if (result != vk::Result::eSuccess)
		throw std::runtime_error("No success at acquire. Check if suboptimal ?");

	return currentImageIndex;
}

void Present(vk::raii::Semaphore& waitSemaphore, vk::raii::Queue& queue, vk::raii::SwapchainKHR& swapchain, uint32_t imageIndex)
{
	const auto presentInfo = vk::PresentInfoKHR{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*waitSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &*swapchain,
		.pImageIndices = &imageIndex
	};

	if (queue.presentKHR(presentInfo) != vk::Result::eSuccess)
		throw std::runtime_error("Error while presenting");
}

void Submit(vk::raii::Queue& queue, vk::raii::CommandBuffer& commandBuffer, vk::raii::Semaphore& waitSemaphore, vk::raii::Semaphore& signalSemaphore, vk::raii::Fence& fence)
{
	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	const auto submitInfo = vk::SubmitInfo{
		   .waitSemaphoreCount = (waitSemaphore == nullptr) ? 0u : 1u,
		   .pWaitSemaphores = waitSemaphore == nullptr ? nullptr : &*waitSemaphore,
		   .pWaitDstStageMask = &waitDestinationStageMask,
		   .commandBufferCount = 1,
		   .pCommandBuffers = &*commandBuffer,
		   .signalSemaphoreCount = (signalSemaphore == nullptr) ? 0u : 1u,
		   .pSignalSemaphores = signalSemaphore == nullptr ? nullptr : &*signalSemaphore,
	};

	queue.submit(submitInfo, fence);
}

void Transition(vk::raii::CommandBuffer& cb, vk::ImageLayout from, vk::ImageLayout to, vk::Image& image, bool isDepth /* = false */, bool force /* = false */)
{
	// FIXME: Maybe concurrency issues when working with the same image in multiple command buffers ?
	if (from == to && !force)
		return;

	vk::ImageMemoryBarrier2 barrier = {
		.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe,
		.srcAccessMask = vk::AccessFlagBits2::eNone,
		.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
		.oldLayout = from,
		.newLayout = to,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange = {
			.aspectMask = isDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor,
			.baseMipLevel = 0,
			.levelCount = 1, // FIXME: Needs to work for multiple mip levels too
			.baseArrayLayer = 0,
			.layerCount = 1  // FIXME: Needs to work for array images too
		}
	};

	vk::DependencyInfo dependencyInfo = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	};

	cb.pipelineBarrier2(dependencyInfo);
}

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	auto window = glfwCreateWindow(1280, 720, "Gaming Campus goes Vulkan", nullptr, nullptr);

	_Vulkan::AppInfo info{};
	

	auto instance = _Vulkan::Instance{std::move(info)};
	instance.AddLayer("VK_LAYER_KHRONOS_validation");
	instance.Create();


	auto physicalDevice = _Vulkan::PhysicDevice{};
	physicalDevice.Create(&instance);

	auto graphicsQueueFamily = physicalDevice.GraphicsQueueIndex();
	auto surface = _Vulkan::Surface{};
	surface.Create(&instance, window);
	auto device = _Vulkan::Device{};
	device.Create(&physicalDevice);
	auto graphicsQueue = _Vulkan::Queue{};
	graphicsQueue.Create(&device, &physicalDevice);
	auto swapchain = CreateSwapchain(physicalDevice.GetDevice(), device.GetDevice(), surface.GetSurface(), window);
	auto swapchainImages = swapchain.getImages();

	auto poolInfo = vk::CommandPoolCreateInfo{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = graphicsQueueFamily
	};
	auto commandPool = vk::raii::CommandPool(device.GetDevice(), poolInfo);

	auto frameData = swapchainImages | std::views::transform([&](const vk::Image&) {
		vk::CommandBufferAllocateInfo allocInfo{
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};

		return FrameData{
			.presentCompleteSemaphore = vk::raii::Semaphore(device.GetDevice(), vk::SemaphoreCreateInfo()),
			.renderFinishedSemaphore = vk::raii::Semaphore(device.GetDevice(), vk::SemaphoreCreateInfo())
		};
		}) | std::ranges::to<std::vector>();

	for (auto& fd : frameData)
	{
		vk::CommandBufferAllocateInfo allocInfo{
			.commandPool = commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};

		fd.commandBuffer = std::move(vk::raii::CommandBuffers(device.GetDevice(), allocInfo).front());
	}

	uint32_t currentFrame = 0;
	vk::raii::Fence drawFence = vk::raii::Fence(device.GetDevice(), { .flags = vk::FenceCreateFlagBits::eSignaled });

	do
	{
		glfwPollEvents();
		auto  currentImageIndex = AcquireNextImage(device.GetDevice(), swapchain, frameData[currentFrame].presentCompleteSemaphore, drawFence);
		auto& currentImage = swapchainImages[currentImageIndex];

		auto& cb = frameData[currentFrame].commandBuffer;
		cb.begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		auto clearRange = vk::ImageSubresourceRange{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = vk::RemainingMipLevels,
			.layerCount = vk::RemainingArrayLayers
		};

		Transition(cb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, currentImage, false, true);
		cb.clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal, vk::ClearColorValue(0.1f, 0.2f, 0.3f, 1.0f), clearRange);
		Transition(cb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR, currentImage, false, false);
		cb.end();

		Submit(graphicsQueue.GetQueue(), cb, frameData[currentFrame].presentCompleteSemaphore, frameData[currentFrame].renderFinishedSemaphore, drawFence);

		Present(frameData[currentFrame].renderFinishedSemaphore, graphicsQueue.GetQueue(), swapchain, currentImageIndex);
		++currentFrame %= frameData.size();
	} while (!glfwWindowShouldClose(window));

	device.WaitIdle();

	drawFence.clear();
	frameData.clear();
	commandPool.clear();
	swapchainImages.clear();
	swapchain.clear();
	graphicsQueue.Clear();
	device.Clear();
	surface.Clear();
	physicalDevice.Clear();
	instance.Clear();
	glfwDestroyWindow(window);
	glfwTerminate();
}

