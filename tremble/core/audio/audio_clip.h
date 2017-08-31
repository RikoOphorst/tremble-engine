#pragma once

#include "fmod.hpp"
#include "fmod_errors.h"

namespace tremble
{
	/**
	* @brief A class that contains the FMOD sound needed to play the associated clip
	* @class tremble::AudioClip
	* @author Max Blom
	*/
	class AudioClip
	{
	private:
		
	public:
		AudioClip(const std::string& file_location);
		FMOD::Sound *sound;
		std::string name;
	};
}