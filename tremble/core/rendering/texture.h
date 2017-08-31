#pragma once

#include "../win32/wic_loader.h"
#include "gpu_resource.h"

namespace tremble
{
	class GameManager;

	class Texture : public GpuResource
	{
	public:
		Texture(const std::string& file_path);
		Texture(WICLoadedData loaded_data, int array_count = 1, bool as_render_target = false);
		~Texture();

		void LoadFromFile(const std::string& file_path);
		void LoadFromMemory(WICLoadedData loaded_data, int array_count);
		void BuildBuffers();
		bool AreBuffersBuilt() const { return buffers_built_; }

		const WICLoadedData& GetTextureData() const { return texture_data_; };
		const UINT& GetSRV() const { return srv_id_; }
	private:
		std::string file_path_;

		WICLoadedData texture_data_;
		UINT srv_id_;
		bool buffers_built_;
		bool is_render_target_;
		int array_size_;
	};
}