#pragma once

#include "../../core/math/math.h"
#include "../../core/scene_graph/component.h"

namespace tremble
{
	enum LightType
	{
		LightTypePoint = 1,
		LightTypeDirectional = 0,
		LightTypeSpot = 2,
		LightTypeDualConeSpot = 3
	};

	class Light : public Component
	{
	public:
		Light();

		void SetLightType(const LightType& light_type) { light_type_ = light_type; }
		void SetDirection(const Vector3& direction) { direction_ = direction; }
		void SetColor(const Vector4& color) { color_ = color; }
		void SetFalloffEnd(const Scalar& end) { falloff_end_ = end; }
		void SetConeAngle(const Scalar& cone_angle) { cos_cone_ = Scalar(std::cos(cone_angle)); }
		void SetCosConeAngle(const Scalar& cone_angle) { cos_cone_ = Scalar(cone_angle); }

		const LightType& GetLightType() const { return light_type_; }
		const Vector3& GetDirection() const { return direction_; }
		const Vector4& GetColor() const { return color_; }
		const Scalar& GetFalloffEnd() const { return falloff_end_; }
		const Scalar& GetCone() const { return Scalar(std::acos(cos_cone_)); }
		const Scalar& GetCosCone() const { return cos_cone_; }

		void SetShadowRange(int index, int range) { shadow_index_ = index; shadow_range_ = range; }
		int GetShadowIndex() { return shadow_index_; }
		int GetShadowRange() { return shadow_range_; }

		bool GetShadowCasting() { return cast_shadow_; }
		void SetShadowCasting(bool casting) { cast_shadow_ = casting; }

	private:
		LightType light_type_;
		Vector3 direction_;
		Vector4 color_;
		Scalar falloff_end_;
		Scalar cos_cone_;

		bool cast_shadow_;

		int shadow_index_;
		int shadow_range_;
	};
}