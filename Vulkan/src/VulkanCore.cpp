#include "VulkanCore.h"

#include "Core/ManagerImple.h"
#include <algorithm>
#include <array>
#include <assert.h>


#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>

KGR::_Vulkan::VulkanCore::VulkanCore(GLFWwindow* window_) : window(window_)
{
}

void KGR::_Vulkan::VulkanCore::initVulkan()
{
	// good Api
	

	// instance creation
	instance = _Vulkan::Instance(AppInfo{},validationLayers);
	instance.setupDebugMessenger<&debugCallback>();
	//surface 
	surface = _Vulkan::Surface(&instance, window);
	// phDevice
	physicalDevice = _Vulkan::PhysicalDevice(&instance, PhysicalDevice::DeviceType::Best);
	// device
	device = _Vulkan::Device(&physicalDevice, &surface);
	// Queue
	queue = _Vulkan::Queue(&device);
	// swapChain
	swapChain = _Vulkan::SwapChain(&physicalDevice, &device, &surface, window, SwapChain::PresMode::Mailbox, 3,nullptr);
	// images view
	swapChainImageViews = _Vulkan::ImagesViews(&swapChain, &device, ImagesViews::ViewType::vt2D);
	// Pipeline
	_Vulkan::ShaderInfo info{
		.ShaderPath = "Shaders/slang.spv",
		.vertexMain = "vertMain",
		.fragmentMain = "fragMain"
	};
	// Layouts 
	std::vector<vk::DescriptorSetLayoutBinding> bindings = {
			vk::DescriptorSetLayoutBinding(0, vk::DescriptorType::eUniformBuffer, 1, vk::ShaderStageFlagBits::eVertex, nullptr),
			vk::DescriptorSetLayoutBinding(1, vk::DescriptorType::eCombinedImageSampler, 1, vk::ShaderStageFlagBits::eFragment, nullptr) };

	auto layout = DescriptorLayout(bindings, &device);
	descriptorSetLayout.Add(std::move(layout));

	graphicsPipeline = _Vulkan::Pipeline(info, &device, &swapChain,&descriptorSetLayout,&physicalDevice);
	// Command Buffer
	commandBuffers = _Vulkan::CommandBuffers(&device);
	// vertex
	size_t vertSize = vertices2.size() * sizeof(vertices2[0]);
	auto vertexTmp = _Vulkan::Buffer(&device, &physicalDevice, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, vertSize);
	vertexTmp.MapMemory(vertSize);
	vertexTmp.Upload(vertices2);
	vertexTmp.UnMapMemory();
	vertexBuffer = _Vulkan::Buffer(&device, &physicalDevice, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, vertSize);
	vertexBuffer.Copy(&vertexTmp, &device, &queue, &commandBuffers);
	// index
	size_t indexSize = indices.size() * sizeof(indices[0]);
	auto indexTmp = _Vulkan::Buffer(&device, &physicalDevice, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, indexSize);
	indexTmp.MapMemory(indexSize);
	indexTmp.Upload(indices);
	indexTmp.UnMapMemory();
	indexBuffer = _Vulkan::Buffer(&device, &physicalDevice, vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, vk::MemoryPropertyFlagBits::eDeviceLocal, indexSize);
	indexBuffer.Copy(&indexTmp, &device, &queue, &commandBuffers);	
	//

	// SyncObject
	syncObject = SyncObject(&device, swapChain.GetImagesCount());
	createDepthResources();
	createTextureImage();
	createTextureImageView();
	createTextureSampler();
	// UniformBuffer
	for (size_t i = 0; i < swapChain.GetImagesCount(); i++) {
		vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
		auto buffer = Buffer(&device, &physicalDevice, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, bufferSize);
		buffer.MapMemory(bufferSize);
		uniformBuffers.emplace_back(std::move(buffer));
	}
	// descriptorPool
	std::vector<vk::DescriptorPoolSize> poolSize{
				vk::DescriptorPoolSize(vk::DescriptorType::eUniformBuffer, swapChain.GetImagesCount()),
				vk::DescriptorPoolSize(vk::DescriptorType::eCombinedImageSampler, swapChain.GetImagesCount()) };
	descriptorPool = DescriptorPool(poolSize, 100, &device);
	
	// DescriptorSet
	descriptorSets = DescriptorSet::Create(&device, &descriptorPool, &descriptorSetLayout.Get(0), swapChain.GetImagesCount());
	for (size_t i = 0; i < swapChain.GetImagesCount(); i++)
	{
		vk::DescriptorBufferInfo bufferInfo{
			.buffer = uniformBuffers[i].Get(),
			.offset = 0,
			.range = sizeof(UniformBufferObject) };
		vk::DescriptorImageInfo imageInfo{
			.sampler = textureSampler,
			.imageView = textureImageView,
			.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal };
		std::array descriptorWrites{
			vk::WriteDescriptorSet{
				.dstSet = descriptorSets[i].Get(),
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eUniformBuffer,
				.pBufferInfo = &bufferInfo},
			vk::WriteDescriptorSet{
				.dstSet = descriptorSets[i].Get(),
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorCount = 1,
				.descriptorType = vk::DescriptorType::eCombinedImageSampler,
				.pImageInfo = &imageInfo} };
		device.Get().updateDescriptorSets(descriptorWrites, {});
	}

}

void KGR::_Vulkan::VulkanCore::mainLoop()
{
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();
		drawFrame();
	}

	device.Get().waitIdle();
}



void KGR::_Vulkan::VulkanCore::recreateSwapChain()
{
	int width = 0, height = 0;
	glfwGetFramebufferSize(window, &width, &height);
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(window, &width, &height);
		glfwWaitEvents();
	}
	
	device.Get().waitIdle();
	swapChain = _Vulkan::SwapChain(&physicalDevice, &device, &surface, window,SwapChain::PresMode::Mailbox ,3, &swapChain);
	swapChainImageViews = _Vulkan::ImagesViews(&swapChain, &device, ImagesViews::ViewType::vt2D);
	_Vulkan::ShaderInfo info{
		.ShaderPath = "Shaders/slang.spv",
		.vertexMain = "vertMain",
		.fragmentMain = "fragMain"
	};
	graphicsPipeline = _Vulkan::Pipeline(info, &device, &swapChain,&descriptorSetLayout,&physicalDevice);
	createDepthResources();
}





void KGR::_Vulkan::VulkanCore::recordCommandBuffer(uint32_t imageIndex, vk::raii::CommandBuffer& buffer)
{


	//
	auto& commandBuffer = buffer;
	commandBuffer.begin({});
	// Before starting rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL
	transition_image_layout(
		swapChain.GetImages()[imageIndex],
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},                                                        // srcAccessMask (no need to wait for previous operations)
		vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // dstStage
		vk::ImageAspectFlagBits::eColor,commandBuffer);
	// Transition depth image to depth attachment optimal layout
	transition_image_layout(
		*depthImage,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eDepthAttachmentOptimal,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::AccessFlagBits2::eDepthStencilAttachmentWrite,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::PipelineStageFlagBits2::eEarlyFragmentTests | vk::PipelineStageFlagBits2::eLateFragmentTests,
		vk::ImageAspectFlagBits::eDepth,commandBuffer);

	vk::ClearValue clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::ClearValue clearDepth = vk::ClearDepthStencilValue(1.0f, 0);

	vk::RenderingAttachmentInfo colorAttachmentInfo = {
		.imageView = swapChainImageViews.Get()[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor };

	vk::RenderingAttachmentInfo depthAttachmentInfo = {
		.imageView = depthImageView,
		.imageLayout = vk::ImageLayout::eDepthAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eDontCare,
		.clearValue = clearDepth };

	vk::RenderingInfo renderingInfo = {
		.renderArea = {.offset = {0, 0}, .extent = swapChain.GetExtend()},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &colorAttachmentInfo,
		.pDepthAttachment = &depthAttachmentInfo };
	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline.Get());
	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChain.GetExtend().width), static_cast<float>(swapChain.GetExtend().height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChain.GetExtend()));
	commandBuffer.bindVertexBuffers(0, *vertexBuffer.Get(), { 0 });
	commandBuffer.bindIndexBuffer(*indexBuffer.Get(), 0, vk::IndexType::eUint16);
	commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, graphicsPipeline.GetLayout(), 0, *descriptorSets[syncObject.GetCurrentFrame()].Get(), nullptr);
	commandBuffer.drawIndexed(indices.size(), 1, 0, 0, 0);
	commandBuffer.endRendering();
	// After rendering, transition the swapchain image to PRESENT_SRC
	transition_image_layout(
		swapChain.GetImages()[imageIndex],
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,                // srcAccessMask
		{},                                                        // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
		vk::PipelineStageFlagBits2::eBottomOfPipe,                 // dstStage
		vk::ImageAspectFlagBits::eColor,commandBuffer);
	commandBuffer.end();



}

void KGR::_Vulkan::VulkanCore::transition_image_layout(vk::Image image, vk::ImageLayout old_layout,
                                                                     vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask,
                                                                     vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask, vk::ImageAspectFlags image_aspect_flags, vk::raii::CommandBuffer& buffer)
{
	vk::ImageMemoryBarrier2 barrier = {
			.srcStageMask = src_stage_mask,
			.srcAccessMask = src_access_mask,
			.dstStageMask = dst_stage_mask,
			.dstAccessMask = dst_access_mask,
			.oldLayout = old_layout,
			.newLayout = new_layout,
			.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
			.image = image,
			.subresourceRange = {
				   .aspectMask = image_aspect_flags,
				   .baseMipLevel = 0,
				   .levelCount = 1,
				   .baseArrayLayer = 0,
				   .layerCount = 1} };
	vk::DependencyInfo dependency_info = {
		.dependencyFlags = {},
		.imageMemoryBarrierCount = 1,
		.pImageMemoryBarriers = &barrier };

	buffer.pipelineBarrier2(dependency_info);
}





std::uint32_t KGR::_Vulkan::VulkanCore::PresentImage()
{
	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*syncObject.GetCurrentRenderSemaphore());

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vkSemaphore;
	presentInfo.swapchainCount = 1;

	VkSwapchainKHR swapchainC = static_cast<VkSwapchainKHR>(*swapChain.Get());
	presentInfo.pSwapchains = &swapchainC;
	presentInfo.pImageIndices = &syncObject.GetCurrentImage();
	presentInfo.pResults = nullptr;

	VkQueue queueC = static_cast<VkQueue>(*queue.Get());
	VkResult result = vkQueuePresentKHR(queueC, &presentInfo);
	return result;
}

void KGR::_Vulkan::VulkanCore::drawFrame()
{
	// Note: inFlightFences, presentCompleteSemaphores, and commandBuffers are indexed by frameIndex,
	//       while renderFinishedSemaphores is indexed by imageIndex
	auto fenceResult = device.Get().waitForFences(*syncObject.GetCurrentFence(), vk::True, UINT64_MAX);
	device.Get().resetFences(*syncObject.GetCurrentFence());

	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}

	std::uint32_t result = syncObject.AcquireNextImage(&swapChain,&device);

	// Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined, eErrorOutOfDateKHR can be checked as a result
	// here and does not need to be caught by an exception.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		recreateSwapChain();
		return;
	}
	// On other success codes than eSuccess and eSuboptimalKHR we just throw an exception.


	// Only reset the fence if we are submitting work
	auto& buffer = commandBuffers.Acquire(&device);
	buffer.reset();


	updateUniformBuffer(syncObject.GetCurrentFrame());

	recordCommandBuffer(syncObject.GetCurrentImage(), buffer);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const auto submitInfo = vk::SubmitInfo{
			   .waitSemaphoreCount = 1,
			   .pWaitSemaphores = &*syncObject.GetCurrentPresentSemaphore(),
			   .pWaitDstStageMask = &waitDestinationStageMask,
			   .commandBufferCount = 1,
			   .pCommandBuffers = &*buffer,
			   .signalSemaphoreCount = 1,
			   .pSignalSemaphores = &*syncObject.GetCurrentRenderSemaphore(),
	};

	device.Get().resetFences({ commandBuffers.GetFence(buffer) });
	queue.Get().submit(submitInfo, commandBuffers.GetFence(buffer));

	
	queue.Get().submit({}, *syncObject.GetCurrentFence());

	result = PresentImage();
	// Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined, eErrorOutOfDateKHR can be checked as a result
	// here and does not need to be caught by an exception.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		recreateSwapChain();
	}
	commandBuffers.ReleaseCommandBuffer(buffer);
	syncObject.IncrementFrame();
}



void KGR::_Vulkan::VulkanCore::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = STBManager::Load("Textures\\texture.jpg", &texWidth, &texHeight, &texChannels);
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error("failed to load texture image!");
	}
	size_t size = imageSize;
	Buffer buffer = Buffer(&device, &physicalDevice, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, imageSize);
	buffer.MapMemory(imageSize);
	buffer.Upload(pixels,imageSize);
	buffer.UnMapMemory();
	stbi_image_free(pixels);
	STBManager::Unload("Textures\\texture.jpg");
	createImage(texWidth, texHeight, vk::Format::eR8G8B8A8Srgb, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal, textureImage, textureImageMemory);
	transitionImageLayout(textureImage, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	copyBufferToImage(buffer.Get(), textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
	transitionImageLayout(textureImage, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

void KGR::_Vulkan::VulkanCore::createImage(uint32_t width, uint32_t height, vk::Format format, vk::ImageTiling tiling,
	vk::ImageUsageFlags usage, vk::MemoryPropertyFlags properties, vk::raii::Image& image,
	vk::raii::DeviceMemory& imageMemory)
{
	vk::ImageCreateInfo imageInfo{
		.imageType = vk::ImageType::e2D,
		.format = format,
		.extent = {width, height, 1},
		.mipLevels = 1,
		.arrayLayers = 1,
		.samples = vk::SampleCountFlagBits::e1,
		.tiling = tiling,
		.usage = usage,
		.sharingMode = vk::SharingMode::eExclusive,
		.initialLayout = vk::ImageLayout::eUndefined };	image = vk::raii::Image(device.Get(), imageInfo);
	vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocInfo{ .allocationSize = memRequirements.size,
		.memoryTypeIndex = Buffer::findMemoryType(memRequirements.memoryTypeBits, properties,&physicalDevice) };
	imageMemory = vk::raii::DeviceMemory(device.Get(), allocInfo);
	image.bindMemory(imageMemory, 0);
}

void KGR::_Vulkan::VulkanCore::transitionImageLayout(const vk::raii::Image& image, vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout)
{
	auto& commandBuffer = commandBuffers.Acquire(&device);
	commandBuffer.begin({ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });
	vk::ImageMemoryBarrier barrier{ .oldLayout = oldLayout, .newLayout = newLayout, .image = image, .subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1} };
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal)
	{
		barrier.srcAccessMask = {};
		barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal)
	{
		barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
		barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else
	{
		throw std::invalid_argument("unsupported layout transition!");
	}
	commandBuffer.pipelineBarrier(sourceStage, destinationStage, {}, {}, nullptr, barrier);
	commandBuffer.end();
	vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
	queue.Get().submit(submitInfo, nullptr);
	queue.Get().waitIdle();
}

void KGR::_Vulkan::VulkanCore::copyBufferToImage(const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width,
	uint32_t height)
{
	vk::raii::CommandBuffer& commandBuffer = commandBuffers.Acquire(&device);
	vk::BufferImageCopy  region{ .bufferOffset = 0, .bufferRowLength = 0, .bufferImageHeight = 0, .imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1}, .imageOffset = {0, 0, 0}, .imageExtent = {width, height, 1} };
	vk::CommandBufferBeginInfo beginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit };
	commandBuffer.begin(beginInfo);
	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, { region });
	commandBuffer.end();
	vk::SubmitInfo submitInfo{ .commandBufferCount = 1, .pCommandBuffers = &*commandBuffer };
	queue.Get().submit(submitInfo, nullptr);
	queue.Get().waitIdle();
	commandBuffers.ReleaseCommandBuffer(commandBuffer);
}

void KGR::_Vulkan::VulkanCore::createTextureImageView()
{
	textureImageView = createImageView(textureImage, vk::Format::eR8G8B8A8Srgb, vk::ImageAspectFlagBits::eColor);
}

void KGR::_Vulkan::VulkanCore::createTextureSampler()
{
	vk::PhysicalDeviceProperties properties = physicalDevice.Get().getProperties();
	vk::SamplerCreateInfo        samplerInfo{
		.magFilter = vk::Filter::eLinear,
		.minFilter = vk::Filter::eLinear,
		.mipmapMode = vk::SamplerMipmapMode::eLinear,
		.addressModeU = vk::SamplerAddressMode::eRepeat,
		.addressModeV = vk::SamplerAddressMode::eRepeat,
		.addressModeW = vk::SamplerAddressMode::eRepeat,
		.mipLodBias = 0.0f,
		.anisotropyEnable = vk::True,
		.maxAnisotropy = properties.limits.maxSamplerAnisotropy,
		.compareEnable = vk::False,
		.compareOp = vk::CompareOp::eAlways };
	textureSampler = vk::raii::Sampler(device.Get(), samplerInfo);
}

vk::raii::ImageView KGR::_Vulkan::VulkanCore::createImageView(vk::raii::Image& image, vk::Format format, vk::ImageAspectFlags aspectFlags)
{
	vk::ImageViewCreateInfo viewInfo{
	.image = image,
	.viewType = vk::ImageViewType::e2D,
	.format = format,
    .subresourceRange = {aspectFlags, 0, 1, 0, 1}
	};
	return vk::raii::ImageView(device.Get(), viewInfo);
}

void KGR::_Vulkan::VulkanCore::createDepthResources()
{
	vk::Format depthFormat = findDepthFormat(&physicalDevice);
	auto swapChainExtent = swapChain.GetExtend();
	createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eDepthStencilAttachment, vk::MemoryPropertyFlagBits::eDeviceLocal, depthImage, depthImageMemory);
	depthImageView = createImageView(depthImage, depthFormat, vk::ImageAspectFlagBits::eDepth);
}

vk::Format KGR::_Vulkan::VulkanCore::findSupportedFormat(const std::vector<vk::Format>& candidates,
	vk::ImageTiling tiling, vk::FormatFeatureFlags features, PhysicalDevice* phDevice)
{
	auto formatIt = std::ranges::find_if(candidates, [&](auto const format) {
		vk::FormatProperties props = phDevice->Get().getFormatProperties(format);
		return (((tiling == vk::ImageTiling::eLinear) && ((props.linearTilingFeatures & features) == features)) ||
			((tiling == vk::ImageTiling::eOptimal) && ((props.optimalTilingFeatures & features) == features)));
	});
	if (formatIt == candidates.end())
	{
		throw std::runtime_error("failed to find supported format!");
	}
	return *formatIt;
}

bool KGR::_Vulkan::VulkanCore::hasStencilComponent(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint;
}

vk::Format KGR::_Vulkan::VulkanCore::findDepthFormat(PhysicalDevice* phDevice)
{
	return findSupportedFormat(
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment,phDevice);
}


void KGR::_Vulkan::VulkanCore::updateUniformBuffer(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto  currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float>(currentTime - startTime).count();

	UniformBufferObject ubo{};
	ubo.model = rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.view = lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(swapChain.GetExtend().width) / static_cast<float>(swapChain.GetExtend().height), 0.1f, 10.0f);
	ubo.proj[1][1] *= -1;

	uniformBuffers[currentImage].Upload(&ubo, sizeof(ubo));
}


vk::Bool32 KGR::_Vulkan::VulkanCore::debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT severity,
                                                   vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
{
	if (severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eError || severity == vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning)
	{
		std::cerr << "validation layer: type " << to_string(type) << " msg: " << pCallbackData->pMessage << std::endl;
	}

	return vk::False;
}



//IMPL


