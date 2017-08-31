#pragma once

#include "constant_buffers.h"
#include "../utilities/timer.h"
#include "upload_buffer.h"

namespace tremble
{
	class Camera;
	struct Material;
	class Renderable;
	class Timer;
	class Light;

	class ConstantsHelper
	{
	public:
		static void UpdatePassConstants(
			UploadBuffer& buffer,
			const DirectX::XMFLOAT2& render_target_size,
			Timer* timer,
			Camera* camera
		);

		static void UpdateLightConstants(
			UploadBuffer& buffer,
			const std::vector<Light*>& lights,
			Camera* camera
		);
	};
}