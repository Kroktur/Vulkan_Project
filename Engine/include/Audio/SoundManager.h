#pragma once
#include <soloud_wav.h>
#include <string>

namespace KGR
{
	namespace Audio
	{
		/**
		 * @brief SoundManager is a utility class for managing sound effects in the application.
		 * It provides methods to load and play sound effects using the underlying AudioEngine.
		 */
		class SoundManager
		{
		public:
			// Default constructor and destructor
			SoundManager() = default;
			~SoundManager() = default;

			/**
			 * @brief LoadSound loads a sound effect from a file and returns a pointer to the SoLoud::Wav instance.
			 *
			 * \param filePath The path to the sound file to load.
			 * \return A pointer to the loaded SoLoud::Wav instance, or nullptr if loading failed.
			 */
			SoLoud::Wav* LoadSound(const std::string& filePath);

			/**
			 * @brief PlaySound plays a loaded sound effect.
			 */
			void PlaySound();

			/**
			 * @brief StopSound stops a currently playing sound effect.
			 */
			void StopSound();

			/**
			 * @brief SetVolume sets the volume of a sound effect.
			 *
			 * \param volume The desired volume level (0.0f for silence, 1.0f for full volume).
			 */
			void SetVolume(float volume);
		private:
			SoLoud::Wav m_sound; ///< The SoLoud::Wav instance representing the loaded sound effect.
			int m_soundHandle = -1; ///< Handle for the currently playing sound effect, used for controlling playback.
		};
	}// namespace Audio
}// namespace KGR