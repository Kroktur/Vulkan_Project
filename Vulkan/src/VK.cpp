#include "VKFiles.h"
#include "VK.h"

KGR::_Vulkan::_Vulkan()
	: m_graphicsQueueFamily(0)
	, m_currentFrame(0)
	, m_currentImageIndex(0)
	, m_glfwExtensionCount(0)
	, m_glfwExtensions(nullptr)
{
}

void KGR::_Vulkan::Init(_GLFW::Window* window)
{
	InitInstance();
	CreatePhysicalDevice();
	CreateSurface(window);
	CreateDevice();
	CreateSwapchain(window);
	CreateCommandResources();
	CreateObjects();
}

void KGR::_Vulkan::InitInstance()
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

void KGR::_Vulkan::CreatePhysicalDevice()
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

void KGR::_Vulkan::CreateSurface(_GLFW::Window* window)
{
	VkSurfaceKHR tmpSurface;
	if (glfwCreateWindowSurface(**m_instance, &window->GetWindow(), nullptr, &tmpSurface)
		!= VK_SUCCESS)
		throw std::runtime_error("Failed to create window surface");

	m_surface = std::make_unique<SurfaceKHR>(*m_instance, tmpSurface);
}

void KGR::_Vulkan::CreateDevice()
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

void KGR::_Vulkan::CreateSwapchain(_GLFW::Window* window)
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

void KGR::_Vulkan::CreateCommandResources()
{
	auto poolInfo = vk::CommandPoolCreateInfo
	{
		.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		.queueFamilyIndex = m_graphicsQueueFamily
	};

	m_commandPool = std::make_unique<CommandPool>(*m_device, poolInfo);
}

void KGR::_Vulkan::CreateObjects()
{
	m_frameData.clear();

	for (size_t i = 0; i < m_scImages.size(); ++i)
	{
		FrameData frame;
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

void KGR::_Vulkan::TransitionToTransferDst(CommandBuffer& cb, vk::Image& image)
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

void KGR::_Vulkan::TransitionToPresent(CommandBuffer& cb, vk::Image& image)
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

ui32t KGR::_Vulkan::AcquireNextImage(ui32t frameIndex)
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

void KGR::_Vulkan::SubmitCommands(ui32t frameIndex)
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

void KGR::_Vulkan::Present(ui32t frameIndex, ui32t imageIndex)
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

void KGR::_Vulkan::WaitIdle()
{
	m_device->waitIdle();
}

void KGR::_Vulkan::Cleanup()
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

Instance& KGR::_Vulkan::GetInstance()
{
	return *m_instance;
}

Device& KGR::_Vulkan::GetDevice()
{
	return *m_device;
}

CommandBuffer& KGR::_Vulkan::GetCommandBuffer(ui32t frameIndex)
{
	return m_frameData[frameIndex].commandBuffer;
}

std::vector<vk::Image>& KGR::_Vulkan::GetSCImages()
{
	return m_scImages;
}

ui32t KGR::_Vulkan::GetFrameCount() const
{
	return static_cast<ui32t>(m_frameData.size());
}

ui32t KGR::_Vulkan::GetCurrentImageIndex() const
{
	return m_currentImageIndex;
}

vk::Image& KGR::_Vulkan::GetCurrentImage()
{
	return m_scImages[m_currentImageIndex];
}

void KGR::_Vulkan::CheckLayerProperties()
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

void KGR::_Vulkan::GetExtensions()
{
	m_glfwExtensions = glfwGetRequiredInstanceExtensions(&m_glfwExtensionCount);
	m_extensionProperties = m_vkContext.enumerateInstanceExtensionProperties();
}

void KGR::_Vulkan::AddExtensions()
{
	for (ui32t i = 0; i < m_glfwExtensionCount; ++i)
		m_activeExtensions.push_back(m_glfwExtensions[i]);
}

void KGR::_Vulkan::VerifyExtensions()
{
	for (ui32t i = 0; i < m_glfwExtensionCount; ++i)
		if (std::ranges::none_of(m_extensionProperties, [glfwExtension = m_glfwExtensions[i]](auto const& extensionProperty)
			{
				return strcmp(extensionProperty.extensionName, glfwExtension) == 0;
			}))
			throw std::runtime_error("Required GLFW extensions not supported");
}

ui32t KGR::_Vulkan::FindGraphicsQueueFamily()
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

void KGR::_Vulkan::TransitionImage(
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

vk::PhysicalDeviceType KGR::_Vulkan::GetGPU()
{
	return vk::PhysicalDeviceType::eDiscreteGpu;
}

CommandBuffer& KGR::_Vulkan::Begin()
{
	AcquireNextImage(m_currentFrame);

	auto& cb = m_frameData[m_currentFrame].commandBuffer;
	cb.begin(vk::CommandBufferBeginInfo{});

	return cb;
}

void KGR::_Vulkan::End()
{
	m_frameData[m_currentFrame].commandBuffer.end();
	SubmitCommands(m_currentFrame);
	Present(m_currentFrame, m_currentImageIndex);
	++m_currentFrame %= m_frameData.size();
}