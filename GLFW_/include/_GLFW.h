#pragma once
#include "GLFW/glfw3.h"
#include <GLFW/glfw3native.h>

namespace KGR
{
	class _GLFW
	{
	public:
		_GLFW(int width, int height, const char* title, int windowHint, int windowValue);
		~_GLFW();

		void SetHint(int hint, int value);
		GLFWwindow* GetNativeWindow() const;
		bool ShouldClose() const;

	private:
		GLFWwindow* m_window;
		int m_width;
		int m_height;
	};
}