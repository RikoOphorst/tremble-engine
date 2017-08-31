#pragma once

namespace tremble
{
	class Device
	{
	public:
		Device();
		~Device();

		void Create();
		void Create(IDXGIAdapter1* adapter);

		IDXGIFactory4* GetFactory() const { return factory_; }
		IDXGIAdapter1* GetAdapter() const { return adapter_; }
		ID3D12Device* Get() const { return device_; }

	private:
		IDXGIFactory4* factory_;
		IDXGIAdapter1* adapter_;
		ID3D12Device* device_;
	};
}