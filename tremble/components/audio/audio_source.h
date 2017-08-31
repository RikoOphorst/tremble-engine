#pragma once
#include "core/scene_graph/component.h"
#include <fmod.hpp>

namespace tremble
{
	class AudioClip;

	/**
	* @brief A class that contains the audio source component and functionality
	* @class tremble::AudioSource
	* @author Max Blom
	*/
	class AudioSource : public Component
	{
	public:
		AudioSource();
		void Awake();
		void Play();
		void TogglePaused(bool paused);

		/**
		* Enables this audiosource to have a 3d position
		* @param[in] position a vector3 world position
		* @param[in] velocity a vector3 velocity of the object
		*/
		void AudioSource::Enable3DAudio(Vector3 position, Vector3 velocity);

		/**
		* Sets the time of the song.
		* @param[in] time_in_seconds self explanatory lad
		*/
		void SetAudioPosition(int time_in_seconds);

		void Stop();

		void PlayOnce();

		/**
		* Sets the audioclip of the audiosource
		* @param[in] audio an audioclip
		*/
		void SetAudioClip(AudioClip* audio);

		/**
		* Sets the minimum and maximum distance of the source for dropoff
		* @param[in] min minimum distance
		* @param[in] max maximum distance
		*/
		void Set3DSettings(float min, float max);


		void SetVolume(float set_vol) { volume = set_vol; }
		void SetPitch(float set_pitch) { pitch = set_pitch; }
		void SetPanning(float set_pan) { pan = set_pan; }
		void SetMode(FMOD_MODE set_mode) { mode = set_mode; }

		AudioClip* GetAudioClip() const { return saved_clip; }
		float GetVolume() const { float vol; channel->getVolume(&vol); return vol; }
		float GetPitch() const { return pitch; }
		float GetPanning() const { return pan; }
		FMOD_MODE GetMode() const { return mode; }
		bool IsPlaying() const { bool playing; channel->isPlaying(&playing); return playing; }
		FMOD::Channel* GetChannel();

	private:
		AudioClip* saved_clip;//!< the audioClip associated with this audioSource

		float volume;//!< volume of the clip
		float pitch;//!< pitch of the clip
		float pan;//!< pan of the audio

		float original_frequency;//!< original frequency of the track. Remains unchanged

		FMOD::Channel *channel;//!< Channel attached to this track
		int channel_ID;//!< ID of the channel 

		FMOD_MODE mode;//!< mode of the channel

		void SetChannelAtrributes();//!< Used to set the values of the above variables on the trakc whne it is paused.

		float max_volume_multiplier;//!< Used to hold the value of the config windows slider thingy
	};
}

