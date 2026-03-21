#pragma once
#include <glm/vec4.hpp>

struct TextComponent
{
    TextComponent() = default;
    explicit TextComponent(const glm::vec4& color);
    void SetColor(const glm::vec4& color);
    glm::vec4 GetColor() const;

private:
    glm::vec4 m_color = { 1.0f, 1.0f, 1.0f, 1.0f };
};
