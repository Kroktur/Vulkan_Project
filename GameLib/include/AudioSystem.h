#include "AudioComponent.h"
#include "Audio/SoundManager.h"
#include "Audio/MusicManager.h"

namespace KGR
{
	namespace GameLib
	{
        /**
         * @brief AudioSystem updates all audio components and manages global audio.
         */
        class AudioSystem
        {
        public:
			// Default constructor and destructor
            AudioSystem() = default;
            ~AudioSystem() = default;

            /**
			 * @brief Init initializes the audio system, including the sound and music managers. This should be called before using any audio functionality.
             */
            void Init();

			/**
			 * @brief Update updates the audio system, processing any necessary audio updates.
			 * This should be called every frame with the elapsed time since the last update.
			 * 
			 * \param deltaTime The elapsed time in seconds since the last update.
			 */
            void Update(float deltaTime);

            /**
			 * @brief Shutdown shuts down the audio system and releases any resources used by it. This should be called when the application is closing.
             */
            void Shutdown();

            /**
			 * @brief GetSoundManager returns a reference to the SoundManager instance used by the audio system.
			 * This allows other parts of the application to access sound management functionality.
             * 
			 * \return A reference to the SoundManager instance.
             */
            KGR::Audio::SoundManager& GetSoundManager();

            /**
			 * @brief GetMusicManager returns a reference to the MusicManager instance used by the audio system.
			 * This allows other parts of the application to access music management functionality.
             * 
			 * \return A reference to the MusicManager instance.
             */
            KGR::Audio::MusicManager& GetMusicManager();

        private:
			KGR::Audio::SoundManager m_soundManager; //< The SoundManager instance responsible for managing sound effects in the application.
			KGR::Audio::MusicManager m_musicManager; //< The MusicManager instance responsible for managing music tracks in the application.
        };
	}
}