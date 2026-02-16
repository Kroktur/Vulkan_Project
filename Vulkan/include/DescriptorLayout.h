#pragma once
#include "Global.h"
namespace KGR
{
	namespace _Vulkan
	{
		class Pipeline;
		class Device;
		class DescriptorLayout
		{
		public:
			using vkDescriptorLayout = vk::raii::DescriptorSetLayout;
			using vkBinding = vk::DescriptorSetLayoutBinding;
			DescriptorLayout() = default;
			DescriptorLayout(const std::vector<vkBinding>& bindings, Device* device);
			vkDescriptorLayout& Get();
			const vkDescriptorLayout& Get() const;
		private:
			vkDescriptorLayout m_layout = nullptr;
		};
		class DescriptorLayouts
		{
		public:
			friend Pipeline;
			DescriptorLayouts() = default;
			void Add(DescriptorLayout&& layout);
			DescriptorLayout& Get(size_t index);
			const DescriptorLayout& Get(size_t index) const;
			size_t Size();

		private:
			std::vector<DescriptorLayout::vkDescriptorLayout*>& GetLayouts();
			std::vector<DescriptorLayout> m_layouts;
			std::vector<DescriptorLayout::vkDescriptorLayout*> m_realLayouts;
		};
	}
}