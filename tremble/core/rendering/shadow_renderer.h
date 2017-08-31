#pragma once
#include "root_signature.h"
#include "graphics_context.h"
#include "upload_buffer.h"
#include "descriptor_heap.h"
#include "../math/math.h"
#include "depth_buffer.h"
#include "color_buffer.h"

namespace tremble
{
	class Shader;
	class Texture;

	struct ShadowData {
		DirectX::XMMATRIX view_projection_;
		ColorBuffer* depth_map_;
	};

	struct ShadowPassConstants {
		DirectX::XMMATRIX view_projection;
	};

	/**
	* @class tremble::ShadowRenderer
	* @author Sander Brouwers
	* @brief Handles shadow rendering
	*/
	class ShadowRenderer
	{
	public:
		ShadowRenderer();

		/// Initializes shadow renderer
		void Startup();

		/// Render shadow components to depth maps
		void Draw();

		/// Upload depth maps for shadow rendering
		void UploadData(GraphicsContext&);

		/// Frees up memory used by shadow renderer
		void Destroy();

	private:
		void CreateRenderResources();
		void CreatePSO();
		void CreateRootSignature();
		void RenderMap(int index, DirectX::XMMATRIX view, DirectX::XMMATRIX projection);

		UploadBuffer info_constants_;
		StructuredBuffer map_buffer_;

		Shader* pixel_shader_;
		Shader* vertex_shader_;

		int render_width_ = 1024;
		int render_height_ = 1024;
		int max_maps_ = 24;

		DepthBuffer depth_buffer_;
		Texture* shadow_map_array_;

		RootSignature root_signature_;
		RootSignature root_signature_skinned_;
		D3D12_SAMPLER_DESC sampler_state_;
		D3D12_BLEND_DESC blend_state_;
		D3D12_DEPTH_STENCIL_DESC depth_stencil_state_;
		D3D12_RASTERIZER_DESC rasterizer_state_;

		ShadowPassConstants* map_data_;
		std::vector<ShadowData> shadow_maps_;

		int rendered_maps_;
	};

	struct ShadowInfoConstants {
		unsigned int shadow_count;
	};

	const static D3D12_INPUT_ELEMENT_DESC shadow_object_input[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}