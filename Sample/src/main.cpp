#include <filesystem>
#include <iostream>
#include "Core/CameraComponent.h"
#include "VulkanCore.h"
#include "_GLFW.h"
#include "Core/ManagerImple.h"
#include "Core/Mesh.h"
#include "Core/TrasformComponent.h"
#include "Core/LightComponent.h"

int main(int argc, char** argv)
{

	std::filesystem::path exePath = argv[0];
	std::filesystem::path projectRoot = exePath.parent_path().parent_path().parent_path().parent_path().parent_path();

	fileManager::SetGlobalFIlePath(projectRoot / "Ressources");
	STBManager::SetGlobalFIlePath(projectRoot / "Ressources");
	MeshLoader::SetGlobalFIlePath(projectRoot / "Ressources");


	KGR::_GLFW::Window::Init();
	KGR::_GLFW::Window::AddHint(GLFW_CLIENT_API, GLFW_NO_API);
	KGR::_GLFW::Window::AddHint(GLFW_RESIZABLE, GLFW_TRUE);
	KGR::_GLFW::Window window;


	window.CreateMyWindow({ 1280, 720 }, "GC goes Vulkan", nullptr, nullptr);


	KGR::_Vulkan::VulkanCore app(&window.GetWindow());


	app.initVulkan();


	MeshComponent meshComp;
	meshComp.mesh = &MeshLoader::Load("Models\\viking_room.obj", &app);
	MeshComponent meshComp2;
	meshComp2.mesh = &MeshLoader::Load("Models\\CUBE.obj", &app);
	TransformComponent transform;
	transform.SetPosition(glm::vec3(0, 0, 0));
	transform.SetScale({2, 2, 2});
	transform.RotateQuat<RotData::Orientation::Pitch>(glm::radians(-90.0f));
	TransformComponent transform2;
	transform2.SetScale({ 10,0.1,10 });
	CameraComponent cam = CameraComponent :: Create(45.0f,static_cast<float>(window.GetSize().x),static_cast<float>(window.GetSize().y),0.01f,1000.0f,CameraComponent::Type::Perspective);
	TransformComponent camTransform;

	auto lComp = LightComponent<LightData::Type::Directional>::Create({ 1,1,1 }, { 1,1,1 }, 10.0f);
	
	TransformComponent lTransform;
	//lTransform.SetPosition({ 0,1,0 });

	lTransform.LookAtDir({ 1,-1,1 });
	auto loclAxes = lTransform.GetLocalAxe<RotData::Dir::Forward>();
	std::cout << loclAxes.z;


	auto lComp2 = LightComponent<LightData::Type::Point>::Create({ 0,0,1 }, { 1,1,1 }, 10.0f, 10.0f);

	TransformComponent lTransform2;
	lTransform2.SetPosition({ 5,1,0 });

	auto lComp3 = LightComponent<LightData::Type::Spot>::Create({ 0,1,0 }, { 1,1,1 }, 100.0f, 10.0f,glm::radians(45.0f),10.0f);

	TransformComponent lTransform3;
	lTransform3.SetPosition({ -5,1,0 });
	lTransform3.LookAtDir({1,-1,0});

	do
	{
		// event
		KGR::_GLFW::Window::PollEvent();
		//Update
		static auto lastTime = std::chrono::high_resolution_clock::now();
		static float angle = 0.0f;
		const float rotationSpeed = 1.0f;

		auto currentTime = std::chrono::high_resolution_clock::now();
		float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
		lastTime = currentTime;

		angle += deltaTime * rotationSpeed;


		float radius = 5.0f;
		float camX = std::cos(angle) * radius;
		float camY = 5.0f;
		float camZ = std::sin(angle) * radius;

		camTransform.SetPosition({ camX, camY, camZ });
		camTransform.LookAt({ 0.0f, 0.0f, 0.0f });
		cam.UpdateCamera(camTransform.GetFullTransform());

		// Render
		
		app.RegisterCam(cam, camTransform);
		app.RegisterRender(meshComp2, transform2);
		app.RegisterLight(lComp,lTransform);
		app.RegisterLight(lComp2, lTransform2);
		app.RegisterLight(lComp3, lTransform3);

		app.Render({0.53f,0.81f,0.92f ,1.0f});
		
	}
	while (!window.ShouldClose());

	 window.DestroyMyWindow();
	 MeshLoader::UnloadAll();
     KGR::_GLFW::Window::Destroy();

	
}

