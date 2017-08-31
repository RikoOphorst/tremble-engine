#include "interface_sprite_renderer.h"

#include "renderer.h"
#include "shader.h"
#include "upload_buffer.h"
#include "pipeline_state.h"
#include "root_signature.h"
#include "graphics_context.h"
#include "texture.h"
#include "../scene_graph/component_manager.h"
#include "../resources/resource_manager.h"
#include "../../components/rendering/image_component.h"
#include "../../components/rendering/button_component.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	void InterfaceSpriteRenderer::Startup()
	{
		// load shaders
		pixel_shader_ = Get::ResourceManager()->GetShader("interface_sprite_ps.cso");
		vertex_shader_ = Get::ResourceManager()->GetShader("interface_sprite_vs.cso");

		// create constant buffers
		pass_constants_.Create(L"SpritePassConstants", 1U, (sizeof(InterfaceSpritePassConstants) + 255) & ~255);
		object_constants_.Create(L"SpriteObjectConstants", 1024U, (sizeof(InterfaceSpriteObjectConstants) + 255) & ~255);
		
		CreateRenderResources();

		CreateRootSignature();

		CreatePSO();

		CreateGeometry();
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceSpriteRenderer::Draw(GraphicsContext& context)
	{
		std::vector<ImageComponent*> images_ = Get::ComponentManager()->GetComponents<ImageComponent>();
		std::vector<ButtonComponent*> buttons_ = Get::ComponentManager()->GetComponents<ButtonComponent>();

		if (images_.size() == 0 && buttons_.size() == 0) 
		{
			return;
		}

		// activate shader
		context.SetRootSignature(root_signature_);
		context.SetPipelineState(GraphicsPSO::Get("interface_sprite_draw"));

		InterfaceSpritePassConstants pass_data;
		InterfaceSpriteObjectConstants object_data;
		Texture* texture_;

		int render_id = 0;

		// sprites
		for (size_t i = 0; (i < images_.size() && i < 1024); i++) {

			// prepare constant buffers
			pass_data.view_projection = DirectX::XMMatrixOrthographicLH(Get::Renderer()->GetVirtualSizeX(), Get::Renderer()->GetVirtualSizeY(), 0.01f, 1000.0f);
			pass_data.view_view = DirectX::XMMatrixScaling(1, -1, 1) * DirectX::XMMatrixTranslation(-1.0f, 1.0f, 0);
			pass_constants_.InsertDataByElement(0, &pass_data);

			if (images_[i]->UsesCustomMatrix()) {
				object_data.transform = images_[i]->GetMatrix();
			}
			else {
				object_data.transform = DirectX::XMMatrixTranslation(-images_[i]->GetCenter().GetX(), -images_[i]->GetCenter().GetY(), 0) * DirectX::XMMatrixScaling(images_[i]->GetSize().GetX(), images_[i]->GetSize().GetY(), 1) * DirectX::XMMatrixRotationRollPitchYaw(0, 0, (images_[i]->GetRotation() * 3.24f) / 180.0f) * DirectX::XMMatrixTranslation(images_[i]->GetPosition().GetX(), images_[i]->GetPosition().GetY(), 1 + images_[i]->GetLayer());
			}
			object_data.color = images_[i]->GetColor();
			object_data.uv_min = images_[i]->GetUVMin();
			object_data.uv_max = images_[i]->GetUVMax();
			object_constants_.InsertDataByElement(render_id, &object_data);

			texture_ = images_[i]->GetTexture();
			if (texture_ == nullptr) 
			{
				continue;
			}

			//upload buffers and texture
			context.SetConstantBuffer(0, pass_constants_.GetAddressByElement(0));
			context.SetConstantBuffer(1, object_constants_.GetAddressByElement(render_id));
			if (!texture_->AreBuffersBuilt())
			{
				texture_->BuildBuffers();
			}
			context.SetDescriptorTable(2, Get::CbvSrvUavHeap().GetGPUDescriptorById(texture_->GetSRV()));

			// render
			context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context.SetVertexBuffer(0, vertex_buffer_view_);
			context.DrawInstanced(static_cast<UINT>(vertices_.size()), 1);

			render_id++;
		}

		// buttons
		for (size_t i = 0; (i < buttons_.size() && i < 1024); i++) {

			// prepare constant buffers
			pass_data.view_projection = DirectX::XMMatrixOrthographicLH(Get::Renderer()->GetVirtualSizeX(), Get::Renderer()->GetVirtualSizeY(), 0.01f, 1000.0f);
			pass_data.view_view = DirectX::XMMatrixScaling(1, -1, 1) * DirectX::XMMatrixTranslation(-1.0f, 1.0f, 0);
			pass_constants_.InsertDataByElement(0, &pass_data);

			// Get texture, revert to normal state if possible
			ButtonState used_state = buttons_[i]->GetState();
			texture_ = buttons_[i]->GetTexture(used_state);
			if (texture_ == nullptr) {
				texture_ = buttons_[i]->GetTexture(ButtonState::Normal);
				used_state = ButtonState::Normal;
			}

			if (texture_ == nullptr)
			{
				continue;
			}

			object_data.transform = buttons_[i]->GetFinalTransform();
			object_data.uv_min = buttons_[i]->GetUVMin(used_state);
			object_data.uv_max = buttons_[i]->GetUVMax(used_state);
			object_data.color = buttons_[i]->GetColor(buttons_[i]->GetState());
			object_constants_.InsertDataByElement(render_id, &object_data);

			//upload buffers and texture
			context.SetConstantBuffer(0, pass_constants_.GetAddressByElement(0));
			context.SetConstantBuffer(1, object_constants_.GetAddressByElement(render_id));
			if (!texture_->AreBuffersBuilt())
			{
				texture_->BuildBuffers();
			}
			context.SetDescriptorTable(2, Get::CbvSrvUavHeap().GetGPUDescriptorById(texture_->GetSRV()));

			// render
			context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context.SetVertexBuffer(0, vertex_buffer_view_);
			context.DrawInstanced(static_cast<UINT>(vertices_.size()), 1);

			render_id++;
		}
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceSpriteRenderer::Destroy()
	{
		delete pixel_shader_;
		delete vertex_shader_;
		delete pass_constants_;
		delete object_constants_;
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceSpriteRenderer::CreateRenderResources()
	{
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
		rasterizer_state_.CullMode = D3D12_CULL_MODE_NONE;
		rasterizer_state_.FrontCounterClockwise = FALSE;
		rasterizer_state_.DepthBias = D3D12_DEFAULT_DEPTH_BIAS;
		rasterizer_state_.DepthBiasClamp = D3D12_DEFAULT_DEPTH_BIAS_CLAMP;
		rasterizer_state_.SlopeScaledDepthBias = D3D12_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
		rasterizer_state_.DepthClipEnable = TRUE;
		rasterizer_state_.MultisampleEnable = FALSE;
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
	}

	//------------------------------------------------------------------------------------------------------

	void InterfaceSpriteRenderer::CreatePSO()
	{
		GraphicsPSO& pipeline_state = GraphicsPSO::Get("interface_sprite_draw");
		pipeline_state.SetRootSignature(root_signature_);
		pipeline_state.SetVertexShader(vertex_shader_->GetShaderByteCode());
		pipeline_state.SetPixelShader(pixel_shader_->GetShaderByteCode());
		pipeline_state.SetBlendState(blend_state_);
		pipeline_state.SetDepthStencilState(depth_stencil_state_);
		pipeline_state.SetRasterizerState(rasterizer_state_);
		pipeline_state.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		pipeline_state.SetInputLayout(_countof(interface_sprite_input), interface_sprite_input);
		pipeline_state.SetSampleMask(0xFFFFFFFF);
		pipeline_state.SetRenderTargetFormat(Get::Renderer()->GetSwapChain().GetBackBuffer().GetFormat(), DXGI_FORMAT_D32_FLOAT); //TODO use proper depth stencil format
		pipeline_state.Finalize();
	}

	//------------------------------------------------------------------------------------------------------

	void InterfaceSpriteRenderer::CreateRootSignature()
	{
		root_signature_.Create(3, 1);
		root_signature_[0].InitAsConstantBuffer(0); // pass
		root_signature_[1].InitAsConstantBuffer(1); // object
		root_signature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);  // texture
		root_signature_.InitStaticSampler(0, sampler_state_);
		root_signature_.Finalize(L"RootSignatureInterfaceSprite", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	}

	//------------------------------------------------------------------------------------------------------

	void InterfaceSpriteRenderer::CreateGeometry()
	{
		vertices_.clear();
		vertices_.push_back(SpriteVertex{ { 0, 0, 0, 1 },{ 0,0 } });
		vertices_.push_back(SpriteVertex{ { 1, 0, 0, 1 },{ 1,0 } });
		vertices_.push_back(SpriteVertex{ { 1, 1, 0, 1 },{ 1,1 } });

		vertices_.push_back(SpriteVertex{ { 1, 1, 0, 1 },{ 1,1 } });
		vertices_.push_back(SpriteVertex{ { 0, 1, 0, 1 },{ 0,1 } });
		vertices_.push_back(SpriteVertex{ { 0, 0, 0, 1 },{ 0,0 } });

		vertex_buffer_.Create(L"SpriteGeometry", static_cast<UINT>(vertices_.size()), sizeof(SpriteVertex), &vertices_[0]);

		vertex_buffer_view_ = {};
		vertex_buffer_view_.StrideInBytes = sizeof(SpriteVertex);
		vertex_buffer_view_.SizeInBytes = static_cast<UINT>(vertices_.size() * sizeof(SpriteVertex));
		vertex_buffer_view_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();
	}
}