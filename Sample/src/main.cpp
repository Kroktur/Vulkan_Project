#include "AllFiles.h"
#include "Core/ManagerImple.h"
#include <fstream>
#include <filesystem>
#include <iostream>






#include <algorithm>
#include <assert.h>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <stdexcept>
#include <vector>

#include "Core/ManagerImple.h"
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS

#if defined(__INTELLISENSE__) || !defined(USE_CPP20_MODULES)
#	include <vulkan/vulkan_raii.hpp>
#else
import vulkan_hpp;
#endif

#define GLFW_INCLUDE_VULKAN        // REQUIRED only for GLFW CreateWindowSurface.
#include <GLFW/glfw3.h>

constexpr uint32_t WIDTH = 800;
constexpr uint32_t HEIGHT = 600;
constexpr int      MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<char const*> validationLayers = {
	"VK_LAYER_KHRONOS_validation" };

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

int main(int argc, char** argv)
{

	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();

	fileManager::SetGlobalFIlePath(projectRoot / "Ressources");

	std::cout << projectRoot;
	KGR::_GLFW::Window::Init();
	KGR::_GLFW::Window::AddHint(GLFW_CLIENT_API, GLFW_NO_API);
	KGR::_GLFW::Window::AddHint(GLFW_RESIZABLE, GLFW_TRUE);
	KGR::_GLFW::Window window;


	window.CreateMyWindow({ 1280, 720 }, "GC goes Vulkan", nullptr, nullptr);

	KGR::Core_Vulkan vulkan;
	vulkan.Init(&window);

	do
	{
		KGR::_GLFW::Window::PollEvent();
		if (vulkan.Begin() == -1)
		{
			vulkan.RecreateSwapchain();
			continue;
		}

		auto& cb = vulkan.GetCommandBuffer(vulkan.GetCurrentFrame()).GetBuffer();
		auto& currentImage = vulkan.GetCurrentImage();
		auto extent = vulkan.GetSwapchain().GetSwapchainExtent();

		vulkan.Transition(KGR::_Vulkan::TransitionType::ColorAttachment, cb);

		vk::RenderingAttachmentInfo attachmentInfo
		{
			.imageView = vulkan.GetCurrentImageView(),
			.imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
			.loadOp = vk::AttachmentLoadOp::eClear,
			.storeOp = vk::AttachmentStoreOp::eStore,
			.clearValue = vk::ClearColorValue(0.0f, 0.0f, 0.0f, 1.0f)
		};

		vk::RenderingInfo renderingInfo
		{
			.renderArea = vk::Rect2D{{0,0}, extent},
			.layerCount = 1,
			.colorAttachmentCount = 1,
			.pColorAttachments = &attachmentInfo
		};

		cb.beginRendering(renderingInfo);
		cb.bindPipeline(vk::PipelineBindPoint::eGraphics, *vulkan.GetPipeline().GetPipeline());

		cb.setViewport(0, vk::Viewport(0.f, 0.f,
			static_cast<float>(extent.width),
			static_cast<float>(extent.height), 0.f, 1.f));

		cb.setScissor(0, vk::Rect2D({ 0,0 }, extent));

		cb.draw(3, 1, 0, 0);
		cb.endRendering();

		vulkan.Transition(KGR::_Vulkan::TransitionType::Present, cb);

		if (vulkan.End() == -1)
		{
			vulkan.RecreateSwapchain();
			continue;
		}

	} while (!window.ShouldClose());

	vulkan.WaitIdle();
	vulkan.Cleanup();

	window.DestroyMyWindow();
	KGR::_GLFW::Window::Destroy();

	return 0;
}