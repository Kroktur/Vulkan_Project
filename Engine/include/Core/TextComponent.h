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
	Text() = default;
	AtlasFont* font = nullptr;


	void SetText(  char c);

	void Bind(const vk::raii::CommandBuffer* buffer);

	void Upload(KGR::_Vulkan::VulkanCore* core);
private:
	KGR::_Vulkan::Buffer m_vertexBuffer;
	KGR::_Vulkan::Buffer m_indexBuffer;   
	DataDirty<std::string> message {true," "};
};

struct TextComp
{
	TextComp() = default;
	Text text;
};