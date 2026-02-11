#include "_ImGui.h"

namespace KGR
{
	namespace _imGui
	{
		RenderMode Core_ImGui::s_RenderMode = RenderMode::IMGUI_RENDER_MODE_2D;

		void Core_ImGui::Initialize(_GLFW::Window* window, Core_Vulkan* vulkan)
		{
			ImGui::CreateContext();
			ImGui_ImplGlfw_InitForVulkan(&window->GetWindow(), true);
		}

		void Core_ImGui::Shutdown()
		{
			ImGui_ImplVulkan_Shutdown();
			ImGui_ImplGlfw_Shutdown();
			ImGui::DestroyContext();
		}

		void Core_ImGui::NewFrame()
		{
			ImGui_ImplVulkan_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
		}

		void Core_ImGui::Render()
		{
			ImGui::Render();
		}

		void Core_ImGui::SetRenderMode(RenderMode mode)
		{
			s_RenderMode = mode;
		}

		RenderMode Core_ImGui::GetRenderMode()
		{
			return s_RenderMode;
		}
	}
}