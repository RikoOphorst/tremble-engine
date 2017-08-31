#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

#include "../ivector4.h"
#include "scalar.h"

namespace tremble
{
	class Vector4 : public IVector4
	{
		friend class Mat44;
	public:
		INLINE Vector4() { v = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); }
		INLINE Vector4(const DirectX::XMVECTOR& vec) { v = vec; }
		INLINE Vector4(const DirectX::XMFLOAT4& vec) { v = DirectX::XMVectorSet(vec.x, vec.y, vec.z, vec.w); }
		INLINE Vector4(const DirectX::XMFLOAT3& vec) { v = DirectX::XMVectorSet(vec.x, vec.y, vec.z, 0.0f); }
		INLINE Vector4(const DirectX::XMFLOAT3& vec, const Scalar& s) { v = DirectX::XMVectorSet(vec.x, vec.y, vec.z, s); }
		INLINE Vector4(const DirectX::XMFLOAT2& vec) { v = DirectX::XMVectorSet(vec.x, vec.y, 0.0f, 0.0f); }
		INLINE Vector4(const Scalar& s) { v = DirectX::XMVectorReplicate(s.Get()); }
		INLINE Vector4(float x, float y, float z, float w = 0.0f) { v = DirectX::XMVectorSet(x, y, z, w); }
		INLINE Vector4(const Scalar& x, const Scalar& y, const Scalar& z, const Scalar& w = Scalar(0.0f)) { v = DirectX::XMVectorSet(x.Get(), y.Get(), z.Get(), w.Get()); }

		INLINE operator DirectX::XMVECTOR()			const { return v; }
		INLINE operator DirectX::XMFLOAT4()			const { return DirectX::XMFLOAT4(GetX().Get(), GetY().Get(), GetZ().Get(), GetW().Get()); }
		INLINE operator DirectX::XMFLOAT3()			const { return DirectX::XMFLOAT3(GetX().Get(), GetY().Get(), GetZ().Get()); }
		INLINE operator DirectX::XMFLOAT2()			const { return DirectX::XMFLOAT2(GetX().Get(), GetY().Get()); }

		INLINE Scalar GetX()				const { return Scalar(DirectX::XMVectorGetX(v)); }
		INLINE Scalar GetY()				const { return Scalar(DirectX::XMVectorGetY(v)); }
		INLINE Scalar GetZ()				const { return Scalar(DirectX::XMVectorGetZ(v)); }
		INLINE Scalar GetW()				const { return Scalar(DirectX::XMVectorGetW(v)); }
		INLINE void SetX(const Scalar& s)	{ v = DirectX::XMVectorSetX(v, s.Get()); }
		INLINE void SetY(const Scalar& s)	{ v = DirectX::XMVectorSetY(v, s.Get()); }
		INLINE void SetZ(const Scalar& s)	{ v = DirectX::XMVectorSetZ(v, s.Get()); }
		INLINE void SetW(const Scalar& s)	{ v = DirectX::XMVectorSetW(v, s.Get()); }
		INLINE std::string ToString()		const { return std::string(GetX()) + "," + std::string(GetY()) + "," + std::string(GetZ()) + "," + std::string(GetW()); }
		INLINE operator std::string()				const { return ToString(); }

		INLINE Vector4 operator- ()						const { return Vector4(DirectX::XMVectorNegate(v)); }
		INLINE Vector4 operator+ (const Vector4& v2)	const { return Vector4(DirectX::XMVectorAdd(v, v2)); }
		INLINE Vector4 operator- (const Vector4& v2)	const { return Vector4(DirectX::XMVectorSubtract(v, v2)); }
		INLINE Vector4 operator* (const Vector4& v2)	const { return Vector4(DirectX::XMVectorMultiply(v, v2)); }
		INLINE Vector4 operator/ (const Vector4& v2)	const { return Vector4(DirectX::XMVectorDivide(v, v2)); }
		INLINE Vector4 operator* (const Scalar& v2)		const { return *this * Vector4(v2); }
		INLINE Vector4 operator/ (const Scalar& v2)		const { return *this / Vector4(v2); }
		INLINE Vector4 operator* (float v2)				const { return *this * Scalar(v2); }
		INLINE Vector4 operator/ (float v2)				const { return *this / Scalar(v2); }

		INLINE Vector4 operator+= (const Vector4& v2)	 { v = DirectX::XMVectorAdd(v, v2); return v; }
		INLINE Vector4 operator-= (const Vector4& v2)	 { v = Vector4(DirectX::XMVectorSubtract(v, v2)); return v; }
		INLINE Vector4 operator*= (const Vector4& v2)	 { v = Vector4(DirectX::XMVectorMultiply(v, v2)); return v; }
		INLINE Vector4 operator/= (const Vector4& v2)	 { v = Vector4(DirectX::XMVectorDivide(v, v2)); return v; }
		INLINE Vector4 operator*= (const Scalar& v2)	 { v = *this * Vector4(v2); return v; }
		INLINE Vector4 operator/= (const Scalar& v2)	 { v = *this / Vector4(v2); return v; }
		INLINE Vector4 operator*= (float v2)			 { v = *this * Scalar(v2); return v; }
		INLINE Vector4 operator/= (float v2)			 { v = *this / Scalar(v2); return v; }

		INLINE Vector4 operator% (const Vector4& v2)	const { return Vector4(DirectX::XMVector3Cross(v, v2)); }
		INLINE Scalar operator& (const Vector4& v2)		const { return Scalar(DirectX::XMVector3Dot(v, v2)); }

		INLINE Vector4 Negate()							const { return -(*this); }
		INLINE Vector4 Add(const Vector4& v2)			const { return *this + v2; }
		INLINE Vector4 Subtract(const Vector4& v2)		const { return *this - v2; }
		INLINE Vector4 Multiply(const Vector4& v2)		const { return *this * v2; }
		INLINE Vector4 Divide(const Vector4& v2)		const { return *this / v2; }
		INLINE Vector4 Scale(const Scalar& v2)			const { return *this * v2; }
		INLINE Vector4 Divide(const Scalar& v2)			const { return *this / v2; }
		INLINE Vector4 Scale(float v2)					const { return *this * v2; }
		INLINE Vector4 Divide(float v2)					const { return *this / v2; }

		INLINE Vector4 Cross(Vector4 v2)				const { return *this % v2; }
		INLINE Scalar Dot(Vector4 v2)					const { return *this & v2; }

		INLINE Vector4 Normalize4D()					const { return Vector4(DirectX::XMVector4Normalize(v)); } //!< normalize this vector
		INLINE Vector4 NormalizeEst4D()					const { return Vector4(DirectX::XMVector4NormalizeEst(v)); } //!< normalize this vector
		INLINE Scalar Length4D()						const { return Scalar(DirectX::XMVector4Length(v)); } //!< get length of this vector
		INLINE Scalar LengthEst4D()						const { return Scalar(DirectX::XMVector4LengthEst(v)); } //!< get estimated length of this vector
		INLINE Scalar LengthSq4D()						const { return Scalar(DirectX::XMVector4LengthSq(v)); } //!< get squared length of this vector

		INLINE Vector4 Normalize3D()					const { return Vector4(DirectX::XMVector3Normalize(v)); } //!< normalize this vector
		INLINE Vector4 NormalizeEst3D()					const { return Vector4(DirectX::XMVector3NormalizeEst(v)); } //!< normalize this vector
		INLINE Scalar Length3D()						const { return Scalar(DirectX::XMVector3Length(v)); } //!< get length of this vector
		INLINE Scalar LengthEst3D()						const { return Scalar(DirectX::XMVector3LengthEst(v)); } //!< get estimated length of this vector
		INLINE Scalar LengthSq3D()						const { return Scalar(DirectX::XMVector3LengthSq(v)); } //!< get squared length of this vector

		INLINE physx::PxVec4 ToPxVec4()					const { return physx::PxVec4(GetX(), GetY(), GetZ(), GetW()); }
	protected:
		DirectX::XMVECTOR v;
	};
}