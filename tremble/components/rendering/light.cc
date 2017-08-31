#include "light.h"

#include "../../core/scene_graph/scene_graph.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Light::Light() :
		direction_(0.0f, -1.0f, 0.0f),
		color_(1.0f, 1.0f, 1.0f, 1.0f),
		falloff_end_(0.0f),
		cos_cone_(0.0f),
		cast_shadow_(false),
		shadow_index_(0),
		shadow_range_(0)
	{

	}
}