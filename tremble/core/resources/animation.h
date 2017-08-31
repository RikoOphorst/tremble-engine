#pragma once

#include "../math/math.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla

namespace tremble
{
	struct AnimationVectorKey
	{
		Vector3 value;
		float time;
	};

	struct AnimationQuatKey
	{
		Quaternion value;
		float time;
		aiMatrix3x3t<float> mat;
	};

	struct AnimationChannel
	{
		std::string bone_name;
		std::vector<AnimationVectorKey> position_keys;
		std::vector<AnimationQuatKey> rotation_keys;
		std::vector<AnimationVectorKey> scaling_keys;
	};

	struct Animation
	{
		std::string name;
		float ticks_per_second;
		float duration;
		std::vector<AnimationChannel> channels;
		std::unordered_map<std::string, int> node_name_to_channel_mapping;
	};
}