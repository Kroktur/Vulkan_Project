#pragma once
#include <string>
#include <memory>

class Font;
class Texture;

namespace KGR::_Vulkan
{
    class VulkanCore;
}

/**
 * @brief ECS component for rendering text as a textured UI element.
 *
 * Holds a text string and a font reference. When the text or font changes,
 * the component is marked dirty and its internal texture is regenerated
 * on the next rendering pass.
 *
 * Usage:
 * @code
 *   TextComponent text;
 *   text.SetFont(font);
 *   text.SetText("Hello World!");
 *   // Register with window->RegisterText(text, ui, transform);
 * @endcode
 */
struct TextComponent
{
    TextComponent() = default;

    /**
     * @brief Sets the text string to render.
     * @param text The new text string.
     */
    void SetText(const std::string& text);

    /**
     * @brief Returns the current text string.
     */
    const std::string& GetText() const;

    /**
     * @brief Sets the font used for rendering.
     * @param font Pointer to a Font resource.
     */
    void SetFont(Font* font);

    /**
     * @brief Sets a gradient texture to apply on the text.
     *
     * The gradient image is sampled across the text quad and replaces
     * the white RGB while preserving the text alpha mask.
     * Pass an empty string to remove the gradient.
     *
     * @param texturePath Relative path to the gradient image (e.g. "Textures/gradient.png").
     */
    void SetTextTexture(const std::string& texturePath);

    /**
     * @brief Returns the current font.
     */
    Font* GetFont() const;

    /**
     * @brief Returns true if the text or font has changed since last texture update.
     */
    bool IsDirty() const;

    /**
     * @brief Regenerates the internal texture from the current text and font.
     *
     * Called by the rendering system when the component is dirty.
     *
     * @param core VulkanCore instance used for GPU texture creation.
     */
    void UpdateTexture(KGR::_Vulkan::VulkanCore* core);

    /**
     * @brief Returns the cached texture for rendering.
     * @return Pointer to the internal Texture, or nullptr if not yet generated.
     */
    Texture* GetTexture();

private:
    std::string m_text;                        ///< Text string to render.
    Font* m_font = nullptr;                    ///< Font used for rendering.
    std::string m_gradientPath;                ///< Relative path to a gradient texture (empty = no gradient).
    bool m_dirty = true;                       ///< True when texture needs regeneration.
    std::unique_ptr<Texture> m_cachedTexture;  ///< GPU texture of the rendered text.
};
