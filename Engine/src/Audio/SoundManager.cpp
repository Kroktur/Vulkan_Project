#include "Audio/SoundManager.h"
#include "Audio/AudioEngine.h"

/**
 * @brief LoadSound loads a sound effect from a file and returns a pointer to the SoLoud::Wav instance.
 *
 * \param filePath The path to the sound file to load.
 * \return A pointer to the loaded SoLoud::Wav instance, or nullptr if loading failed.
 */
SoLoud::Wav* KGR::Audio::SoundManager::LoadSound(const std::string& filePath)
{
	if (m_sound.load(filePath.c_str()) != SoLoud::SO_NO_ERROR)
		return nullptr; // Return nullptr if loading failed
	return &m_sound; // Return a pointer to the loaded sound
}

/**
 * @brief PlaySound plays a loaded sound effect.
 */
void KGR::Audio::SoundManager::PlaySound()
{
	SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
	m_soundHandle = audioEngine.play(m_sound); // Play the sound and store the handle for
}

/**
 * @brief StopSound stops a currently playing sound effect.
 */
void KGR::Audio::SoundManager::StopSound()
{
	if (m_soundHandle != -1) // Check if the sound is currently playing
	{
		SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
		audioEngine.stop(m_soundHandle); // Stop the sound using the stored handle
		m_soundHandle = -1; // Reset the handle to indicate that the sound is no longer playing
	}
}

/**
 * @brief SetVolume sets the volume of a sound effect.
 *
 * \param volume The desired volume level (0.0f for silence, 1.0f for full volume).
 */
void KGR::Audio::SoundManager::SetVolume(float volume)
{
	if(m_soundHandle != -1) // Check if the sound is currently playing
	{
		SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
		audioEngine.setVolume(m_soundHandle, volume); // Set the volume of the sound using the stored handle
	}
}