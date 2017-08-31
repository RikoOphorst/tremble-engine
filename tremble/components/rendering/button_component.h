#pragma once
#include "../../core/scene_graph/component.h"
#include "../../core/math/math.h"

namespace tremble 
{

	class Texture;

	/// Defines the state a button is in
	enum ButtonState {
		Normal,		/// Not being interacted with
		Hover,		/// Mouse is on button
		Click,		/// Mouse is on button and button is held down
		Disabled	/// Button is not interactable
	};

	/**
	* @class tremble::ButtonComponent
	* @author Sander Brouwers
	* @brief Adds a button to the user interface
	*/
	class ButtonComponent : public Component
	{
	public:
		ButtonComponent();

		/// Assigns texture and uv to specified state
		void SetTexture(ButtonState state, Texture* texture, DirectX::XMFLOAT2 uvMin = { 0, 0 }, DirectX::XMFLOAT2 uvMax = {1, 1}) { texture_[state] = texture; }
		/// Returns texture assigned to specified state
		Texture* GetTexture(ButtonState state) { return texture_[state]; }

		/// Return the top-left uv for the button state
		DirectX::XMFLOAT2 GetUVMin(ButtonState state) { return uv_min_[state]; }
		/// Return the bottom-right uv for the button state
		DirectX::XMFLOAT2 GetUVMax(ButtonState state) { return uv_max_[state]; }

		/// Assigns position to button
		void SetPosition(Vector3 position) { position_ = position; custom_matrix_ = false; }
		/// Returns position of button
		Vector3 GetPosition() { return position_; }

		/// Assigns size to button
		void SetSize(Vector3 size) { size_ = size; custom_matrix_ = false; }
		/// Returns size of button
		Vector3 GetSize() { return size_; }

		/// Assigns rotation to button
		void SetRotation(float rotation) { rotation_ = rotation; custom_matrix_ = false; }
		/// Returns rotation of button
		float GetRotation() { return rotation_; }

		/// Assigns center point to button
		void SetCenter(Vector3 center) { center_ = center; custom_matrix_ = false; }
		/// Returns center of button
		Vector3 GetCenter() { return center_; }

		/// Assigns custom transformation matrix to button
		void SetMatrix(DirectX::XMMATRIX matrix) { transform_ = matrix; custom_matrix_ = true; }
		/// Returns custom transformation matrix of button
		DirectX::XMMATRIX GetMatrix() { return transform_; }

		/// Assigns render layer
		void SetLayer(int layer) { layer_ = layer; custom_matrix_ = false; }
		/// Returns render layer
		float GetLayer() { return layer_; }

		/// Assigns color to specified state
		void SetColor(ButtonState state, Vector4 color) { color_[state] = color; custom_matrix_ = false; }
		/// Returns color assigned to specified state
		Vector4 GetColor(ButtonState state) { return color_[state]; }

		/// Return state button is currently in
		ButtonState GetState() { return state_; }

		/// Checks if button is using a custom transformation matrix
		bool UsesCustomMatrix() { return custom_matrix_; }

		/// Assigns function for click event
		void SetOnclick(std::function<void(SGNode*)> on_click) { on_click_ = on_click; }

		/// Updates button state
		void Update();

		/// Returns transformation matrix used for rendering
		DirectX::XMMATRIX GetFinalTransform();

		/// Sets button interactable state
		void Enable(bool enabled) { enabled_ = enabled; }

	private:
		bool custom_matrix_;

		Texture* texture_[4];
		DirectX::XMFLOAT2 uv_min_[4];
		DirectX::XMFLOAT2 uv_max_[4];

		Vector3 position_;
		Vector3 size_;
		float rotation_;
		Vector3 center_;
		Vector4 color_[4];
		int layer_;
		DirectX::XMMATRIX transform_;

		ButtonState state_;

		bool enabled_;

		std::function<void(SGNode*)> on_click_;

		bool IsMouseInside();

		bool pointed_;
	};
}