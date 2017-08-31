#pragma once

namespace tremble
{
	struct WICLoadedData
	{
		BYTE* image_data;
		UINT image_data_byte_size;
		UINT bytes_per_row;
		UINT image_width;
		UINT image_height;
		DXGI_FORMAT dxgi_format;
		bool success;
	};
	
	class WICLoader
	{
	public:
		WICLoader();
		~WICLoader();

		WICLoadedData LoadFromFile(const std::string& file_path);

		static DXGI_FORMAT GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wic_format_guid);
		static WICPixelFormatGUID GetConvertToWICFormat(WICPixelFormatGUID& wic_format_guid);
		static int GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgi_format);

	private:
		IWICImagingFactory* wic_factory_;
		IWICBitmapDecoder* wic_decoder_;
		IWICBitmapFrameDecode* wic_frame_;
		IWICFormatConverter* wic_converter_;
	};
}