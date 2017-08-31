#pragma once

#include "audio_clip.h"
#include "core/math/dxmath/vector3.h"
#include "components/audio/audio_source.h"

namespace tremble
{
	/**
	* @brief A class that contains the audio system and all audio clips. It also manages all the audi channels
	* @class tremble::AudioManager
	* @author Max Blom
	*/
	class AudioManager
	{
	public:
        friend class GameManager;

		/**
		* Checks the result against FMODs existing error checker
		* @param[in] result any FMOD result
		*/
		void ErrorCheck(FMOD_RESULT result);

		AudioManager();

		FMOD::System* GetSystem() const { return system_; } //!<Returns the system used to execute FMOD commands
		AudioSource* GetAudioSource(int channel_ID); //!< returns the channel that the sound is playing on

		int AudioManager::AddAudioSource(AudioSource* audio_source);//!<Sets audio source to the vector

		/**
		* Adds the newly created audio clip to the vector
		* @param[in] audio_clip pointer to an audioclip created in the createSound function
		*/
		void AddAudioClip(AudioClip* audio_clip);
//-----------------------------------------------------------------------------------------------
		FMOD_VECTOR Vector3toFMODVector(Vector3 vector)
		{
			FMOD_VECTOR vector_holder;
			vector_holder.x = vector.GetX();
			vector_holder.y = vector.GetY();
			vector_holder.z = vector.GetZ();
			return vector_holder;
		}

		Vector3 FMODVectorToVector3(FMOD_VECTOR vector)
		{
			Vector3 vector_holder;
			vector_holder.SetX(vector.x);
			vector_holder.SetY(vector.y);
			vector_holder.SetZ(vector.z);
			return vector_holder;
		}
	private:

        void UpdateAudioSystem();

		FMOD_RESULT result_;//!< FMOD Result that can be set to any variable that is part of the FMOD library in order to check for errors

		std::vector<AudioClip*> audio_clips_;//!<Vector of pointers to audio clips

		std::vector<AudioSource*> audio_sources_;//!<Vector of all active channels

		AudioClip* audio_clip_;//!< template audio clip to set and add to the vector

							  /**
							  * Main FMOD audio system. Manages all the audio playback in the engine
							  */
		FMOD::System* system_;

	};
}
