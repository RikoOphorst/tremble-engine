#include "audio_manager.h"

#include "../utilities/debug.h"
#include "components/rendering/camera.h"
#include "core/rendering/renderer.h"
#include "core/scene_graph/scene_graph.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	AudioManager::AudioManager()
	{
		result_ = FMOD::System_Create(&system_);
		ErrorCheck(result_);

		result_ = system_->init(32, FMOD_INIT_NORMAL, 0);
		ErrorCheck(result_);
	}

	//------------------------------------------------------------------------------------------------------
	void AudioManager::ErrorCheck(FMOD_RESULT result)
	{
		if (result_ != FMOD_OK)
		{
			DLOG("FMOD Error");
			DLOG(FMOD_ErrorString(result_));
			assert(result_ == FMOD_OK);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void AudioManager::AddAudioClip(AudioClip* clip)
	{
		audio_clips_.push_back(clip);
	}

    //------------------------------------------------------------------------------------------------------
    void AudioManager::UpdateAudioSystem()
    {
        Camera* camera = Get::Renderer()->GetCamera();
        if (camera)
        {
            SGNode* camera_node = camera->GetNode();

            FMOD_VECTOR listener_pos = Vector3toFMODVector(camera_node->GetPosition());
            FMOD_VECTOR listener_vel = Vector3toFMODVector(Vector3(0));
            FMOD_VECTOR listener_forward = Vector3toFMODVector(camera_node->GetForwardVectorWorld());
            FMOD_VECTOR listener_up = Vector3toFMODVector(camera_node->GetUpVectorWorld());

            system_->set3DListenerAttributes(0, &listener_pos, &listener_vel, &listener_forward, &listener_up);
        }
        system_->update();
    }

    //------------------------------------------------------------------------------------------------------
	AudioSource * AudioManager::GetAudioSource(int channel_ID)
	{
		AudioSource* source = new AudioSource();
		source = audio_sources_[channel_ID];
		return source;
	}

	//------------------------------------------------------------------------------------------------------
	int AudioManager::AddAudioSource(AudioSource* audio_source)
	{
		audio_sources_.push_back(audio_source);
		return int(audio_sources_.size());
	}
}
