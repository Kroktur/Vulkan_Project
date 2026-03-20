#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include "Core/Font.h"

#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <cmath>

Font::Font(const std::string& fontPath, float fontSize)
    : m_fontSize(fontSize)
{
    std::ifstream file(fontPath, std::ios::binary | std::ios::ate);
    if (!file.is_open())
        throw std::runtime_error("Failed to open font file : " + fontPath);

    auto size = file.tellg();
    file.seekg(0, std::ios::beg);
    m_fontData.resize(static_cast<size_t>(size));
    file.read(reinterpret_cast<char*>(m_fontData.data()), size);

    m_fontInfo = new stbtt_fontinfo();
    if (!stbtt_InitFont(m_fontInfo, m_fontData.data(), stbtt_GetFontOffsetForIndex(m_fontData.data(), 0)))
        throw std::runtime_error("Failed to initialize font: " + fontPath);

    m_scale = stbtt_ScaleForPixelHeight(m_fontInfo, m_fontSize);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(m_fontInfo, &ascent, &descent, &lineGap);
    m_ascent = static_cast<int>(ascent * m_scale);
    m_descent = static_cast<int>(descent * m_scale);
    m_lineGap = static_cast<int>(lineGap * m_scale);    
}

Font::~Font()
{
    delete m_fontInfo;
}

Font::Font(Font&& other) noexcept
    : m_fontData(std::move(other.m_fontData))
    , m_fontInfo(other.m_fontInfo)
    , m_fontSize(other.m_fontSize)
    , m_scale(other.m_scale)
    , m_ascent(other.m_ascent)
    , m_descent(other.m_descent)
    , m_lineGap(other.m_lineGap)
{
    other.m_fontInfo = nullptr;
}

Font& Font::operator=(Font&& other) noexcept
{
    if (this != &other)
    {
        delete m_fontInfo;
        m_fontData = std::move(other.m_fontData);
        m_fontInfo = other.m_fontInfo;
        m_fontSize = other.m_fontSize;
        m_scale = other.m_scale;
        m_ascent = other.m_ascent;
        m_descent = other.m_descent;
        m_lineGap = other.m_lineGap;
        other.m_fontInfo = nullptr;
    }
    return *this;
}

Font::TextBitmap Font::RenderText(const std::string& text) const
{
    if (text.empty() || !m_fontInfo)
        return TextBitmap{ {}, 1, 1 };

    int totalWidth = 0;
    int lineHeight = m_ascent - m_descent;

    for (size_t i = 0; i < text.size(); ++i)
    {
        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(m_fontInfo, text[i], &advanceWidth, &leftSideBearing);
        totalWidth += static_cast<int>(advanceWidth * m_scale);

        if (i + 1 < text.size())
        {
            int kern = stbtt_GetCodepointKernAdvance(m_fontInfo, text[i], text[i + 1]);
            totalWidth += static_cast<int>(kern * m_scale);
        }
    }

    totalWidth = std::max(totalWidth, 1);
    lineHeight = std::max(lineHeight, 1);

    std::vector<unsigned char> alpha(totalWidth * lineHeight, 0);

    float xCursor = 0.0f;
    for (size_t i = 0; i < text.size(); ++i)
    {
        int ix0, iy0, ix1, iy1;
        stbtt_GetCodepointBitmapBox(m_fontInfo, text[i], m_scale, m_scale, &ix0, &iy0, &ix1, &iy1);

        int charW = ix1 - ix0;
        int charH = iy1 - iy0;

        int xPos = static_cast<int>(xCursor) + ix0;
        int yPos = m_ascent + iy0;

        if (charW > 0 && charH > 0)
        {
            int safeX = std::max(xPos, 0);
            int safeY = std::max(yPos, 0);
            int offsetX = safeX - xPos;
            int offsetY = safeY - yPos;

            int renderW = std::min(charW - offsetX, totalWidth - safeX);
            int renderH = std::min(charH - offsetY, lineHeight - safeY);

            if (renderW > 0 && renderH > 0)
            {
                std::vector<unsigned char> glyphBitmap(charW * charH, 0);
                stbtt_MakeCodepointBitmap(m_fontInfo, glyphBitmap.data(),
                    charW, charH, charW,
                    m_scale, m_scale, text[i]);

                for (int row = 0; row < renderH; ++row)
                {
                    for (int col = 0; col < renderW; ++col)
                    {
                        int srcIdx = (row + offsetY) * charW + (col + offsetX);
                        int dstIdx = (safeY + row) * totalWidth + (safeX + col);
                        unsigned char val = glyphBitmap[srcIdx];
                        if (val > alpha[dstIdx])
                            alpha[dstIdx] = val;
                    }
                }
            }
        }

        int advanceWidth, leftSideBearing;
        stbtt_GetCodepointHMetrics(m_fontInfo, text[i], &advanceWidth, &leftSideBearing);
        xCursor += advanceWidth * m_scale;

        if (i + 1 < text.size())
        {
            int kern = stbtt_GetCodepointKernAdvance(m_fontInfo, text[i], text[i + 1]);
            xCursor += kern * m_scale;
        }
    }

    std::vector<unsigned char> rgba(totalWidth * lineHeight * 4);
    for (int i = 0; i < totalWidth * lineHeight; ++i)
    {
        rgba[i * 4 + 0] = 255;
        rgba[i * 4 + 1] = 255;
        rgba[i * 4 + 2] = 255;
        rgba[i * 4 + 3] = alpha[i];
    }

    return TextBitmap{ std::move(rgba), totalWidth, lineHeight };
}

float Font::GetFontSize() const
{
    return m_fontSize;
}

std::unique_ptr<Font> LoadFont(const std::string& filePath, float fontSize)
{
    return std::make_unique<Font>(filePath, fontSize);
}
