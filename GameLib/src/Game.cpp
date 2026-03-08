//#include "Game.h"
//#include "GameFiles.h"
//#include "glm/glm.hpp"
//#include "Tools/Random.h"
//#include "Core/Spline.h"
//#include <chrono>
//#include <iostream>
//
//Game::Game(int argc, char** argv)
//    : window(
//        KGR::RenderWindow(
//            { 1920, 1080 },
//            "test",
//            std::filesystem::path(argv[0])
//            .parent_path().parent_path().parent_path()
//            .parent_path().parent_path() / "Ressources"
//        )
//    )
//{
//    KGR::RenderWindow::Init();
//    window.GetInputManager()->SetMode(GLFW_CURSOR_DISABLED);
//    InitScene();
//}
//
//Game::~Game()
//{
//    window.Destroy();
//    KGR::RenderWindow::End();
//}
//
//// -----------------------------------------------------------------------------
//// Scene Initialization
//// -----------------------------------------------------------------------------
//void Game::InitScene()
//{
//    using ecsType = KGR::ECS::Registry<KGR::ECS::Entity::_64, 100>;
//    registry = ecsType{};
//
//    KGR::Tools::Random rd;
//    float y = -10;
//
//    // -------------------
//    // Player
//    // -------------------
//    {
//        auto player = registry.CreateEntity();
//
//        MeshComponent meshComp;
//        meshComp.mesh = &MeshLoader::Load("Models\\CUBE.obj", window.App());
//
//        CameraComponent camComp = CameraComponent::Create(45.0f,
//            static_cast<float>(window.GetSize().x),
//            static_cast<float>(window.GetSize().y),
//            0.01f, 1000000.0f,
//            CameraComponent::Type::Perspective);
//
//        TransformComponent camTransform;
//
//        LivingComponent playerLife{ 5.0f, true };
//
//        TextureComponent texture;
//        texture.SetSize(meshComp.mesh->GetSubMeshesCount());
//        for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
//            texture.AddTexture(i, &TextureLoader::Load("Textures\\BaseTexture.png", window.App()));
//
//        CollisionComp collider;
//        collider.collider = &ColliderManager::Load("playerCollider", meshComp.mesh);
//
//        registry.AddComponents<MeshComponent, CameraComponent, TransformComponent, TextureComponent,
//            ControllerComponent, PlayerComponent, KGR::GameLib::WeaponComponent,
//            LivingComponent, CollisionComp>
//            (player, std::move(meshComp), std::move(camComp), std::move(camTransform),
//                std::move(texture), ControllerComponent{}, PlayerComponent{},
//                KGR::GameLib::WeaponComponent{}, std::move(playerLife), std::move(collider));
//    }
//
//    // -------------------
//    // Floor
//    // -------------------
//    {
//        MeshComponent meshComp;
//        meshComp.mesh = &MeshLoader::Load("Models\\CUBE.obj", window.App());
//
//        TransformComponent meshTransform;
//        meshTransform.SetScale({ 100,1,100 });
//        meshTransform.SetPosition({ 50,y,-50 });
//
//        TextureComponent texture;
//        texture.SetSize(meshComp.mesh->GetSubMeshesCount());
//        for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
//            texture.AddTexture(i, &TextureLoader::Load("Textures\\BaseTexture.png", window.App()));
//
//        registry.AddComponents<MeshComponent, TransformComponent, TextureComponent>
//            (registry.CreateEntity(), std::move(meshComp), std::move(meshTransform), std::move(texture));
//    }
//
//    // -------------------
//    // Buildings
//    // -------------------
//    auto yScale = rd.getRandomNumberRange(5.0f, 100.0f, 8);
//    std::vector<std::pair<glm::vec3, glm::vec3>> buildingDatas
//    { {{25,y + yScale[0] / 2.0f,-90},{15 / 1.5f,yScale[0] ,10 / 1.5f}},
//      {{80,y + yScale[1] / 2.0f,-85},{20 / 1.5f,yScale[1] ,10 / 1.5f}},
//      {{40,y + yScale[2] / 2.0f,-70},{20 / 1.5f,yScale[2] ,10 / 1.5f}},
//      {{10,y + yScale[3] / 2.0f,-67.5f},{10 / 1.5f,yScale[3] ,15 / 1.5f}},
//      {{22.5,y + yScale[4] / 2.0f,-40},{35 / 1.5f, yScale[4],15 / 1.5f}},
//      {{67.5f,y + yScale[5] / 2.0f,-45},{25 / 1.5f, yScale[5],20 / 1.5f}},
//      {{105,y + yScale[6] / 2.0f,-52.5},{20 / 1.5f, yScale[6],35 / 1.5f}},
//      {{90,y + yScale[7] / 2.0f,-15},{30 / 1.5f,yScale[7] ,20 / 1.5f}} };
//
//    for (int i = 0; i < buildingDatas.size(); ++i)
//    {
//        MeshComponent meshComp;
//        meshComp.mesh = &MeshLoader::Load("Models\\CUBE.obj", window.App());
//
//        TransformComponent meshTransform;
//        meshTransform.SetScale(buildingDatas[i].second);
//        meshTransform.SetPosition(buildingDatas[i].first);
//
//        TextureComponent texture;
//        texture.SetSize(meshComp.mesh->GetSubMeshesCount());
//        for (int i = 0; i < meshComp.mesh->GetSubMeshesCount(); ++i)
//            texture.AddTexture(i, &TextureLoader::Load("Textures\\BaseTexture.png", window.App()));
//
//        registry.AddComponents<MeshComponent, TransformComponent, TextureComponent>
//            (registry.CreateEntity(), std::move(meshComp), std::move(meshTransform), std::move(texture));
//    }
//
//    // -------------------
//    // Lights
//    // -------------------
//    auto colorTransform = [](const glm::vec3& color) { return color / 255.0f; };
//
//    // Directional light
//    {
//        auto light = registry.CreateEntity();
//        auto lComp = LightComponent<LightData::Type::Directional>::Create(colorTransform({ 255,240,200 }), { 1,1,1 }, 1.0f);
//        TransformComponent lTransform;
//        lTransform.LookAt({ 1,-1,0 });
//        registry.AddComponents<LightComponent<LightData::Type::Directional>, TransformComponent>(light, std::move(lComp), std::move(lTransform));
//    }
//
//    // -------------------
//    // Hermit Curve & RMF
//    // -------------------
//    std::vector<glm::vec3> points{
//        {-5,0,5}, {10,0,-20}, {20,0,-10}, {35,0,-15}, {50,0,-25}, {60,0,-20}, {70,0,-25},
//        {85,0,-35}, {90,0,-50}, {70,0,-70}, {55,0,-60}, {40,0,-50}, {25,0,-60}, {20,0,-75}, {10,0,-90}, {0,0,-100}, {-5,0,-105}
//    };
//    curve = HermitCurve::FromPoints(points, 0);
//    const float rmfStep = 0.001f;
//    const int rmfSampleCount = static_cast<int>(curve.MaxT() / rmfStep) + 1;
//
//    rmfPoints.reserve(rmfSampleCount);
//    for (int i = 0; i < rmfSampleCount; ++i)
//        rmfPoints.push_back(curve.Compute(i * rmfStep));
//
//    auto rmfFrames = KGR::RMF::BuildFrames(rmfPoints, KGR::RMF::EstimateForwardDirs(rmfPoints));
//
//    // Spot lights along curve
//    uint32_t count = points.size() / 2;
//    float maxT = curve.MaxT() - 0.001f;
//    std::vector<float> result;
//    float step = maxT / (count - 1);
//    for (uint32_t i = 0; i < count; ++i)
//    {
//        result.push_back(i * step);
//        auto light = registry.CreateEntity();
//        auto lComp = LightComponent<LightData::Type::Spot>::Create({ 0.4f,0.9f,0.6f }, { 1,1,1 }, 100.0f, 1.0f, glm::radians(5.0f), 1);
//        TransformComponent lTransform;
//        lTransform.LookAt({ 0,-1,0 });
//        static glm::vec3 upCoord = { 0,2,0 };
//        lTransform.SetPosition(curve.Compute(i * step) + upCoord);
//        registry.AddComponents<LightComponent<LightData::Type::Spot>, TransformComponent>(light, std::move(lComp), std::move(lTransform));
//    }
//
//    waveManager = WaveManager(result);
//}
//
//// -----------------------------------------------------------------------------
//// Run Loop
//// -----------------------------------------------------------------------------
//void Game::Run()
//{
//    auto lastTime = std::chrono::high_resolution_clock::now();
//    while (!window.ShouldClose())
//    {
//        KGR::RenderWindow::PollEvent();
//        window.Update();
//
//        auto currentTime = std::chrono::high_resolution_clock::now();
//        float deltaTime = std::chrono::duration<float>(currentTime - lastTime).count();
//        lastTime = currentTime;
//
//        // Update
//        Update(deltaTime);
//
//        // Render
//        Render();
//    }
//}
//
//// -----------------------------------------------------------------------------
//// Update
//// -----------------------------------------------------------------------------
//void Game::Update(float deltaTime)
//{
//	UpdatePlayer(deltaTime);
//	UpdateBullets(deltaTime);
//	UpdateEnemies(deltaTime);
//    UpdateCurveFollower(deltaTime);
//    UpdateWaves(deltaTime);
//}
//
//void Game::UpdatePlayer(float deltatime)
//{
//    // Player input & shoot
//    {
//        auto view = registry.GetAllComponentsView<PlayerComponent, KGR::GameLib::WeaponComponent, TransformComponent>();
//        auto* input = window.GetInputManager();
//        for (auto& e : view)
//        {
//            auto& weapon = registry.GetComponent<KGR::GameLib::WeaponComponent>(e);
//            auto& transform = registry.GetComponent<TransformComponent>(e);
//            weapon.cooldown -= deltatime;
//            weapon.UpdateReload(deltatime);
//
//            // Weapon switch
//            if (input->IsKeyPressed(KGR::Key::Num1)) weapon.SwitchWeapon(KGR::GameLib::WeaponType::Shotgun);
//            if (input->IsKeyPressed(KGR::Key::Num2)) weapon.SwitchWeapon(KGR::GameLib::WeaponType::Auto);
//            if (input->IsKeyPressed(KGR::Key::Num3)) weapon.SwitchWeapon(KGR::GameLib::WeaponType::Sniper);
//
//            // Reload
//            if (input->IsKeyPressed(KGR::Key::R) && !weapon.isReloading && weapon.currentAmmo < weapon.GetCurrentWeaponData().maxAmmo)
//                weapon.StartReload();
//
//            if (weapon.isReloading) continue;
//
//            // Shoot
//            if (input->IsMouseDown(KGR::Mouse::Left) && weapon.cooldown <= 0.0f && weapon.currentAmmo > 0)
//            {
//                weapon.cooldown = weapon.GetCurrentWeaponData().fireRate;
//                weapon.currentAmmo--;
//
//                glm::vec3 forward = transform.GetLocalAxe<RotData::Dir::Forward>();
//                if (weapon.current == KGR::GameLib::WeaponType::Shotgun)
//                {
//                    static KGR::Tools::Random rng;
//                    for (int i = 0; i < weapon.GetCurrentWeaponData().maxAmmo; ++i)
//                    {
//                        glm::vec3 spreadDir = forward;
//                        float sx = rng.getRandomNumber(-weapon.GetCurrentWeaponData().spread, weapon.GetCurrentWeaponData().spread);
//                        float sy = rng.getRandomNumber(-weapon.GetCurrentWeaponData().spread, weapon.GetCurrentWeaponData().spread);
//                        spreadDir = glm::normalize(spreadDir + transform.GetLocalAxe<RotData::Dir::Right>() * sx + transform.GetLocalAxe<RotData::Dir::Up>() * sy);
//                        weapon.CreateBullet(registry, window, transform.GetPosition(), spreadDir);
//                    }
//                }
//                else
//                    weapon.CreateBullet(registry, window, transform.GetPosition(), forward);
//            }
//        }
//    }
//}
//
//void Game::UpdateBullets(float deltatime)
//{
//    // Bullets
//    {
//        auto view = registry.GetAllComponentsView<KGR::GameLib::BulletComponent, TransformComponent, LivingTimeComponent, CollisionComp>();
//        for (auto& e : view)
//        {
//            auto& bullet = registry.GetComponent<KGR::GameLib::BulletComponent>(e);
//            auto& bulletTransform = registry.GetComponent<TransformComponent>(e);
//            auto& bulletCol = registry.GetComponent<CollisionComp>(e);
//
//            auto bulletOBB = bulletCol.collider->ComputeGlobalOBB(bulletTransform.GetScale(), bulletTransform.GetPosition(), bulletTransform.GetOrientation());
//            bulletTransform.SetPosition(bulletTransform.GetPosition() + bullet.direction * bullet.speed * deltatime);
//            bullet.lifetime -= deltatime;
//            if (bullet.lifetime <= 0.0f) { registry.DestroyEntity(e); continue; }
//
//            // Collision with enemies
//            auto enemies = registry.GetAllComponentsView<KGR::GameLib::EnemyComponent, TransformComponent, CollisionComp>();
//            for (auto enemyEntity : enemies)
//            {
//                auto& enemy = registry.GetComponent<KGR::GameLib::EnemyComponent>(enemyEntity);
//                auto& enemyTransform = registry.GetComponent<TransformComponent>(enemyEntity);
//                auto& enemyCol = registry.GetComponent<CollisionComp>(enemyEntity);
//
//                auto enemyOBB = enemyCol.collider->ComputeGlobalOBB(enemyTransform.GetScale(), enemyTransform.GetPosition(), enemyTransform.GetOrientation());
//                auto collision = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(bulletOBB, enemyOBB);
//                if (collision.IsColliding())
//                {
//                    enemy.health -= bullet.damage;
//                    registry.DestroyEntity(e);
//                    break;
//                }
//            }
//        }
//    }
//}
//
//// -----------------------------------------------------------------------------
//// Update enemies and handle combat
//// -----------------------------------------------------------------------------
//void Game::UpdateEnemies(float deltaTime)
//{
//    glm::vec3 playerPos{ 0.0f };
//    auto playerView = registry.GetAllComponentsView<PlayerComponent, TransformComponent>();
//    for (auto& e : playerView) { playerPos = registry.GetComponent<TransformComponent>(e).GetPosition(); break; }
//
//    // Spawn new wave if needed
//    if (!waveManager.isWaveActive && waveManager.currentWave < waveManager.wavePositions.size())
//    {
//        if (curvesTest >= waveManager.wavePositions[waveManager.currentWave])
//        {
//            waveManager.isWaveActive = true;
//            waveManager.platformPaused = true;
//
//            int enemyCount = 5 + waveManager.currentWave * 3;
//            for (int i = 0; i < enemyCount; i++)
//            {
//                KGR::GameLib::AIComponent ai;
//                ai.SpawnEnemy(registry, window, playerPos);
//                waveManager.enemiesAlive++;
//            }
//        }
//    }
//
//    // Update each enemy
//    auto enemies = registry.GetAllComponentsView<KGR::GameLib::AIComponent, TransformComponent, KGR::GameLib::EnemyComponent, CollisionComp>();
//    for (auto& enemyEntity : enemies)
//    {
//        auto& enemyTransform = registry.GetComponent<TransformComponent>(enemyEntity);
//        auto& enemyAI = registry.GetComponent<KGR::GameLib::AIComponent>(enemyEntity);
//        auto& enemy = registry.GetComponent<KGR::GameLib::EnemyComponent>(enemyEntity);
//        auto& enemyCol = registry.GetComponent<CollisionComp>(enemyEntity);
//
//        auto playerEntity = *playerView.begin();
//        auto& playerTransform = registry.GetComponent<TransformComponent>(playerEntity);
//        auto& playerCol = registry.GetComponent<CollisionComp>(playerEntity);
//
//        auto playerOBB = playerCol.collider->ComputeGlobalOBB(playerTransform.GetScale(), playerTransform.GetPosition(), playerTransform.GetOrientation());
//        auto enemyOBB = enemyCol.collider->ComputeGlobalOBB(enemyTransform.GetScale(), enemyTransform.GetPosition(), enemyTransform.GetOrientation());
//
//        enemyAI.UpdateTarget(playerPos);
//        glm::vec3 newPos = enemyAI.Update(deltaTime, enemyTransform.GetPosition(), enemy.speed);
//        enemyTransform.SetPosition(newPos);
//
//        auto collision = KGR::SeparatingAxisTheorem::CheckCollisionOBB3D(enemyOBB, playerOBB);
//        enemy.timeSinceLastAttack -= deltaTime;
//
//        if (collision.IsColliding() && enemy.timeSinceLastAttack <= 0.0f)
//        {
//            enemy.timeSinceLastAttack = enemy.attackCooldown;
//            auto& life = registry.GetComponent<LivingComponent>(playerEntity);
//            life.health -= enemy.damage;
//            if (life.health <= 0.0f)
//            {
//                life.isAlive = false;
//                std::cout << "Player Dead\n";
//            }
//        }
//
//        if (collision.IsColliding())
//            enemyTransform.SetPosition(enemyTransform.GetPosition() - collision.GetCollisionNormal() * collision.GetPenetration());
//
//        if (enemy.health <= 0.0f)
//        {
//            registry.DestroyEntity(enemyEntity);
//            waveManager.enemiesAlive--;
//            std::cout << "Enemy defeated!\n";
//        }
//    }
//
//    // Wave cleared
//    if (waveManager.enemiesAlive <= 0 && waveManager.isWaveActive)
//    {
//        waveManager.isWaveActive = false;
//        waveManager.platformPaused = false;
//        waveManager.currentWave++;
//        std::cout << "Wave cleared!\n";
//    }
//}
//
//// -----------------------------------------------------------------------------
//// Update entities that follow the Hermit curve
//// -----------------------------------------------------------------------------
//void Game::UpdateCurveFollower(float deltatime)
//{
//	auto rmfFrames = KGR::RMF::BuildFrames(rmfPoints, KGR::RMF::EstimateForwardDirs(rmfPoints));
//    auto followers = registry.GetAllComponentsView<ControllerComponent, TransformComponent, MeshComponent>();
//    for (auto& e : followers)
//    {
//        auto& transform = registry.GetComponent<TransformComponent>(e);
//        transform.SetPosition(curve.Compute(curvesTest));
//
//        int frameIndex = glm::clamp(static_cast<int>(curvesTest / 0.001f), 0, static_cast<int>(rmfFrames.size() - 1));
//        if (!waveManager.isWaveActive)
//            transform.SetOrientation(glm::quatLookAt(rmfFrames[frameIndex].forward, rmfFrames[frameIndex].up));
//    }
//}
//
//// -----------------------------------------------------------------------------
//// Update wave manager and curve progress
//// -----------------------------------------------------------------------------
//void Game::UpdateWaves(float deltatime)
//{
//    if (!waveManager.platformPaused)
//    {
//        curvesTest += 0.001f;
//        if (curvesTest > curve.MaxT())
//            curvesTest = 0.0f;
//
//        if (waveManager.currentWave >= waveManager.wavePositions.size())
//        {
//            waveManager.platformPaused = true;
//            std::cout << "Game Finished!\n";
//        }
//    }
//}
//
//void Game::UpdateCamera(float deltatime)
//{
//    auto camView = registry.GetAllComponentsView<CameraComponent, TransformComponent>();
//    for (auto& e : camView)
//        registry.GetComponent<CameraComponent>(e).UpdateCamera(registry.GetComponent<TransformComponent>(e).GetTranslationMatrix() * registry.GetComponent<TransformComponent>(e).GetRotationMatrix() * registry.GetComponent<TransformComponent>(e).GetScaleMatrix());
//}
//
//// -----------------------------------------------------------------------------
//// Render
//// -----------------------------------------------------------------------------
//void Game::Render()
//{
//    // Register camera
//    auto camView = registry.GetAllComponentsView<CameraComponent, TransformComponent>();
//    for (auto& e : camView)
//        window.RegisterCam(registry.GetComponent<CameraComponent>(e), registry.GetComponent<TransformComponent>(e));
//
//    // Register meshes
//    auto meshView = registry.GetAllComponentsView<MeshComponent, TransformComponent, TextureComponent>();
//    for (auto& e : meshView)
//        window.RegisterRender(registry.GetComponent<MeshComponent>(e), registry.GetComponent<TransformComponent>(e), registry.GetComponent<TextureComponent>(e));
//
//    // Register lights
//    auto pointLights = registry.GetAllComponentsView<LightComponent<LightData::Type::Point>, TransformComponent>();
//    for (auto& e : pointLights) window.RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Point>>(e), registry.GetComponent<TransformComponent>(e));
//
//    auto spotLights = registry.GetAllComponentsView<LightComponent<LightData::Type::Spot>, TransformComponent>();
//    for (auto& e : spotLights) window.RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Spot>>(e), registry.GetComponent<TransformComponent>(e));
//
//    auto dirLights = registry.GetAllComponentsView<LightComponent<LightData::Type::Directional>, TransformComponent>();
//    for (auto& e : dirLights) window.RegisterLight(registry.GetComponent<LightComponent<LightData::Type::Directional>>(e), registry.GetComponent<TransformComponent>(e));
//
//    window.Render({ 0.53f, 0.81f, 0.92f, 1.0f });
//}