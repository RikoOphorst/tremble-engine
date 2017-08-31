#pragma once
#include "../../core/scene_graph/component.h"

namespace tremble
{
	/**
	* @brief A class that contains the FMOD listener properties
	* @class tremble::AudioListener
	* @author Max Blom
	*/

	class AudioClip;

	class AudioListener : public Component
	{
	public:
		AudioListener();
		void Set3DListener(const Vector3& position, const Vector3& velocity, const Vector3& forward, const Vector3& upward);
	private:
	};
}
