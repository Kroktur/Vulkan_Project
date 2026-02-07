#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vma/vk_mem_alloc.h>
#include "_GLFW.h"

using namespace vk::raii;
using ui32t = uint32_t;

namespace KGR
{
	struct FrameData
	{
		Semaphore presentCompleteSemaphore = nullptr;
		Semaphore renderFinishedSemaphore = nullptr;
		CommandBuffer commandBuffer = nullptr;
		Fence perFrameFence = nullptr;
	};

	class _Vulkan
	{
	public:
		_Vulkan();
		void Init(_GLFW* window);

		void InitInstance();
		void CreatePhysicalDevice();
		void CreateSurface(_GLFW* window);
		void CreateDevice();
		void CreateSwapchain(_GLFW* window);
		void CreateCommandResources();
		void CreateObjects();

		void TransitionToTransferDst(CommandBuffer& cb, vk::Image& image);
		void TransitionToPresent(CommandBuffer& cb, vk::Image& image);

		ui32t AcquireNextImage(ui32t frameIndex);
		void SubmitCommands(ui32t frameIndex);
		void Present(ui32t frameIndex, ui32t imageIndex);

		void WaitIdle();
		void Cleanup();

		Instance& GetInstance();
		Device& GetDevice();
		CommandBuffer& GetCommandBuffer(ui32t frameIndex);
		std::vector<vk::Image>& GetSCImages();
		ui32t GetFrameCount() const;
		ui32t GetCurrentImageIndex() const;
		vk::Image& GetCurrentImage();

		vk::PhysicalDeviceType GetGPU();
		CommandBuffer& Begin();
		void End();

	private:

		void CheckLayerProperties();
		void GetExtensions();
		void AddExtensions();
		void VerifyExtensions();
		ui32t FindGraphicsQueueFamily();

		// Generic method for Transitions
		void TransitionImage(
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
			ui32t layerCount);

	private:
		Context m_vkContext;
		std::unique_ptr<Instance> m_instance;
		std::unique_ptr<PhysicalDevice> m_physicalDevice;
		std::unique_ptr<SurfaceKHR> m_surface;
		std::unique_ptr<Device> m_device;
		std::unique_ptr<Queue> m_graphicsQueue;
		std::unique_ptr<SwapchainKHR> m_swapchain;
		std::unique_ptr<CommandPool> m_commandPool;

		std::vector<vk::Image> m_scImages;
		std::vector<FrameData> m_frameData;

		vk::ApplicationInfo m_appInfo;
		vk::InstanceCreateInfo m_instanceCreateInfo;

		ui32t m_graphicsQueueFamily = 0;
		ui32t m_currentFrame = 0;
		ui32t m_currentImageIndex = 0;

		std::vector<const char*> m_requiredLayers;
		const std::vector<char const*> m_validationLayers =
		{ "VK_LAYER_KHRONOS_validation" };

		ui32t m_glfwExtensionCount = 0;
		const char** m_glfwExtensions = nullptr;
		std::vector<vk::ExtensionProperties> m_extensionProperties;
		std::vector<const char*> m_activeExtensions;

		std::vector<PhysicalDevice> m_devices;
	};
}