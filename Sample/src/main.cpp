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
#include <thread>

#include <glm/glm.hpp>


namespace KGR
{
	enum class WnState
	{
		Error,
		Windowed,
		FullScreen
	};
	 struct WnInfo
	{
		 glm::ivec2 pos ={0,0};
		 bool posUpdated = false;

		 glm::ivec2 size = { 0,0 };
		 bool sizeUpdated = false;
		 WnState state = WnState::Error;
	};
	enum class MonitorState
	{
		Primary,
		Current
	};
	struct Monitor
	{
		GLFWmonitor* glfwMonitor = nullptr;
	};
	struct GLFW
	{
		
		GLFW()
		: m_window(nullptr)
		,m_lasWindowedPos(0,0)
		,m_lasWindowedSize(0,0)
		{}
		static void Init()
		{
			glfwInit();
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
			glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
		}
		static void Destruct()
		{
			glfwTerminate();
		}
		static void PollEvent()
		{
			glfwPollEvents();
		}

		const GLFWwindow& GetWindow() const 
		{
			return *m_window;
		}
		GLFWwindow& GetWindow()
		{
			return *m_window;
		}

		void CreateMyWindow(glm::ivec2 size,const char* name, Monitor monitor, GLFWwindow* share)
		{
			if (!IsState<WnState::Error>())
				DestroyMyWindow();

			m_info.state = monitor.glfwMonitor == nullptr ? WnState::Windowed : WnState::FullScreen;
			m_window = glfwCreateWindow(size.x,size.y, name, monitor.glfwMonitor, share);

			glfwSetWindowUserPointer(m_window, this);
			glfwSetWindowPosCallback(m_window,&GLFW::PosCallBack);
			glfwSetWindowSizeCallback(m_window, &GLFW::SizeCallBack);

			glfwGetWindowPos(m_window, &m_info.pos.x, &m_info.pos.y);
			glfwGetWindowSize(m_window, &m_info.size.x, &m_info.size.y);
		}
		void DestroyMyWindow()
		{
			if (!IsState<WnState::Error>())
				glfwDestroyWindow(m_window);

			m_window = nullptr;
			m_info.state = WnState::Error;

			m_info.size = {0,0};
			m_info.pos  = {0,0};
		}
		bool ShouldClose() const
		{
			if (IsState<WnState::Error>())
				throw std::runtime_error("invalid Window");
			return glfwWindowShouldClose(m_window);
		}

		glm::ivec2 GetPos() const
		{
			return m_info.pos;
		}
		glm::ivec2 GetSize() const
		{
			return m_info.size;
		}

		void SetSize(glm::ivec2 size)
		{
			if (!IsState<WnState::Windowed>())
				throw std::runtime_error("resize only on window mode");

			glfwSetWindowSize(m_window, size.x, size.y);
		}
		void SetPos(glm::ivec2 pos)
		{
			if (!IsState<WnState::Windowed>())
				throw std::runtime_error("repos only on window mode");

			glfwSetWindowPos(m_window, pos.x, pos.y);
		}

		void Update()
		{
			m_info.sizeUpdated = false;
			m_info.posUpdated = false;
		}

		bool PosUpdated() const
		{
			return m_info.posUpdated;
		}
		bool SizeUpdated() const
		{
			return m_info.sizeUpdated;
		}

		template<WnState state>
		bool IsState() const
		{
			return m_info.state == state;
		}
		void SetWindowState(WnState state, Monitor monitor = Monitor{})
		{
			switch (state)
			{
			case WnState::Windowed:
				{
					Windowed();
					break;
				}
			case WnState::FullScreen:
				{

					FullScreen(monitor);
					break;
				}
			case WnState::Error:
				{
				throw std::runtime_error("invalid Window");
				}
			}
		}


		template<MonitorState state>
		Monitor GetMonitor() const
		{
			if constexpr (state == MonitorState::Primary)
			{
				return { glfwGetPrimaryMonitor() };
			}
			else
			{
				int count = 0;
				GLFWmonitor** monitors = glfwGetMonitors(&count);

				int finalIndex = 0;
				float minDist = std::numeric_limits<float>::max();

				glm::vec2 windowMidPoint = m_info.pos + m_info.size / 2 ;

				for (int i = 0 ; i < count; ++i)
				{
					GLFWmonitor* current = monitors[i];
					glm::ivec2 monitorPos;
					glfwGetMonitorPos(current,&monitorPos.x, &monitorPos.y);
					glm::vec2 monitorPosF = { monitorPos.x,monitorPos.y };
					const GLFWvidmode* mode = glfwGetVideoMode(current);
					glm::vec2 monitorSize = { mode->width,mode->height };
					glm::vec2 monitorMidPoint = monitorPosF + monitorSize /2.0f;
					// TODO length squared ?? where in glm or maybe dot product ?
					float dist = glm::length(windowMidPoint - monitorMidPoint);
					if (dist < minDist)
					{
						minDist = dist;
						finalIndex = i;
					}
				}
				return Monitor{ monitors[finalIndex] };
			}
		}
	private:
		static void PosCallBack(GLFWwindow* window, int posX,int posY)
		{
			auto user = static_cast<GLFW*>(glfwGetWindowUserPointer(window));
			user->m_info.pos.x = posX;
			user->m_info.pos.y = posY;
			user->m_info.posUpdated = true;
		}
		static void SizeCallBack(GLFWwindow* window, int width, int height)
		{
			auto user = static_cast<GLFW*>(glfwGetWindowUserPointer(window));
			user->m_info.size.x = width;
			user->m_info.size.y = height;
			user->m_info.sizeUpdated = true;
		}

		void Windowed()
		{
			if (IsState<WnState::Windowed>())
				return;

			m_info.state = WnState::Windowed;

			glfwSetWindowMonitor(
				m_window,
				nullptr,
				m_lasWindowedPos.x,
				m_lasWindowedPos.y,
				m_lasWindowedSize.x,
				m_lasWindowedSize.y,
				0
			);

		}
		void FullScreen(Monitor monitor)
		{
			if (IsState<WnState::FullScreen>())
				return;

			if (!monitor.glfwMonitor)
				throw std::runtime_error("invalid monitor");

			m_info.state = WnState::FullScreen;

			m_lasWindowedPos = m_info.pos;
			m_lasWindowedSize = m_info.size;
			;
			const GLFWvidmode* mode = glfwGetVideoMode(monitor.glfwMonitor);

			glfwSetWindowMonitor(
				m_window,
				monitor.glfwMonitor,
				0, 0,
				mode->width,
				mode->height,
				mode->refreshRate
			);
		}
		
		GLFWwindow* m_window;
		WnInfo m_info;

		glm::ivec2 m_lasWindowedPos = { 0,0 };
		glm::ivec2 m_lasWindowedSize = { 0,0 };
	};
}



bool f11_pressed(GLFWwindow* window) {
	return glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
}



const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

vk::raii::Context gVkRaiiContext;

vk::raii::Instance InitInstance()
{
	// précise the vulkan version require
	const auto appInfo = vk::ApplicationInfo{
		.pApplicationName = "GCVulkan",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "None",
		.engineVersion = VK_MAKE_VERSION(0, 1, 0),
		.apiVersion = vk::ApiVersion14
	};

	// assign the validation layer here for debug
	std::vector<char const*> requiredLayers;
	requiredLayers.assign(validationLayers.begin(), validationLayers.end());

	// verify that all extension exist
	const auto layerProperties = gVkRaiiContext.enumerateInstanceLayerProperties();
	if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer)
	{
		return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
		{
			return strcmp(layerProperty.layerName, requiredLayer) == 0;
		});
	}))
		throw std::runtime_error("One or more required layers are not supported !");

	// add all glfwExtension
	uint32_t glfwExtensionCount = 0;
	auto     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	auto     extensionProperties = gVkRaiiContext.enumerateInstanceExtensionProperties();

	std::vector<const char*> activeExtensions;
	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		activeExtensions.push_back(glfwExtensions[i]);

	// verify that all extension are supported 
	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		if (std::ranges::none_of(extensionProperties, [glfwExtension = glfwExtensions[i]](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
			throw std::runtime_error("Required GLFW extensions not supported");

	// Create the Instance Vulkan
	auto instanceCreateInfo = vk::InstanceCreateInfo{
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(activeExtensions.size()),
		.ppEnabledExtensionNames = activeExtensions.data()
	};
	// Return the Instance
	return vk::raii::Instance(gVkRaiiContext, instanceCreateInfo);
}

vk::raii::PhysicalDevice FetchPhysicalDevice(vk::raii::Instance& instance)
{
	auto devices = instance.enumeratePhysicalDevices();
	if (devices.empty())
		return nullptr;

	vk::raii::PhysicalDevice result = nullptr;

	for (const auto& d : devices)
	{
		auto deviceProperties = d.getProperties();

		if (deviceProperties.apiVersion < VK_API_VERSION_1_4)
			continue;

		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
			return d;

		result = d;
	}

	return result;
}

uint32_t GetGraphicsQueueFamily(vk::raii::PhysicalDevice& physicalDevice)
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
	auto graphicsQueueFamilyProperty = std::find_if(
		queueFamilyProperties.begin(),
		queueFamilyProperties.end(),
		[](vk::QueueFamilyProperties const& qfp)
		{
			return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
		}
	);

	return static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
}

vk::raii::SurfaceKHR CreateGlfwWindowSurface(vk::raii::Instance& instance, GLFWwindow* window)
{

	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*instance, window, nullptr, &_surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create GLFW window surface");
	return vk::raii::SurfaceKHR(instance, _surface);
}

vk::raii::Device CreateDevice(vk::raii::PhysicalDevice& physicalDevice, uint32_t graphicsQueueFamily)
{

	// the priority between 0 and 1 of the application for this queue
	float queuePriority = 0.0f;
	// Get The Queue Info
	auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo{
		.queueFamilyIndex = graphicsQueueFamily,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	// the feature that we want for this device in chained struct
	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features> featureChain = {
		{.features = {.samplerAnisotropy = true } },
		{.bufferDeviceAddress = true },
		{.synchronization2 = true, .dynamicRendering = true },
	};

	// All the extensions we need
	std::vector<const char*> deviceExtensions = {
		vk::KHRSwapchainExtensionName,
		vk::KHRSpirv14ExtensionName,
		vk::KHRSynchronization2ExtensionName
	};

	// Create the device 
	vk::DeviceCreateInfo deviceCreateInfo{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data()
	};

	return vk::raii::Device(physicalDevice, deviceCreateInfo);
}

vk::raii::SwapchainKHR CreateSwapchain(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& device, vk::raii::SurfaceKHR& surface, GLFWwindow* window)
{

	auto surfaceCaps = physicalDevice.getSurfaceCapabilitiesKHR(surface);
	auto availableFormats = physicalDevice.getSurfaceFormatsKHR(surface);
	auto availablePresentModes = physicalDevice.getSurfacePresentModesKHR(surface);


	// we found the optimal surface format 
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
	KGR::GLFW myWindow;
	// GLFW ini
	KGR::GLFW::Init();
	// Create a window
	myWindow.CreateMyWindow({ 200, 100 }, "Gaming Campus goes Vulkan", KGR::Monitor{}, nullptr);

	//// Create The Vulkan Instance 
	//auto instance = InitInstance();
	//// Get The Physical Device Alias Gpu
	//auto physicalDevice = FetchPhysicalDevice(instance);
	//if (physicalDevice == nullptr)
	//	throw std::runtime_error("Unable to fetch physical device");
	//// Get The Right Family Queue
	//auto graphicsQueueFamily = GetGraphicsQueueFamily(physicalDevice);

	//// Create The Surface
	//auto surface = CreateGlfwWindowSurface(instance, &myWindow.GetWindow());
	//// Create the Device the context of gpu
	//auto device = CreateDevice(physicalDevice, graphicsQueueFamily);

	//// get the graphic queue from the device and queue index
	//auto graphicsQueue = vk::raii::Queue(device, graphicsQueueFamily, 0);
	//// Create the SwapChain
	//auto swapchain = CreateSwapchain(physicalDevice, device, surface, &myWindow.GetWindow());

	//// get the swapChained Images
	//auto swapchainImages = swapchain.getImages();

	//// Create the right CommandPool with auto reset buffer
	//auto poolInfo = vk::CommandPoolCreateInfo{
	//	.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
	//	.queueFamilyIndex = graphicsQueueFamily
	//};
	//auto commandPool = vk::raii::CommandPool(device, poolInfo);

	//// for each image create frameData
	//auto frameData = swapchainImages | std::views::transform([&](const vk::Image&) {
	//	vk::CommandBufferAllocateInfo allocInfo{
	//		.commandPool = commandPool,
	//		.level = vk::CommandBufferLevel::ePrimary,
	//		.commandBufferCount = 1
	//	};

	//	return FrameData{
	//		.presentCompleteSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo()),
	//		.renderFinishedSemaphore = vk::raii::Semaphore(device, vk::SemaphoreCreateInfo()),
	//		.commandBuffer = std::move(vk::raii::CommandBuffers(device, allocInfo).front())
	//	};
	//	}) | std::ranges::to<std::vector>();


	//uint32_t currentFrame = 0;
	//vk::raii::Fence drawFence = vk::raii::Fence(device, { .flags = vk::FenceCreateFlagBits::eSignaled });

	do
	{
		// poll Events
		KGR::GLFW::PollEvent();
		if (f11_pressed(&myWindow.GetWindow()) && myWindow.IsState<KGR::WnState::Windowed>())
		{
			auto monitor = myWindow.GetMonitor<KGR::MonitorState::Current>();
			myWindow.SetWindowState(KGR::WnState::FullScreen, monitor);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		else if (f11_pressed(&myWindow.GetWindow()) && myWindow.IsState<KGR::WnState::FullScreen>())
		{
			myWindow.SetWindowState(KGR::WnState::Windowed);
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
		myWindow.Update();

		//// Get the index of the images 
		//auto  currentImageIndex = AcquireNextImage(device, swapchain, frameData[currentFrame].presentCompleteSemaphore, drawFence);
		//// Get the images from the index 
		//auto& currentImage = swapchainImages[currentImageIndex];

		//// get the current buffer
		//auto& cb = frameData[currentImageIndex].commandBuffer;

		//// start the buffer
		//cb.begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		//// DO your action
		//auto clearRange = vk::ImageSubresourceRange{
		//	.aspectMask = vk::ImageAspectFlagBits::eColor,
		//	.levelCount = vk::RemainingMipLevels,
		//	.layerCount = vk::RemainingArrayLayers
		//};

		//Transition(cb, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, currentImage, false, true);
		//cb.clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal, vk::ClearColorValue(0.1f, 0.2f, 0.3f, 1.0f), clearRange);
		//// last transition to present 
		//Transition(cb, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR, currentImage, false, false);
		//cb.end();

		//// Submit the frame to the queue ask a draw 
		//Submit(graphicsQueue, cb, frameData[currentFrame].presentCompleteSemaphore, frameData[currentImageIndex].renderFinishedSemaphore, drawFence);

		//// when ready frame on screen
		//Present(frameData[currentImageIndex].renderFinishedSemaphore, graphicsQueue, swapchain, currentImageIndex);

		//currentFrame = currentImageIndex;

	} while (!myWindow.ShouldClose());


	/*device.waitIdle();
	drawFence.clear();
	frameData.clear();
	commandPool.clear();
	swapchainImages.clear();
	swapchain.clear();
	graphicsQueue.clear();
	device.clear();
	surface.clear();
	physicalDevice.clear();
	instance.clear();*/

	myWindow.DestroyMyWindow();
	glfwTerminate();
}
