#pragma once

#if USED_MATH_LIB == MATH_LIB_DXMATH

#ifndef INLINE
#define INLINE __forceinline
#endif

#include "../core/rendering/direct3d.h"
#include "../iscalar.h"

namespace tremble
{
#define PI 3.141592653589793f
	class Scalar : IScalar
	{
	public:
		INLINE Scalar() { s = 0.0f; }
		INLINE Scalar(float s_)			{ s = s_; };
		INLINE explicit Scalar(int s_)				{ s = static_cast<float>(s_); };
		INLINE explicit Scalar(double s_)			{ s = static_cast<float>(s_); };
		INLINE explicit Scalar(long s_)				{ s = static_cast<float>(s_); };
		INLINE explicit Scalar(long long s_)		{ s = static_cast<float>(s_); };
		INLINE Scalar(const DirectX::XMVECTOR& v)	{ s = DirectX::XMVectorGetX(v); }
	
		INLINE float Get() const { return s; }
		INLINE operator DirectX::XMVECTOR()			const { return DirectX::XMVectorSet(s, s, s, s); }
		INLINE operator DirectX::XMFLOAT4()			const { return DirectX::XMFLOAT4(s, s, s, s); }
		INLINE operator DirectX::XMFLOAT3()			const { return DirectX::XMFLOAT3(s, s, s); }
		INLINE operator DirectX::XMFLOAT2()			const { return DirectX::XMFLOAT2(s, s); }
		INLINE operator float()						const { return s; }

		INLINE Scalar operator- ()					const { return Scalar(-s); } //!< negate a scalar
		INLINE Scalar operator+ (const Scalar& v2)	const { return Scalar(s + v2.s); } //!< add two scalars
		INLINE Scalar operator- (const Scalar& v2)	const { return Scalar(s - v2.s); } //!< subtract two scalars
		INLINE Scalar operator* (const Scalar& v2)	const { return Scalar(s * v2.s); } //!< multiply two scalars
		INLINE Scalar operator/ (const Scalar& v2)	const { return Scalar(s / v2.s); } //!< divide two scalars

		INLINE Scalar operator+= (const Scalar& v2) { s = *this + v2; return Scalar(s); }
		INLINE Scalar operator-= (const Scalar& v2) { s = *this - v2; return Scalar(s); }
		INLINE Scalar operator*= (const Scalar& v2) { s = *this * v2; return Scalar(s); }
		INLINE Scalar operator/= (const Scalar& v2) { s = *this / v2; return Scalar(s); }

		INLINE std::string ToString() const { return std::to_string(s); }
		INLINE operator std::string() const { return ToString(); }
		//INLINE operator float() const { return s; }

		INLINE Scalar ToDegrees() const
		{
			return Scalar(s * 180 / PI);
		}

		INLINE Scalar ToRadians() const
		{
			return Scalar(s * PI / 180);
		}

        INLINE Scalar Abs() const
        {
            return s > 0 ? s : -s;
        }
	private:
		float s;
	};
}

#endif