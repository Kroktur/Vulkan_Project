#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Device;
		class DescriptorLayout;
		class DescriptorPool;
		class DescriptorSet
		{
		public:
			using vkDescriptorSet = vk::raii::DescriptorSet;
			using vkWriteSet = vk::WriteDescriptorSet;

			DescriptorSet() = default;
			DescriptorSet(Device* device, DescriptorPool* pool, DescriptorLayout* layout);
			static std::vector<DescriptorSet> Create(Device* device, DescriptorPool* pool, DescriptorLayout* layout, size_t count);
			vkDescriptorSet& Get()
			{
				return m_set;
			}
			const vkDescriptorSet& Get() const
			{
				return m_set;
			}
		private:
			DescriptorSet(vkDescriptorSet&& set) : m_set(std::move(set)){}
			vkDescriptorSet m_set = nullptr; 
		};
	}

}