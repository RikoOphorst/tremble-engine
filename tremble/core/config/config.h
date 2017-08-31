#pragma once

#include "../math/math.h"
#include "resolution.h"

namespace tremble
{
	struct Config
	{
		bool fullscreen = false;
		UINT adapter = 0;
		UINT window_resolution = 5;
		UINT render_resolution = 5;
		UINT scene = 0;
		bool load_scene = true;
		bool god_mode = false;
		bool frustum_culling = true;
		bool depth_pre_pass = true;
		bool physics_base_plane = false;
		bool d3d12_debug_layer = false;
		bool wireframe_rendering = false;
		bool light_demo = false;
		UINT num_lights = 0;
		UINT master_volume = 50.0f;
		UINT mouse_sensitivity = 50.0f;
		bool host = true;
		std::string host_ip_address = "127.0.0.1";
		std::string nickname = "Optic[S0_L3g1t]";
		bool spawn_clients = false;
		UINT num_clients = 0;
	};
}