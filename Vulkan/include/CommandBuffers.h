#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Device;
		class SwapChain;
		class CommandBuffers
		{
		public:
			using vkCommandPool = vk::raii::CommandPool;
			using vkCommandBuffer = vk::raii::CommandBuffer;
			using vkFence = vk::raii::Fence;

			struct KGRCommandBuffer
			{
				vkCommandBuffer commandBuffer = nullptr;
				vkFence     isGpuFree = nullptr;
				bool                    isCpuFree;
			};

			CommandBuffers() = default;
			CommandBuffers(Device* device);

			vkCommandPool& GetPool();
			const vkCommandPool& GetPool() const;

			vkCommandBuffer& Acquire(Device* device);
			vkFence& GetFence(vkCommandBuffer& buffer);
			void ReleaseCommandBuffer(vkCommandBuffer& commandBuffer);
		private:
			vkCommandPool m_pool = nullptr;
			std::unordered_map<VkCommandBuffer, KGRCommandBuffer> m_commandBuffers;
		};
	}
}