#pragma once
#include <string>
#include <soloud_wav.h>

namespace KGR
{
    namespace GameLib
    {
        /**
         * @brief AudioComponent represents a sound attached to an entity.
         * It can be played, stopped, and have its volume adjusted.
         */
        struct AudioComponent
        {
            std::string filePath;     ///< Path to the sound file
            SoLoud::Wav sound;        ///< Loaded sound
            int handle = -1;          ///< Playback handle
            float volume = 1.0f;      ///< Volume of the sound
            bool playOnStart = false; ///< Should the sound play automatically
        };
    }
}