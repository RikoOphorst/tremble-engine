#include "frame_resource.h"

#include "dynamic_upload_buffer.h"
#include "upload_buffer.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	FrameResource::FrameResource(ID3D12Device* device, UINT pass_count, UINT object_count)
	{
		CHECKHR(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator_)));

		pass_constants_.Create(L"Pass Constants Buffer", pass_count, (sizeof(PassConstants) + 255) & ~255);

		object_constants_ = new DynamicUploadBuffer<ObjectConstants>(device, object_count, true);
		object_constants_->GetBuffers()[0].buffer->SetName(L"Object Constants Buffer");

		debug_constants_ = new DynamicUploadBuffer<DebugConstants>(device, 10, true);
		debug_constants_->GetBuffers()[0].buffer->SetName(L"Debug Constants Buffer");

		material_constants_ = new DynamicUploadBuffer<MaterialConstants>(device, 10, true);
		material_constants_->GetBuffers()[0].buffer->SetName(L"Material Constants Buffer");
	}

	//------------------------------------------------------------------------------------------------------
	FrameResource::~FrameResource()
	{
		delete pass_constants_;
		delete object_constants_;
		delete debug_constants_;
		delete material_constants_;

		SAFE_RELEASE(command_allocator_);
	}
}