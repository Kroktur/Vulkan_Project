#pragma once
#include "Backends/imgui_impl_glfw.h"
#include "Backends/imgui_impl_vulkan.h"

#include "imgui.h"
#include "imconfig.h"
#include "imgui_internal.h"
#include "imstb_rectpack.h"
#include "imstb_textedit.h"
#include "imstb_truetype.h"

#include "VKFiles.h"
#include "GLFWFiles.h"

namespace KGR
{
	namespace _imGui
	{
		enum class RenderMode
		{
			IMGUI_RENDER_MODE_2D,
			IMGUI_RENDER_MODE_3D
		};

		class Core_ImGui
		{
		public:
			static void Initialize(_GLFW::Window* window, Core_Vulkan* vulkan);
			static void Shutdown();
			static void NewFrame();
			static void Render();
			static void SetRenderMode(RenderMode mode);
			static RenderMode GetRenderMode();

		private:
			static RenderMode s_RenderMode;

		};
	}
}