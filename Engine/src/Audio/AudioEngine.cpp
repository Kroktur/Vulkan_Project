#include "Audio/AudioEngine.h"

/**
 * @brief GetInstance returns a reference to the singleton instance of the AudioEngine class.
 *
 * \return A reference to the AudioEngine instance.
 */
KGR::Audio::AudioEngine& KGR::Audio::AudioEngine::GetInstance()
{
	static KGR::Audio::AudioEngine instance; // Create a static instance of the AudioEngine class
	return instance; // Return a reference to the instance
}

/**
 * @brief GetAudioEngine returns a reference to the underlying Soloud::Soloud instance.
 *
 * \return A reference to the Soloud::Soloud instance.
 */
SoLoud::Soloud& KGR::Audio::AudioEngine::GetAudioEngine()
{
	return GetInstance().m_audioEngine; // Return a reference to the m_audioEngine member of the singleton instance
}

/**
 * @brief Init initializes the audio engine. This should be called before using any audio functionality.
 */
void KGR::Audio::AudioEngine::Init()
{
	GetInstance().m_audioEngine.init(); // Initialize the audio engine by calling the init() method on the m_audioEngine member of the singleton instance
}

/**
 * @brief Shutdown shuts down the audio engine and releases any resources used by it. This should be called when the application is closing.
 */
void KGR::Audio::AudioEngine::Shutdown()
{
	GetInstance().m_audioEngine.deinit(); // Shutdown the audio engine by calling the deinit() method on the m_audioEngine member of the singleton instance
}