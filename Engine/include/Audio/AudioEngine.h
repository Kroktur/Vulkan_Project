#pragma once 
#include <soloud.h>

namespace KGR
{
	namespace Audio
	{
		/**
		* @brief AudioEngine is a singleton class that manages the audio engine for the application.
		* It provides methods to initialize and shutdown the audio engine, as well as access to the underlying Soloud::Soloud instance.
		*/
		class AudioEngine
		{
		public:
			// Default constructor and destructor
			AudioEngine() = default;
			~AudioEngine() = default;

			/**
			 * @brief GetInstance returns a reference to the singleton instance of the AudioEngine class.
			 *
			 * \return A reference to the AudioEngine instance.
			 */
			static AudioEngine& GetInstance();

			/**
			 * @brief GetAudioEngine returns a reference to the underlying Soloud::Soloud instance.
			 *
			 * \return A reference to the Soloud::Soloud instance.
			 */
			SoLoud::Soloud& GetAudioEngine();

			/**
			 * @brief Init initializes the audio engine. This should be called before using any audio functionality.
			 */
			void Init();

			/**
			 * @brief Shutdown shuts down the audio engine and releases any resources used by it. This should be called when the application is closing.
			 */
			void Shutdown();

		private:
			SoLoud::Soloud m_audioEngine; ///< The underlying Soloud::Soloud instance that manages the audio engine.
		};
	}// namespace Audio
}// namespace KGR