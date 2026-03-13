#include "../../Editor/include/Context.h"

namespace KGR
{
	namespace Editor
	{
		Context::Context(KGR::_ImGui::ImGuiCore& imgui, KGR::_Vulkan::VulkanCore& vulkan)
			: m_imGui(imgui), m_vulkan(vulkan)
		{
		}

		void Context::Init(uint32_t viewportWidth, uint32_t viewportHeight)
		{
			RegisterClone();
			RegisterInspector();

			m_toolbar = std::make_unique<Toolbar>(&m_editorScene);
			m_hierarchy = std::make_unique<HierarchyPanel>(&m_editorScene);
			m_inspector = std::make_unique<InspectorPanel>(&m_editorScene);
			m_viewport = std::make_unique<Viewport>(m_imGui, m_vulkan);

			m_camera = CameraComponent::Create(
				45.0f,
				static_cast<float>(viewportWidth),
				static_cast<float>(viewportHeight),
				0.01f, 1000.0f,
				CameraComponent::Type::Perspective
			);

			m_cameraTransform.SetPosition({ 0.0f, 5.0f, 5.0f });
			m_cameraTransform.LookAt({ 0.0f, 0.0f, 0.0f });
			m_camera.UpdateCamera(m_cameraTransform.GetFullTransform());
			m_imGui.SetCamera(&m_camera, &m_cameraTransform);

			VkDevice vkDevice = *m_vulkan.GetDevice().Get();
			VkPhysicalDevice vkPhysicDevice = *m_vulkan.GetPhysicalDevice().Get();
			VkDescriptorPool vkPool = *m_vulkan.GetDescriptorPool().Get();
			VkFormat colorFormat = static_cast<VkFormat>(m_vulkan.GetSwapChain().GetFormat().format);
			m_offscreen.Create(vkDevice, vkPhysicDevice, vkPool, viewportWidth, viewportHeight, colorFormat);
			m_viewport->SetSceneDescriptor(m_offscreen.GetDescriptorSet());

			Layer::ApplyStyle();
		}

		void Context::Render()
		{
			HandleResize();

			Layer::BeginDockspace();

			m_toolbar->Render();

			Scene* activeScene = m_toolbar->GetActiveScene();
			m_hierarchy->SetScene(activeScene);
			m_inspector->SetScene(activeScene);

			m_hierarchy->Render();
			m_inspector->Render(m_hierarchy->GetSelectedEntity());

			m_viewport->Render(m_hierarchy->GetSelectedEntity(), activeScene, &m_camera);

			Layer::EndDockspace();
		}

		void Context::HandleResize()
		{
			if (!m_viewport->WasResizedThisFrame())
				return;

			glm::vec2 newSize = m_viewport->GetSize();

			m_vulkan.GetDevice().Get().waitIdle();

			VkDevice vkDevice = *m_vulkan.GetDevice().Get();
			VkPhysicalDevice vkPhysicDevice = *m_vulkan.GetPhysicalDevice().Get();
			VkDescriptorPool vkPool = *m_vulkan.GetDescriptorPool().Get();

			m_offscreen.Resize(vkDevice, vkPhysicDevice, vkPool,
				static_cast<uint32_t>(newSize.x),
				static_cast<uint32_t>(newSize.y));

			m_viewport->SetSceneDescriptor(m_offscreen.GetDescriptorSet());

			m_camera = CameraComponent::Create(
				45.0f, newSize.x, newSize.y,
				0.01f, 1000.0f,
				CameraComponent::Type::Perspective);

			m_camera.UpdateCamera(m_cameraTransform.GetFullTransform());
			m_imGui.SetCamera(&m_camera, &m_cameraTransform);
		}

		void Context::Destroy()
		{
			VkDevice vkDevice = *m_vulkan.GetDevice().Get();
			m_offscreen.Destroy(vkDevice);
		}

		Scene* Context::GetActiveScene() const
		{
			return m_toolbar->GetActiveScene();
		}

		SceneEntity Context::GetSelectedEntity() const
		{
			return m_hierarchy->GetSelectedEntity();
		}

		CameraComponent& Context::GetCamera()
		{
			return m_camera;
		}

		TransformComponent& Context::GetCamTransform()
		{
			return m_cameraTransform;
		}

		KGR::Editor::Offscreen& Context::GetOffscreen()
		{
			return m_offscreen;
		}

		glm::vec2 Context::GetViewportPos() const
		{
			return m_viewport->GetPos();
		}

		glm::vec2 Context::GetViewportSize() const
		{
			return m_viewport->GetSize();
		}

		void Context::RegisterClone()
		{
			m_editorScene.RegisterClone<NameComponent>();
			m_editorScene.RegisterClone<HierarchyComponent>();
			m_editorScene.RegisterClone<TransformComponent>();
			m_editorScene.RegisterClone<MeshComponent>();
		}

		void Context::RegisterInspector()
		{
			m_editorScene.RegisterInspector<MeshComponent>("Mesh",
				[this](SceneEntity e, SceneRegistry& registry)
				{
					if (!registry.HasComponent<MeshComponent>(e))
					{
						if (ImGui::Button("Add Mesh Component"))
							registry.AddComponent<MeshComponent>(e);

						return;
					}

					auto& meshComponent = registry.GetComponent<MeshComponent>(e);

					const std::string label = meshComponent.sourcePath.empty() ? "(none)"
						: std::filesystem::path(meshComponent.sourcePath).filename().string();

					ImGui::LabelText("File", "%s", label.c_str());

					if (ImGui::Button("Load mesh..."))
					{
						std::string newPath = KGR::_ImGui::ImGuiCore::OpenFile();
						if (!newPath.empty())
						{
							m_vulkan.GetDevice().Get().waitIdle();

							if (!meshComponent.sourcePath.empty())
								MeshLoader::Unload(meshComponent.sourcePath);

							meshComponent.sourcePath = newPath;
							meshComponent.mesh = &MeshLoader::Load(newPath, &m_vulkan);
						}
					}

					ImGui::SameLine();

					if (meshComponent.mesh && ImGui::Button("Clear"))
					{
						m_vulkan.GetDevice().Get().waitIdle();
						MeshLoader::Unload(meshComponent.sourcePath);
						meshComponent.mesh = nullptr;
						meshComponent.sourcePath.clear();
					}
				});
		}
	}
}