#include "AudioSystem.h"
#include "Audio/AudioEngine.h"

/**
 * @brief Initializes the audio system, including the sound and music managers. This should be called before using any audio functionality.
 * 
 */
void KGR::GameLib::AudioSystem::Init()
{
	KGR::Audio::AudioEngine::GetInstance().Init();
}

/**
 * @brief Update updates the audio system, processing any necessary audio updates.
 * This should be called every frame with the elapsed time since the last update.
 *
 * \param deltaTime The elapsed time in seconds since the last update.
 */
void KGR::GameLib::AudioSystem::Update(float deltaTime)
{
	//No update logic needed for now
}

/**
 * @brief Shutdown shuts down the audio system and releases any resources used by it. This should be called when the application is closing.
 */
void KGR::GameLib::AudioSystem::Shutdown()
{
	KGR::Audio::AudioEngine::GetInstance().Shutdown();
}

/**
 * @brief GetSoundManager returns a reference to the SoundManager instance used by the audio system.
 * This allows other parts of the application to access sound management functionality.
 *
 * \return A reference to the SoundManager instance.
 */
KGR::Audio::SoundManager& KGR::GameLib::AudioSystem::GetSoundManager()
{
	return m_soundManager;
}

/**
 * @brief GetMusicManager returns a reference to the MusicManager instance used by the audio system.
 * This allows other parts of the application to access music management functionality.
 *
 * \return A reference to the MusicManager instance.
 */
KGR::Audio::MusicManager& KGR::GameLib::AudioSystem::GetMusicManager()
{
	return m_musicManager;
}