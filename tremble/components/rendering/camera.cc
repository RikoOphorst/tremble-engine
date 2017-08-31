#include "camera.h"

#include "../../core/scene_graph/scene_graph.h"
#include "../../core/scene_graph/component.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Camera::Camera() :
		fov_(90.0f),
		width_(1280),
		height_(720),
		near_z_(1.0f),
		far_z_(200.0f),
		cam_move_forward_backward_(0.0f),
		cam_move_side_to_side_(0.0f),
		cam_move_up_down_(0.0f),
		cam_rot_x(0.0f),
		cam_rot_y(0.0f),
		cam_rot_z(0.0f)
	{
		
	}

	//------------------------------------------------------------------------------------------------------
	void Camera::Awake()
	{
		Get::Renderer()->SetCamera(this);
		ComputeView();
		ComputeProjection();
		ComputeViewProjection();
		ComputeBoundingFrustum();
	}

	//------------------------------------------------------------------------------------------------------
	void Camera::Update()
	{
		ComputeView();
		ComputeProjection();
		ComputeViewProjection();
		ComputeBoundingFrustum();
	}

	//------------------------------------------------------------------------------------------------------
	void Camera::ComputeView()
	{
		Vector3 target = GetNode()->GetForwardVectorWorld() + GetNode()->GetPosition();
		Vector3 cam_up = GetNode()->GetUpVectorWorld();

		view_ = DirectX::XMMatrixLookAtLH(GetNode()->GetPosition(), target, cam_up);
		inv_view_ = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(view_), view_);
	}

	//------------------------------------------------------------------------------------------------------
	void Camera::ComputeProjection()
	{
		projection_ = DirectX::XMMatrixPerspectiveFovLH(fov_, static_cast<float>(width_) / static_cast<float>(height_), near_z_, far_z_);
		inv_projection_ = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(projection_), projection_);
	}

	//------------------------------------------------------------------------------------------------------
	void Camera::ComputeViewProjection()
	{
		view_projection_ = view_ * projection_;
		inv_view_projection_ = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(view_projection_), view_projection_);
	}

	//------------------------------------------------------------------------------------------------------
	void Camera::ComputeBoundingFrustum()
	{
		DirectX::BoundingFrustum frustum_a, frustum_b;
		DirectX::BoundingFrustum::CreateFromMatrix(frustum_a, projection_);
		frustum_a.Transform(frustum_b, inv_view_);
		frustum_b.GetPlanes(&frustum_.near_plane, &frustum_.far_plane, &frustum_.right_plane, &frustum_.left_plane, &frustum_.top_plane, &frustum_.bottom_plane);
	}
}