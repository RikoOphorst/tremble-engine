#include "audio_source.h"

#include "../../core/audio/audio_manager.h"
#include "../../core/audio/audio_clip.h"
#include "../../core/get.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	AudioSource::AudioSource()
	{
		max_volume_multiplier = static_cast<float>(Get::Config().master_volume) / 100.0f;
		volume = 1.0f;
		pitch = 1.0f;
		pan = 0.0f;
		original_frequency = 0.0f;

		mode = FMOD_LOOP_OFF;
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::Awake()
	{
		channel_ID = Get::AudioManager()->AddAudioSource(this);
	}
	
	//------------------------------------------------------------------------------------------------------
	void AudioSource::SetAudioClip(AudioClip* audio_clip)
	{
		saved_clip = audio_clip;
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::Set3DSettings(float min, float max)
	{
		FMOD_RESULT result = saved_clip->sound->set3DMinMaxDistance(min, max);
		Get::AudioManager()->ErrorCheck(result);
	}

	//------------------------------------------------------------------------------------------------------
	FMOD::Channel* AudioSource::GetChannel()
	{
		FMOD::Channel* chan;
		Get::AudioManager()->GetSystem()->getChannel(channel_ID, &chan);
		return chan;
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::SetChannelAtrributes()
	{
		if(original_frequency <= 0)
		{
			channel->getFrequency(&original_frequency);
		}
		channel->setVolume(volume * max_volume_multiplier);
		channel->setPan(pan);
		channel->setMode(mode);
		channel->setFrequency(original_frequency * pitch);
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::Play()
	{
        if (saved_clip != nullptr)
        {
            FMOD::Channel* temp_channel = GetChannel();
            FMOD_RESULT result = Get::AudioManager()->GetSystem()->playSound(static_cast<FMOD_CHANNELINDEX>(channel_ID), saved_clip->sound, true, &channel);
            Get::AudioManager()->ErrorCheck(result);
            SetChannelAtrributes();
            channel->setPaused(false);

            Get::AudioManager()->ErrorCheck(result);
        }
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::TogglePaused(bool paused)
	{
		GetChannel()->setPaused(paused);
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::Enable3DAudio(Vector3 position, Vector3 velocity)
	{
		FMOD_RESULT result = channel->set3DPanLevel(1.0f);
		Get::AudioManager()->ErrorCheck(result);

		FMOD_VECTOR pos = Get::AudioManager()->Vector3toFMODVector(position);
		FMOD_VECTOR vel = Get::AudioManager()->Vector3toFMODVector(velocity);

		result = channel->set3DAttributes(&pos, 0);
		Get::AudioManager()->ErrorCheck(result);
	}
	//------------------------------------------------------------------------------------------------------
	void AudioSource::SetAudioPosition(int time_in_seconds)
	{
		int timeMS = time_in_seconds * 1000;
		//int loopendMS = loop_end * 1000;
		//saved_clip->sound->setLoopPoints(loopstartMS, FMOD_TIMEUNIT_MS, loopendMS, FMOD_TIMEUNIT_MS);
		FMOD_RESULT result = channel->setPosition(timeMS, FMOD_TIMEUNIT_MS);
		Get::AudioManager()->ErrorCheck(result);
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::Stop()
	{
		GetChannel()->stop();
	}

	//------------------------------------------------------------------------------------------------------
	void AudioSource::PlayOnce()
	{
		if (saved_clip != nullptr && !IsPlaying())
		{
			FMOD::Channel* temp_channel = GetChannel();
			FMOD_RESULT result = Get::AudioManager()->GetSystem()->playSound(static_cast<FMOD_CHANNELINDEX>(channel_ID), saved_clip->sound, true, &channel);
			Get::AudioManager()->ErrorCheck(result);
			SetChannelAtrributes();
			channel->setPaused(false);

			Get::AudioManager()->ErrorCheck(result);
		}
	}
}
