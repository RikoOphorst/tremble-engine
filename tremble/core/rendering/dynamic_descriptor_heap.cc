#include "dynamic_descriptor_heap.h"

#include "../utilities/utilities.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	DynamicDescriptorHeap::DynamicDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT descriptor_count_per_heap) :
		heap_type_(heap_type),
		flags_(flags),
		descriptor_count_per_heap_(descriptor_count_per_heap),
		allocated_descriptors_(0)
	{
		CreateNewHeap(device);

		descriptor_size_ = device->GetDescriptorHandleIncrementSize(heap_type_);
	}

	//------------------------------------------------------------------------------------------------------
	DynamicDescriptorHeap::~DynamicDescriptorHeap()
	{
		for (int i = 0; i < heaps_.size(); i++)
		{
			GUARANTEE_RELEASE(heaps_[i]);
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	UINT DynamicDescriptorHeap::CreateDepthStencilView(ID3D12Device* device, ID3D12Resource* dsv_buffer, D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		assert(heap_type_ == D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(allocated_descriptors_, &heap_index, &descriptor_index);

		if (heap_index > heaps_.size() - 1)
		{
			CreateNewHeap(device);
		}

		device->CreateDepthStencilView(dsv_buffer, desc, GetDescriptorHandle(heap_index, descriptor_index));

		return allocated_descriptors_++;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DynamicDescriptorHeap::CreateRenderTargetView(ID3D12Device* device, ID3D12Resource* rtv_buffer, D3D12_RENDER_TARGET_VIEW_DESC* desc)
	{
		assert(heap_type_ == D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(allocated_descriptors_, &heap_index, &descriptor_index);

		if (heap_index > heaps_.size() - 1)
		{
			CreateNewHeap(device);
		}

		device->CreateRenderTargetView(rtv_buffer, desc, GetDescriptorHandle(heap_index, descriptor_index));

		return allocated_descriptors_++;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DynamicDescriptorHeap::CreateConstantBufferView(ID3D12Device* device, D3D12_CONSTANT_BUFFER_VIEW_DESC* desc)
	{
		assert(heap_type_ == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(allocated_descriptors_, &heap_index, &descriptor_index);

		if (heap_index > heaps_.size() - 1)
		{
			CreateNewHeap(device);
		}

		device->CreateConstantBufferView(desc, GetDescriptorHandle(heap_index, descriptor_index));

		return allocated_descriptors_++;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DynamicDescriptorHeap::CreateShaderResourceView(ID3D12Device* device, ID3D12Resource* srv_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
	{
		assert(heap_type_ == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(allocated_descriptors_, &heap_index, &descriptor_index);

		if (heap_index > heaps_.size() - 1)
		{
			CreateNewHeap(device);
		}

		device->CreateShaderResourceView(srv_buffer, desc, GetDescriptorHandle(heap_index, descriptor_index));

		return allocated_descriptors_++;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DynamicDescriptorHeap::CreateUnorderedAccessView(ID3D12Device* device, ID3D12Resource* buffer, ID3D12Resource* counter_buffer, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
	{
		assert(heap_type_ == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(allocated_descriptors_, &heap_index, &descriptor_index);

		if (heap_index > heaps_.size() - 1)
		{
			CreateNewHeap(device);
		}

		device->CreateUnorderedAccessView(buffer, counter_buffer, desc, GetDescriptorHandle(heap_index, descriptor_index));

		return allocated_descriptors_++;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DynamicDescriptorHeap::CreateSampler(ID3D12Device* device, D3D12_SAMPLER_DESC* desc)
	{
		assert(heap_type_ == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(allocated_descriptors_, &heap_index, &descriptor_index);

		if (heap_index > heaps_.size() - 1)
		{
			CreateNewHeap(device);
		}

		device->CreateSampler(desc, GetDescriptorHandle(heap_index, descriptor_index));

		return allocated_descriptors_++;
	}

	//------------------------------------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::GetCPUDescriptorById(UINT id)
	{
		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(id, &heap_index, &descriptor_index);
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(CD3DX12_CPU_DESCRIPTOR_HANDLE(heaps_[heap_index]->GetCPUDescriptorHandleForHeapStart()));
		handle.Offset(descriptor_index, descriptor_size_);

		return handle;
	}

	//------------------------------------------------------------------------------------------------------
	D3D12_GPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::GetGPUDescriptorById(UINT id)
	{
		int heap_index, descriptor_index;
		FindHeapIndexAndDescriptorIndex(id, &heap_index, &descriptor_index);
		auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(CD3DX12_GPU_DESCRIPTOR_HANDLE(heaps_[heap_index]->GetGPUDescriptorHandleForHeapStart()));
		handle.Offset(descriptor_index, descriptor_size_);

		return handle;
	}
	
	//------------------------------------------------------------------------------------------------------
	void DynamicDescriptorHeap::FindHeapIndexAndDescriptorIndex(int idx, int* heap_index, int* descriptor_index)
	{
		*heap_index = (RoundUpToMultiple(idx + 1, static_cast<int>(descriptor_count_per_heap_)) - static_cast<int>(descriptor_count_per_heap_)) / static_cast<int>(descriptor_count_per_heap_);
		*descriptor_index = idx % descriptor_count_per_heap_;
	}
	
	//------------------------------------------------------------------------------------------------------
	void DynamicDescriptorHeap::CreateNewHeap(ID3D12Device* device)
	{
		D3D12_DESCRIPTOR_HEAP_DESC desc;
		desc.Flags = flags_;
		desc.NodeMask = 0;
		desc.NumDescriptors = descriptor_count_per_heap_;
		desc.Type = heap_type_;

		ID3D12DescriptorHeap* heap = nullptr;
		heaps_.push_back(heap);
		HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&heaps_.back()));
	}
	
	//------------------------------------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE DynamicDescriptorHeap::GetDescriptorHandle(int heap_index, int descriptor_index)
	{
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heaps_[heap_index]->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(descriptor_index, descriptor_size_);

		return handle;
	}
}