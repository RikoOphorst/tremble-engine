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

	/// Holds information about a character in a font
	struct CharInfo
	{
		int width, height;
		int advance;
		int offset_x, offset_y;
		float uv_x1, uv_x2, uv_y1, uv_y2;
	};

	/// Holds all data for a specific size font
	struct FontLevel {
		int size;
		CharInfo char_info[255];
		Texture* char_map;
	};

	/// Holds all data of a loaded font
	struct Font
	{
		std::string name;
		std::vector<FontLevel> levels;
		/// Returns font level that should be used for specified size
		FontLevel& FindLevel(float);
		/// Generates font level with specified size
		void GenerateLevel(int);
	};

	/**
	* @class tremble::InterfaceFontRenderer
	* @author Sander Brouwers
	* @brief Renders TextComponents to user interface
	*/
	class InterfaceFontRenderer
	{
	public:
		/// Initializes interface renderer
		void Startup();

		/// Render interface components to specified context
		void Draw(GraphicsContext&);

		/// Frees up memory used by interface renderer
		void Destroy();

		/// Returns list of currently present fonts
		std::vector<Font>& GetFontList() { return fonts_; };

		/// Adds font for rendering
		void AddFont(std::string);

	private:
		void CreateRenderResources();
		void CreatePSO();
		void CreateRootSignature();
		void CreateGeometry();

		void RenderBatch(GraphicsContext&, int, int);

		UploadBuffer pass_constants_;

		Shader* pixel_shader_;
		Shader* geometry_shader_;
		Shader* vertex_shader_;

		RootSignature root_signature_;
		D3D12_SAMPLER_DESC sampler_state_;
		D3D12_BLEND_DESC blend_state_;
		D3D12_DEPTH_STENCIL_DESC depth_stencil_state_;
		D3D12_RASTERIZER_DESC rasterizer_state_;

		std::vector<FontVertex> indices_;
		D3D12_VERTEX_BUFFER_VIEW index_buffer_view_;
		StructuredBuffer index_buffer_;

		std::vector<Font> fonts_;

		StructuredBuffer char_buffer_;

		float time_;

		int char_limit_ = 1024;
		InterfaceFontData* render_data_;

		/// Stores char batch data for rendering
		struct CharBatch {
			int offset;
			int count;
			Texture* texture;
		};
	};
	
}