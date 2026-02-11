#include "AllFiles.h"

int main()
{
	KGR::_GLFW::Window window;
	KGR::_GLFW::Window::Init();
	KGR::_GLFW::Window::AddHint(GLFW_CLIENT_API, GLFW_NO_API);
	KGR::_GLFW::Window::AddHint(GLFW_RESIZABLE, GLFW_TRUE);
	window.CreateMyWindow({ 1280, 720 }, "GC goes Vulkan", nullptr, nullptr);

	KGR::Core_Vulkan vulkan(&window);

	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForVulkan(&window.GetWindow(), true);

	do
	{
		KGR::_GLFW::Window::PollEvent();

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
	KGR::_GLFW::Window::Destroy();

	return 0;
}