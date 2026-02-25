#include <filesystem>

#include "imgui.h"
#include "VulkanCore.h"
#include "_GLFW.h"
#include "Backends/imgui_impl_glfw.h"
#include "Backends/imgui_impl_vulkan.h"
#include "Core/ManagerImple.h"
#include "KGR_ImGui.h"


int main(int argc, char** argv)
{

	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();

	fileManager::SetGlobalFIlePath(projectRoot / "Ressources");
	STBManager::SetGlobalFIlePath(projectRoot / "Ressources");
	TOLManager::SetGlobalFIlePath(projectRoot / "Ressources");


	KGR::_GLFW::Window::Init();
	KGR::_GLFW::Window::AddHint(GLFW_CLIENT_API, GLFW_NO_API);
	KGR::_GLFW::Window::AddHint(GLFW_RESIZABLE, GLFW_TRUE);
	KGR::_GLFW::Window window;


	window.CreateMyWindow({ 1400, 900 }, "GC goes Vulkan", nullptr, nullptr);


	KGR::_Vulkan::VulkanCore app(&window.GetWindow());
	KGR::_ImGui::ImGuiCore imguiCore;

	app.initVulkan();

	imguiCore.InitImGui(&app, &window);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGuiStyle& style = ImGui::GetStyle();
	style.ScaleAllSizes(1.f);
	style.FontScaleDpi = 3.0f;

	do
	{
		KGR::_GLFW::Window::PollEvent();

		imguiCore.BeginFrame(KGR::_ImGui::ContextTarget::Engine);

		{
			static int counter;

			ImGui::Begin("LE H");
			if (ImGui::Button("Click on me daddy"))
				counter++;

			ImGui::Text("Daddy Ludwig <3 = %d", counter);

			ImGui::End();
		}

		 ImGui::Render();
		 app.drawFrame(ImGui::GetDrawData());
	} 
	while (!window.ShouldClose());

	app.GetDevice().Get().waitIdle();

	imguiCore.Destroy();
	window.DestroyMyWindow();
    KGR::_GLFW::Window::Destroy();

}