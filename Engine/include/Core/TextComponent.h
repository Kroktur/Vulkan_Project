#pragma once
#include "Core/Font.h"
#include <vector>
#include "Buffer.h"

namespace KGR::_Vulkan
{
	class VulkanCore;
}

struct Text
{
	Text() = default;
	AtlasFont* font;

	void CreateVertexBuffer();

	void SetText( KGR::_Vulkan::VulkanCore* core, char c);

	void Bind(const vk::raii::CommandBuffer* buffer);

private:
	KGR::_Vulkan::Buffer m_vertexBuffer;
	KGR::_Vulkan::Buffer m_indexBuffer;   
	std::string message = "";
};

struct TextComp
{
	TextComp() = default;
	Text text;
};