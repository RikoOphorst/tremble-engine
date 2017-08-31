#pragma once

#include "constant_buffers.h"
#include "upload_buffer.h"

namespace tremble
{
	template<typename T>
	class DynamicUploadBuffer;

	class FrameResource
	{
	public:
		FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount);
		~FrameResource();

		DynamicUploadBuffer<ObjectConstants>* GetObjectConstants() { return object_constants_; }
		DynamicUploadBuffer<DebugConstants>* GetDebugConstants() { return debug_constants_; }
		DynamicUploadBuffer<MaterialConstants>* GetMaterialConstants() { return material_constants_; }
		UploadBuffer& GetPassConstants() { return pass_constants_; }

		void SetFenceValue(UINT64 fence) { fence_ = fence; }
		UINT64 GetFenceValue() { return fence_; }

		ID3D12CommandAllocator* GetCommandAllocator() { return command_allocator_; }
	private:
		DynamicUploadBuffer<ObjectConstants>* object_constants_;
		DynamicUploadBuffer<DebugConstants>* debug_constants_;
		DynamicUploadBuffer<MaterialConstants>* material_constants_;
		UploadBuffer pass_constants_;

		ID3D12CommandAllocator* command_allocator_;
		UINT64 fence_ = 0;
	};
}