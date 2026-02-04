#include "GLFWFiles.h"
#include <iostream>

KGR::_GLFW::_GLFW(int width, int height, const char* title, int windowHint, int windowValue)
	: m_width(width)
	, m_height(height)
{
	glfwInit();
	SetHint(windowHint, windowValue); // NOT THE RIGHT THING TO DO !! NEED TO CHANGE THIS ASAP
	m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!m_window)
		throw std::runtime_error("Failed to creare the window");
}

KGR::_GLFW::~_GLFW()
{
	if (m_window)
		glfwDestroyWindow(m_window);
}

void KGR::_GLFW::SetHint(int hint, int value)
{
	glfwWindowHint(hint, value);
}

GLFWwindow* KGR::_GLFW::GetNativeWindow() const
{
	return m_window;
}

bool KGR::_GLFW::ShouldClose() const
{
	return glfwWindowShouldClose(m_window);
}
