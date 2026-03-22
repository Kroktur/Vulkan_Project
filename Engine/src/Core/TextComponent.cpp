#include "Core/TextComponent.h"
#include "VulkanCore.h"
#include "Core/Vertex.h"
void Text::CreateVertexBuffer()
{
	std::vector<Vertex2D> vertices;
	for (auto& letter : message)
	{
		// 4 vertices et 2 triangles 
	}
	char c = '\n';

}

void Text::SetText(KGR::_Vulkan::VulkanCore* core, char c)
{
	if (m_vertexBuffer.GetSize() !=0)
	{
		m_vertexBuffer.Get().clear();
		m_indexBuffer.Get().clear();
	}
	auto glyph = font->GetGlyph(c);
	std::vector<Vertex2D> vertices =
	{
		{{-0.5f, -0.5f}, {	glyph.min.x, 	glyph.min.y}},
		{{ 0.5f, -0.5f}, {glyph.max.x, 	glyph.min.y}},
		{{ 0.5f,  0.5f}, {glyph.max.x, glyph.max.y}},
		{{-0.5f,  0.5f}, {	glyph.min.x,glyph.max.y}},
	};
	std::vector<std::uint32_t> indices =
	{
		0, 2, 1,
		2, 0, 3
	};
	m_vertexBuffer = core->CreateVertexBuffer(vertices);
	m_indexBuffer = core->CreateIndexBuffer(indices);
}

void Text::Bind(const vk::raii::CommandBuffer* buffer)
{
	buffer->bindVertexBuffers(0, *m_vertexBuffer.Get(), { 0 });
	buffer->bindIndexBuffer(*m_indexBuffer.Get(), 0, vk::IndexType::eUint32);

}
