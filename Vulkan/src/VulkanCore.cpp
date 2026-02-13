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
	graphicsPipeline = _Vulkan::Pipeline(info, &device, &swapChain);
	// Command Buffer
	commandBuffers = _Vulkan::CommandBuffers(&device);
	// vertex
	vertexBuffer = _Vulkan::Buffer(&device, &queue, &commandBuffers, &physicalDevice, vk::BufferUsageFlagBits::eVertexBuffer, sizeof(vertices[0])*vertices.size(),vertices.data());
	// index
	indexBuffer = _Vulkan::Buffer(&device, &queue, &commandBuffers, &physicalDevice, vk::BufferUsageFlagBits::eIndexBuffer, sizeof(indices[0]) *indices.size(), indices.data());
	//
	createSyncObjects();
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
	graphicsPipeline = _Vulkan::Pipeline(info, &device, &swapChain);
}





void KGR::_Vulkan::VulkanCore::recordCommandBuffer(uint32_t imageIndex, vk::raii::CommandBuffer& buffer)
{
	auto& commandBuffer =buffer;
	commandBuffer.begin({});
	// Before starting rendering, transition the swapchain image to COLOR_ATTACHMENT_OPTIMAL
	transition_image_layout(
		imageIndex,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		{},                                                        // srcAccessMask (no need to wait for previous operations)
		vk::AccessFlagBits2::eColorAttachmentWrite,                // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
		vk::PipelineStageFlagBits2::eColorAttachmentOutput         // dstStage
		,commandBuffer
	);
	vk::ClearValue              clearColor = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f);
	vk::RenderingAttachmentInfo attachmentInfo = {
		.imageView = swapChainImageViews.Get()[imageIndex],
		.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
		.loadOp = vk::AttachmentLoadOp::eClear,
		.storeOp = vk::AttachmentStoreOp::eStore,
		.clearValue = clearColor };
	vk::RenderingInfo renderingInfo = {
		.renderArea = {.offset = {0, 0}, .extent = swapChain.GetExtend()},
		.layerCount = 1,
		.colorAttachmentCount = 1,
		.pColorAttachments = &attachmentInfo };
	commandBuffer.beginRendering(renderingInfo);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline.Get());
	commandBuffer.setViewport(0, vk::Viewport(0.0f, 0.0f, static_cast<float>(swapChain.GetExtend().width), static_cast<float>(swapChain.GetExtend().height), 0.0f, 1.0f));
	commandBuffer.setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChain.GetExtend()));
	commandBuffer.bindVertexBuffers(0, *vertexBuffer.Get(), { 0 });
	commandBuffer.bindIndexBuffer(*indexBuffer.Get(), 0, vk::IndexTypeValue<decltype(indices)::value_type>::value);
	commandBuffer.drawIndexed(indices.size(), 1, 0, 0, 0);
	commandBuffer.endRendering();
	// After rendering, transition the swapchain image to PRESENT_SRC
	transition_image_layout(
		imageIndex,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::AccessFlagBits2::eColorAttachmentWrite,                // srcAccessMask
		{},                                                        // dstAccessMask
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,        // srcStage
		vk::PipelineStageFlagBits2::eBottomOfPipe                  // dstStage,
		,commandBuffer
	);
	commandBuffer.end();
}

void KGR::_Vulkan::VulkanCore::transition_image_layout(uint32_t imageIndex, vk::ImageLayout old_layout,
                                                                     vk::ImageLayout new_layout, vk::AccessFlags2 src_access_mask, vk::AccessFlags2 dst_access_mask,
                                                                     vk::PipelineStageFlags2 src_stage_mask, vk::PipelineStageFlags2 dst_stage_mask, vk::raii::CommandBuffer& buffer)
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
		.image = swapChain.GetImages()[imageIndex],
		.subresourceRange = {
			.aspectMask = vk::ImageAspectFlagBits::eColor,
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

void KGR::_Vulkan::VulkanCore::createSyncObjects()
{
	assert(presentCompleteSemaphores.empty() && renderFinishedSemaphores.empty() && inFlightFences.empty());

	for (size_t i = 0; i < swapChain.GetImages().size(); i++)
	{
		renderFinishedSemaphores.emplace_back(device.Get(), vk::SemaphoreCreateInfo());
	}

	for (size_t i = 0; i < swapChain.GetImagesCount(); i++)
	{
		presentCompleteSemaphores.emplace_back(device.Get(), vk::SemaphoreCreateInfo());
		inFlightFences.emplace_back(device.Get(), vk::FenceCreateInfo{ .flags = vk::FenceCreateFlagBits::eSignaled });
	}
}

std::uint32_t KGR::_Vulkan::VulkanCore::AcquireNextImage()
{
	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*presentCompleteSemaphores[frameIndex]);
	VkDevice vkDevice = static_cast<VkDevice>(*device.Get());
	VkSwapchainKHR vkSwapChain = static_cast<VkSwapchainKHR>(*swapChain.Get());

	VkResult result = vkAcquireNextImageKHR
	(
		vkDevice,
		vkSwapChain,
		UINT64_MAX,
		vkSemaphore,
		VK_NULL_HANDLE,
		&imageIndex
	);
	return result;
}

std::uint32_t KGR::_Vulkan::VulkanCore::PresentImage()
{
	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*renderFinishedSemaphores[imageIndex]);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vkSemaphore;
	presentInfo.swapchainCount = 1;

	VkSwapchainKHR swapchainC = static_cast<VkSwapchainKHR>(*swapChain.Get());
	presentInfo.pSwapchains = &swapchainC;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	VkQueue queueC = static_cast<VkQueue>(*queue.Get());
	VkResult result = vkQueuePresentKHR(queueC, &presentInfo);
	return result;
}

void KGR::_Vulkan::VulkanCore::drawFrame()
{
	// Note: inFlightFences, presentCompleteSemaphores, and commandBuffers are indexed by frameIndex,
	//       while renderFinishedSemaphores is indexed by imageIndex
	auto fenceResult = device.Get().waitForFences(*inFlightFences[frameIndex], vk::True, UINT64_MAX);
	device.Get().resetFences(*inFlightFences[frameIndex]);

	if (fenceResult != vk::Result::eSuccess)
	{
		throw std::runtime_error("failed to wait for fence!");
	}

	std::uint32_t result = AcquireNextImage();

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


	recordCommandBuffer(imageIndex, buffer);

	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);
	const auto submitInfo = vk::SubmitInfo{
			   .waitSemaphoreCount = 1,
			   .pWaitSemaphores = &*presentCompleteSemaphores[frameIndex],
			   .pWaitDstStageMask = &waitDestinationStageMask,
			   .commandBufferCount = 1,
			   .pCommandBuffers = &*buffer,
			   .signalSemaphoreCount = 1,
			   .pSignalSemaphores = &*renderFinishedSemaphores[imageIndex],
	};

	device.Get().resetFences({ commandBuffers.GetFence(buffer) });
	queue.Get().submit(submitInfo, commandBuffers.GetFence(buffer));

	queue.Get().submit({}, *inFlightFences[frameIndex]);

	result = PresentImage();
	// Due to VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS being defined, eErrorOutOfDateKHR can be checked as a result
	// here and does not need to be caught by an exception.
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		recreateSwapChain();
	}
	commandBuffers.ReleaseCommandBuffer(buffer);
	frameIndex = (frameIndex + 1) % swapChain.GetImagesCount();
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



