#include "typed_buffer.h"

#include "descriptor_heap.h"
#include "renderer.h"
#include "device.h"
#include "../get.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	TypedBuffer::TypedBuffer() :
		data_format_(DXGI_FORMAT_UNKNOWN)
	{
	}
	
	//------------------------------------------------------------------------------------------------------
	TypedBuffer::TypedBuffer(DXGI_FORMAT format) :
		data_format_(format)
	{
	
	}
	
	//------------------------------------------------------------------------------------------------------
	void TypedBuffer::CreateDerivedViews()
	{
		D3D12_SHADER_RESOURCE_VIEW_DESC srv_desc = {};
		srv_desc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		srv_desc.Format = data_format_;
		srv_desc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srv_desc.Buffer.NumElements = element_count_;
		srv_desc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;

		srv_id_ = Get::CbvSrvUavHeap().CreateShaderResourceView(resource_, &srv_desc);

		D3D12_UNORDERED_ACCESS_VIEW_DESC uav_desc = {};
		uav_desc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
		uav_desc.Format = data_format_;
		uav_desc.Buffer.NumElements = element_count_;
		uav_desc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;

		uav_id_ = Get::CbvSrvUavHeap().CreateUnorderedAccessView(resource_, nullptr, &uav_desc);
	}
}