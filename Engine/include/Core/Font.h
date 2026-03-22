#pragma once
#include <string>
#include <vector>
#include <memory>
#include <glm/vec2.hpp>

#include "array"
#include "DescriptorSet.h"
#include "Image.h"
#include "RessourcesManager.h"
#include "Texture.h"

namespace KGR::_Vulkan
{
	class VulkanCore;
}

struct Ascii
{
    Ascii();
    Ascii(char c);
    Ascii(int i);

	int ToInt() const;
    char ToChar() const;

    static constexpr int ToInt(char c);
    static constexpr char ToChar(int i);
    static constexpr bool IsValid(char c);
private:
    char m_data = ' ';
};

struct Glyph {
    Ascii code;
    float advance;
	glm::vec2 size; 
    glm::vec2 offset;    
    glm::vec2 min;
    glm::vec2 max;
};




struct AtlasFont
{
    AtlasFont(KGR::_Vulkan::Image&& image, KGR::_Vulkan::DescriptorSet&& set,const std::array<Glyph, 95>& glyphs,int pixelSize);
    Glyph GetGlyph(const Ascii& asci) const;
    glm::ivec2 Size() const;
    int pixSize() const;
    void Bind(const vk::raii::CommandBuffer* buffer,
        const vk::raii::PipelineLayout* layout,
        int set);
    Texture* GetTexture();
private:
    Texture m_texture;
    int m_pixelSize = 0;
    std::array<Glyph, 95> m_glyphs = {};
};

std::unique_ptr<AtlasFont> loadFont(const std::string& filePath, KGR::_Vulkan::VulkanCore* core);
using FontLoader =
KGR::ResourceManager<AtlasFont,
    KGR::TypeWrapper<KGR::_Vulkan::VulkanCore*>,
    loadFont>;

