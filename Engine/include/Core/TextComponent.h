#pragma once
#include "Core/Font.h"
#include <vector>
#include "Buffer.h"
#include "Tools/Util.h"

namespace KGR::_Vulkan
{
	class VulkanCore;
}

struct Text
{
	friend KGR::_Vulkan::VulkanCore;
	Text() = default;
	AtlasFont* font = nullptr;
	void SetText(const std::string& text);
	void Bind(const vk::raii::CommandBuffer* buffer);
private:
	void Upload(KGR::_Vulkan::VulkanCore* core);
	size_t GetIndexSize() const;
	KGR::_Vulkan::Buffer m_vertexBuffer;
	KGR::_Vulkan::Buffer m_indexBuffer;   
	DataDirty<std::string> message {true," "};
	size_t m_size = 0;
};

struct TextComp
{
	TextComp() = default;
	Text text;
};