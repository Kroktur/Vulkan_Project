#pragma once
#include <string>
#include <vector>
#include <memory>

#include "RessourcesManager.h"

struct stbtt_fontinfo;

namespace KGR::_Vulkan
{
    class VulkanCore;
}

/**
 * @brief Represents a loaded TrueType font at a specific pixel size.
 *
 * Uses stb_truetype to load a TTF file and render text strings
 * into RGBA bitmaps that can be uploaded as GPU textures.
 */
class Font
{
public:
    /**
     * @brief RGBA bitmap produced by RenderText().
     */
    struct TextBitmap
    {
        std::vector<unsigned char> pixels; ///< RGBA pixel data.
        int width = 0;                     ///< Bitmap width in pixels.
        int height = 0;                    ///< Bitmap height in pixels.
    };

    /**
     * @brief Loads a TrueType font from disk.
     *
     * @param fontPath Path to the .ttf file.
     * @param fontSize Desired font size in pixels.
     */
    Font(const std::string& fontPath, float fontSize);
    ~Font();

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;
    Font(Font&& other) noexcept;
    Font& operator=(Font&& other) noexcept;

    /**
     * @brief Renders a text string into an RGBA bitmap.
     *
     * White text on a transparent background.
     *
     * @param text The text string to render.
     * @return TextBitmap containing RGBA pixel data and dimensions.
     */
    TextBitmap RenderText(const std::string& text) const;

    /**
     * @brief Returns the font size in pixels.
     */
    float GetFontSize() const;

private:
    std::vector<unsigned char> m_fontData; ///< Raw TTF file data.
    stbtt_fontinfo* m_fontInfo = nullptr;  ///< stb_truetype font info.
    float m_fontSize = 0.0f;              ///< Font size in pixels.
    float m_scale = 0.0f;                 ///< Scale factor for the font size.
    int m_ascent = 0;                      ///< Font ascent in scaled units.
    int m_descent = 0;                     ///< Font descent in scaled units.
    int m_lineGap = 0;                     ///< Line gap in scaled units.
};

/**
 * @brief Loads a font from disk and returns a unique_ptr.
 *
 * @param filePath Path to the .ttf file.
 * @param fontSize Desired font size in pixels.
 * @return A unique_ptr to a Font.
 */
std::unique_ptr<Font> LoadFont(const std::string& filePath, float fontSize);

/**
 * @brief Resource manager alias for loading and caching fonts.
 *
 * Uses:
 * - Font as the resource type
 * - float (fontSize) as the dependency
 * - LoadFont as the loading function
 */
using FontLoader =
    KGR::ResourceManager<Font,
        KGR::TypeWrapper<float>,
        LoadFont>;
