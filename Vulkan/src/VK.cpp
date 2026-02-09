#include "VKFiles.h"
#include "VK.h"

// STRUCT APPLICATION INFO
void KGR::_Vulkan::_AppInfo::Create()
{
	m_Info = vk::ApplicationInfo
	{
		.pApplicationName = appName,
		.applicationVersion = appVersion,
		.pEngineName = engineName,
		.engineVersion = engineVersion,
		.apiVersion = version
	};
}

vk::ApplicationInfo& KGR::_Vulkan::_AppInfo::GetInfo()
{
	return m_Info;
}

// STRUCT FENCE
KGR::_Vulkan::_Fence::_Fence(_Device* device)
{
	m_fence = Fence(device->GetDevice(), { .flags = vk::FenceCreateFlagBits::eSignaled });
}

Fence& KGR::_Vulkan::_Fence::GetFence()
{
	return m_fence;
}

const Fence& KGR::_Vulkan::_Fence::GetFence() const
{
	return m_fence;
}

void KGR::_Vulkan::_Fence::Clear()
{
	m_fence.clear();
}

// STRUCT INSTANCE
KGR::_Vulkan::_Instance::_Instance(_AppInfo&& info, std::vector<char const*> validationLayers)
	: m_info(std::move(info))
	, m_validationLayers(validationLayers)
{
	std::vector<char const*> requiredLayers;
	requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());

	const auto layerProperties = vkContext.enumerateInstanceLayerProperties();
	if (std::ranges::any_of(requiredLayers, [&layerProperties](auto const& requiredLayer) { return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty) { return strcmp(layerProperty.layerName, requiredLayer) == 0; }); }))
		throw std::runtime_error("One or more required layers are not supported !");

	uint32_t glfwExtensionCount = 0;
	auto     glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	auto     extensionProperties = vkContext.enumerateInstanceExtensionProperties();

	std::vector<const char*> activeExtensions;
	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		activeExtensions.push_back(glfwExtensions[i]);

	for (uint32_t i = 0; i < glfwExtensionCount; ++i)
		if (std::ranges::none_of(extensionProperties, [glfwExtension = glfwExtensions[i]](auto const& extensionProperty) { return strcmp(extensionProperty.extensionName, glfwExtension) == 0; }))
			throw std::runtime_error("Required GLFW extensions not supported");

	m_info.Create();

	auto instanceCreateInfo = vk::InstanceCreateInfo
	{
		.pApplicationInfo = &m_info.GetInfo(),
		.enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
		.ppEnabledLayerNames = requiredLayers.data(),
		.enabledExtensionCount = uint32_t(activeExtensions.size()),
		.ppEnabledExtensionNames = activeExtensions.data()
	};

	m_instance = Instance(vkContext, instanceCreateInfo);
}

void KGR::_Vulkan::_Instance::AddLayer(const char* layer)
{
	m_validationLayers.push_back(layer);
}

Instance& KGR::_Vulkan::_Instance::GetInstance()
{
	return m_instance;
}

const Instance& KGR::_Vulkan::_Instance::GetInstance() const
{
	return m_instance;
}

void KGR::_Vulkan::_Instance::Clear()
{
	m_instance.clear();
}

// STRUCT PHYSICAL DEVICE
KGR::_Vulkan::_PhysicalDevice::_PhysicalDevice(_Instance* instance)
{
	auto devices = instance->GetInstance().enumeratePhysicalDevices();
	if (devices.empty())
		throw std::runtime_error("Unable to fetch physical device");

	PhysicalDevice* selectedDevice = nullptr;

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

	m_device = PhysicalDevice(std::move(*selectedDevice));

	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_device.getQueueFamilyProperties();
	auto graphicsQueueFamilyProperty = std::find_if(
		queueFamilyProperties.begin(),
		queueFamilyProperties.end(),
		[](vk::QueueFamilyProperties const& qfp) { return qfp.queueFlags & vk::QueueFlagBits::eGraphics; }
	);
	m_queueIndex = static_cast<uint32_t>(std::distance(queueFamilyProperties.begin(), graphicsQueueFamilyProperty));
}

PhysicalDevice& KGR::_Vulkan::_PhysicalDevice::GetDevice()
{
	return m_device;
}

const PhysicalDevice& KGR::_Vulkan::_PhysicalDevice::GetDevice() const
{
	return m_device;
}

void KGR::_Vulkan::_PhysicalDevice::Clear()
{
	m_device.clear();
}

ui32t KGR::_Vulkan::_PhysicalDevice::GraphicsQueueIndex() const
{
	return m_queueIndex;
}

// STRUCT SURFACE
KGR::_Vulkan::_Surface::_Surface(_Instance* instance, _GLFW::Window* window)
{
	VkSurfaceKHR _surface;
	if (glfwCreateWindowSurface(*instance->GetInstance(), &window->GetWindow(), nullptr, &_surface) != VK_SUCCESS)
		throw std::runtime_error("Failed to create GLFW window surface");

	m_surface = SurfaceKHR(instance->GetInstance(), _surface);
}

SurfaceKHR& KGR::_Vulkan::_Surface::GetSurface()
{
	return m_surface;
}

const SurfaceKHR& KGR::_Vulkan::_Surface::GetSurface() const
{
	return m_surface;
}

void KGR::_Vulkan::_Surface::Clear()
{
	m_surface.clear();
}

// STRUCT DEVICE
KGR::_Vulkan::_Device::_Device(_PhysicalDevice* device, ui32t count)
{
	float queuePriority = 0.0f;
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device->GetDevice().getQueueFamilyProperties();
	auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo
	{
		.queueFamilyIndex = device->GraphicsQueueIndex(),
		.queueCount = count,
		.pQueuePriorities = &queuePriority
	};

	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features> featureChain = {
		{.features = {.samplerAnisotropy = true } },
		{.bufferDeviceAddress = true },
		{.synchronization2 = true, .dynamicRendering = true },
	};

	std::vector<const char*> deviceExtensions =
	{
		vk::KHRSwapchainExtensionName,
		vk::KHRSpirv14ExtensionName,
		vk::KHRSynchronization2ExtensionName
	};

	vk::DeviceCreateInfo deviceCreateInfo
	{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data()
	};

	m_device = Device(device->GetDevice(), deviceCreateInfo);
}

Device& KGR::_Vulkan::_Device::GetDevice()
{
	return m_device;
}

const Device& KGR::_Vulkan::_Device::GetDevice() const
{
	return m_device;
}

void KGR::_Vulkan::_Device::Clear()
{
	m_device.clear();
}

void KGR::_Vulkan::_Device::WaitIdle()
{
	m_device.waitIdle();
}

// STRUCT QUEUE
KGR::_Vulkan::_Queue::_Queue(_Device* device, _PhysicalDevice* pDevice, ui32t index)
{
	m_queue = Queue(device->GetDevice(), pDevice->GraphicsQueueIndex(), index);
}

Queue& KGR::_Vulkan::_Queue::GetQueue()
{
	return m_queue;
}

const Queue& KGR::_Vulkan::_Queue::GetQueue() const
{
	return m_queue;
}

void KGR::_Vulkan::_Queue::Clear()
{
	m_queue.clear();
}


// STRUCT SWAPCHAIN
KGR::_Vulkan::_Swapchain::_Swapchain(_PhysicalDevice* pDevice,
	_Device* device,
	_Surface* surface,
	_GLFW::Window* window,
	ui32t imageCount,
	_Swapchain* old)
{
	auto surfaceCaps = pDevice->GetDevice().getSurfaceCapabilitiesKHR(surface->GetSurface());
	auto availableFormats = pDevice->GetDevice().getSurfaceFormatsKHR(surface->GetSurface());
	auto availablePresentModes = pDevice->GetDevice().getSurfacePresentModesKHR(surface->GetSurface());

	auto swapSurfaceFormat = ([](const decltype(availableFormats)& formats)
		{
			for (const auto& availableFormat : formats)
				if (availableFormat.format == vk::Format::eB8G8R8A8Srgb && availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
					return availableFormat;

			return formats[0];
		})(availableFormats);

	auto swapExtent = ([&window](const vk::SurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(&window->GetWindow(), &width, &height);

		return vk::Extent2D
		{
			std::clamp<uint32_t>(width,  capabilities.minImageExtent.width,  capabilities.maxImageExtent.width),
			std::clamp<uint32_t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
		})(surfaceCaps);

	auto swapPresentMode = ([](const decltype(availablePresentModes)& modes)
		{
			for (const auto& availablePresentMode : modes)
				if (availablePresentMode == vk::PresentModeKHR::eFifoRelaxed)
					return availablePresentMode;

			return vk::PresentModeKHR::eFifo;
		})(availablePresentModes);

	auto minImageCount = std::clamp(imageCount, surfaceCaps.minImageCount, surfaceCaps.maxImageCount);

	auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR
	{
		.flags = vk::SwapchainCreateFlagsKHR(0),
		.surface = surface->GetSurface(),
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
		.oldSwapchain = old == nullptr ? nullptr : *old->GetSwapchain()
	};

	m_chain = SwapchainKHR(device->GetDevice(), swapchainCreateInfo);
}

void KGR::_Vulkan::_Swapchain::Clear()
{
	m_chain.clear();
}

SwapchainKHR& KGR::_Vulkan::_Swapchain::GetSwapchain()
{
	return m_chain;
}

const SwapchainKHR& KGR::_Vulkan::_Swapchain::GetSwapchain() const
{
	return m_chain;
}

// STRUCT VKIMAGES
KGR::_Vulkan::_VkImages::_VkImages(_Swapchain* swapchain)
{
	m_images = swapchain->GetSwapchain().getImages();
}

std::vector<vk::Image>& KGR::_Vulkan::_VkImages::GetImages()
{
	return m_images;
}

const std::vector<vk::Image>& KGR::_Vulkan::_VkImages::GetImages() const
{
	return m_images;
}

void KGR::_Vulkan::_VkImages::Clear()
{
	m_images.clear();
}

// STRUCT COMMAND POOL
KGR::_Vulkan::_CommandPool::_CommandPool(_PhysicalDevice* pDevice, _Device* device)
{
	auto poolInfo = vk::CommandPoolCreateInfo
	{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = pDevice->GraphicsQueueIndex()
	};

	m_pool = CommandPool(device->GetDevice(), poolInfo);
}

CommandPool& KGR::_Vulkan::_CommandPool::GetPool()
{
	return m_pool;
}

const CommandPool& KGR::_Vulkan::_CommandPool::GetPool() const
{
	return m_pool;
}

void KGR::_Vulkan::_CommandPool::Clear()
{
	m_pool.clear();
}

// STRUCT COMMAND BUFFER
KGR::_Vulkan::_CommandBuffer::_CommandBuffer(_Device* device, _CommandPool* pool)
{
	vk::CommandBufferAllocateInfo allocInfo
	{
		.commandPool = pool->GetPool(),
		.level = vk::CommandBufferLevel::ePrimary,
		.commandBufferCount = 1
	};

	m_buffer = std::move(CommandBuffers(device->GetDevice(), allocInfo).front());
}

CommandBuffer& KGR::_Vulkan::_CommandBuffer::GetBuffer()
{
	return m_buffer;
}

const CommandBuffer& KGR::_Vulkan::_CommandBuffer::GetBuffer() const
{
	return m_buffer;
}

void KGR::_Vulkan::_CommandBuffer::Clear()
{
	m_buffer.clear();
}

// STRUCT SEMAPHORE
KGR::_Vulkan::_Semaphore::_Semaphore(_Device* device)
{
	m_semaphore = Semaphore(device->GetDevice(), vk::SemaphoreCreateInfo());
}

Semaphore& KGR::_Vulkan::_Semaphore::GetSemaphore()
{
	return m_semaphore;
}

const Semaphore& KGR::_Vulkan::_Semaphore::GetSemaphore() const
{
	return m_semaphore;
}

void KGR::_Vulkan::_Semaphore::Clear()
{
	m_semaphore.clear();
}

// CLASS VULKAN
KGR::TMP_Vulkan::TMP_Vulkan()
	: m_graphicsQueueFamily(0)
	, m_currentFrame(0)
	, m_currentImageIndex(0)
	, m_glfwExtensionCount(0)
	, m_glfwExtensions(nullptr)
{
}

void KGR::TMP_Vulkan::Init(_GLFW::Window* window)
{
	InitInstance();
	CreatePhysicalDevice();
	CreateSurface(window);
	CreateDevice();
	CreateSwapchain(window);
	CreateCommandResources();
	CreateObjects();
}

void KGR::TMP_Vulkan::InitInstance()
{
	m_appInfo = 
	{
		.pApplicationName = "GCVulkan",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "None",
		.engineVersion = VK_MAKE_VERSION(0, 1, 0),
		.apiVersion = vk::ApiVersion14
	};

	m_requiredLayers.assign(m_validationLayers.begin(), m_validationLayers.end());

	CheckLayerProperties();
	GetExtensions();
	AddExtensions();
	VerifyExtensions();

	m_instanceCreateInfo =
	{
		.pApplicationInfo = &m_appInfo,
		.enabledLayerCount = static_cast<uint32_t>(m_requiredLayers.size()),
		.ppEnabledLayerNames = m_requiredLayers.data(),
		.enabledExtensionCount = static_cast<uint32_t>(m_activeExtensions.size()),
		.ppEnabledExtensionNames = m_activeExtensions.data()
	};

	m_instance = std::make_unique<Instance>(m_vkContext, m_instanceCreateInfo);
}

void KGR::TMP_Vulkan::CreatePhysicalDevice()
{
	m_devices = m_instance->enumeratePhysicalDevices();
	if (m_devices.empty())
		throw std::runtime_error("No physical devices found");

	PhysicalDevice* selectedDevice = nullptr;
	
	for (auto& d : m_devices)
	{
		auto dProperties = d.getProperties();

		if (dProperties.apiVersion < VK_API_VERSION_1_4)
			continue;

		if (dProperties.deviceType == GetGPU())
		{
			selectedDevice = &d;
			break;
		}

		if (!selectedDevice)
			selectedDevice = &d;
	}

	if (!selectedDevice)
		throw std::runtime_error("No suitable phyisical device found");

	m_physicalDevice = std::make_unique<PhysicalDevice>(std::move(*selectedDevice));
}

void KGR::TMP_Vulkan::CreateSurface(_GLFW::Window* window)
{
	VkSurfaceKHR tmpSurface;
	if (glfwCreateWindowSurface(**m_instance, &window->GetWindow(), nullptr, &tmpSurface)
		!= VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface");

	m_surface = std::make_unique<SurfaceKHR>(*m_instance, tmpSurface);
}

void KGR::TMP_Vulkan::CreateDevice()
{
	m_graphicsQueueFamily = FindGraphicsQueueFamily();

	float queuePriority = 0.0f;
	auto deviceQueueCreateInfo = vk::DeviceQueueCreateInfo
	{
		.queueFamilyIndex = m_graphicsQueueFamily,
		.queueCount = 1,
		.pQueuePriorities = &queuePriority
	};

	vk::StructureChain<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan12Features, vk::PhysicalDeviceVulkan13Features> featureChain = 
	{
		{.features = {.samplerAnisotropy = true } },
		{.bufferDeviceAddress = true },
		{.synchronization2 = true, .dynamicRendering = true },
	};

	std::vector<const char*> deviceExtensions = 
	{
		vk::KHRSwapchainExtensionName,
		vk::KHRSpirv14ExtensionName,
		vk::KHRSynchronization2ExtensionName
	};

	vk::DeviceCreateInfo deviceCreateInfo
	{
		.pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
		.queueCreateInfoCount = 1,
		.pQueueCreateInfos = &deviceQueueCreateInfo,
		.enabledExtensionCount = static_cast<ui32t>(deviceExtensions.size()),
		.ppEnabledExtensionNames = deviceExtensions.data()
	};

	m_device = std::make_unique<Device>(*m_physicalDevice, deviceCreateInfo);
	m_graphicsQueue = std::make_unique<Queue>(*m_device, m_graphicsQueueFamily, 0);
}

void KGR::TMP_Vulkan::CreateSwapchain(_GLFW::Window* window)
{
	auto surfaceCaps = m_physicalDevice->getSurfaceCapabilitiesKHR(**m_surface);
	auto availableFormats = m_physicalDevice->getSurfaceFormatsKHR(**m_surface);
	auto availablePresentModes = m_physicalDevice->getSurfacePresentModesKHR(**m_surface);

	auto swapSurfaceFormat = ([](const decltype(availableFormats)& formats) 
		{
		for (const auto& availableFormat : formats)
			if (availableFormat.format == vk::Format::eB8G8R8A8Srgb &&
				availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
				return availableFormat;
		return formats[0];
		})(availableFormats);

	auto swapExtent = ([&window](const vk::SurfaceCapabilitiesKHR& capabilities) 
		{
		if (capabilities.currentExtent.width != std::numeric_limits<ui32t>::max())
			return capabilities.currentExtent;

		int width, height;
		glfwGetFramebufferSize(&window->GetWindow(), &width, &height);

		return vk::Extent2D
		{
			std::clamp<ui32t>(width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width),
			std::clamp<ui32t>(height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height)
		};
		})(surfaceCaps);

	auto swapPresentMode = ([](const decltype(availablePresentModes)& modes) 
		{
		for (const auto& availablePresentMode : modes)
			if (availablePresentMode == vk::PresentModeKHR::eFifoRelaxed)
				return availablePresentMode;
		return vk::PresentModeKHR::eFifo;
		})(availablePresentModes);

	auto minImageCount = std::clamp(3u, surfaceCaps.minImageCount, surfaceCaps.maxImageCount);

	auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR
	{
		.flags = vk::SwapchainCreateFlagsKHR(0),
		.surface = **m_surface,
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

	m_swapchain = std::make_unique<SwapchainKHR>(*m_device, swapchainCreateInfo);
	m_scImages = m_swapchain->getImages();
}

void KGR::TMP_Vulkan::CreateCommandResources()
{
	auto poolInfo = vk::CommandPoolCreateInfo
	{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = m_graphicsQueueFamily
	};

	m_commandPool = std::make_unique<CommandPool>(*m_device, poolInfo);
}

void KGR::TMP_Vulkan::CreateObjects()
{
	m_frameData.clear();

	for (size_t i = 0; i < m_scImages.size(); ++i)
	{
		KGR::_Vulkan::_FrameData frame;
		frame.presentCompleteSemaphore = Semaphore(*m_device, vk::SemaphoreCreateInfo());
		frame.renderFinishedSemaphore = Semaphore(*m_device, vk::SemaphoreCreateInfo());
		frame.perFrameFence = Fence(*m_device, vk::FenceCreateInfo
			{
			.flags = vk::FenceCreateFlagBits::eSignaled
			});

		m_frameData.push_back(std::move(frame));
	}

	for (auto& fd : m_frameData)
	{
		vk::CommandBufferAllocateInfo allocInfo
		{
			.commandPool = **m_commandPool,
			.level = vk::CommandBufferLevel::ePrimary,
			.commandBufferCount = 1
		};

		fd.commandBuffer = std::move(CommandBuffers(*m_device, allocInfo).front());
	}
}

void KGR::TMP_Vulkan::TransitionToTransferDst(CommandBuffer& cb, vk::Image& image)
{
	TransitionImage
	(
		cb,
		image,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferDstOptimal,
		vk::PipelineStageFlagBits2::eTopOfPipe,
		vk::AccessFlagBits2::eNone,
		vk::PipelineStageFlagBits2::eTransfer,
		vk::AccessFlagBits2::eTransferWrite,
		vk::ImageAspectFlagBits::eColor,
		0, 1, 0, 1
	);
}

void KGR::TMP_Vulkan::TransitionToPresent(CommandBuffer& cb, vk::Image& image)
{
	TransitionImage
	(
		cb,
		image,
		vk::ImageLayout::eTransferDstOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::PipelineStageFlagBits2::eTransfer,
		vk::AccessFlagBits2::eTransferWrite,
		vk::PipelineStageFlagBits2::eBottomOfPipe,
		vk::AccessFlagBits2::eNone,
		vk::ImageAspectFlagBits::eColor,
		0, 1, 0, 1
	);
}

i32t KGR::TMP_Vulkan::AcquireNextImage(ui32t frameIndex)
{
	vk::Result waitResult = m_device->waitForFences
	(
		*m_frameData[frameIndex].perFrameFence,
		true,
		UINT64_MAX
	);

	if (waitResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed to wait for fence");
	}

	m_device->resetFences(*m_frameData[frameIndex].perFrameFence);

	auto [result, imageIndex] = m_swapchain->acquireNextImage
	(
		UINT64_MAX,
		*m_frameData[frameIndex].presentCompleteSemaphore,
		nullptr
	);

	if (result == vk::Result::eErrorOutOfDateKHR)
	{
		throw std::runtime_error("Swapchain out of date - recreation needed");
	}
	else if (result != vk::Result::eSuccess && result != vk::Result::eSuboptimalKHR)
	{
		throw std::runtime_error("Failed to acquire swapchain image");
	}

	m_currentImageIndex = imageIndex;
	return imageIndex;
}

void KGR::TMP_Vulkan::SubmitCommands(ui32t frameIndex)
{
	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	const auto submitInfo = vk::SubmitInfo
	{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*m_frameData[frameIndex].presentCompleteSemaphore,
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*m_frameData[frameIndex].commandBuffer,
		.signalSemaphoreCount = 1,
		.pSignalSemaphores = &*m_frameData[frameIndex].renderFinishedSemaphore,
	};

	m_graphicsQueue->submit(submitInfo, *m_frameData[frameIndex].perFrameFence);
}

void KGR::TMP_Vulkan::Present(ui32t frameIndex, ui32t imageIndex)
{
	const auto presentInfo = vk::PresentInfoKHR
	{
	   .waitSemaphoreCount = 1,
	   .pWaitSemaphores = &*m_frameData[frameIndex].renderFinishedSemaphore,
	   .swapchainCount = 1,
	   .pSwapchains = &**m_swapchain,
	   .pImageIndices = &imageIndex
	};

	if (m_graphicsQueue->presentKHR(presentInfo) != vk::Result::eSuccess)
		throw std::runtime_error("Failed to present");
}

void KGR::TMP_Vulkan::WaitIdle()
{
	m_device->waitIdle();
}

void KGR::TMP_Vulkan::Cleanup()
{
	m_frameData.clear();
	m_commandPool.reset();
	m_scImages.clear();
	m_swapchain.reset();
	m_graphicsQueue.reset();
	m_device.reset();
	m_surface.reset();
	m_physicalDevice.reset();
	m_instance.reset();
}

Instance& KGR::TMP_Vulkan::GetInstance()
{
	return *m_instance;
}

Device& KGR::TMP_Vulkan::GetDevice()
{
	return *m_device;
}

CommandBuffer& KGR::TMP_Vulkan::GetCommandBuffer(ui32t frameIndex)
{
	return m_frameData[frameIndex].commandBuffer;
}

std::vector<vk::Image>& KGR::TMP_Vulkan::GetSCImages()
{
	return m_scImages;
}

ui32t KGR::TMP_Vulkan::GetFrameCount() const
{
	return static_cast<ui32t>(m_frameData.size());
}

ui32t KGR::TMP_Vulkan::GetCurrentImageIndex() const
{
	return m_currentImageIndex;
}

vk::Image& KGR::TMP_Vulkan::GetCurrentImage()
{
	return m_scImages[m_currentImageIndex];
}

void KGR::TMP_Vulkan::CheckLayerProperties()
{
	const auto layerProperties = m_vkContext.enumerateInstanceLayerProperties();
	if (std::ranges::any_of(m_requiredLayers, [&layerProperties](auto const& requiredLayer)
		{
			return std::ranges::none_of(layerProperties, [requiredLayer](auto const& layerProperty)
				{
					return strcmp(layerProperty.layerName, requiredLayer) == 0;
				});
		}))
		throw std::runtime_error("One or more required layers are not supported !");
}

void KGR::TMP_Vulkan::GetExtensions()
{
	m_glfwExtensions = glfwGetRequiredInstanceExtensions(&m_glfwExtensionCount);
	m_extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();
}

void KGR::TMP_Vulkan::AddExtensions()
{
	for (ui32t i = 0; i < m_glfwExtensionCount; ++i)
		m_activeExtensions.push_back(m_glfwExtensions[i]);
}

void KGR::TMP_Vulkan::VerifyExtensions()
{
	for (ui32t i = 0; i < m_glfwExtensionCount; ++i)
		if (std::ranges::none_of(m_extensionProperties, [glfwExtension = m_glfwExtensions[i]](auto const& extensionProperty)
			{
				return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
			}))
			throw std::runtime_error("Required GLFW extensions not supported");
}

ui32t KGR::TMP_Vulkan::FindGraphicsQueueFamily()
{
	std::vector<vk::QueueFamilyProperties> queueFamilyProperties = m_physicalDevice->getQueueFamilyProperties();
	auto graphicsQueueFamilyProperty = std::find_if
	(
		queueFamilyProperties.begin(),
		queueFamilyProperties.end(),
		[](vk::QueueFamilyProperties const& qfp) 
		{
			return qfp.queueFlags & vk::QueueFlagBits::eGraphics;
		}
	);

	return static_cast<ui32t>(std::distance(queueFamilyProperties.begin(),
								graphicsQueueFamilyProperty));
}

void KGR::TMP_Vulkan::TransitionImage(
	CommandBuffer& cb,
	vk::Image& image,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	vk::PipelineStageFlags2 srcStage,
	vk::AccessFlags2 srcAccess,
	vk::PipelineStageFlags2 dstStage,
	vk::AccessFlags2 dstAccess,
	vk::ImageAspectFlags aspectMask,
	ui32t baseMipLevel,
	ui32t levelCount,
	ui32t baseArrayLayer,
	ui32t layerCount)
{
	vk::ImageMemoryBarrier2 barrier =
	{
		.srcStageMask = srcStage,        // Wait for this pipeline stage to finish
		.srcAccessMask = srcAccess,      // Flush these memory operations
		.dstStageMask = dstStage,        // Block this pipeline stage until ready
		.dstAccessMask = dstAccess,      // Prepare for these memory operations
		.oldLayout = oldLayout,          // Current image layout
		.newLayout = newLayout,          // Target image layout
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,  // No queue transfer
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange =
		{
			.aspectMask = aspectMask,         // Which part: color, depth, or stencil
			.baseMipLevel = baseMipLevel,     // Starting mip level
			.levelCount = levelCount,         // Number of mip levels
			.baseArrayLayer = baseArrayLayer, // Starting array layer
			.layerCount = layerCount          // Number of array layers
		}
	};

	vk::DependencyInfo dependencyInfo =
	{
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier
	};

	cb.pipelineBarrier2(dependencyInfo);
}

vk::PhysicalDeviceType KGR::TMP_Vulkan::GetGPU()
{
	return vk::PhysicalDeviceType::eDiscreteGpu;
}

CommandBuffer& KGR::TMP_Vulkan::Begin()
{
	AcquireNextImage(m_currentFrame);

	auto& cb = m_frameData[m_currentFrame].commandBuffer;
	cb.(vk::CommandBufferBeginInfo{});

	return cb;
}

void KGR::TMP_Vulkan::End()
{
	m_frameData[m_currentFrame].commandBuffer.end();
	SubmitCommands(m_currentFrame);
	Present(m_currentFrame, m_currentImageIndex);
	++m_currentFrame %= m_frameData.size();
}