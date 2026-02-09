#pragma once
#include <vulkan/vulkan_raii.hpp>
#include <vma/vk_mem_alloc.h>
#include "_GLFW.h"

using namespace vk::raii;
using ui32t = uint32_t;
using i32t = int32_t;

namespace KGR
{
	namespace _Vulkan
	{
		inline static Context vkContext;

		struct _FrameData
		{
			_Semaphore presentCompleteSemaphore;
			_Semaphore renderFinishedSemaphore;
			_CommandBuffer commandBuffer;
			_Fence perFrameFence;
		};

		struct _AppInfo
		{
			const char* appName = "Basic_Api";
			const char* engineName = "None";
			std::uint32_t engineVersion = VK_MAKE_VERSION(1, 0, 0);
			std::uint32_t appVersion = VK_MAKE_VERSION(1, 0, 0);
			std::uint32_t version = vk::ApiVersion14;

			void Create();
			vk::ApplicationInfo& GetInfo();

		private:
			vk::ApplicationInfo m_Info;
		};

		struct _Instance
		{
			_Instance() = default;
			_Instance(_AppInfo&& info, std::vector<char const*> validationLayers);

			void AddLayer(const char* layer);
			Instance& GetInstance();
			const Instance& GetInstance() const;
			void Clear();

		private:
			std::vector<char const*> m_validationLayers;
			_AppInfo m_info;
			Instance m_instance = nullptr;
		};

		struct _PhysicalDevice
		{
			_PhysicalDevice() = default;
			_PhysicalDevice(_Instance* instance);

			PhysicalDevice& GetDevice();
			const PhysicalDevice& GetDevice() const;
			void Clear();
			ui32t GraphicsQueueIndex() const;

		private:
			ui32t m_queueIndex;
			PhysicalDevice m_device = nullptr;
		};

		struct _Surface
		{
			_Surface() = default;
			_Surface(_Instance* instance, _GLFW::Window* window);
			SurfaceKHR& GetSurface();
			const SurfaceKHR& GetSurface() const;
			void Clear();

		private:
			SurfaceKHR m_surface = nullptr;
		};

		struct _Device
		{
			_Device() = default;
			_Device(_PhysicalDevice* device, ui32t count = 1);
			Device& GetDevice();
			const Device& GetDevice() const;
			void Clear();
			void WaitIdle();

		private:
			Device m_device = nullptr;
		};

		struct _Queue
		{
			_Queue() = default;
			_Queue(_Device* device, _PhysicalDevice* pDevice, ui32t index = 0);
			Queue& GetQueue();
			const Queue& GetQueue() const;
			void Clear();

		private:
			Queue m_queue = nullptr;
		};

		struct _Swapchain
		{
			_Swapchain() = default;
			_Swapchain(_PhysicalDevice* pDevice,
				_Device* device,
				_Surface* surface,
				_GLFW::Window* window,
				ui32t imageCount = 3,
				_Swapchain* old = nullptr);

			SwapchainKHR& GetSwapchain();
			const SwapchainKHR& GetSwapchain() const;
			void Clear();

		private:
			SwapchainKHR m_chain = nullptr;
		};

		struct _VkImages
		{
			_VkImages() = default;
			_VkImages(_Swapchain* swapchain);

			std::vector<vk::Image>& GetImages();
			const std::vector<vk::Image>& GetImages() const;
			void Clear();

		private:
			std::vector<vk::Image> m_images;
		};

		struct _CommandPool
		{
			_CommandPool() = default;
			_CommandPool(_PhysicalDevice* pDevice, _Device* device);

			CommandPool& GetPool();
			const CommandPool& GetPool() const;
			void Clear();

		private:
			CommandPool m_pool = nullptr;
		};

		struct _CommandBuffer
		{
			_CommandBuffer() = default;
			_CommandBuffer(_Device* device, _CommandPool* pool);

			CommandBuffer& GetBuffer();
			const CommandBuffer& GetBuffer() const;
			void Clear();

		private:
			CommandBuffer m_buffer = nullptr;
		};

		struct _Semaphore
		{
			_Semaphore() = default;
			_Semaphore(_Device* device);

			Semaphore& GetSemaphore();
			const Semaphore& GetSemaphore() const;
			void Clear();

		private:
			Semaphore m_semaphore = nullptr;
		};

		struct _Fence
		{
			_Fence() = default;
			_Fence(_Device* device);

			Fence& GetFence();
			const Fence& GetFence() const;
			void Clear();

		private:
			Fence m_fence = nullptr;
		};
	}

	class TMP_Vulkan
	{
	public:
		TMP_Vulkan();
		void Init(_GLFW::Window* window);

		void InitInstance();
		void CreatePhysicalDevice();
		void CreateSurface(_GLFW::Window* window);
		void CreateDevice();
		void CreateSwapchain(_GLFW::Window* window);
		void CreateCommandResources();
		void CreateObjects();

		void TransitionToTransferDst(CommandBuffer& cb, vk::Image& image);
		void TransitionToPresent(CommandBuffer& cb, vk::Image& image);

		i32t AcquireNextImage(ui32t frameIndex);
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
		std::vector<KGR::_Vulkan::_FrameData> m_frameData;

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