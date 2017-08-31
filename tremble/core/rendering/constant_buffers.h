#pragma once

#include "../../components/rendering/light.h"

namespace tremble
{
	struct DebugConstants
	{
		DirectX::XMMATRIX world_view_projection = DirectX::XMMatrixIdentity();
	};

	struct ObjectConstants
	{
		DirectX::XMMATRIX world = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX world_view = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX world_view_projection = DirectX::XMMatrixIdentity();
	};

	struct PassConstants
	{
		float total_time;
		float delta_time;
		float near_z;
		float far_z;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX inv_view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX inv_projection;
		DirectX::XMMATRIX view_projection;
		DirectX::XMMATRIX inv_view_projection;
		DirectX::XMFLOAT3 eye_pos_world;
		float padding;
		DirectX::XMFLOAT2 render_target_size;
		DirectX::XMFLOAT2 inv_render_target_size;
	};

	struct MaterialConstants
	{
		DirectX::XMFLOAT4 global_ambient;
		DirectX::XMFLOAT4 ambient_color;
		DirectX::XMFLOAT4 emissive_color;
		DirectX::XMFLOAT4 diffuse_color;
		DirectX::XMFLOAT4 specular_color;
		DirectX::XMFLOAT4 reflectance;
		float opacity;
		float specular_power;
		float index_of_refraction;
		bool has_ambient_texture;
		bool has_emissive_texture;
		bool has_diffuse_texture;
		bool has_specular_texture;
		bool has_specular_power_texture;
		bool has_normal_texture;
		bool has_bump_texture;
		bool has_opacity_texture;
		float bump_intensity;
		float specular_scale;
		float alpha_threshold;
		float padding[2];
	};

	struct LightConstants
	{
		DirectX::XMFLOAT4 position_world;
		DirectX::XMFLOAT4 direction_world;
		DirectX::XMFLOAT4 position_view;
		DirectX::XMFLOAT4 direction_view;
		DirectX::XMFLOAT4 color;
		float spot_light_angle;
		float range;
		float intensity;
		uint32_t enabled;
		uint32_t selected;
		uint32_t type;
		int shadow_index;
		int shadow_range;
	};
}