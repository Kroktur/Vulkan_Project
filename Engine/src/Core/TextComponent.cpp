#include "Core/TextComponent.h"

TextComponent::TextComponent(const glm::vec4& color) : m_color(color)
{}

void TextComponent::SetColor(const glm::vec4& color)
{ m_color = color; }

glm::vec4 TextComponent::GetColor() const
{ return m_color; }
