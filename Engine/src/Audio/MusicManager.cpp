#include "Audio/MusicManager.h"
#include "Audio/AudioEngine.h"

/**
 * @brief LoadMusic loads a music track from a file and returns a pointer to the SoLoud::WavStream instance.
 *
 * \param filePath The path to the music file to load.
 * \return A pointer to the loaded SoLoud::WavStream instance, or nullptr if loading failed.
 */
SoLoud::WavStream* KGR::Audio::MusicManager::LoadMusic(const std::string& filePath)
{
	if (m_music.load(filePath.c_str()) != SoLoud::SO_NO_ERROR)
		return nullptr; // Return nullptr if loading failed
	return &m_music; // Return a pointer to the loaded music
}

/**
 * @brief PlayMusic plays a loaded music track.
 */
void KGR::Audio::MusicManager::PlayMusic()
{
	SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
	m_musicHandle = audioEngine.play(m_music); // Play the music and store the handle for
}

/**
 * @brief StopMusic stops a currently playing music track.
 */
void KGR::Audio::MusicManager::StopMusic()
{
	if (m_musicHandle != -1) // Check if the music is currently playing
	{
		SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
		audioEngine.stop(m_musicHandle); // Stop the music using the stored handle
		m_musicHandle = -1; // Reset the handle to indicate that the music is no longer playing
	}
}

/**
 * @brief PauseMusic pauses a currently playing music track.
 */
void KGR::Audio::MusicManager::PauseMusic()
{
	if (m_musicHandle != -1) // Check if the music is currently playing
	{
		SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
		audioEngine.setPause(m_musicHandle, true); // Pause the music using the stored handle
	}
}

/**
 * @brief ResumeMusic resumes a paused music track.
 */
void KGR::Audio::MusicManager::ResumeMusic()
{
	if (m_musicHandle != -1) // Check if the music is currently paused
	{
		SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
		audioEngine.setPause(m_musicHandle, false); // Resume the music using the stored handle
	}
}

/**
 * @brief SetVolume sets the volume of a music track.
 *
 * \param volume The desired volume level (0.0f for silence, 1.0f for full volume).
 */
void KGR::Audio::MusicManager::SetVolume(float volume)
{
	if (m_musicHandle != -1) // Check if the music is currently playing
	{
		SoLoud::Soloud& audioEngine = AudioEngine::GetInstance().GetAudioEngine(); // Get the audio engine instance
		audioEngine.setVolume(m_musicHandle, volume); // Set the volume of the music using the stored handle
	}
}