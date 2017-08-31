#pragma once

#include "../../core/scene_graph/component.h"
#include "../../core/utilities/octree.h"

namespace tremble
{
	class Camera : public Component
	{
	public:
		Camera();

		void Awake();

		void Update();

		void SetFOV(float fov) { fov_ = fov; }
		void SetNearZ(float near_z) { near_z_ = near_z; }
		void SetFarZ(float far_z) { far_z_ = far_z; }
		void SetWidth(float width) { width_ = width; }
		void SetHeight(float height) { height_ = height; }

		float GetFOV() const { return fov_; }
		float GetNearZ() const { return near_z_; }
		float GetFarZ() const { return far_z_; }
		float GetWidth() const { return width_; }
		float GetHeight() const { return height_; }

		const DirectX::XMMATRIX GetView() { return view_; }
		const DirectX::XMMATRIX GetInvView() { return inv_view_; }
		const DirectX::XMMATRIX GetProjection() { return projection_; }
		const DirectX::XMMATRIX GetInvProjection() { return inv_projection_; }
		const DirectX::XMMATRIX GetViewProjection() { return view_projection_; }
		const DirectX::XMMATRIX GetInvViewProjection() { return inv_view_projection_; }

		const PlaneFrustum GetFrustum() const { return frustum_; }

	public:
		void ComputeView();
		void ComputeProjection();
		void ComputeViewProjection();
		void ComputeBoundingFrustum();

	private:

		DirectX::XMMATRIX view_;
		DirectX::XMMATRIX inv_view_;
		DirectX::XMMATRIX projection_;
		DirectX::XMMATRIX inv_projection_;
		DirectX::XMMATRIX view_projection_;
		DirectX::XMMATRIX inv_view_projection_;

		PlaneFrustum frustum_;

		float cam_move_forward_backward_;
		float cam_move_side_to_side_;
		float cam_move_up_down_;

		float cam_rot_x;
		float cam_rot_y;
		float cam_rot_z;

		float fov_;
		float near_z_;
		float far_z_;
		float width_;
		float height_;
	};
}