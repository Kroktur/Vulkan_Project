#pragma once
#include <soloud_wavstream.h>
#include <string>

namespace KGR
{
	namespace Audio
	{
		/**
		 * @brief MusicManager is a utility class for managing music playback in the application.
		 * It provides methods to load and play music tracks using the underlying AudioEngine.
		 */
		class MusicManager
		{
		public:
			// Default constructor and destructor
			MusicManager() = default;
			~MusicManager() = default;

			/**
			 * @brief LoadMusic loads a music track from a file and returns a pointer to the SoLoud::WavStream instance.
			 *
			 * \param filePath The path to the music file to load.
			 * \return A pointer to the loaded SoLoud::WavStream instance, or nullptr if loading failed.
			 */
			SoLoud::WavStream* LoadMusic(const std::string& filePath);

			/**
			 * @brief PlayMusic plays a loaded music track.
			 */
			void PlayMusic();

			/**
			 * @brief StopMusic stops a currently playing music track.
			 */
			void StopMusic();

			/**
			 * @brief PauseMusic pauses a currently playing music track.
			 */
			void PauseMusic();

			/**
			 * @brief ResumeMusic resumes a paused music track.
			 */
			void ResumeMusic();

			/**
			 * @brief SetVolume sets the volume of a music track.
			 *
			 * \param volume The desired volume level (0.0f for silence, 1.0f for full volume).
			 */
			void SetVolume(float volume);
		private:
			SoLoud::WavStream m_music; ///< The SoLoud::WavStream instance representing the loaded music track.
			int m_musicHandle = -1; ///< Handle for the currently playing music track, used for controlling playback.
		};
	}// namespace Audio
}// namespace KGR