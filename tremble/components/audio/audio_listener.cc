#include "audio_listener.h"

#include "../../core/audio/audio_manager.h"
#include "../../core/audio/audio_clip.h"
#include "../../core/get.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	AudioListener::AudioListener()
	{
		
	}

	//------------------------------------------------------------------------------------------------------
	void AudioListener::Set3DListener(const Vector3& position, const Vector3& velocity, const Vector3& forward, const Vector3& upward)
	{
		FMOD_VECTOR pos = Get::AudioManager()->Vector3toFMODVector(position);
		FMOD_VECTOR vel = Get::AudioManager()->Vector3toFMODVector(velocity);
		FMOD_VECTOR fw = Get::AudioManager()->Vector3toFMODVector(forward);
		FMOD_VECTOR up = Get::AudioManager()->Vector3toFMODVector(upward);
		Get::AudioManager()->GetSystem()->set3DListenerAttributes(0, &pos, &vel, &fw, &up);
	}


}