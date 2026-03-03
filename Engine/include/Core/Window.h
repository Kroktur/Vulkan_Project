#pragma once
#include "InputManager.h"
#include "ManagerImple.h"
#include "Mesh.h"
#include "Texture.h"
#include "VulkanCore.h"
#include "_GLFW.h"

namespace KGR
{
	class RenderWindow
	{
	public:
		RenderWindow(glm::ivec2 size, const char* name, const std::filesystem::path& GlobResourcesPath);

		static void Init();
		void Destroy();
		bool ShouldClose() const;
		static void PollEvent();
		static void End();
		void Update();
		_Vulkan::VulkanCore* App();
		glm::ivec2 GetSize() const;
		template<LightData::Type Type>
		void RegisterLight(LightComponent<Type>& light, TransformComponent& transform);
		void RegisterCam(CameraComponent& cam, TransformComponent& transform);
		void RegisterRender(MeshComponent& mesh, TransformComponent& transform, TextureComponent& texture);
		void Render(const glm::vec4& clearColor = { 0,0,0,1 }, ImDrawData* imguiDraw = nullptr);
		void SetWindowState(_GLFW::WinState state, _GLFW::Monitor* monitor = nullptr);

		template<_GLFW::WinState state>
		bool IsState() const;
		InputManager* GetInputManager();
	private:
		_Vulkan::VulkanCore m_core;
		KGR::_GLFW::Window m_window;
		KGR::InputManager m_manager;
	};

	template <LightData::Type Type>
	void RenderWindow::RegisterLight(LightComponent<Type>& light, TransformComponent& transform)
	{
		LightData lightData = light.ToData();
		lightData.pos = transform.GetPosition();
		lightData.dir = transform.GetLocalAxe<RotData::Dir::Forward>();
		m_core.RegisterLight(lightData);
	}
}
