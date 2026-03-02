#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Surface;
		class Device;
		class PhysicalDevice;

		class SwapChain
		{
		public:
			using vkSwapChain = vk::raii::SwapchainKHR;
			using vkFormat = vk::SurfaceFormatKHR;
			using vkExtend = vk::Extent2D;
			using vkImage = vk::Image;


			enum class PresMode
			{
				Immediate,
				Mailbox,
				Fifo,
				FifoRelaxed,
				SharedDemandRefresh,
				SharedContinuousRefresh,
				FifoLatestReady,
				FifoLatestReadyEXT,
			};

			SwapChain() = default;
			SwapChain(PhysicalDevice* pDevice,
			Device* device,
				Surface* surface,
				GLFWwindow* window,
				 PresMode wanted,
				ui32t imageCount = 3,
				SwapChain* old = nullptr);

			vkSwapChain& Get();
			const vkSwapChain& Get()const;
			vkExtend GetExtend() const;
			vkFormat& GetFormat();
			const vkFormat& GetFormat()const;
			std::vector<vkImage>& GetImages();
			const std::vector<vkImage>& GetImages() const;

			uint32_t GetImagesCount() const;

			static vkExtend chooseSwapExtent(const vk::SurfaceCapabilitiesKHR& capabilities,GLFWwindow* window);
			static uint32_t chooseSwapMinImageCount(vk::SurfaceCapabilitiesKHR const& surfaceCapabilities,uint32_t imageTargetCount);
			static vkFormat chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
			static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes,PresMode wanted);

		private:
			static bool IsWantedPresentModel(vk::PresentModeKHR get , PresMode wanted);
			static vk::PresentModeKHR KGRToVulkan(PresMode wanted);
			vkSwapChain m_swapChain = nullptr;
			vkFormat            m_swapChainSurfaceFormat;
			vkExtend                     m_swapChainExtent;
			std::vector<vkImage>           m_swapChainImages;
			uint32_t m_imageCount;
		};
	}
}