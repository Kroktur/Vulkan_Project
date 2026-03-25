#include "Core/SceneManager.h"
// TODO remove
#include <iostream>

#include "Hasher.h"
#include "Core/Scene.h"


void SceneManager::AddScene(std::unique_ptr<Scene> scene, const std::string& name, bool isActive)
{
	std::uint64_t hash = Hash::FNV1aHash(name.c_str(), name.size());
	if (m_scenes.contains(hash))
		throw std::out_of_range("name already in use");

	m_scenes[hash] = std::move(scene);
	m_scenes[hash]->Init(this);

	if (isActive)
		m_currentIndex = hash;
}

void SceneManager::Run(const KGR::Tools::Chrono<float>::Time& fixedTime)
{
	const KGR::Tools::Chrono clock;
	float previous = clock.GetElapsedTime().AsSeconds();

	float renderFrameDt = clock.GetElapsedTime().AsMilliSeconds();

	const float fixTick = fixedTime.AsMilliSeconds();

	auto lag = 0.0f;

	//TODO remove
	int count = 0;

	Init();
	while (LoopCondition())
	{
		float now = clock.GetElapsedTime().AsSeconds();
		float dt = now - previous;
		previous = now;

		auto* Scene = GetCurrentScene();
		if (count > 60)
		{
			std::cout << (dt * 0.001f) << std::endl;
			count = 0;
		}
		Scene->Update(dt);
		count++;
		Scene->Render();
	}
	Destroy();
}

void SceneManager::SetCurrentScene(const std::string& name)
{
	m_currentIndex = Hash::FNV1aHash(name.c_str(), name.size());
	if (!m_scenes.contains(m_currentIndex.value()))
		throw std::out_of_range("invalid scene name not register");
}

Scene* SceneManager::GetCurrentScene()
{
	if (!m_currentIndex.has_value())
		throw std::out_of_range("index not valid");

	return m_scenes[m_currentIndex.value()].get();
}
