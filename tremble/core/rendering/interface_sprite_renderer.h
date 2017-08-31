#pragma once
#include "interface_buffers.h"
#include "root_signature.h"
#include "graphics_context.h"
#include "upload_buffer.h"
#include "../math/math.h"

namespace tremble
{
	class Shader;
	class Texture;

	/**
	* @class tremble::InterfaceSpriteRenderer
	* @author Sander Brouwers
	* @brief Renders ImageComponents and ButtonComponents to user interface
	*/
	class InterfaceSpriteRenderer
	{
	public:
		/// Initializes interface renderer
		void Startup();

		/// Render interface components to specified context
		void Draw(GraphicsContext&);

		/// Frees up memory used by interface renderer
		void Destroy();

	private:
		void CreateRenderResources();
		void CreatePSO();
		void CreateRootSignature();
		void CreateGeometry();

		UploadBuffer pass_constants_;
		UploadBuffer object_constants_;

		Shader* pixel_shader_;
		Shader* vertex_shader_;

		RootSignature root_signature_;
		D3D12_SAMPLER_DESC sampler_state_;
		D3D12_BLEND_DESC blend_state_;
		D3D12_DEPTH_STENCIL_DESC depth_stencil_state_;
		D3D12_RASTERIZER_DESC rasterizer_state_;

		std::vector<SpriteVertex> vertices_;
		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;
		StructuredBuffer vertex_buffer_;
	};
}