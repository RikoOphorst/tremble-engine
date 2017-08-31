#include "audio_clip.h"
#include "audio_manager.h"
#include "core/get.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	AudioClip::AudioClip(const std::string& file_location)
	{
		Get::AudioManager()->ErrorCheck(Get::AudioManager()->GetSystem()->createSound(file_location.c_str(), FMOD_3D, 0, &sound));
		name = file_location;
		Get::AudioManager()->AddAudioClip(this);
	}

}
