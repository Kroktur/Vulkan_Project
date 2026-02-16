#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Device;

		class DescriptorPool
		{
		public:
			using vkPool = vk::raii::DescriptorPool;
			using vkPoolSize = vk::DescriptorPoolSize;
			DescriptorPool() = default;
			DescriptorPool(const std::vector<vkPoolSize>& info,size_t maxCount,Device* device);

			vkPool& Get()
			{
				return m_pool;
			}
			const vkPool& Get() const
			{
				return m_pool;
			}
		private:
			vkPool m_pool = nullptr;
		};
	}
}