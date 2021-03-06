#include "descriptor_heap.h"

#include "device.h"
#include "renderer.h"
#include "../get.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	DescriptorHeap::DescriptorHeap() :
		current_allocations_(0),
		descriptor_size_(0),
		device_(nullptr),
		heap_(nullptr)
	{
		
	}

	//------------------------------------------------------------------------------------------------------
	DescriptorHeap::~DescriptorHeap()
	{
		GUARANTEE_RELEASE(heap_);
	}

	//------------------------------------------------------------------------------------------------------
	void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE heap_type, D3D12_DESCRIPTOR_HEAP_FLAGS flags, UINT descriptor_count)
	{
		device_ = &Get::Renderer()->GetDevice();

		heap_desc_.Flags = flags;
		heap_desc_.NodeMask = 0;
		heap_desc_.NumDescriptors = descriptor_count;
		heap_desc_.Type = heap_type;

		CHECKHR(device_->Get()->CreateDescriptorHeap(&heap_desc_, IID_PPV_ARGS(&heap_)));

		descriptor_size_ = device_->Get()->GetDescriptorHandleIncrementSize(heap_type);
	}

	//------------------------------------------------------------------------------------------------------
	UINT DescriptorHeap::CreateDepthStencilView(ID3D12Resource* dsv_buffer, D3D12_DEPTH_STENCIL_VIEW_DESC* desc)
	{
		assert(current_allocations_ + 1 <= heap_desc_.NumDescriptors);
		assert(heap_desc_.Type == D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

		device_->Get()->CreateDepthStencilView(dsv_buffer, desc, GetCPUDescriptorById(current_allocations_));
		current_allocations_ += 1;
		return current_allocations_ - 1;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DescriptorHeap::CreateRenderTargetView(ID3D12Resource* rtv_buffer, D3D12_RENDER_TARGET_VIEW_DESC* desc)
	{
		assert(current_allocations_ + 1 <= heap_desc_.NumDescriptors);
		assert(heap_desc_.Type == D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

		device_->Get()->CreateRenderTargetView(rtv_buffer, desc, GetCPUDescriptorById(current_allocations_));
		current_allocations_ += 1;
		return current_allocations_ - 1;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DescriptorHeap::CreateConstantBufferView(D3D12_CONSTANT_BUFFER_VIEW_DESC* desc)
	{
		assert(current_allocations_ + 1 <= heap_desc_.NumDescriptors);
		assert(heap_desc_.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device_->Get()->CreateConstantBufferView(desc, GetCPUDescriptorById(current_allocations_));
		current_allocations_ += 1;
		return current_allocations_ - 1;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DescriptorHeap::CreateShaderResourceView(ID3D12Resource* srv_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC* desc)
	{
		assert(current_allocations_ + 1 <= heap_desc_.NumDescriptors);
		assert(heap_desc_.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device_->Get()->CreateShaderResourceView(srv_buffer, desc, GetCPUDescriptorById(current_allocations_));
		current_allocations_ += 1;
		return current_allocations_ - 1;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DescriptorHeap::CreateUnorderedAccessView(ID3D12Resource* buffer, ID3D12Resource* counter_buffer, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc)
	{
		assert(current_allocations_ + 1 <= heap_desc_.NumDescriptors);
		assert(heap_desc_.Type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

		device_->Get()->CreateUnorderedAccessView(buffer, counter_buffer, desc, GetCPUDescriptorById(current_allocations_));
		current_allocations_ += 1;
		return current_allocations_ - 1;
	}

	//------------------------------------------------------------------------------------------------------
	UINT DescriptorHeap::CreateSampler(D3D12_SAMPLER_DESC* desc)
	{
		assert(current_allocations_ + 1 <= heap_desc_.NumDescriptors);
		assert(heap_desc_.Type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);

		device_->Get()->CreateSampler(desc, GetCPUDescriptorById(current_allocations_));
		current_allocations_ += 1;
		return current_allocations_ - 1;
	}

	//------------------------------------------------------------------------------------------------------
	D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorById(UINT id)
	{
		auto handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(heap_->GetCPUDescriptorHandleForHeapStart());
		handle.Offset(id, descriptor_size_);

		return handle;
	}
	
	//------------------------------------------------------------------------------------------------------
	D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorById(UINT id)
	{
		assert(id < current_allocations_);

		auto handle = CD3DX12_GPU_DESCRIPTOR_HANDLE(heap_->GetGPUDescriptorHandleForHeapStart());
		handle.Offset(id, descriptor_size_);

		return handle;
	}

	//------------------------------------------------------------------------------------------------------
	void DescriptorHeap::Clear()
	{
		GUARANTEE_RELEASE(heap_);
		current_allocations_ = 0;
		CHECKHR(device_->Get()->CreateDescriptorHeap(&heap_desc_, IID_PPV_ARGS(&heap_)));
	}
}