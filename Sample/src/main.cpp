#include "AllFiles.h"

int main()
{
	KGR::_GLFW::Window window;
	window.Init();
	window.AddHint(GLFW_CLIENT_API, GLFW_NO_API);
	window.AddHint(GLFW_RESIZABLE, GLFW_TRUE);
	window.CreateMyWindow({ 1280, 720 }, "GC goes Vulkan", nullptr, nullptr);

	KGR::Core_Vulkan vulkan;
	vulkan.Init(&window);

	do
	{
		window.PollEvent();

		if (vulkan.Begin() == -1)
		{
			vulkan.RecreateSwapchain(&window);
			continue;
		}

		auto& cb = vulkan.GetCommandBuffer(vulkan.GetCurrentFrame()).GetBuffer();
		auto& currentImage = vulkan.GetCurrentImage();

		auto clearRange = vk::ImageSubresourceRange
		{
			.aspectMask = vk::ImageAspectFlagBits::eColor,
			.levelCount = vk::RemainingMipLevels,
			.layerCount = vk::RemainingArrayLayers
		};

		vulkan.TransitionToTransferDst(cb, currentImage);
		cb.clearColorImage(currentImage, vk::ImageLayout::eTransferDstOptimal,
			vk::ClearColorValue(0.1f, 0.2f, 0.3f, 1.0f), clearRange);
		vulkan.TransitionToPresent(cb, currentImage);

		if (vulkan.End() == -1)
		{
			vulkan.RecreateSwapchain(&window);
		}

	} while (!window.ShouldClose());

	vulkan.WaitIdle();
	vulkan.Cleanup();

	window.DestroyMyWindow();
	window.Destroy();

	return 0;
}