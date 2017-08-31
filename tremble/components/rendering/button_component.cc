#include "button_component.h"
#include "..\..\core\input\input_manager.h"
#include "..\..\core\rendering\renderer.h"

namespace tremble
{

	template <typename T> int sgn(T val) {
		return (T(0) < val) - (val < T(0));
	}

	//------------------------------------------------------------------------------------------------------
	ButtonComponent::ButtonComponent()
	{
		custom_matrix_ = false;
		texture_[0] = nullptr;
		texture_[1] = nullptr;
		texture_[2] = nullptr;
		texture_[3] = nullptr;
		position_ = { 0, 0, 0 };
		size_ = { 64, 64, 1 };
		rotation_ = 0;
		center_ = { 0, 0, 0 };
		color_[0] = { 1, 1, 1, 1 };
		color_[1] = { 1, 1, 1, 1 };
		color_[2] = { 1, 1, 1, 1 };
		color_[3] = { .2f, .2f, .2f, 1 };
		layer_ = 0;
		transform_ = DirectX::XMMatrixIdentity();
		state_ = Normal;
		on_click_ = nullptr;
		enabled_ = true;
		uv_min_[0] = { 0, 0 };
		uv_min_[1] = { 0, 0 };
		uv_min_[2] = { 0, 0 };
		uv_min_[3] = { 0, 0 };
		uv_max_[0] = { 1, 1 };
		uv_max_[1] = { 1, 1 };
		uv_max_[2] = { 1, 1 };
		uv_max_[3] = { 1, 1 };
	}

	//------------------------------------------------------------------------------------------------------
	void ButtonComponent::Update()
	{
		if (enabled_ == false)
		{
			state_ = Disabled;
			return;
		}

		DirectX::XMINT2 mouse_delta = Get::InputManager()->GetMouseDeltaPos();
		if (mouse_delta.x != 0 || mouse_delta.y != 0)
		{
			pointed_ = IsMouseInside();
		}

		if (Get::InputManager()->GetMouseDown(MOUSE_BUTTON_LEFT) && pointed_ == true)
		{
			state_ = Click;
		}
		else if (pointed_ == true) {
			state_ = Hover;
		}
		else {
			state_ = Normal;
		}

		if (Get::InputManager()->GetMousePressed(MOUSE_BUTTON_LEFT) && (state_ == Hover || state_ == Click) && on_click_ != nullptr)
		{
			on_click_(GetNode());
		}
	}

	//------------------------------------------------------------------------------------------------------

	DirectX::XMMATRIX ButtonComponent::GetFinalTransform()
	{
		if (UsesCustomMatrix()) {
			return GetMatrix();
		}
		else {
			return DirectX::XMMatrixTranslation(-GetCenter().GetX(), -GetCenter().GetY(), 0) * DirectX::XMMatrixScaling(GetSize().GetX(), GetSize().GetY(), 1) * DirectX::XMMatrixRotationRollPitchYaw(0, 0, (GetRotation() * 3.24f) / 180.0f) * DirectX::XMMatrixTranslation(GetPosition().GetX(), GetPosition().GetY(), 1 + GetLayer());
		}
	}

	//------------------------------------------------------------------------------------------------------

	bool ButtonComponent::IsMouseInside()
	{
		DirectX::XMINT2 mouse_pos = Get::InputManager()->GetMousePos();

		int w = Get::Renderer()->GetSwapChain().GetBackBuffer().GetWidth();
		int x = Get::Renderer()->GetVirtualSizeX();

		mouse_pos.x /= (float)Get::Renderer()->GetSwapChain().GetBackBuffer().GetWidth() / Get::Renderer()->GetVirtualSizeX();
		mouse_pos.y /= (float)Get::Renderer()->GetSwapChain().GetBackBuffer().GetHeight() / Get::Renderer()->GetVirtualSizeY();

		// get vertex positions
		DirectX::XMMATRIX transform = GetFinalTransform();
		DirectX::XMVECTOR positions[4];
		positions[0] = { 0, 0, 0 };
		positions[1] = { 1, 0, 0 };
		positions[2] = { 1, 1, 0 };
		positions[3] = { 0, 1, 0 };

		positions[0] = DirectX::XMVector3Transform(positions[0], transform);
		positions[1] = DirectX::XMVector3Transform(positions[1], transform);
		positions[2] = DirectX::XMVector3Transform(positions[2], transform);
		positions[3] = DirectX::XMVector3Transform(positions[3], transform);

		// get center of button
		DirectX::XMVECTOR center = DirectX::XMVectorAdd(positions[0], positions[1]);
		center = DirectX::XMVectorAdd(center, positions[2]);
		center = DirectX::XMVectorAdd(center, positions[3]);
		center = DirectX::XMVectorScale(center, 0.25f);

		float dot1, dot2;
		DirectX::XMVECTOR relative_to_line;
		DirectX::XMVECTOR normal;
		DirectX::XMVECTOR p1, p2;
		DirectX::XMVECTOR mouse = { (float)mouse_pos.x, (float)mouse_pos.y, 0 };

		// check lines
		p1 = positions[0];
		p2 = positions[1];
		normal = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(p1, p2), { 0, 0, 1 });
		normal = DirectX::XMVector3Normalize(normal);
		relative_to_line = DirectX::XMVectorSubtract(center, p1);
		dot1 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		relative_to_line = DirectX::XMVectorSubtract(mouse, p1);
		dot2 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		if (sgn(dot1) != sgn(dot2)) { return false; }

		p1 = positions[1];
		p2 = positions[2];
		normal = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(p1, p2), { 0, 0, 1 });
		normal = DirectX::XMVector3Normalize(normal);
		relative_to_line = DirectX::XMVectorSubtract(center, p1);
		dot1 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		relative_to_line = DirectX::XMVectorSubtract(mouse, p1);
		dot2 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		if (sgn(dot1) != sgn(dot2)) { return false; }

		p1 = positions[2];
		p2 = positions[3];
		normal = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(p1, p2), { 0, 0, 1 });
		normal = DirectX::XMVector3Normalize(normal);
		relative_to_line = DirectX::XMVectorSubtract(center, p1);
		dot1 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		relative_to_line = DirectX::XMVectorSubtract(mouse, p1);
		dot2 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		if (sgn(dot1) != sgn(dot2)) { return false; }

		p1 = positions[3];
		p2 = positions[0];
		normal = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(p1, p2), { 0, 0, 1 });
		normal = DirectX::XMVector3Normalize(normal);
		relative_to_line = DirectX::XMVectorSubtract(center, p1);
		dot1 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		relative_to_line = DirectX::XMVectorSubtract(mouse, p1);
		dot2 = DirectX::XMVector3Dot(relative_to_line, normal).m128_f32[0];
		if (sgn(dot1) != sgn(dot2)) { return false; }

		return true;
	}
}