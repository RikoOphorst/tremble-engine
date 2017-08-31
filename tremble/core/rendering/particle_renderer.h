#pragma once
#include "interface_buffers.h"
#include "root_signature.h"
#include "graphics_context.h"
#include "../math/math.h"
#include "upload_buffer.h"

namespace tremble
{
	class Shader;
	class Texture;
	class Camera;

	struct ParticlePassData{
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX view;
		float aspect_ratio;
		int textured;
	};

	struct ParticleRenderable {
		DirectX::XMFLOAT4 Color;
		DirectX::XMFLOAT3 Position;
		float Size;
	};

	/**
	* @class tremble::ParticleRenderer
	* @author Sander Brouwers
	* @brief Renders ParticleSystem components
	*/
	class ParticleRenderer
	{
	public:
		/// Initializes particle renderer
		void Startup();
		
		/// Render particle system components to specified context
		void Draw(GraphicsContext&, Camera*);

		/// Frees up memory used by particle renderer
		void Destroy();

	private:
		void CreateRenderResources();
		void CreatePSO();
		void CreateRootSignature();

		Shader* pixel_shader_;
		Shader* geometry_shader_;
		Shader* vertex_shader_;

		RootSignature root_signature_;
		D3D12_SAMPLER_DESC sampler_state_;
		D3D12_BLEND_DESC blend_state_;
		D3D12_DEPTH_STENCIL_DESC depth_stencil_state_;
		D3D12_RASTERIZER_DESC rasterizer_state_;

		UploadBuffer pass_constants_;
	};
}