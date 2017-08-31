#include "shadow_renderer.h"
#include "../resources/resource_manager.h"
#include "color_buffer.h"
#include "depth_buffer.h"
#include "renderer.h"
#include "interface_font_renderer.h"
#include "../components/rendering/renderable.h"
#include "../components/rendering/skinned_renderable.h"
#include "vertex.h"
#include "../scene_graph/scene_graph.h"
#include "../../components/rendering/camera.h"
#include "../resources/mesh.h"
#include "../resources/model.h"
#include "shader.h"
#include "descriptor_heap.h"
#include "texture.h"

// needed for vector * float
using namespace DirectX;

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	ShadowRenderer::ShadowRenderer() : depth_buffer_(1)
	{
	}

	//------------------------------------------------------------------------------------------------------
	void ShadowRenderer::Startup()
	{

		rendered_maps_ = 0;

		vertex_shader_ = Get::ResourceManager()->GetShader("shadow_vs.cso");
		pixel_shader_ = Get::ResourceManager()->GetShader("shadow_ps.cso");

		info_constants_.Create(L"ShadowInfoConstants", 1U, (sizeof(ShadowInfoConstants) + 255) & ~255);

		map_buffer_.Create(L"ShadowMapData", max_maps_, sizeof(ShadowPassConstants));

		map_data_ = (ShadowPassConstants*)malloc(max_maps_ * sizeof(ShadowPassConstants));

		depth_buffer_.Create(L"DepthMap", render_width_, render_height_, DXGI_FORMAT_D32_FLOAT);

		// empty texture data
		unsigned char* blank_temp_map = (unsigned char*)malloc(render_width_*render_height_ * 4 * sizeof(float));
		for (int i = 0; i < render_width_ * render_height_ * 4 * sizeof(float); i++)
		{
			blank_temp_map[i] = 0;
		}

		WICLoadedData blank_map;
		blank_map.image_width = render_width_;
		blank_map.image_height = render_height_;
		blank_map.image_data = blank_temp_map;
		blank_map.image_data_byte_size = render_width_ * render_height_ * 4;
		blank_map.bytes_per_row = render_width_ * 4;
		blank_map.dxgi_format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		blank_map.success = true;

		shadow_map_array_ = new Texture(blank_map, max_maps_, true);
		shadow_map_array_->BuildBuffers();

		delete blank_temp_map;
		
		// render targets from texture array
		for (int i = 0; i < max_maps_; i++)
		{
			shadow_maps_.push_back({});
			shadow_maps_[i].depth_map_ = new ColorBuffer();
			shadow_maps_[i].depth_map_->CreateFromTexture(shadow_map_array_->Get(), i); //TODO initialize render targets properly
		}

		CreateRenderResources();
		CreateRootSignature();
		CreatePSO();
	}

	//------------------------------------------------------------------------------------------------------
	void ShadowRenderer::Draw()
	{
		DirectX::XMMATRIX view, projection;

		int rendered = 0;
		rendered_maps_ = 0;

		std::vector<Light*> lights = SGNode::FindAllComponents<Light>();
		for (int i = 0; i < lights.size(); i++)
		{
			lights[i]->SetShadowRange(0, 0);

			// map limit
			if (rendered >= max_maps_) continue;

			// not casting shadow
			if (!lights[i]->GetShadowCasting()) continue;

			// not compatible light
			if (lights[i]->GetLightType() != LightTypeDirectional && lights[i]->GetLightType() != LightTypePoint) continue;

			switch (lights[i]->GetLightType())
			{
				case LightTypePoint: 
				{
					lights[i]->SetShadowRange(rendered, 6);
					DirectX::XMVECTOR direction = { 1, 0, 0 };
					DirectX::XMVECTOR center = lights[i]->GetNode()->GetPosition();
					float fov = 95 * 3.14f / 180.0f;

					view = DirectX::XMMatrixLookAtLH(center, center + direction, { 0, 1, 0 });
					projection = DirectX::XMMatrixPerspectiveFovLH(fov, 1, 0.1f, 100.0f);

					RenderMap(rendered, view, projection);
					rendered++;

					direction = { -1, 0, 0 };
					view = DirectX::XMMatrixLookAtLH(center, center + direction, { 0, 1, 0 });
					RenderMap(rendered, view, projection);
					rendered++;

					direction = { 0, 0, 1 };
					view = DirectX::XMMatrixLookAtLH(center, center + direction, { 0, 1, 0 });
					RenderMap(rendered, view, projection);
					rendered++;

					direction = { 0, 0, -1 };
					view = DirectX::XMMatrixLookAtLH(center, center + direction, { 0, 1, 0 });
					RenderMap(rendered, view, projection);
					rendered++;

					direction = { 0.001f, 1, 0 };
					view = DirectX::XMMatrixLookAtLH(center, center + direction, { 0, 1, 0 });
					RenderMap(rendered, view, projection);
					rendered++;

					direction = { 0.001f, -1, 0 };
					view = DirectX::XMMatrixLookAtLH(center, center + direction, { 0, 1, 0 });
					RenderMap(rendered, view, projection);
					rendered++;

					break; 
				}

				case LightTypeDirectional: 
				{
					std::vector<Renderable*> objects = SGNode::FindAllComponents<Renderable>();
					
					// Get all points in scene
					std::vector<DirectX::XMFLOAT3> all_points;
					for (int o = 0; o < objects.size(); o++) 
					{
						std::vector<OctreeObject*> octreeObjects = objects[o]->GetOctreeObjects();
						for (int b = 0; b < octreeObjects.size(); b++) 
						{
							if (!octreeObjects[b]->alive) continue;
							DirectX::XMFLOAT3 corners[8];
							octreeObjects[b]->bounds.GetCorners(&corners[0]);
							all_points.insert(all_points.end(), corners, corners + 8);
						}
					}

					// Create scene bounding box
					DirectX::BoundingBox box;
					DirectX::BoundingBox::CreateFromPoints(box, all_points.size(), &all_points[0], sizeof(DirectX::XMFLOAT3));

					DirectX::XMVECTOR boxCorners[8];
					DirectX::XMVECTOR boxCenter = DirectX::XMLoadFloat3(&box.Center);

					DirectX::XMFLOAT3 boxCornerFloats[8];
					box.GetCorners(&boxCornerFloats[0]);
					for (int bc = 0; bc < 8; bc++) {
						boxCorners[bc] = DirectX::XMLoadFloat3(&boxCornerFloats[bc]);
					}

					// Transform scene bounds into shadow direction
					DirectX::XMMATRIX cameraView = DirectX::XMMatrixLookAtLH({0, 0, 0}, lights[i]->GetDirection().Normalize(), { 0, 1, 0 });
					for (int c = 0; c < 8; c++) 
					{
						boxCorners[c] = DirectX::XMVector3Transform(boxCorners[c], cameraView);
					}

					// Get frustum dimensions
					
					float xMin = DirectX::XMVectorGetX(boxCorners[0]), xMax = DirectX::XMVectorGetX(boxCorners[0]);
					float yMin = DirectX::XMVectorGetY(boxCorners[0]), yMax = DirectX::XMVectorGetY(boxCorners[0]);
					float zMin = DirectX::XMVectorGetZ(boxCorners[0]), zMax = DirectX::XMVectorGetZ(boxCorners[0]);
					for (int c = 0; c < 8; c++)
					{
						xMin = std::min(xMin, DirectX::XMVectorGetX(boxCorners[c]));
						xMax = std::max(xMax, DirectX::XMVectorGetX(boxCorners[c]));

						yMin = std::min(yMin, DirectX::XMVectorGetY(boxCorners[c]));
						yMax = std::max(yMax, DirectX::XMVectorGetY(boxCorners[c]));

						zMin = std::min(zMin, DirectX::XMVectorGetZ(boxCorners[c]));
						zMax = std::max(zMax, DirectX::XMVectorGetZ(boxCorners[c]));
					}

					DirectX::XMVECTOR direction = -lights[i]->GetDirection();

					// Create view and projection
					view = DirectX::XMMatrixLookAtLH(boxCenter + direction * -zMin, boxCenter, { 0, 1, 0 });
					projection = DirectX::XMMatrixOrthographicLH(xMax - xMin, yMax - yMin, 0.01f, 1000);

					// Render map
					lights[i]->SetShadowRange(rendered, 1);
					RenderMap(rendered, view, projection);
					rendered++;

					break; 
				}
			}

			
		}

		rendered_maps_ = rendered;
	}

	void ShadowRenderer::UploadData(GraphicsContext& context)
	{
		for (size_t i = 0; i < rendered_maps_; i++) {
			
			map_data_[i].view_projection = shadow_maps_[i].view_projection_;
		}

		ShadowInfoConstants info;
		info.shadow_count = (unsigned int)rendered_maps_;
		info_constants_.InsertDataByElement(0, &info);

		DescriptorHeap& srv_heap = Get::CbvSrvUavHeap();

		if (rendered_maps_ > 0) {
			context.SetDescriptorTable(12, srv_heap.GetGPUDescriptorById(shadow_map_array_->GetSRV()));
			CommandContext::InitializeBuffer(map_buffer_, map_data_, (unsigned int)rendered_maps_ * sizeof(ShadowPassConstants), false, 0);
			context.SetBufferSRV(13, map_buffer_, 0);
		}

		context.SetConstantBuffer(14, info_constants_.GetAddressByElement(0));
	}

	//------------------------------------------------------------------------------------------------------
	void ShadowRenderer::Destroy()
	{
		delete shadow_map_array_;
		delete map_data_;
	}

	//------------------------------------------------------------------------------------------------------
	void ShadowRenderer::CreateRenderResources()
	{
		depth_stencil_state_ = {};
		depth_stencil_state_.DepthEnable = TRUE;
		depth_stencil_state_.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
		depth_stencil_state_.StencilEnable = FALSE;
		depth_stencil_state_.StencilReadMask = D3D12_DEFAULT_STENCIL_READ_MASK;
		depth_stencil_state_.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
		depth_stencil_state_.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
		depth_stencil_state_.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
		depth_stencil_state_.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
		depth_stencil_state_.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
		depth_stencil_state_.BackFace = depth_stencil_state_.FrontFace;
		depth_stencil_state_.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		rasterizer_state_ = {};
		rasterizer_state_.FillMode = D3D12_FILL_MODE_SOLID;
		rasterizer_state_.CullMode = D3D12_CULL_MODE_BACK;
		rasterizer_state_.FrontCounterClockwise = FALSE;
		rasterizer_state_.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizer_state_.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizer_state_.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizer_state_.DepthClipEnable = FALSE;
		rasterizer_state_.MultisampleEnable = TRUE;
		rasterizer_state_.ForcedSampleCount = 4;
		rasterizer_state_.AntialiasedLineEnable = FALSE;
		rasterizer_state_.ForcedSampleCount = 0;
		rasterizer_state_.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

		sampler_state_ = {};
		sampler_state_.BorderColor[0] = 0.0f;
		sampler_state_.BorderColor[1] = 0.0f;
		sampler_state_.BorderColor[2] = 0.0f;
		sampler_state_.BorderColor[3] = 0.0f;
		sampler_state_.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		sampler_state_.MinLOD = 0.0f;
		sampler_state_.MaxLOD = D3D12_FLOAT32_MAX;
		sampler_state_.MipLODBias = 0.0f;
		sampler_state_.MaxAnisotropy = 8;
		sampler_state_.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
		sampler_state_.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_state_.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		sampler_state_.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;

		blend_state_ = {};
		blend_state_.IndependentBlendEnable = FALSE;
		blend_state_.RenderTarget[0].BlendEnable = FALSE;
		blend_state_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		blend_state_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		blend_state_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		blend_state_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		blend_state_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		blend_state_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		blend_state_.RenderTarget[0].RenderTargetWriteMask = 0;
		blend_state_.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		blend_state_.RenderTarget[0].BlendEnable = TRUE;
	}

	//------------------------------------------------------------------------------------------------------
	void ShadowRenderer::CreatePSO()
	{
		GraphicsPSO& pipeline_state = GraphicsPSO::Get("shadow_object_render");
		pipeline_state.SetRootSignature(root_signature_);
		pipeline_state.SetVertexShader(vertex_shader_->GetShaderByteCode());
		pipeline_state.SetPixelShader(pixel_shader_->GetShaderByteCode());
		pipeline_state.SetBlendState(blend_state_);
		pipeline_state.SetDepthStencilState(depth_stencil_state_);
		pipeline_state.SetRasterizerState(rasterizer_state_);
		pipeline_state.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_state.SetInputLayout(_countof(shadow_object_input), shadow_object_input);
		pipeline_state.SetSampleMask(0xFFFFFFFF);
		pipeline_state.SetRenderTargetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_D32_FLOAT);
		pipeline_state.Finalize();

		GraphicsPSO& pipeline_state2 = GraphicsPSO::Get("shadow_object_render_skinned");
		pipeline_state2.SetRootSignature(root_signature_skinned_);
		pipeline_state2.SetVertexShader(Get::ResourceManager()->GetShader("shadow_skinned_vs.cso")->GetShaderByteCode());
		pipeline_state2.SetPixelShader(Get::ResourceManager()->GetShader("shadow_ps.cso")->GetShaderByteCode());
		pipeline_state2.SetBlendState(blend_state_);
		pipeline_state2.SetDepthStencilState(depth_stencil_state_);
		pipeline_state2.SetRasterizerState(rasterizer_state_);
		pipeline_state2.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_state2.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		pipeline_state2.SetSampleMask(0xFFFFFFFF);
		pipeline_state2.SetRenderTargetFormat(DXGI_FORMAT_R32G32B32A32_FLOAT, DXGI_FORMAT_D32_FLOAT);
		pipeline_state2.Finalize();
	}

	//------------------------------------------------------------------------------------------------------
	void ShadowRenderer::CreateRootSignature()
	{
		root_signature_.Create(1, 1);
		root_signature_[0].InitAsConstantBuffer(0);
		root_signature_.InitStaticSampler(0, sampler_state_);
		root_signature_.Finalize(L"RootSignatureShadowObject", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		root_signature_skinned_.Create(2, 1);
		root_signature_skinned_[0].InitAsConstantBuffer(0);
		root_signature_skinned_[1].InitAsBufferSRV(0);
		root_signature_skinned_.InitStaticSampler(0, sampler_state_);
		root_signature_skinned_.Finalize(L"RootSignatureShadowObjectSkinned", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	}

	//------------------------------------------------------------------------------------------------------
	void ShadowRenderer::RenderMap(int index, DirectX::XMMATRIX view, DirectX::XMMATRIX projection)
	{
		ShadowData map;
		map.depth_map_ = shadow_maps_[index].depth_map_;
		map.view_projection_ = DirectX::XMMatrixMultiply(view, projection);

		GraphicsContext& context = GraphicsContext::Begin(L"ShadowMap");

		ID3D12DescriptorHeap* heaps[2] = { Get::CbvSrvUavHeap().Get(), Get::SamplerHeap().Get() };
		D3D12_DESCRIPTOR_HEAP_TYPE heap_types[2] = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER };
		context.SetDescriptorHeaps(2, heap_types, heaps);

		context.TransitionResource(*map.depth_map_, D3D12_RESOURCE_STATE_RENDER_TARGET);
		context.SetRenderTarget(map.depth_map_->GetRTV(), depth_buffer_.GetDSV());

		context.ClearColor(*map.depth_map_);
		context.ClearDepth(depth_buffer_);
		context.SetViewportAndScissor(0, 0, render_width_, render_height_);
		context.FlushResourceBarriers();

		context.SetRootSignature(root_signature_);
		context.SetPipelineState(GraphicsPSO::Get("shadow_object_render"));

		std::vector<Renderable*> all_renderables = SGNode::FindAllComponents<Renderable>();
		for (int i = 0; i < all_renderables.size(); i++)
		{
			all_renderables[i]->DrawBasic(context, view, projection);
		}

		context.SetRootSignature(root_signature_skinned_);
		context.SetPipelineState(GraphicsPSO::Get("shadow_object_render_skinned"));

		std::vector<SkinnedRenderable*> all_renderables2 = SGNode::FindAllComponents<SkinnedRenderable>();
		for (int i = 0; i < all_renderables2.size(); i++)
		{
			all_renderables2[i]->DrawBasic(context, view, projection);
		}

		context.Finish(true);

		shadow_maps_[index] = map;
	}
}