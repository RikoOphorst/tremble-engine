#include "renderer.h"

#include "command_context_manager.h"
#include "command_manager.h"
#include "command_context.h"
#include "graphics_context.h"
#include "command_queue.h"
#include "device.h"
#include "descriptor_heap.h"
#include "root_signature.h"
#include "pipeline_state.h"
#include "constants_helper.h"
#include "material.h"
#include "shader.h"
#include "texture.h"
#include "graphics.h"
#include "../utilities/debug.h"
#include "../win32/window.h"
#include "../resources/mesh.h"
#include "../resources/resource_manager.h"
#include "../utilities/timer.h"
#include "../utilities/octree.h"
#include "../game_manager.h"
#include "../input/input_manager.h"
#include "../get.h"
#include "../scene_graph/scene_graph.h"
#include "../../components/rendering/renderable.h"
#include "../../components/rendering/skinned_renderable.h"
#include "../../components/rendering/camera.h"
#include "../../components/rendering/light.h"

#define SWAP_CHAIN_BUFFER_COUNT 2

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Renderer::Renderer() :
		num_object_cbs_(0),
		num_material_cbs_(0),
		camera_(nullptr)
	{
		depthpass_ = Get::Config().depth_pre_pass;
	}

	//------------------------------------------------------------------------------------------------------
	Renderer::~Renderer()
	{
		FlushCommandQueue();
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::Startup()
	{
		CreateDevice();
		Graphics::Initialize();
		CreateCommandObjects();
		CreateDescriptorHeaps();
		CreateSwapChain();
		CreateBaseResources();
		CreatePSOs();
		FlushCommandQueue();

		buffer_manager_.Create(swap_chain_.GetBufferWidth(), swap_chain_.GetBufferHeight());
		sprite_renderer_.Startup();
		font_renderer_.Startup();
		shadow_renderer_.Startup();
		particle_renderer_.Startup();
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::CreateDevice()
	{
		if (Get::Config().d3d12_debug_layer)
		{
			ID3D12Debug* debug_controller;
			if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller))))
			{
				debug_controller->EnableDebugLayer();
			}
			debug_controller->Release();
		}

		device_.Create(Get::ConfigManager()->GetAdapters()[Get::Config().adapter]);
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::CreateCommandObjects()
	{
		Get::CommandManager()->Startup(device_);
		Get::CommandContextManager()->Startup();
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::CreateSwapChain()
	{
		swap_chain_.Create(
			Get::Window()->GetWindowHandle(),
			Get::ConfigManager()->GetRenderResolutions()[Get::Config().render_resolution].width,
			Get::ConfigManager()->GetRenderResolutions()[Get::Config().render_resolution].height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			SWAP_CHAIN_BUFFER_COUNT,
			DXGI_SWAP_EFFECT_FLIP_DISCARD,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		);
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::CreateDescriptorHeaps()
	{
		rtv_heap_.Create(			D3D12_DESCRIPTOR_HEAP_TYPE_RTV,			D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 4096U);
		dsv_heap_.Create(			D3D12_DESCRIPTOR_HEAP_TYPE_DSV,			D3D12_DESCRIPTOR_HEAP_FLAG_NONE, 4096U);
		cbv_srv_uav_heap_.Create(	D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 4096U * 8);
		sampler_heap_.Create(		D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER,		D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, 1024U);
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::CreateBaseResources()
	{
		depth_buffer_.Create(L"SceneDepthBuffer", swap_chain_.GetBufferWidth(), swap_chain_.GetBufferHeight(), DXGI_FORMAT_D32_FLOAT);
		pass_constants_.Create(L"PassConstantsBuffer", 1U, (sizeof(PassConstants) + 255) & ~255);
		object_constants_.Create(L"ObjectConstantsBuffer", 1024U, (sizeof(ObjectConstants) + 255) & ~255);
		material_constants_.Create(L"MaterialConstantsBuffer", 1024U, (sizeof(MaterialConstants) + 255) & ~255);
		debug_constants_.Create(L"DebugConstantsBuffer", 1024U, (sizeof(DebugConstants) + 255) & ~255);
		light_upload_buffer_.Create(L"LightUploadBuffer", 64U, sizeof(LightConstants));
		light_buffer_.Create(L"LightBuffer", 64U, sizeof(LightConstants));
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::CreatePSOs()
	{
		D3D12_DEPTH_STENCIL_DESC desc = Graphics::depth_state_default;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;

		GraphicsPSO& depth_pre_pass = GraphicsPSO::Get("depth_pre_pass");
		depth_pre_pass.SetRootSignature(Graphics::root_signature_depth_pre_pass);
		depth_pre_pass.SetVertexShader(Get::ResourceManager()->GetShader("shadow_vs.cso")->GetShaderByteCode());
		depth_pre_pass.SetBlendState(Graphics::blend_state_traditional);
		depth_pre_pass.SetDepthStencilState(desc);
		depth_pre_pass.SetRasterizerState(Graphics::rasterizer_default_cw);
		depth_pre_pass.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		depth_pre_pass.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		depth_pre_pass.SetSampleMask(0xFFFFFFFF);
		depth_pre_pass.SetRenderTargetFormats(0, nullptr, depth_buffer_.GetFormat());
		depth_pre_pass.Finalize();

		GraphicsPSO& depth_pre_pass_skinned = GraphicsPSO::Get("depth_pre_pass_skinned");
		depth_pre_pass_skinned.SetRootSignature(Graphics::root_signature_depth_pre_pass);
		depth_pre_pass_skinned.SetVertexShader(Get::ResourceManager()->GetShader("shadow_skinned_vs.cso")->GetShaderByteCode());
		depth_pre_pass_skinned.SetBlendState(Graphics::blend_state_traditional);
		depth_pre_pass_skinned.SetDepthStencilState(desc);
		depth_pre_pass_skinned.SetRasterizerState(Graphics::rasterizer_default_cw);
		depth_pre_pass_skinned.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		depth_pre_pass_skinned.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		depth_pre_pass_skinned.SetSampleMask(0xFFFFFFFF);
		depth_pre_pass_skinned.SetRenderTargetFormats(0, nullptr, depth_buffer_.GetFormat());
		depth_pre_pass_skinned.Finalize();

		desc = Graphics::depth_state_disabled;
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

		GraphicsPSO& render_lit_prepass = GraphicsPSO::Get("render_lit_prepass");
		render_lit_prepass.SetRootSignature(Graphics::root_signature_default);
		render_lit_prepass.SetVertexShader(Get::ResourceManager()->GetShader("default_vs.cso")->GetShaderByteCode());
		render_lit_prepass.SetPixelShader(Get::ResourceManager()->GetShader("default_ps.cso")->GetShaderByteCode());
		render_lit_prepass.SetBlendState(Graphics::blend_state_traditional);
		render_lit_prepass.SetDepthStencilState(desc);
		render_lit_prepass.SetRasterizerState(Get::Config().wireframe_rendering ? Graphics::rasterizer_no_culling_wireframe : Graphics::rasterizer_default_cw);
		render_lit_prepass.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		render_lit_prepass.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		render_lit_prepass.SetSampleMask(0xFFFFFFFF);
		render_lit_prepass.SetRenderTargetFormat(swap_chain_.GetBackBuffer().GetFormat(), depth_buffer_.GetFormat());
		render_lit_prepass.Finalize();

		GraphicsPSO& render_lit_prepass_skinned = GraphicsPSO::Get("render_lit_prepass_skinned");
		render_lit_prepass_skinned.SetRootSignature(Graphics::root_signature_default);
		render_lit_prepass_skinned.SetVertexShader(Get::ResourceManager()->GetShader("default_skinned_vs.cso")->GetShaderByteCode());
		render_lit_prepass_skinned.SetPixelShader(Get::ResourceManager()->GetShader("default_ps.cso")->GetShaderByteCode());
		render_lit_prepass_skinned.SetBlendState(Graphics::blend_state_traditional);
		render_lit_prepass_skinned.SetDepthStencilState(desc);
		render_lit_prepass_skinned.SetRasterizerState(Get::Config().wireframe_rendering ? Graphics::rasterizer_no_culling_wireframe : Graphics::rasterizer_default_cw);
		render_lit_prepass_skinned.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		render_lit_prepass_skinned.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		render_lit_prepass_skinned.SetSampleMask(0xFFFFFFFF);
		render_lit_prepass_skinned.SetRenderTargetFormat(swap_chain_.GetBackBuffer().GetFormat(), depth_buffer_.GetFormat());
		render_lit_prepass_skinned.Finalize();

		GraphicsPSO& render_lit = GraphicsPSO::Get("render_lit");
		render_lit.SetRootSignature(Graphics::root_signature_default);
		render_lit.SetVertexShader(Get::ResourceManager()->GetShader("default_vs.cso")->GetShaderByteCode());
		render_lit.SetPixelShader(Get::ResourceManager()->GetShader("default_ps.cso")->GetShaderByteCode());
		render_lit.SetBlendState(Graphics::blend_state_traditional);
		render_lit.SetDepthStencilState(Graphics::depth_state_default);
		render_lit.SetRasterizerState(Get::Config().wireframe_rendering ? Graphics::rasterizer_no_culling_wireframe : Graphics::rasterizer_default_cw);
		render_lit.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		render_lit.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		render_lit.SetSampleMask(0xFFFFFFFF);
		render_lit.SetRenderTargetFormat(swap_chain_.GetBackBuffer().GetFormat(), depth_buffer_.GetFormat());
		render_lit.Finalize();

		GraphicsPSO& render_lit_skinned = GraphicsPSO::Get("render_lit_skinned");
		render_lit_skinned.SetRootSignature(Graphics::root_signature_default);
		render_lit_skinned.SetVertexShader(Get::ResourceManager()->GetShader("default_skinned_vs.cso")->GetShaderByteCode());
		render_lit_skinned.SetPixelShader(Get::ResourceManager()->GetShader("default_ps.cso")->GetShaderByteCode());
		render_lit_skinned.SetBlendState(Graphics::blend_state_traditional);
		render_lit_skinned.SetDepthStencilState(Graphics::depth_state_default);
		render_lit_skinned.SetRasterizerState(Get::Config().wireframe_rendering ? Graphics::rasterizer_no_culling_wireframe : Graphics::rasterizer_default_cw);
		render_lit_skinned.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		render_lit_skinned.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		render_lit_skinned.SetSampleMask(0xFFFFFFFF);
		render_lit_skinned.SetRenderTargetFormat(swap_chain_.GetBackBuffer().GetFormat(), depth_buffer_.GetFormat());
		render_lit_skinned.Finalize();

		GraphicsPSO& default_debug = GraphicsPSO::Get("default_debug");
		default_debug.SetRootSignature(Graphics::root_signature_default);
		default_debug.SetVertexShader(Get::ResourceManager()->GetShader("debug_default_vs.cso")->GetShaderByteCode());
		default_debug.SetPixelShader(Get::ResourceManager()->GetShader("debug_default_ps.cso")->GetShaderByteCode());
		default_debug.SetBlendState(Graphics::blend_state_disabled);
		default_debug.SetDepthStencilState(Graphics::depth_state_read_write);
		default_debug.SetRasterizerState(Graphics::rasterizer_default);
		default_debug.SetPrimitiveTopologyType(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
		default_debug.SetInputLayout(_countof(input_element_vertex_desc), input_element_vertex_desc);
		default_debug.SetSampleMask(0xFFFFFFFF);
		default_debug.SetRenderTargetFormat(swap_chain_.GetBackBuffer().GetFormat(), depth_buffer_.GetFormat());
		default_debug.Finalize();
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::Draw(Timer* timer)
	{
		camera_->Update();

		shadow_renderer_.Draw();

		GraphicsContext& context = GraphicsContext::Begin(L"SceneRender");
		
		ID3D12DescriptorHeap* heaps[2] = { cbv_srv_uav_heap_.Get(), sampler_heap_.Get() };
		D3D12_DESCRIPTOR_HEAP_TYPE heap_types[2] = { D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER };
		
		context.SetDescriptorHeaps(2, heap_types, heaps);
		context.TransitionResource(swap_chain_.GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET);
		
		if (depthpass_ == true)
		{
			context.FlushResourceBarriers();
			context.SetPipelineState(GraphicsPSO::Get("depth_pre_pass"));
			context.SetRootSignature(Graphics::root_signature_depth_pre_pass);
			context.SetRenderTargets(0, nullptr, depth_buffer_.GetDSV());
			context.ClearDepth(depth_buffer_);
			context.SetViewportAndScissor(0, 0, swap_chain_.GetBufferWidth(), swap_chain_.GetBufferHeight());

			if (!Get::Config().frustum_culling)
			{
				const std::vector<Renderable*>& all_renderables = SGNode::FindAllComponents<Renderable>();

				for (int i = 0; i < all_renderables.size(); i++)
				{
					all_renderables[i]->DrawBasic(context, camera_);
				}
			}
			else
			{
				std::vector<OctreeObject*> all_nodes;
				Get::Octree()->GetContainedObjects(camera_->GetFrustum(), all_nodes);

				for (int i = 0; i < all_nodes.size(); i++)
				{
					all_nodes[i]->renderable->DrawBasic(context, all_nodes[i], camera_);
				}
			}

			context.SetPipelineState(GraphicsPSO::Get("depth_pre_pass_skinned"));
			context.SetRootSignature(Graphics::root_signature_depth_pre_pass);
			context.SetViewportAndScissor(0, 0, swap_chain_.GetBufferWidth(), swap_chain_.GetBufferHeight());

			auto& skinned_renderables = SGNode::FindAllComponents<SkinnedRenderable>();
			for (int i = 0; i < skinned_renderables.size(); i++)
			{
				skinned_renderables[i]->DrawBasic(context, camera_);
			}
		}

		ConstantsHelper::UpdatePassConstants(pass_constants_, DirectX::XMFLOAT2((float)swap_chain_.GetBufferWidth(), (float)swap_chain_.GetBufferHeight()), timer, camera_);
		ConstantsHelper::UpdateLightConstants(light_upload_buffer_, SGNode::FindAllComponents<Light>(), camera_);

		context.CopyBuffer(light_buffer_, light_upload_buffer_);
		context.TransitionResource(light_buffer_, D3D12_RESOURCE_STATE_GENERIC_READ);
		
		context.SetRenderTarget(swap_chain_.GetBackBuffer().GetRTV(), depth_buffer_.GetDSV());
		context.ClearColor(swap_chain_.GetBackBuffer());
		if (depthpass_ != true)
		{
			context.TransitionResource(depth_buffer_, D3D12_RESOURCE_STATE_DEPTH_WRITE, true);
			context.ClearDepth(depth_buffer_);
		}
		context.SetViewportAndScissor(0, 0, swap_chain_.GetBufferWidth(), swap_chain_.GetBufferHeight());
		
		context.FlushResourceBarriers();

		{
			context.SetRootSignature(Graphics::root_signature_default);
			if (depthpass_ == true)
			{
				context.SetPipelineState(GraphicsPSO::Get("render_lit_prepass"));
			}
			else
			{
				context.SetPipelineState(GraphicsPSO::Get("render_lit"));
			}
			context.SetConstantBuffer(1, pass_constants_.GetRootCBV());
			context.SetBufferSRV(3, light_buffer_);

			shadow_renderer_.UploadData(context);

			if (!Get::Config().frustum_culling)
			{
				std::vector<Renderable*> all_renderables = SGNode::FindAllComponents<Renderable>();

				for (int i = 0; i < all_renderables.size(); i++)
				{
					all_renderables[i]->Draw(context, camera_);
				}
			}
			else
			{
				std::vector<OctreeObject*> all_nodes;
				Get::Octree()->GetContainedObjects(camera_->GetFrustum(), all_nodes);

				for (int i = 0; i < all_nodes.size(); i++)
				{
					all_nodes[i]->renderable->Draw(context, all_nodes[i], camera_);
				}
			}
		}

		{
			context.SetRootSignature(Graphics::root_signature_default);
			if (depthpass_ == true)
			{
				context.SetPipelineState(GraphicsPSO::Get("render_lit_prepass_skinned"));
			}
			else
			{
				context.SetPipelineState(GraphicsPSO::Get("render_lit_skinned"));
			}
			context.SetConstantBuffer(1, pass_constants_.GetRootCBV());
			context.SetBufferSRV(3, light_buffer_);

			shadow_renderer_.UploadData(context);

			auto& skinned_renderables = SGNode::FindAllComponents<SkinnedRenderable>();
			for (int i = 0; i < skinned_renderables.size(); i++)
			{
				skinned_renderables[i]->Draw(context, camera_);
			}
		}
		
		DrawDebugVolumes(context, debug_volumes_);
		
		particle_renderer_.Draw(context, camera_);
		sprite_renderer_.Draw(context);
		font_renderer_.Draw(context);
		
		context.TransitionResource(swap_chain_.GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT);
		
		CheckDeviceRemoved();
		
		context.Finish(true);

		swap_chain_.Present(false);

		for (int i = static_cast<int>(debug_volumes_.size()) - 1; i >= 0; i--)
		{
//			delete debug_volumes_[i].mesh;

			debug_volumes_.erase(debug_volumes_.begin() + i);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::FlushCommandQueue()
	{
		Get::CommandManager()->WaitForIdleGPU();
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::DrawDebugVolumes(GraphicsContext& context, std::vector<DebugVolume>& debug_volumes)
	{
		context.SetRootSignature(Graphics::root_signature_default);
		context.SetPipelineState(GraphicsPSO::Get("default_debug"));
		
		for (int i = 0; i < static_cast<int>(debug_volumes.size()); i++)
		{
			DebugVolume& current = debug_volumes[i];

			current.mesh->Draw(context);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::CheckDeviceRemoved()
	{
		HRESULT hr = device_.Get()->GetDeviceRemovedReason();
		if (hr != S_OK)
		{
			_com_error error(hr);
			std::cout << error.ErrorMessage() << std::endl;
			assert(false);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderer::SetCamera(Camera* camera)
	{
		camera_ = camera;
	}
	
	//------------------------------------------------------------------------------------------------------
	void Renderer::RenderDebugVolume(const DebugVolume& volume)
	{
		debug_volumes_.push_back(volume);
	}
}