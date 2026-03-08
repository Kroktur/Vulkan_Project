//#pragma once
//#include <filesystem>
//#include <vector>
//#include "CollisionComponent.h"
//#include "Core/CameraComponent.h"
//#include "VulkanCore.h"
//#include "_GLFW.h"
//#include "Core/ManagerImple.h"
//#include "Core/Mesh.h"
//#include "Core/TrasformComponent.h"
//#include "Core/LightComponent.h"
//#include "Core/Spline.h"
//#include "Core/Frenet.h"
//#include "Core/Texture.h"
//#include "Core/Window.h"
//#include "ECS/Registry.h"
//#include "ECS/Entities.h"
//#include "Tools/Random.h"
//#include "DebugRenderer.h"
//#include "Event.h"
//#include "EventBus.h"
//
//#include "GameFiles.h"
//
//struct ControllerComponent {};
//struct LivingComponent { float health; bool isAlive; };
//struct TakeDamageComponent { float damage; };
//struct WaveManager
//{
//    WaveManager(const std::vector<float>& pts) : wavePositions(pts) {}
//    WaveManager() = default;
//    int currentWave = 0;
//    int enemiesAlive = 0;
//    bool isWaveActive = false;
//    bool platformPaused = false;
//    std::vector<float> wavePositions = { 0.2f, 0.5f, 0.8f };
//};
//
//class Game
//{
//public:
//    Game(int argc, char** argv);
//    ~Game();
//
//    void Run();
//
//private:
//    void InitScene();
//    void Update(float deltatime);
//    void UpdatePlayer(float deltatime);
//    void UpdateBullets(float deltatime);
//    void UpdateEnemies(float deltatime);
//    void UpdateCurveFollower(float deltatime);
//	void UpdateWaves(float deltatime);
//    void UpdateCamera(float deltatime);
//    void Render();
//
//private:
//    KGR::RenderWindow window;
//    KGR::ECS::Registry<KGR::ECS::Entity::_64, 100> registry;
//
//    WaveManager waveManager;
//    HermitCurve curve;
//    std::vector<glm::vec3> rmfPoints;
//    float curvesTest = 0.0f;
//};