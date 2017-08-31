#pragma once
#include "../../core/scene_graph/component.h"
#include "../../core/math/math.h"

namespace tremble 
{

	class Texture;

	/**
	* @class tremble::ImageComponent
	* @author Sander Brouwers
	* @brief Adds image to user interface
	*/
	class ImageComponent : public Component
	{
	public:
		ImageComponent();

		/// Assigns texture to image
		void SetTexture(Texture* texture) { texture_ = texture; }
		/// Returns texture assigned to image
		Texture* GetTexture() { return texture_; }

		/// Assigns position to image
		void SetPosition(Vector3 position) { position_ = position; custom_matrix_ = false; }
		/// Returns position assigned to image
		Vector3 GetPosition() { return position_; }

		/// Assigns size to image
		void SetSize(Vector3 size) { size_ = size; custom_matrix_ = false; }
		/// Returns size assigned to image
		Vector3 GetSize() { return size_; }

		/// Assigns rotation to image
		void SetRotation(float rotation) { rotation_ = rotation; custom_matrix_ = false; }
		/// Returns rotation assigned to image
		float GetRotation() { return rotation_; }

		/// Assigns center point to image
		void SetCenter(Vector3 center) { center_ = center; custom_matrix_ = false; }
		/// Returns center point assigned to image
		Vector3 GetCenter() { return center_; }

		/// Assigns custom transformation matrix to image
		void SetMatrix(DirectX::XMMATRIX matrix) { transform_ = matrix; custom_matrix_ = true; }
		/// Returns custom transformation matrix of image
		DirectX::XMMATRIX GetMatrix() { return transform_; }

		/// Assigns render layer
		void SetLayer(int layer) { layer_ = layer; custom_matrix_ = false; }
		/// Returns render layer
		float GetLayer() { return layer_; }

		/// Assigns color to image
		void SetColor(Vector4 color) { color_ = color; custom_matrix_ = false; }
		/// Returns color assigned to image
		Vector4 GetColor() { return color_; }

		/// Checks if image is using a custom transformation matrix
		bool UsesCustomMatrix() { return custom_matrix_; }

		/// Sets the uv for the sprite
		void SetUV(DirectX::XMFLOAT2 min, DirectX::XMFLOAT2 max) { uv_min_ = min; uv_max_ = max; }
		/// Return the top-left uv for the sprite
		DirectX::XMFLOAT2 GetUVMin() { return uv_min_; }
		/// Return the bottom-right uv for the sprite
		DirectX::XMFLOAT2 GetUVMax() { return uv_max_; }

	private:
		bool custom_matrix_;

		Texture* texture_;

		Vector3 position_;
		Vector3 size_;
		float rotation_;
		Vector3 center_;
		Vector4 color_;
		int layer_;
		DirectX::XMMATRIX transform_;
		DirectX::XMFLOAT2 uv_min_;
		DirectX::XMFLOAT2 uv_max_;
	};
}