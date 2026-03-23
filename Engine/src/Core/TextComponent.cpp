#include "Core/TextComponent.h"
#include "VulkanCore.h"
#include "Core/Vertex.h"

void Text::SetText(const std::string& text)
{
	message.data = text;
	message.isDirty = true;
}

void Text::Bind(const vk::raii::CommandBuffer* buffer)
{
	buffer->bindVertexBuffers(0, *m_vertexBuffer.Get(), { 0 });
	buffer->bindIndexBuffer(*m_indexBuffer.Get(), 0, vk::IndexType::eUint32);
}

void Text::Upload(KGR::_Vulkan::VulkanCore* core)
{
	if (!message.isDirty)
		return;
	message.isDirty = false;
	if (m_vertexBuffer.GetSize() != 0)
	{
		m_vertexBuffer.Get().clear();
		m_indexBuffer.Get().clear();
	}
	
	std::vector<Vertex2D> vertices;
	std::vector<uint32_t> indices;
	// scale 
	float totalAdvance = 0.0f;
	for (auto c :message.data)
	{
		totalAdvance += font->GetGlyph(c).advance;
	}
	float scaleX = 1.0f / totalAdvance;

	auto ascent = std::abs(font->GetAscent());

	float scalePixel = 1.0f / static_cast<float>(ascent + font->GetDescent());
	// scale 
	uint32_t indexOffset = 0;
	float xStart = -0.5f;
	for (int i = 0 ; i < message.data.size() ; ++i)
	{
		auto glyph = font->GetGlyph(message.data[i]);
		float minX = xStart + glyph.offset.x * scaleX;
		float minY = 0.5f + glyph.offset.y * scalePixel - font->GetDescent() * scalePixel;
		float maxX = minX + glyph.size.x * scaleX;
		float maxY = minY + glyph.size.y * scalePixel;
		

		vertices.push_back({ {minX, minY},     {glyph.min.x, glyph.min.y} });
		vertices.push_back({ {maxX, minY},     {glyph.max.x, glyph.min.y} });
		vertices.push_back({ {maxX, maxY}, {glyph.max.x, glyph.max.y} });
		vertices.push_back({ {minX, maxY}, {glyph.min.x, glyph.max.y} });

		indices.push_back(indexOffset + 0);
		indices.push_back(indexOffset + 2);
		indices.push_back(indexOffset + 1);

		indices.push_back(indexOffset + 2);
		indices.push_back(indexOffset + 0);
		indices.push_back(indexOffset + 3);
		indexOffset += 4;
		xStart += glyph.advance * scaleX;
	}
	
	m_vertexBuffer = core->CreateVertexBuffer(vertices);
	m_indexBuffer = core->CreateIndexBuffer(indices);
	m_size = indices.size();
}

size_t Text::GetIndexSize() const 
{
	return m_size;
}
