#include "device.h"

#include "../get.h"
#include "renderer.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Device::Device() :
		factory_(nullptr),
		adapter_(nullptr),
		device_(nullptr)
	{

	}
	
	//------------------------------------------------------------------------------------------------------
	Device::~Device()
	{
		SAFE_RELEASE(factory_);
		SAFE_RELEASE(device_);
	}
	
	//------------------------------------------------------------------------------------------------------
	void Device::Create()
	{
#if defined(DEBUG) || defined(_DEBUG)
		{
			ID3D12Debug* debug_controller;
			CHECKHR(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_controller)));
			debug_controller->EnableDebugLayer();
			debug_controller->Release();
		}
#endif

		CHECKHR(CreateDXGIFactory1(IID_PPV_ARGS(&factory_)));

		UINT i = 0;
		while (factory_->EnumAdapters1(i, &adapter_) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC1 desc;
			adapter_->GetDesc1(&desc);

			using convert_type = std::codecvt_utf8<wchar_t>;
			std::wstring_convert<convert_type, wchar_t> converter;
			std::string converted_str = converter.to_bytes(desc.Description);

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE || converted_str[0] == 'I')
			{
				// this adapter is a software adapter, skip it
				i++;
				continue;
			}

			if (D3D12CreateDevice(adapter_, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_)) != S_OK)
			{
				i++;
				continue;
			}
			else
			{
				std::cout << "Using DirectX 12 compatible GPU: " << converted_str << std::endl;
				break;
			}
		}

		if (adapter_ == nullptr)
		{
			std::cout << "No valid adapter (GPU) was found, creating WARP adapter.." << std::endl;
			factory_->EnumWarpAdapter(IID_PPV_ARGS(&adapter_));

			CHECKHR(D3D12CreateDevice(adapter_, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_)));
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	void Device::Create(IDXGIAdapter1* adapter)
	{
		CHECKHR(CreateDXGIFactory1(IID_PPV_ARGS(&factory_)));

		if (D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_)) != S_OK)
		{
			std::cout << "Supplied adapter was invalid, falling back to WARP adapter.." << std::endl;
			factory_->EnumWarpAdapter(IID_PPV_ARGS(&adapter_));

			CHECKHR(D3D12CreateDevice(adapter_, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device_)));
		}
		else
		{
			adapter_ = adapter;
		}
	}
}