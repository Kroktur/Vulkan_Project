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
#include <iostream>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <ranges>
#include "AllFiles.h"

const std::vector<char const*> validationLayers = 
{
	"VK_LAYER_KHRONOS_validation"
};

int32_t AcquireNextImage(vk::raii::Device& device, vk::raii::SwapchainKHR& swapchain, vk::raii::Semaphore& semaphore, vk::raii::Fence& fence)
{
	auto fenceResult = device.waitForFences(*fence, true, UINT64_MAX);
	device.resetFences(*fence);

	uint32_t imageIndex = 0;

	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*semaphore);

	VkDevice vkDevice = static_cast<VkDevice>(*device);
	VkSwapchainKHR vkSwapChain = static_cast<VkSwapchainKHR>(*swapchain);

	VkResult result = vkAcquireNextImageKHR(
		vkDevice,
		vkSwapChain,
		UINT64_MAX,                   
		vkSemaphore,          
		VK_NULL_HANDLE,                     
		&imageIndex
	);

	if (result == VK_ERROR_OUT_OF_DATE_KHR ||  result == VK_SUBOPTIMAL_KHR) {
		return  -1;
	}

	return static_cast<int>(imageIndex);
}

int Present(vk::raii::Semaphore& waitSemaphore, vk::raii::Queue& queue, vk::raii::SwapchainKHR& swapchain, uint32_t imageIndex)
{
	VkSemaphore vkSemaphore = static_cast<VkSemaphore>(*waitSemaphore);

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &vkSemaphore; // VkSemaphore
	presentInfo.swapchainCount = 1;

	VkSwapchainKHR swapchainC = static_cast<VkSwapchainKHR>(*swapchain);
	presentInfo.pSwapchains = &swapchainC;
	presentInfo.pImageIndices = &imageIndex;
	presentInfo.pResults = nullptr;

	VkQueue queueC = static_cast<VkQueue>(*queue);
	VkResult result = vkQueuePresentKHR(queueC, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
		return -1;
	}
	return 0;
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	KGR::_GLFW::Window window;
	window.CreateMyWindow({ 1280, 720 }, "Test", nullptr, nullptr);

	KGR::_Vulkan::_AppInfo info{};
	auto instance = KGR::_Vulkan::_Instance{ std::move(info),std::vector<const char*>{"VK_LAYER_KHRONOS_validation"} };
	auto physicalDevice = KGR::_Vulkan::_PhysicalDevice{ &instance };
	auto surface = KGR::_Vulkan::_Surface{&instance, &window};
	auto device = KGR::_Vulkan::_Device{&physicalDevice};
	auto graphicsQueue = KGR::_Vulkan::_Queue{&device,&physicalDevice};
	auto swapchain = KGR::_Vulkan::_Swapchain{&physicalDevice, &device, &surface, &window};
	auto swapchainImages = KGR::_Vulkan::_VkImages{&swapchain};
	auto commandPool = KGR::_Vulkan::_CommandPool{&physicalDevice,&device};



	auto frameData = swapchainImages.GetImages() | std::views::transform([&](const vk::Image& i) {
		return KGR::_Vulkan::_FrameData
		{
			.presentCompleteSemaphore = KGR::_Vulkan::_Semaphore(&device),
			.renderFinishedSemaphore = KGR::_Vulkan::_Semaphore(&device),
			.commandBuffer = KGR::_Vulkan::_CommandBuffer(&device,&commandPool)
		};
		
		}) | std::ranges::to<std::vector>();


	uint32_t currentFrame = 0;
	auto drawFence = KGR::_Vulkan::_Fence(&device);

	do
	{
		glfwPollEvents();
		auto  currentImageIndex = AcquireNextImage(device.GetDevice(), swapchain.GetSwapchain(), frameData[currentFrame].presentCompleteSemaphore.GetSemaphore(), drawFence.GetFence());

		if (currentImageIndex == -1)
		{
			swapchain = KGR::_Vulkan::_Swapchain(&physicalDevice, &device, &surface, &window, 3, &swapchain);
			currentImageIndex = AcquireNextImage(device.GetDevice(), swapchain.GetSwapchain(), frameData[currentFrame].presentCompleteSemaphore.GetSemaphore(), drawFence.GetFence());
			swapchainImages= (&swapchain);
		}

		auto& currentImage = swapchainImages.GetImages()[currentImageIndex];

		auto& cb = frameData[currentFrame].commandBuffer;
		cb.GetBuffer().begin(vk::CommandBufferBeginInfo{ .flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit });

		auto clearRange = vk::ImageSubresourceRange{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = vk::RemainingMipLevels,
			.layerCount = vk::RemainingArrayLayers
		};

		Transition(cb.GetBuffer(), vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal, currentImage, false, true);
		cb.GetBuffer().clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal, vk::ClearColorValue(0.1f, 0.2f, 0.3f, 1.0f), clearRange);
		Transition(cb.GetBuffer(), vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::ePresentSrcKHR, currentImage, false, false);
		cb.GetBuffer().end();

		Submit(graphicsQueue.GetQueue(), cb.GetBuffer(), frameData[currentFrame].presentCompleteSemaphore.GetSemaphore(), frameData[currentFrame].renderFinishedSemaphore.GetSemaphore(), drawFence.GetFence());

		auto reult = Present(frameData[currentFrame].renderFinishedSemaphore.GetSemaphore(), graphicsQueue.GetQueue(), swapchain.GetSwapchain(), currentImageIndex);

		if (reult == -1)
		{
			swapchain = KGR::_Vulkan::_Swapchain(&physicalDevice, & device, & surface, &window, 3, & swapchain);
			swapchainImages = (&swapchain);
		}
		++currentFrame %= frameData.size();
	} while (!glfwWindowShouldClose(&window.GetWindow()));


	device.WaitIdle();

	drawFence.Clear();
	frameData.clear();
	commandPool.Clear();
	swapchainImages.Clear();
	swapchain.Clear();
	graphicsQueue.Clear();
	device.Clear();
	surface.Clear();
	physicalDevice.Clear();
	instance.Clear();
	glfwDestroyWindow(&window.GetWindow());
	glfwTerminate();
}

