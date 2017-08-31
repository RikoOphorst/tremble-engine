#include "texture.h"

#include "../win32/wic_loader.h"
#include "../game_manager.h"
#include "renderer.h"
#include "device.h"
#include "descriptor_heap.h"
#include "../get.h"
#include "command_context.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Texture::Texture(const std::string& file_path) :
		file_path_(file_path),
		buffers_built_(false),
		is_render_target_(false),
		array_size_(1)
	{
		LoadFromFile(file_path_);
	}

	//------------------------------------------------------------------------------------------------------
	Texture::Texture(WICLoadedData loaded_data, int array_count, bool as_render_target) :
		buffers_built_(false),
		is_render_target_(as_render_target)
	{
		LoadFromMemory(loaded_data, array_count);
	}



	//------------------------------------------------------------------------------------------------------
	Texture::~Texture()
	{
		
	}

	//------------------------------------------------------------------------------------------------------
	void Texture::LoadFromFile(const std::string& file_path)
	{
		int width, height, comp;
		unsigned char* image = nullptr;

		bool failed = false;
		
		image = stbi_load(file_path.c_str(), &width, &height, &comp, STBI_rgb_alpha);
		struct stat buffer;
		if (stat(file_path.c_str(), &buffer) != 0 || image == nullptr)
		{
			failed = true;
			width = 1;
			height = 1;
			comp = 4;

			image = static_cast<BYTE*>(malloc(4));
			image[0] = 255;
			image[1] = 0;
			image[2] = 255;
			image[3] = 255;

			DLOG((std::string("Warning: Tried to load a texture (") + file_path + std::string(") but couldn't find that file or failed to parse it. Using a default replacement texture instead.")).c_str());
		}

		WICLoadedData data;
		data.bytes_per_row = width * 32 / 8;
		data.dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
		data.image_data = image;
		data.image_width = width;
		data.image_height = height;
		data.image_data_byte_size = sizeof(unsigned char) * width * height;
		data.success = true;

		texture_data_ = data;

		BuildBuffers();

		if (!failed)
		{
			stbi_image_free(image);
		}
		else
		{
			free(image);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Texture::LoadFromMemory(WICLoadedData loaded_data, int array_count)
	{
		texture_data_ = loaded_data;
		buffers_built_ = false;
		array_size_ = array_count;
	}

	//------------------------------------------------------------------------------------------------------
	void Texture::BuildBuffers()
	{
		if (texture_data_.success != true)
		{
			return;
		}

		D3D12_RESOURCE_DESC desc;
		desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		desc.Alignment = 0;
		desc.Width = texture_data_.image_width;
		desc.Height = texture_data_.image_height;
		desc.DepthOrArraySize = array_size_;
		desc.MipLevels = 1;
		desc.Format = texture_data_.dxgi_format;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		desc.Flags = ((is_render_target_) ? D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET : D3D12_RESOURCE_FLAG_NONE);

		ID3D12Device* device = Get::Device().Get();

		CHECKHR(
			device->CreateCommittedResource(
				&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_COPY_DEST,
				nullptr,
				IID_PPV_ARGS(&resource_)
			)
		);
		resource_->SetName(L"Texture");
		usage_state_ = D3D12_RESOURCE_STATE_COPY_DEST;

		D3D12_SUBRESOURCE_DATA resource_data = {};
		resource_data.pData = &texture_data_.image_data[0];
		resource_data.RowPitch = texture_data_.bytes_per_row;
		resource_data.SlicePitch = texture_data_.bytes_per_row * texture_data_.image_height;
		CommandContext::InitializeTexture(*this, 1, &resource_data);

		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.Format = texture_data_.dxgi_format;
		srv_desc.ViewDimension = ((array_size_ > 1) ? D3D12_SRV_DIMENSION_TEXTURE2DARRAY : D3D12_SRV_DIMENSION_TEXTURE2D);

		if (array_size_ == 1) {
			srv_desc.Texture2D.MipLevels = desc.MipLevels;
			srv_desc.Texture2D.MostDetailedMip = 0;
			srv_desc.Texture2D.ResourceMinLODClamp = 0.0f;
			srv_desc.Texture2D.PlaneSlice = 0;
		}
		else 
		{
			srv_desc.Texture2DArray.MipLevels = desc.MipLevels;
			srv_desc.Texture2DArray.MostDetailedMip = 0;
			srv_desc.Texture2DArray.ResourceMinLODClamp = 0.0f;
			srv_desc.Texture2DArray.PlaneSlice = 0;
			srv_desc.Texture2DArray.ArraySize = array_size_;
			srv_desc.Texture2DArray.FirstArraySlice = 0;
		}

		srv_id_ = Get::CbvSrvUavHeap().CreateShaderResourceView(resource_, &srv_desc);

		buffers_built_ = true;
	}
}