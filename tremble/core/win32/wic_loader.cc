#include "wic_loader.h"

#include "../utilities/debug.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	WICLoader::WICLoader() :
		wic_factory_(nullptr),
		wic_decoder_(nullptr),
		wic_frame_(nullptr),
		wic_converter_(nullptr)
	{
		CoInitialize(NULL);

		CHECKHR(CoCreateInstance(
			CLSID_WICImagingFactory,
			NULL,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&wic_factory_)
		));
	}

	//------------------------------------------------------------------------------------------------------
	WICLoader::~WICLoader()
	{
		SAFE_RELEASE(wic_factory_);
		SAFE_RELEASE(wic_frame_);
		SAFE_RELEASE(wic_decoder_);
		SAFE_RELEASE(wic_converter_);
	}

	//------------------------------------------------------------------------------------------------------
	WICLoadedData WICLoader::LoadFromFile(const std::string& file_path)
	{
		WICLoadedData data;

		data.success = false;

		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
		std::wstring wide = converter.from_bytes(file_path);

		HRESULT hr = S_OK;
		hr = wic_factory_->CreateDecoderFromFilename(
			wide.c_str(),
			NULL,
			GENERIC_READ,
			WICDecodeMetadataCacheOnLoad,
			&wic_decoder_
		);

		if (hr != S_OK)
		{
			DLOG((std::string("Warning: Tried to load a texture (") + file_path + std::string(") but couldn't find that file. Using default. THIS INCURS A MEMORY LEAK, SO FIX THIS!")).c_str());

			BYTE* img_data = static_cast<BYTE*>(malloc(4));
			img_data[0] = 255;
			img_data[1] = 0;
			img_data[2] = 255;
			img_data[3] = 255;

			data.success = true;
			data.dxgi_format = DXGI_FORMAT_R8G8B8A8_UNORM;
			data.image_data = reinterpret_cast<BYTE*>(img_data);
			data.image_data_byte_size = sizeof(BYTE) * 4;
			data.image_width = 1;
			data.image_height = 1;
			data.bytes_per_row = GetDXGIFormatBitsPerPixel(data.dxgi_format) / 8;
			return data;
		}

		CHECKHR(wic_decoder_->GetFrame(0, &wic_frame_));

		WICPixelFormatGUID pixel_format;
		CHECKHR(wic_frame_->GetPixelFormat(&pixel_format));

		CHECKHR(wic_frame_->GetSize(&data.image_width, &data.image_height));

		DXGI_FORMAT dxgi_format = GetDXGIFormatFromWICFormat(pixel_format);

		bool image_converted = false;

		if (dxgi_format == DXGI_FORMAT_UNKNOWN)
		{
			WICPixelFormatGUID convert_to_pixel_format = GetConvertToWICFormat(pixel_format);

			if (convert_to_pixel_format == GUID_WICPixelFormatDontCare)
			{
				data.success = false;
				return data;
			}

			dxgi_format = GetDXGIFormatFromWICFormat(convert_to_pixel_format);

			CHECKHR(wic_factory_->CreateFormatConverter(&wic_converter_));

			BOOL can_convert = FALSE;
			CHECKHR(wic_converter_->CanConvert(pixel_format, convert_to_pixel_format, &can_convert));
			if (!can_convert)
			{
				data.success = false;
				return data;
			}

			CHECKHR(wic_converter_->Initialize(wic_frame_, convert_to_pixel_format, WICBitmapDitherTypeErrorDiffusion, 0, 0, WICBitmapPaletteTypeCustom));

			image_converted = true;
		}

		data.bytes_per_row = (data.image_width * GetDXGIFormatBitsPerPixel(dxgi_format)) / 8;
		data.image_data_byte_size = data.bytes_per_row * data.image_height;
		data.image_data = (BYTE*)malloc(data.image_data_byte_size);

		if (image_converted)
		{
			CHECKHR(wic_converter_->CopyPixels(0, data.bytes_per_row, data.image_data_byte_size, data.image_data));
		}
		else
		{
			CHECKHR(wic_frame_->CopyPixels(0, data.bytes_per_row, data.image_data_byte_size, data.image_data));
		}

		data.dxgi_format = dxgi_format;
		data.success = true;

		return data;
	}

	//------------------------------------------------------------------------------------------------------
	DXGI_FORMAT WICLoader::GetDXGIFormatFromWICFormat(WICPixelFormatGUID& wic_format_guid)
	{
		if (wic_format_guid == GUID_WICPixelFormat128bppRGBAFloat) return DXGI_FORMAT_R32G32B32A32_FLOAT;
		else if (wic_format_guid == GUID_WICPixelFormat64bppRGBAHalf) return DXGI_FORMAT_R16G16B16A16_FLOAT;
		else if (wic_format_guid == GUID_WICPixelFormat64bppRGBA) return DXGI_FORMAT_R16G16B16A16_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat32bppRGBA) return DXGI_FORMAT_R8G8B8A8_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat32bppBGRA) return DXGI_FORMAT_B8G8R8A8_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat32bppBGR) return DXGI_FORMAT_B8G8R8X8_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat32bppRGBA1010102XR) return DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM;

		else if (wic_format_guid == GUID_WICPixelFormat32bppRGBA1010102) return DXGI_FORMAT_R10G10B10A2_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat16bppBGRA5551) return DXGI_FORMAT_B5G5R5A1_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat16bppBGR565) return DXGI_FORMAT_B5G6R5_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat32bppGrayFloat) return DXGI_FORMAT_R32_FLOAT;
		else if (wic_format_guid == GUID_WICPixelFormat16bppGrayHalf) return DXGI_FORMAT_R16_FLOAT;
		else if (wic_format_guid == GUID_WICPixelFormat16bppGray) return DXGI_FORMAT_R16_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat8bppGray) return DXGI_FORMAT_R8_UNORM;
		else if (wic_format_guid == GUID_WICPixelFormat8bppAlpha) return DXGI_FORMAT_A8_UNORM;

		else return DXGI_FORMAT_UNKNOWN;
	}

	//------------------------------------------------------------------------------------------------------
	WICPixelFormatGUID WICLoader::GetConvertToWICFormat(WICPixelFormatGUID& wic_format_guid)
	{
		if (wic_format_guid == GUID_WICPixelFormatBlackWhite) return GUID_WICPixelFormat8bppGray;
		else if (wic_format_guid == GUID_WICPixelFormat1bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat2bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat4bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat8bppIndexed) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat2bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wic_format_guid == GUID_WICPixelFormat4bppGray) return GUID_WICPixelFormat8bppGray;
		else if (wic_format_guid == GUID_WICPixelFormat16bppGrayFixedPoint) return GUID_WICPixelFormat16bppGrayHalf;
		else if (wic_format_guid == GUID_WICPixelFormat32bppGrayFixedPoint) return GUID_WICPixelFormat32bppGrayFloat;
		else if (wic_format_guid == GUID_WICPixelFormat16bppBGR555) return GUID_WICPixelFormat16bppBGRA5551;
		else if (wic_format_guid == GUID_WICPixelFormat32bppBGR101010) return GUID_WICPixelFormat32bppRGBA1010102;
		else if (wic_format_guid == GUID_WICPixelFormat24bppBGR) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat24bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat32bppPBGRA) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat32bppPRGBA) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat48bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat48bppBGR) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat64bppBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat64bppPRGBA) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat64bppPBGRA) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat48bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wic_format_guid == GUID_WICPixelFormat48bppBGRFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wic_format_guid == GUID_WICPixelFormat64bppRGBAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wic_format_guid == GUID_WICPixelFormat64bppBGRAFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wic_format_guid == GUID_WICPixelFormat64bppRGBFixedPoint) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wic_format_guid == GUID_WICPixelFormat64bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wic_format_guid == GUID_WICPixelFormat48bppRGBHalf) return GUID_WICPixelFormat64bppRGBAHalf;
		else if (wic_format_guid == GUID_WICPixelFormat128bppPRGBAFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wic_format_guid == GUID_WICPixelFormat128bppRGBFloat) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wic_format_guid == GUID_WICPixelFormat128bppRGBAFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wic_format_guid == GUID_WICPixelFormat128bppRGBFixedPoint) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wic_format_guid == GUID_WICPixelFormat32bppRGBE) return GUID_WICPixelFormat128bppRGBAFloat;
		else if (wic_format_guid == GUID_WICPixelFormat32bppCMYK) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat64bppCMYK) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat40bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat80bppCMYKAlpha) return GUID_WICPixelFormat64bppRGBA;

#if (_WIN32_WINNT >= _WIN32_WINNT_WIN8) || defined(_WIN7_PLATFORM_UPDATE)
		else if (wic_format_guid == GUID_WICPixelFormat32bppRGB) return GUID_WICPixelFormat32bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat64bppRGB) return GUID_WICPixelFormat64bppRGBA;
		else if (wic_format_guid == GUID_WICPixelFormat64bppPRGBAHalf) return GUID_WICPixelFormat64bppRGBAHalf;
#endif

		else return GUID_WICPixelFormatDontCare;
	}

	//------------------------------------------------------------------------------------------------------
	int WICLoader::GetDXGIFormatBitsPerPixel(DXGI_FORMAT& dxgi_format)
	{
		if (dxgi_format == DXGI_FORMAT_R32G32B32A32_FLOAT) return 128;
		else if (dxgi_format == DXGI_FORMAT_R16G16B16A16_FLOAT) return 64;
		else if (dxgi_format == DXGI_FORMAT_R16G16B16A16_UNORM) return 64;
		else if (dxgi_format == DXGI_FORMAT_R8G8B8A8_UNORM) return 32;
		else if (dxgi_format == DXGI_FORMAT_B8G8R8A8_UNORM) return 32;
		else if (dxgi_format == DXGI_FORMAT_B8G8R8X8_UNORM) return 32;
		else if (dxgi_format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM) return 32;

		else if (dxgi_format == DXGI_FORMAT_R10G10B10A2_UNORM) return 32;
		else if (dxgi_format == DXGI_FORMAT_B5G5R5A1_UNORM) return 16;
		else if (dxgi_format == DXGI_FORMAT_B5G6R5_UNORM) return 16;
		else if (dxgi_format == DXGI_FORMAT_R32_FLOAT) return 32;
		else if (dxgi_format == DXGI_FORMAT_R16_FLOAT) return 16;
		else if (dxgi_format == DXGI_FORMAT_R16_UNORM) return 16;
		else if (dxgi_format == DXGI_FORMAT_R8_UNORM) return 8;
		else if (dxgi_format == DXGI_FORMAT_A8_UNORM) return 8;

		return 0;
	}
}