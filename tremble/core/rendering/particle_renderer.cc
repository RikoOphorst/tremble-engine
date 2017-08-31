#include "particle_renderer.h"

#include "shader.h"
#include "upload_buffer.h"
#include "pipeline_state.h"
#include "root_signature.h"
#include "graphics_context.h"
#include "texture.h"
#include "descriptor_heap.h"
#include "renderer.h"
#include "../scene_graph/component_manager.h"
#include "../resources/resource_manager.h"
#include "../../components/rendering/camera.h"
#include "../../components/rendering/particle_system.h"

namespace tremble
{
	void ParticleRenderer::Startup()
	{
		// load shaders
		pixel_shader_ = Get::ResourceManager()->GetShader("particle_ps.cso");
		geometry_shader_ = Get::ResourceManager()->GetShader("particle_gs.cso");
		vertex_shader_ = Get::ResourceManager()->GetShader("particle_vs.cso");

		pass_constants_.Create(L"ParticlePassData", 24U, (sizeof(ParticlePassData) + 255) & ~255);

		CreateRenderResources();

		CreateRootSignature();

		CreatePSO();
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleRenderer::Draw(GraphicsContext& context, Camera* camera)
	{
		// Get all particle systems
		std::vector<ParticleSystem*> components_ = Get::ComponentManager()->GetComponents<ParticleSystem>();

		if (components_.size() == 0) return;

		context.SetRootSignature(root_signature_);
		context.SetPipelineState(GraphicsPSO::Get("particle_draw"));

		int rendered = 0;

		for (size_t i = 0; i < components_.size(); i++) {
			// Spawn and update particles
			components_[i]->Update(camera);

			// Skip if no alive particles
			if (components_[i]->GetParticleCount() == 0) continue;

			// Upload scene data
			ParticlePassData pass_data;
			pass_data.projection = camera->GetProjection();
			pass_data.view = camera->GetView();
			pass_data.aspect_ratio = (float)Get::SwapChain().GetBackBuffer().GetWidth() / Get::SwapChain().GetBackBuffer().GetHeight();
			pass_data.textured = components_[i]->GetTexture() != nullptr;
			pass_constants_.InsertDataByElement(rendered, &pass_data);

			// Upload particles
			CommandContext::InitializeBuffer(components_[i]->GetBuffer(), components_[i]->GetParticles(), components_[i]->GetParticleCount() * sizeof(ParticleRenderable), false, 0);

			// Set buffers and texture
			context.SetConstantBuffer(0, pass_constants_.GetAddressByElement(rendered));
			context.SetBufferSRV(1, components_[i]->GetBuffer(), 0);
			if(pass_data.textured) context.SetDescriptorTable(2, Get::CbvSrvUavHeap().GetGPUDescriptorById(components_[i]->GetTexture()->GetSRV()));

			// Render
			context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
			context.SetVertexBuffers(0, 0, nullptr);
			context.Draw(components_[i]->GetParticleCount());

			rendered++;
		}
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleRenderer::Destroy()
	{
		delete pixel_shader_;
		delete geometry_shader_;
		delete vertex_shader_;
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleRenderer::CreateRenderResources()
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
	void ParticleRenderer::CreatePSO()
	{
		GraphicsPSO& pipeline_state = GraphicsPSO::Get("particle_draw");
		pipeline_state.SetRootSignature(root_signature_);
		pipeline_state.SetVertexShader(vertex_shader_->GetShaderByteCode());
		pipeline_state.SetGeometryShader(geometry_shader_->GetShaderByteCode());
		pipeline_state.SetPixelShader(pixel_shader_->GetShaderByteCode());
		pipeline_state.SetBlendState(blend_state_);
		pipeline_state.SetDepthStencilState(depth_stencil_state_);
		pipeline_state.SetRasterizerState(rasterizer_state_);
		pipeline_state.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
		pipeline_state.SetInputLayout(_countof(interface_font_input), interface_font_input);
		pipeline_state.SetSampleMask(0xFFFFFFFF);
		pipeline_state.SetRenderTargetFormat(Get::Renderer()->GetSwapChain().GetBackBuffer().GetFormat(), DXGI_FORMAT_D32_FLOAT); //TODO use proper depth stencil format
		pipeline_state.Finalize();
	}

	//------------------------------------------------------------------------------------------------------
	void ParticleRenderer::CreateRootSignature()
	{
		root_signature_.Create(3, 1);
		root_signature_[0].InitAsConstantBuffer(0); // pass
		root_signature_[1].InitAsBufferSRV(0);  // sprite data
		root_signature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);  // texture
		root_signature_.InitStaticSampler(0, sampler_state_);
		root_signature_.Finalize(L"RootSignatureParticle", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	}
}