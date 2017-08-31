#pragma once

#include "animation.h"

namespace tremble
{
	struct AnimationState
	{
		struct KeyState
		{
			unsigned int current;
			unsigned int next;
		};

		float playback_time;
		Animation& animation;
		KeyState position;
		KeyState scaling;
		KeyState rotation;
	};
}