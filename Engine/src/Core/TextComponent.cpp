#include "Core/TextComponent.h"
#include "Core/Font.h"
#include "Core/ManagerImple.h"
#include "Core/Texture.h"
#include "VulkanCore.h"

void TextComponent::SetText(const std::string& text)
{
    if (m_text != text)
    {
        m_text = text;
        m_dirty = true;
    }
}

const std::string& TextComponent::GetText() const
{
    return m_text;
}

void TextComponent::SetFont(Font* font)
{
    if (m_font != font)
    {
        m_font = font;
        m_dirty = true;
    }
}

Font* TextComponent::GetFont() const
{
    return m_font;
}

void TextComponent::SetTextTexture(const std::string& texturePath)
{
    if (m_gradientPath != texturePath)
    {
        m_gradientPath = texturePath;
        m_dirty = true;
    }
}

bool TextComponent::IsDirty() const
{
    return m_dirty;
}

void TextComponent::UpdateTexture(KGR::_Vulkan::VulkanCore* core)
{
    if (!m_font || m_text.empty())
    {
        m_cachedTexture.reset();
        m_dirty = false;
        return;
    }

    auto bitmap = m_font->RenderText(m_text);
    if (bitmap.width <= 0 || bitmap.height <= 0)
    {
        m_cachedTexture.reset();
        m_dirty = false;
        return;
    }

    if (!m_gradientPath.empty())
    {
        auto& grad = STBManager::Load(m_gradientPath);
        for (int y = 0; y < bitmap.height; ++y)
        {
            for (int x = 0; x < bitmap.width; ++x)
            {
                int gx = static_cast<int>(static_cast<float>(x) / bitmap.width  * (grad.width  - 1));
                int gy = static_cast<int>(static_cast<float>(y) / bitmap.height * (grad.height - 1));
                int gIdx = (gy * grad.width + gx) * 4;
                int idx  = (y * bitmap.width + x) * 4;
                bitmap.pixels[idx + 0] = grad.pixels[gIdx + 0];
                bitmap.pixels[idx + 1] = grad.pixels[gIdx + 1];
                bitmap.pixels[idx + 2] = grad.pixels[gIdx + 2];
            }
        }
    }

    auto image = core->CreateImageFromData(bitmap.pixels.data(), bitmap.width, bitmap.height);
    auto set = core->CreateSetForImage(&image);
    m_cachedTexture = std::make_unique<Texture>(std::move(image), std::move(set));
    m_dirty = false;
}

Texture* TextComponent::GetTexture()
{
    return m_cachedTexture.get();
}
