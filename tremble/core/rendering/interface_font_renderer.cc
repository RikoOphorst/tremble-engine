#include "interface_font_renderer.h"

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
#include "../../components/rendering/text_component.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace tremble
{
	void InterfaceFontRenderer::Startup()
	{
		// load shaders
		pixel_shader_ = Get::ResourceManager()->GetShader("interface_font_ps.cso");
		geometry_shader_ = Get::ResourceManager()->GetShader("interface_font_gs.cso");
		vertex_shader_ = Get::ResourceManager()->GetShader("interface_font_vs.cso");

		// create constant buffers
		pass_constants_.Create(L"FontPassConstants", 1U, (sizeof(InterfaceSpritePassConstants) + 255) & ~255);

		CreateRenderResources();

		CreateRootSignature();

		CreatePSO();

		CreateGeometry();

		char_buffer_.Create(L"FontCharData", char_limit_, sizeof(InterfaceFontData));

		render_data_ = (InterfaceFontData*) malloc(char_limit_ * sizeof(InterfaceFontData));
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceFontRenderer::Draw(GraphicsContext& context)
	{
		std::vector<TextComponent*> components_ = Get::ComponentManager()->GetComponents<TextComponent>();

		if (components_.size() == 0 && components_.size() == 0)
		{
			return;
		}

		// activate shader
		context.SetRootSignature(root_signature_);
		context.SetPipelineState(GraphicsPSO::Get("interface_font_draw"));

		int render_id = 0;

		std::vector<CharBatch> batches;

		// draw all components
		for (int i = 0; i < components_.size(); i++) 
		{
			if (render_id == char_limit_)
			{
				break;
			}

			if (components_[i]->GetText() == std::string("") || components_[i]->GetFont() == std::string(""))
			{
				continue;
			}

			// create batch
			batches.push_back({});
			CharBatch& batch = batches[batches.size() - 1];
			batch.offset = render_id;

			// get text and font
			std::string string = components_[i]->GetText();

			float wanted_size = components_[i]->GetSize();

			FontLevel& font = components_[i]->GetFontPtr()->FindLevel(wanted_size);

			float scale = wanted_size / font.size;

			batch.texture = font.char_map;

			// calculate component alignment
			float offset_x = -components_[i]->GetWidth() * components_[i]->GetCenter().GetX();
			float offset_y = -components_[i]->GetHeight() * components_[i]->GetCenter().GetY();

			std::vector<TextLine> lines = components_[i]->GetLines();

			float x, y;
			x = 0;
			y = lines[0].height;

			// draw every line
			for (int l = 0; l < lines.size(); l++) {
				if (render_id == char_limit_)
				{
					break;
				}
				
				// calculate line alignment
				switch (components_[i]->GetAlignment())
				{
					case Left: { x = 0; break; }
					case Middle: { x = (components_[i]->GetWidth() - lines[l].width) / 2.0f; break; }
					case Right: { x = components_[i]->GetWidth() - lines[l].width; break; }
				}

				// add all characters
				for (int s = 0; s < lines[l].text.length(); s++) {
					unsigned char c = lines[l].text[s];

					render_data_[render_id].transform = DirectX::XMMatrixScaling(font.char_info[c].width * scale, font.char_info[c].height * scale, 1) * DirectX::XMMatrixTranslation(components_[i]->GetPosition().GetX().Get() + x + offset_x + font.char_info[c].offset_x * scale, components_[i]->GetPosition().GetY().Get() + y + offset_y + font.char_info[c].offset_y * scale, 1 + components_[i]->GetLayer());
					render_data_[render_id].color = components_[i]->GetColor();
					render_data_[render_id].uv_min = { font.char_info[c].uv_x1, font.char_info[c].uv_y1 };
					render_data_[render_id].uv_max = { font.char_info[c].uv_x2, font.char_info[c].uv_y2 };

					x += font.char_info[c].advance * scale;

					render_id++;

					if (render_id == char_limit_) 
					{
						break;
					}
				}

				batch.count = render_id - batch.offset;
				y += lines[l].height + components_[i]->GetLineSeperation();
			}

			
		}

		if (render_id != 0)
		{
			// Upload buffer and render batches
			CommandContext::InitializeBuffer(char_buffer_, render_data_, render_id * sizeof(InterfaceFontData), false, 0);
			for (int i = 0; i < batches.size(); i++)
			{
				if (!batches[i].texture->AreBuffersBuilt())
				{
					batches[i].texture->BuildBuffers();
				}
				context.SetDescriptorTable(2, Get::CbvSrvUavHeap().GetGPUDescriptorById(batches[i].texture->GetSRV()));
				RenderBatch(context, batches[i].offset, batches[i].count);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceFontRenderer::Destroy()
	{
		delete pixel_shader_;
		delete geometry_shader_;
		delete vertex_shader_;
		delete pass_constants_;
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceFontRenderer::CreateRenderResources()
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
	void InterfaceFontRenderer::CreatePSO()
	{
		GraphicsPSO& pipeline_state = GraphicsPSO::Get("interface_font_draw");
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
	void InterfaceFontRenderer::CreateRootSignature()
	{
		root_signature_.Create(3, 1);
		root_signature_[0].InitAsConstantBuffer(0); // pass
		root_signature_[1].InitAsBufferSRV(0);  // sprite data
		root_signature_[2].InitAsDescriptorRange(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);  // texture
		root_signature_.InitStaticSampler(0, sampler_state_);
		root_signature_.Finalize(L"RootSignatureInterfaceFont", D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceFontRenderer::CreateGeometry()
	{
		for (unsigned int i = 0; i < 64; i++) {
			indices_.push_back({ i });
		}

		index_buffer_.Create(L"SpriteGeometry", static_cast<UINT>(indices_.size()), sizeof(FontVertex), &indices_[0]);

		index_buffer_view_ = {};
		index_buffer_view_.StrideInBytes = sizeof(FontVertex);
		index_buffer_view_.SizeInBytes = static_cast<UINT>(indices_.size() * sizeof(FontVertex));
		index_buffer_view_.BufferLocation = index_buffer_->GetGPUVirtualAddress();
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceFontRenderer::RenderBatch(GraphicsContext& context, int offset, int count)
	{
		InterfaceSpritePassConstants pass_data;

		context.SetBufferSRV(1, char_buffer_, offset * sizeof(InterfaceFontData));

		pass_data.view_projection = DirectX::XMMatrixOrthographicLH(Get::Renderer()->GetVirtualSizeX(), Get::Renderer()->GetVirtualSizeY(), 0.01f, 1000.0f);
		pass_data.view_view = DirectX::XMMatrixScaling(1, -1, 1) * DirectX::XMMatrixTranslation(-1.0f, 1.0f, 0);
		pass_constants_.InsertDataByElement(0, &pass_data);

		context.SetConstantBuffer(0, pass_constants_.GetAddressByElement(0));

		// render
		context.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
		context.SetVertexBuffer(0, index_buffer_view_);
		context.DrawInstanced(count, 1);
	}

	//------------------------------------------------------------------------------------------------------
	void InterfaceFontRenderer::AddFont(std::string file)
	{
		fonts_.push_back({});
		Font& newFont = fonts_[fonts_.size() - 1];
		newFont.name = file;
	}

	//------------------------------------------------------------------------------------------------------
	FontLevel& Font::FindLevel(float wanted_size)
	{
		int needed_size = std::ceil(wanted_size / 32.0f) * 32.0f;
		for (int i = 0; i < levels.size(); i++) 
		{
			if (levels[i].size == needed_size) {
				return levels[i];
			}
		}

		// Not found, generate it
		GenerateLevel(needed_size);
		return FindLevel(wanted_size);
	}

	//------------------------------------------------------------------------------------------------------
	void Font::GenerateLevel(int size)
	{
		levels.push_back({});
		FontLevel& newLevel = levels[levels.size() - 1];

		// read file
		FILE* read_file;
		fopen_s(&read_file, name.c_str(), "rb");

		fseek(read_file, 0, SEEK_END);
		long file_size = ftell(read_file);
		rewind(read_file);

		unsigned char* file_data = (unsigned char*)malloc(sizeof(char)*file_size);
		fread(file_data, sizeof(char), file_size, read_file);
		fclose(read_file);

		// laod font data
		stbtt_fontinfo font;
		stbtt_InitFont(&font, file_data, 0);

		int width = 12 * size, height = 12 * size;
		unsigned char* bitmap = (unsigned char*)malloc(width*height);

		stbtt_bakedchar baked_chars[255];

		// create bitmap
		int created_height = stbtt_BakeFontBitmap(file_data, 0, size, bitmap, width, height, 0, 255, &baked_chars[0]);

		if (created_height <= 0)
		{
			DLOG(("Font not loaded correctly: " + name).c_str());
			return;
		}

		// assign char info
		for (int i = 0; i < 255; i++)
		{
			newLevel.char_info[i].width = baked_chars[i].x1 - baked_chars[i].x0;
			newLevel.char_info[i].height = baked_chars[i].y1 - baked_chars[i].y0;
			newLevel.char_info[i].advance = baked_chars[i].xadvance;
			newLevel.char_info[i].offset_x = baked_chars[i].xoff;
			newLevel.char_info[i].offset_y = baked_chars[i].yoff;
			newLevel.char_info[i].uv_x1 = (float)baked_chars[i].x0 / width;
			newLevel.char_info[i].uv_x2 = (float)baked_chars[i].x1 / width;
			newLevel.char_info[i].uv_y1 = (float)baked_chars[i].y0 / created_height;
			newLevel.char_info[i].uv_y2 = (float)baked_chars[i].y1 / created_height;
		}

		// convert texture to rgba
		unsigned char* image = (unsigned char*)malloc(width * created_height * 4);
		for (int y = 0; y < created_height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				image[(y*height + x) * 4] = 255;
				image[(y*height + x) * 4 + 1] = 255;
				image[(y*height + x) * 4 + 2] = 255;
				image[(y*height + x) * 4 + 3] = bitmap[y*height + x];
			}
		}

		// create texture from data
		WICLoadedData image_data;
		image_data.image_width = width;
		image_data.image_height = created_height;
		image_data.image_data = image;
		image_data.image_data_byte_size = width * created_height * 4;
		image_data.bytes_per_row = width * 4;
		image_data.dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		image_data.success = true;

		newLevel.char_map = new Texture(image_data);
		newLevel.char_map->BuildBuffers();
		newLevel.size = size;

		delete file_data;
		delete bitmap;
		delete image;

		//TODO sort levels
	}

}