////#include "AllFiles.h"
////
////bool f11_pressed(GLFWwindow* window)
////{
////	return glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS;
////}
////
////int main()
////{
////    KGR::_GLFW myWindow;
////    KGR::_GLFW::Init();
////    KGR::_GLFW::AddHint(GLFW_CLIENT_API, GLFW_NO_API);
////    KGR::_GLFW::AddHint(GLFW_RESIZABLE, GLFW_FALSE);
////    myWindow.CreateMyWindow({ 1280, 720 }, "GC goes Vulkan", nullptr, nullptr);
////
////    KGR::_Vulkan vulkan;
////    vulkan.Init(&myWindow);
////
////    uint32_t currentFrame = 0;
////
////    do
////    {
////        KGR::_GLFW::PollEvent();
////
////        auto& cb = vulkan.Begin();
////        auto& currentImage = vulkan.GetCurrentImage();
////
////        auto clearRange = vk::ImageSubresourceRange
////        {
////            .aspectMask = vk::ImageAspectFlagBits::eColor,
////            .levelCount = vk::RemainingMipLevels,
////            .layerCount = vk::RemainingArrayLayers
////        };
////
////        vulkan.TransitionToTransferDst(cb, currentImage);
////        cb.clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal,
////            vk::ClearColorValue(10.0f, 0.2f, 5.0f, 1.0f), clearRange);
////        vulkan.TransitionToPresent(cb, currentImage);
////
////        vulkan.End();
////
////    } while (!myWindow.ShouldClose());
////
////    vulkan.WaitIdle();
////    vulkan.Cleanup();
////
////    myWindow.DestroyMyWindow();
////    KGR::_GLFW::Destroy();
////
////    return 0;
////}
//
//
//#define NOMINMAX
//#define VK_USE_PLATFORM_WIN32_KHR
//#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
//#include <vulkan/vulkan_raii.hpp>
//#include <vma/vk_mem_alloc.h>
//
//#define GLFW_INCLUDE_VULKAN
//#define GLFW_EXPOSE_NATIVE_WIN32
//#include <iostream>
//#include <GLFW/glfw3.h>
//#include <GLFW/glfw3native.h>
//
//#include <ranges>
//#include "AllFiles.h"
//
//const std::vector<char const*> validationLayers = 
//{
//	"VK_LAYER_KHRONOS_validation"
//};
//
//
//
//
//namespace _Vulkan
//{
//	inline static vk::raii::Context gVkRaiiContext;
//	struct AppInfo
//	{
//		const char* appName = "Basic_Api";
//		const char* engineName = "None";
//		std::uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
//		std::uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
//		std::uint32_t version = vk::ApiVersion14;
//		vk::ApplicationInfo& GetInfo()
//		{
//			return m_Info;
//		}
//		void  Create()  
//		{
//			m_Info = vk::ApplicationInfo{
//			.pApplicationName = appName,
//			.applicationVersion = appVersion,
//			.pEngineName = engineName,
//			.engineVersion = engineVersion,
//			.apiVersion = version
//			};
//		}
//	private:
//		vk::ApplicationInfo m_Info;
//	};
//
//	struct Instance
//	{
//		Instance( AppInfo&& info,std::vector<const char*> validationLayers) : m_info(std::move(info)),m_validationLayers(validationLayers)
//		{
//			
//			std::vector<char const*> requiredLayers;
//			requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());
//
//			const auto layerProperties = gVkRaiiContext.enumerateInstanceLayerProperties();
//			if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) { return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) { return strcmp(layerProperty.layerName, requiredLayer) == 0; }); }))
//				throw std::runtime_error("One or more required layers are not supported !");
//
//			uint32_t glfwExtensionCount = 0;
//			auto     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
//			auto     extensionProperties = gVkRaiiContext.enumerateInstanceExtensionProperties();
//
//			std::vector<const char*> activeExtensions;
//			for (uint32_t i = 0; i < glfwExtensionCount; ++i)
//				activeExtensions.push_back(glfwExtensions[i]);
//
//			for (uint32_t i = 0; i < glfwExtensionCount; ++i)
//				if (std::ranges::none_of(extensionProperties, [glfwExtension = glfwExtensions[i]](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
//					throw std::runtime_error("Required GLFW extensions not supported");
//
//			m_info.Create();
//
//			auto instanceCreateInfo = vk::InstanceCreateInfo{
//				.pApplicationInfo = &m_info.GetInfo(),
//				.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
//				.ppEnabledLayerNames = requiredLayers.data(),
//				.enabledExtensionCount = uint32_t(activeExtensions.size()),
//				.ppEnabledExtensionNames = activeExtensions.data()
//			};
//			m_instance = vk::raii::Instance(gVkRaiiContext, instanceCreateInfo);
//		}
//	
//		vk::raii::Instance& GetInstance()
//		{
//			return m_instance;
//		}
//		const vk::raii::Instance& GetInstance() const
//		{
//			return m_instance	;
//		}
//		void Clear()
//		{
//			m_instance.clear();
//		}
//	private :
//		std::vector<char const*> m_validationLayers;
//		AppInfo m_info;
//		vk::raii::Instance m_instance= nullptr;
//	};
//
//	struct PhysicDevice
//	{
//		PhysicDevice(){}
//		PhysicDevice(Instance* instance)
//		{
//			auto devices = instance->GetInstance().enumeratePhysicalDevices();
//			if (devices.empty())
//				throw std::runtime_error("Unable to fetch physical device");
//
//			vk::raii::PhysicalDevice* selectedDevice = nullptr;
//
//			for (auto& d : devices)
//			{
//				auto dProperties = d.getProperties();
//
//				if (dProperties.apiVersion < VK_API_VERSION_1_4)
//					continue;
//
//				if (dProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu)
//				{
//					selectedDevice = &d;
//					break;
//				}
//
//				if (!selectedDevice)
//					selectedDevice = &d;
//			}
//
//			if (!selectedDevice)
//				throw std::runtime_error("No suitable phyisical device found");
//
//			m_device = vk::raii::PhysicalDevice(std::move(*selectedDevice));
//
//
//
//			std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_device.getQueueFamilyProperties();
//			auto graphicsQueueFamilyProperty = std::find_if(
//				queueFamilyProperties.begin(),
//				queueFamilyProperties.end(),
//				[](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; }
//			);
//			m_queueIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
//		}
//		vk::raii::PhysicalDevice& GetDevice()
//		{
//			return m_device;
//		}
//		const vk::raii::PhysicalDevice& GetDevice() const
//		{
//			return m_device;
//		}
//		void Clear()
//		{
//			m_device.clear();
//		}
//		std::uint32_t GraphicsQueueIndex() const
//		{
//			return m_queueIndex;
//		}
//
//	private:
//		std::uint32_t m_queueIndex = 0;
//		vk::raii::PhysicalDevice m_device = nullptr;
//	};
//
//	struct Surface
//	{
//		Surface(){}
//		Surface(Instance* instance, GLFWwindow* window)
//		{
//			VkSurfaceKHR _surface;
//			if (glfwCreateWindowSurface(*instance->GetInstance(), window, nullptr, &_surface) != VK_SUCCESS)
//				throw std::runtime_error("Failed to create GLFW window surface");
//			 m_surface = vk::raii::SurfaceKHR(instance->GetInstance(), _surface);
//		}
//		vk::raii::SurfaceKHR& GetSurface()
//		{
//			return m_surface;
//		}
//		const vk::raii::SurfaceKHR& GetSurface() const
//		{
//			return m_surface;
//		}
//		void Clear()
//		{
//			m_surface.clear();
//		}
//	private:
//		vk::raii::SurfaceKHR m_surface = nullptr;
//	};
//
//	struct Device
//	{
//		Device(){}
//		Device(PhysicDevice* device,std::uint32_t count = 1)
//		{
//			float queuePriority = 0.0f;
//			std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device->GetDevice().getQueueFamilyProperties();
//			auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo{
//				.queueFamilyIndex = device->GraphicsQueueIndex(),
//				.queueCount = count,
//				.pQueuePriorities = &queuePriority
//			};
//
//			vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features> featureChain = {
//				{.features = {.samplerAnisotropy = true } },
//				{.bufferDeviceAddress = true },
//				{.synchronization2 = true, .dynamicRendering = true },
//			};
//
//			std::vector<const char*> deviceExtensions = {
//				vk::KHRSwapchainExtensionName,
//				vk::KHRSpirv14ExtensionName,
//				vk::KHRSynchronization2ExtensionName
//			};
//
//			vk::DeviceCreateInfo deviceCreateInfo{
//				.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
//				.queueCreateInfoCount = 1,
//				.pQueueCreateInfos = &deviceQueueCreateInfo,
//				.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
//				.ppEnabledExtensionNames = deviceExtensions.data()
//			};
//
//			m_device =  vk::raii::Device(device->GetDevice(), deviceCreateInfo);
//		}
//
//		vk::raii::Device& GetDevice()
//		{
//			return m_device;
//		}
//
//		const vk::raii::Device& GetDevice() const 
//		{
//			return m_device;
//		}
//
//		void Clear()
//		{
//			m_device.clear();
//		}
//		void WaitIdle()
//		{
//			m_device.waitIdle();
//		}
//	private:
//		vk::raii::Device m_device = nullptr;
//	};
//
//
//	struct Queue
//	{
//	public:
//		Queue(){}
//		Queue(Device* device, PhysicDevice* phisicalD, std::uint32_t index = 0)
//		{
//			m_queue = vk::raii::Queue(device->GetDevice(),phisicalD->GraphicsQueueIndex(),index);
//		}
//		vk::raii::Queue& GetQueue()
//		{
//			return m_queue;
//		}
//		const vk::raii::Queue& GetQueue() const
//		{
//			return m_queue;
//		}
//		void Clear()
//		{
//			m_queue.clear();
//		}
//		
//	private:
//		vk::raii::Queue m_queue = nullptr;
//	};
//
//
//	struct SwapChain
//	{
//	public:
//		SwapChain(){}
//		SwapChain(PhysicDevice* phDevice, Device* device, Surface* surface, GLFWwindow* window, uint32_t ImageCount = 3, SwapChain* old = nullptr)
//		{
//			Create(phDevice,device,surface,window,ImageCount,old);
//		}
//		void Create(PhysicDevice* phDevice,Device* device,Surface* surface, GLFWwindow* window , uint32_t ImageCount = 3,SwapChain* old = nullptr)
//		{
//			auto surfaceCaps = phDevice->GetDevice().getSurfaceCapabilitiesKHR(surface->GetSurface());
//			auto availableFormats = phDevice->GetDevice().getSurfaceFormatsKHR(surface->GetSurface());
//			auto availablePresentModes = phDevice->GetDevice().getSurfacePresentModesKHR(surface->GetSurface());
//
//			auto swapSurfaceFormat = ([](const decltype(availableFormats)& formats) {
//				for (const auto& availableFormat : formats)
//					if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
//						return availableFormat;
//				return formats[0];
//				})(availableFormats);
//
//			auto swapExtent = ([&window](const vk::SurfaceCapabilitiesKHR& capabilities) {
//				if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
//					return capabilities.currentExtent;
//
//				int width, height;
//				glfwGetFramebufferSize(window, &width, &height);
//
//				return vk::Extent2D{
//					std::clamp<uint32_t>(width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width),
//					std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
//				};
//				})(surfaceCaps);
//
//			auto swapPresentMode = ([](const decltype(availablePresentModes)& modes) {
//				for (const auto& availablePresentMode : modes)
//					if (availablePresentMode == vk::PresentModeKHR::eFifoRelaxed)
//						return availablePresentMode;
//				return vk::PresentModeKHR::eFifo;
//				})(availablePresentModes);
//
//			auto minImageCount = std::clamp(ImageCount, surfaceCaps.minImageCount, surfaceCaps.maxImageCount);
//
//			auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR{
//				.flags = vk::SwapchainCreateFlagsKHR(0),
//				.surface = surface->GetSurface(),
//				.minImageCount = minImageCount,
//				.imageFormat = swapSurfaceFormat.format,
//				.imageColorSpace = swapSurfaceFormat.colorSpace,
//				.imageExtent = swapExtent,
//				.imageArrayLayers = 1,
//				.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst,
//				.imageSharingMode = vk::SharingMode::eExclusive,
//				.preTransform = surfaceCaps.currentTransform,
//				.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
//				.presentMode = swapPresentMode,
//				.clipped = true,
//				.oldSwapchain = old == nullptr ? nullptr : *old->GetSwapChain()
//			};
//
//			m_chain = vk::raii::SwapchainKHR(device->GetDevice(), swapchainCreateInfo);
//		}
//		void Clear()
//		{
//			m_chain.clear();
//		}
//		vk::raii::SwapchainKHR& GetSwapChain()
//		{
//			return m_chain;
//		}
//		const vk::raii::SwapchainKHR& GetSwapChain() const
//		{
//			return m_chain;
//		}
//	private:
//	  vk::raii::SwapchainKHR m_chain = nullptr;
//	};
//
//	struct VkImages
//	{
//	public:
//		VkImages(){}
//		VkImages(SwapChain* chain)
//		{
//			m_images = chain->GetSwapChain().getImages();
//		}
//		std::vector<vk::Image>& GetImages()
//		{
//			return m_images;
//		}
//		const std::vector<vk::Image>& GetImages() const
//		{
//			return m_images;
//		}
//		void Clear()
//		{
//			m_images.clear();
//		}
//	private:
//		std::vector<vk::Image> m_images;
//	};
//
//
//
//
//	struct CommanPool
//	{
//	public:
//		CommanPool(){}
//		CommanPool(PhysicDevice* phyDevice,Device* device)
//		{
//			auto poolInfo = vk::CommandPoolCreateInfo{
//		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
//		.queueFamilyIndex = phyDevice->GraphicsQueueIndex()
//			};
//			m_pool =  vk::raii::CommandPool(device->GetDevice(), poolInfo);
//		}
//		vk::raii::CommandPool& GetPool()
//		{
//			return m_pool;
//		}
//		const vk::raii::CommandPool& GetPool() const 
//		{
//			return m_pool;
//		}
//		void Clear()
//		{
//			m_pool.clear();
//		}
//	private:
//		vk::raii::CommandPool m_pool = nullptr;
//	};
//
//	struct CommandBuffer
//	{
//	public:
//		CommandBuffer() = default;
//		CommandBuffer(Device* device,CommanPool* pool)
//		{
//			vk::CommandBufferAllocateInfo allocInfo{
//			.commandPool = pool->GetPool(),
//			.level = vk::CommandBufferLevel::ePrimary,
//			.commandBufferCount = 1
//			};
//			m_buffer = std::move(vk::raii::CommandBuffers(device->GetDevice(), allocInfo).front());
//		}
//		vk::raii::CommandBuffer& GetBuffer()
//		{
//			return m_buffer;
//		}
//		const vk::raii::CommandBuffer& GetBuffer()const
//		{
//			return m_buffer;
//		}
//		void Clear()
//		{
//			m_buffer.clear();
//		}
//	private:
//		vk::raii::CommandBuffer m_buffer = nullptr;
//	};
//
//
//	struct Semaphore
//	{
//	public:
//		Semaphore(){}
//		Semaphore(Device* device)
//		{
//			m_semaphore = vk::raii::Semaphore(device->GetDevice(), vk::SemaphoreCreateInfo());
//		}
//		vk::raii::Semaphore& GetSemaphore()
//		{
//			return m_semaphore;
//		}
//		const vk::raii::Semaphore& GetSemaphore() const
//		{
//			return m_semaphore;
//		}
//		void Clear()
//		{
//			m_semaphore.clear();
//		}
//	private:
//		vk::raii::Semaphore m_semaphore = nullptr;
//	};
//
//	struct FrameData
//	{
//		_Vulkan::Semaphore    presentCompleteSemaphore;
//		_Vulkan::Semaphore    renderFinishedSemaphore;
//		_Vulkan::CommandBuffer commandBuffer;
//	};
//
//	struct Fence
//	{
//	public:
//		Fence() = default;
//		Fence(Device* device)
//		{
//			m_fence = vk::raii::Fence(device->GetDevice(), { .flags = vk::FenceCreateFlagBits::eSignaled });
//		}
//		vk::raii::Fence& GetFence()
//		{
//			return m_fence;
//		}
//		const vk::raii::Fence& GetFence() const
//		{
//			return m_fence;
//		}
//		void Clear()
//		{
//			m_fence.clear();
//		}
//	private:
//		vk::raii::Fence m_fence = nullptr;
//	};
//}
//
//
//
//
//
//
//
//
//
//int32_t AcquireNextImage(vk::raii::Device& device, vk::raii::SwapchainKHR& swapchain, vk::raii::Semaphore& semaphore, vk::raii::Fence& fence)
//{
//	auto fenceResult = device.waitForFences(*fence, true, UINT64_MAX);
//	device.resetFences(*fence);
//
//	uint32_t imageIndex = 0;
//
//	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*semaphore);
//
//	VkDevice vkDevice = static_cast<VkDevice>(*device);
//	VkSwapchainKHR vkSwapChain = static_cast<VkSwapchainKHR>(*swapchain);
//
//	VkResult result = vkAcquireNextImageKHR(
//		vkDevice,
//		vkSwapChain,
//		UINT64_MAX,                   
//		vkSemaphore,          
//		VK_NULL_HANDLE,                     
//		&imageIndex
//	);
//
//	if (result == VK_ERROR_OUT_OF_DATE_KHR ||  result == VK_SUBOPTIMAL_KHR) {
//		return  -1;
//	}
//
//	return static_cast<int>(imageIndex);
//}
//
//int Present(vk::raii::Semaphore& waitSemaphore, vk::raii::Queue& queue, vk::raii::SwapchainKHR& swapchain, uint32_t imageIndex)
//{
//	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*waitSemaphore);
//
//	VkPresentInfoKHR presentInfo{};
//	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
//	presentInfo.waitSemaphoreCount = 1;
//	presentInfo.pWaitSemaphores = &vkSemaphore; // VkSemaphore
//	presentInfo.swapchainCount = 1;
//
//	VkSwapchainKHR swapchainC = static_cast<VkSwapchainKHR>(*swapchain);
//	presentInfo.pSwapchains = &swapchainC;
//	presentInfo.pImageIndices = &imageIndex;
//	presentInfo.pResults = nullptr;
//
//	VkQueue queueC = static_cast<VkQueue>(*queue);
//	VkResult result = vkQueuePresentKHR(queueC, &presentInfo);
//	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
//		return -1;
//	}
//	return 0;
//}
//
//void Submit(vk::raii::Queue& queue, vk::raii::CommandBuffer& commandBuffer, vk::raii::Semaphore& waitSemaphore, vk::raii::Semaphore& signalSemaphore, vk::raii::Fence& fence)
//{
//	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
//
//	const auto submitInfo = vk::SubmitInfo{
//		   .waitSemaphoreCount = (waitSemaphore == nullptr) ? 0u : 1u,
//		   .pWaitSemaphores = waitSemaphore == nullptr ? nullptr : &*waitSemaphore,
//		   .pWaitDstStageMask = &waitDestinationStageMask,
//		   .commandBufferCount = 1,
//		   .pCommandBuffers = &*commandBuffer,
//		   .signalSemaphoreCount = (signalSemaphore == nullptr) ? 0u : 1u,
//		   .pSignalSemaphores = signalSemaphore == nullptr ? nullptr : &*signalSemaphore,
//	};
//
//	queue.submit(submitInfo, fence);
//}
//
//void Transition(vk::raii::CommandBuffer& cb, vk::ImageLayout from, vk::ImageLayout to, vk::Image& image, bool isDepth /* = false */, bool force /* = false */)
//{
//	// FIXME: Maybe concurrency issues when working with the same image in multiple command buffers ?
//	if (from == to && !force)
//		return;
//
//	vk::ImageMemoryBarrier2 barrier = {
//		.srcStageMask = vk::PipelineStageFlagBits2::eTopOfPipe,
//		.srcAccessMask = vk::AccessFlagBits2::eNone,
//		.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
//		.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite,
//		.oldLayout = from,
//		.newLayout = to,
//		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
//		.image = image,
//		.subresourceRange = {
//			.aspectMask = isDepth ? vk::ImageAspectFlagBits::eDepth : vk::ImageAspectFlagBits::eColor,
//			.baseMipLevel = 0,
//			.levelCount = 1, // FIXME: Needs to work for multiple mip levels too
//			.baseArrayLayer = 0,
//			.layerCount = 1  // FIXME: Needs to work for array images too
//		}
//	};
//
//	vk::DependencyInfo dependencyInfo = {
//		.dependencyFlags = {},
//		.imageMemoryBarrierCount = 1,
//		.pImageMemoryBarriers = &barrier
//	};
//
//	cb.pipelineBarrier2(dependencyInfo);
//}
//
//int main()
//{
//	glfwInit();
//	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
//	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
//	KGR::_GLFW::Window window;
//	window.CreateMyWindow({ 1280, 720 }, "Test", nullptr, nullptr);
//
//	_Vulkan::AppInfo info{};
//	auto instance = _Vulkan::Instance{ std::move(info),std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} };
//	auto physicalDevice = _Vulkan::PhysicDevice{ &instance };
//	auto surface = _Vulkan::Surface{&instance,&window.GetWindow()};
//	auto device = _Vulkan::Device{&physicalDevice};
//	auto graphicsQueue = _Vulkan::Queue{&device,&physicalDevice};
//	auto swapchain = _Vulkan::SwapChain{&physicalDevice,&device,&surface,&window.GetWindow() };
//	auto swapchainImages = _Vulkan::VkImages{&swapchain};
//	auto commandPool = _Vulkan::CommanPool{&physicalDevice,&device};
//
//
//
//	auto frameData = swapchainImages.GetImages() | std::views::transform([&](const vk::Image& i) {
//		return _Vulkan::FrameData{
//			.presentCompleteSemaphore = _Vulkan::Semaphore(&device),
//			.renderFinishedSemaphore = _Vulkan::Semaphore(&device),
//			.commandBuffer = _Vulkan::CommandBuffer(&device,&commandPool)
//		};
//		
//		}) | std::ranges::to<std::vector>();
//
//
//	uint32_t currentFrame = 0;
//	auto drawFence = _Vulkan::Fence(&device);
//
//	do
//	{
//		glfwPollEvents();
//		auto  currentImageIndex = AcquireNextImage(device.GetDevice(), swapchain.GetSwapChain(), frameData[currentFrame].presentCompleteSemaphore.GetSemaphore(), drawFence.GetFence());
//
//		if (currentImageIndex == -1)
//		{
//			swapchain.Create(&physicalDevice, &device, &surface, &window.GetWindow(), 3, &swapchain);
//			currentImageIndex = AcquireNextImage(device.GetDevice(), swapchain.GetSwapChain(), frameData[currentFrame].presentCompleteSemaphore.GetSemaphore(), drawFence.GetFence());
//			swapchainImages= (&swapchain);
//		}
//
//		auto& currentImage = swapchainImages.GetImages()[currentImageIndex];
//
//		auto& cb = frameData[currentFrame].commandBuffer;
//		cb.GetBuffer().begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
//
//		auto clearRange = vk::ImageSubresourceRange{
//			.aspectMask = vk::ImageAspectFlagBits::eColor,
//			.levelCount = vk::RemainingMipLevels,
//			.layerCount = vk::RemainingArrayLayers
//		};
//
//		Transition(cb.GetBuffer(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, currentImage, false, true);
//		cb.GetBuffer().clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal, vk::ClearColorValue(0.1f, 0.2f, 0.3f, 1.0f), clearRange);
//		Transition(cb.GetBuffer(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR, currentImage, false, false);
//		cb.GetBuffer().end();
//
//		Submit(graphicsQueue.GetQueue(), cb.GetBuffer(), frameData[currentFrame].presentCompleteSemaphore.GetSemaphore(), frameData[currentFrame].renderFinishedSemaphore.GetSemaphore(), drawFence.GetFence());
//
//		auto reult = Present(frameData[currentFrame].renderFinishedSemaphore.GetSemaphore(), graphicsQueue.GetQueue(), swapchain.GetSwapChain(), currentImageIndex);
//
//		if (reult == -1)
//		{
//			swapchain.Create(&physicalDevice, &device, &surface, &window.GetWindow(), 3, &swapchain);
//			swapchainImages = (&swapchain);
//		}
//		++currentFrame %= frameData.size();
//	} while (!glfwWindowShouldClose(&window.GetWindow()));
//
//
//	device.WaitIdle();
//
//	drawFence.Clear();
//	frameData.clear();
//	commandPool.Clear();
//	swapchainImages.Clear();
//	swapchain.Clear();
//	graphicsQueue.Clear();
//	device.Clear();
//	surface.Clear();
//	physicalDevice.Clear();
//	instance.Clear();
//	glfwDestroyWindow(&window.GetWindow());
//	glfwTerminate();
//}
//



#include <iostream>
#include "RTTI/Entities.h"
#include "RTTI/Registry.h"

// Basic Component Class 
struct PosComponent
{
	float x;
	float y;
	float z;
};

// Basic Component Class 
struct VelComponent
{
	float x;
	float y;
	float z;
};

// witch type of registry you want
using registry = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;
int main()
{
	// Components can be Whatever you want if it has no custom constructor
	// create your registry
	registry registry;
	// Create an entity
	auto e1 = registry.CreateEntity();
	auto e2 = registry.CreateEntity();
	auto e3 = registry.CreateEntity();
	auto e4 = registry.CreateEntity();
	// Add component 
	registry.AddComponent<PosComponent>(e1);
	// add multiple Component 
	registry.AddComponents<PosComponent, VelComponent>(e2);
	// Add Component with instance 
	PosComponent toto{ .x = 100.0f,.y = 100.0f,.z = 100.0f };
	//registry.AddComponent<PosComponent>(e3, std::move(toto));
	// OR
	registry.AddComponent<PosComponent>(e3, { .x = 100.0f,.y = 100.0f,.z = 100.0f });
	// Add multiple Component with instance 
	PosComponent toto2{ .x = 100.0f,.y = 100.0f,.z = 100.0f };
	VelComponent toto3{ .x = 100.0f,.y = 100.0f,.z = 100.0f };
	registry.AddComponents<PosComponent, VelComponent>(e4, std::move(toto2), std::move(toto3));
	// verify if has a component 
	bool has = registry.HasComponent<PosComponent>(e1);
	// verify if has multiple component 
	bool has2 = registry.HasAllComponents<PosComponent, VelComponent>(e1);
	bool has3 = registry.HasAnyComponents<PosComponent, VelComponent>(e1);

	std::cout << " ? :" << has2 << " ? :" << has3 << std::endl;
	// remove a component 
	registry.RemoveComponent<PosComponent>(e1);
	// remove multiple component 
	registry.RemoveComponents<PosComponent, VelComponent>(e2);
	// BAD PRACTICE get a component
	auto& comp = registry.GetComponent<PosComponent>(e3);
	
	// GOOD PRACTICE create a filter for system 
	auto filter = registry.GetFilter<PosComponent>();
	// How to use filter for system 
	for (auto& e : filter)
	{
		auto& posComp = filter.GetComponent<PosComponent>(e);
	}
	// GOOD PRACTICE create a filter for system with multiple components
	auto filter2 = registry.GetAllComponentsFilter<PosComponent, VelComponent>();
	// How to use filter for system with multiple component

	std::cout << filter2.Size();
	for (auto& e : filter2.GetEntities())
	{
		auto& posComp = filter2.GetComponent<PosComponent>(e);
		auto& vecComp = filter2.GetComponent<VelComponent>(e);
	}

	

	auto view = registry.GetAnyComponentsView<PosComponent, VelComponent>();

	for (auto& e : view)
	{
		
	}
	std::cout << "\n" << view.Size();
	// Delete an entity ( auto remove components ) 
	registry.DestroyEntity(e1);
	registry.DestroyEntity(e2);
	registry.DestroyEntity(e3);
	registry.DestroyEntity(e4);

	std::cout << '\n' << e1 << e2<< e3<< e4;
}