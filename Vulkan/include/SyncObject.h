#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class SwapChain;
		class Device;
		class SyncObject
		{
		public:
			using MyVkSemaphore = vk::raii::Semaphore;
			using MyvkFence = vk::raii::Fence;
			SyncObject() = default;
			SyncObject( Device* device,uint32_t imageCount);

			MyVkSemaphore& GetCurrentPresentSemaphore();

			const MyVkSemaphore& GetCurrentPresentSemaphore() const;


			MyVkSemaphore& GetCurrentRenderSemaphore();

			const MyVkSemaphore& GetCurrentRenderSemaphore() const;

			MyvkFence& GetCurrentFence();

			const MyvkFence& GetCurrentFence() const;
			uint32_t AcquireNextImage(SwapChain* swapChain, Device* device);

			void IncrementFrame();

			const uint32_t& GetCurrentImage() const;

			const uint32_t& GetCurrentFrame() const;

		private:
			std::vector<MyVkSemaphore> m_presentCompleteSemaphores;
			std::vector<MyVkSemaphore> m_renderFinishedSemaphores;
			std::vector<MyvkFence>     m_inFlightFences;
			uint32_t                         m_frameIndex;
			uint32_t						 m_imageIndex;
			uint32_t                         m_imageCount;
		};
	}
}