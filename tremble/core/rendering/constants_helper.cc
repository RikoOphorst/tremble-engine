#include "constants_helper.h"

#include "../../components/rendering/renderable.h"
#include "../../components/rendering/camera.h"
#include "../../components/rendering/light.h"
#include "../scene_graph/scene_graph.h"
#include "upload_buffer.h"
#include "material.h"

namespace tremble
{
	void ConstantsHelper::UpdatePassConstants(
		UploadBuffer& buffer,
		const DirectX::XMFLOAT2& render_target_size,
		Timer* timer,
		Camera* camera
	)
	{
		PassConstants constants;
		constants.delta_time = static_cast<float>(timer->GetDeltaT());
		constants.total_time = static_cast<float>(timer->GetTimeSinceStartup());
		constants.far_z = camera->GetFarZ();
		constants.near_z = camera->GetNearZ();
		constants.render_target_size = render_target_size;
		constants.inv_render_target_size = DirectX::XMFLOAT2(1.0f / render_target_size.x, 1.0f / render_target_size.y);
		constants.view = camera->GetView();
		constants.inv_view = camera->GetInvView();
		constants.projection = camera->GetProjection();
		constants.inv_projection = camera->GetInvProjection();
		constants.view_projection = camera->GetViewProjection();
		constants.inv_view_projection = camera->GetInvViewProjection();
		DirectX::XMStoreFloat3(&constants.eye_pos_world, camera->GetNode()->GetPosition());

		buffer.InsertDataByElement(0, &constants);
	}
	
	//------------------------------------------------------------------------------------------------------
	void ConstantsHelper::UpdateLightConstants(UploadBuffer& buffer, const std::vector<Light*>& lights, Camera* camera)
	{
		for (int i = 0; i < 64; i++)
		{
			LightConstants c;
			if (i < lights.size())
			{
				Light& l = *(lights[i]);
				c.position_world = Vector4(l.GetNode()->GetLocalPosition(), 1.0f);
				c.direction_world = l.GetDirection();
				DirectX::XMStoreFloat4(&c.position_view, DirectX::XMVector4Transform(Vector4(l.GetNode()->GetLocalPosition(), 1.0f), camera->GetView()));
				DirectX::XMStoreFloat4(&c.direction_view, DirectX::XMVector4Transform(l.GetDirection().ToDxVec(), camera->GetView()));
				c.color = l.GetColor();
				c.spot_light_angle = 30.0f;
				c.range = l.GetFalloffEnd();
				c.intensity = 1.0f;
				c.enabled = 1;
				c.selected = 0;
				c.type = l.GetLightType();
				c.shadow_index = l.GetShadowIndex();
				c.shadow_range = l.GetShadowRange();
			}
			else
			{
				c.enabled = 0;
			}

			buffer.InsertDataByElement(i, &c);
		}
	}
}