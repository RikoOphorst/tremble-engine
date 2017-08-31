#pragma once

#include "gpu_buffer.h"

namespace tremble
{
	class ByteAddressBuffer : public GpuBuffer
	{
	public:
		virtual void CreateDerivedViews() override;
	};
}