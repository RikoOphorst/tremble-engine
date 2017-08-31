#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

#include "../ivector3.h"
#include "vector4.h"
#include "scalar.h"

namespace tremble
{
	class Vector3 : public IVector3
	{
		friend class Mat44;
	public:
		INLINE Vector3() { v = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f); }
		INLINE Vector3(const DirectX::XMVECTOR& vec) { v = vec; }
		INLINE Vector3(const DirectX::XMFLOAT3& f3) { v = DirectX::XMVectorSet(f3.x, f3.y, f3.z, 0.0f); }
		INLINE Vector3(const DirectX::XMFLOAT4& f4) { v = DirectX::XMVectorSet(f4.x, f4.y, f4.z, f4.w); }
		INLINE Vector3(const Scalar& s) { v = DirectX::XMVectorReplicate(s.Get()); }
		INLINE Vector3(float x, float y, float z) { v = DirectX::XMVectorSet(x, y, z, 0.0f); }
		INLINE Vector3(const Scalar& x, const Scalar& y, const Scalar& z) { v = DirectX::XMVectorSet(x.Get(), y.Get(), z.Get(), 0.0f); }
		INLINE Vector3(const Vector4& vec) { v = DirectX::XMVectorSet(vec.GetX(), vec.GetY(), vec.GetZ(), 0.0f); }
		INLINE Vector3(const physx::PxVec3& vec) { v = DirectX::XMVectorSet(vec.x, vec.y, vec.z, 0.0f); }
		INLINE Vector3(const physx::PxExtendedVec3& vec) { v = DirectX::XMVectorSet((float)vec.x, (float)vec.y, (float)vec.z, 0.0f); }

		INLINE operator DirectX::XMVECTOR()			const { return v; }
		INLINE operator DirectX::XMFLOAT4()			const { return DirectX::XMFLOAT4(GetX().Get(), GetY().Get(), GetZ().Get(), 0.0f); }
		INLINE operator DirectX::XMFLOAT3()			const { return DirectX::XMFLOAT3(GetX().Get(), GetY().Get(), GetZ().Get()); }
		INLINE operator DirectX::XMFLOAT2()			const { return DirectX::XMFLOAT2(GetX().Get(), GetY().Get()); }

		INLINE Scalar GetX()				const { return Scalar(DirectX::XMVectorGetX(v)); }
		INLINE Scalar GetY()				const { return Scalar(DirectX::XMVectorGetY(v)); }
		INLINE Scalar GetZ()				const { return Scalar(DirectX::XMVectorGetZ(v)); }
		INLINE void SetX(const Scalar& s)	{ v = DirectX::XMVectorSetX(v, s.Get()); }
		INLINE void SetY(const Scalar& s)	{ v = DirectX::XMVectorSetY(v, s.Get()); }
		INLINE void SetZ(const Scalar& s)	{ v = DirectX::XMVectorSetZ(v, s.Get()); }
		INLINE std::string ToString()		const { return std::string(GetX()) + "," + std::string(GetY()) + "," + std::string(GetZ()); }
		INLINE operator std::string()				const { return ToString(); }

		INLINE Vector3 operator- ()						const { return Vector3(DirectX::XMVectorNegate(v)); }
		INLINE Vector3 operator+ (const Vector3& v2)	const { return Vector3(DirectX::XMVectorAdd(v, v2)); }
		INLINE Vector3 operator- (const Vector3& v2)	const { return Vector3(DirectX::XMVectorSubtract(v, v2)); }
		INLINE Vector3 operator* (const Vector3& v2)	const { return Vector3(DirectX::XMVectorMultiply(v, v2)); }
		INLINE Vector3 operator/ (const Vector3& v2)	const { return Vector3(DirectX::XMVectorDivide(v, v2)); }
		INLINE Vector3 operator* (const Scalar& v2)		const { return *this * Vector3(v2); }
		INLINE Vector3 operator/ (const Scalar& v2)		const { return *this / Vector3(v2); }
		INLINE Vector3 operator* (float v2)				const { return *this * Scalar(v2); }
		INLINE Vector3 operator/ (float v2)				const { return *this / Scalar(v2); }

		INLINE Vector3 operator+= (const Vector3& v2)	{ v = DirectX::XMVectorAdd(v, v2); return v; }
		INLINE Vector3 operator-= (const Vector3& v2)	{ v = Vector3(DirectX::XMVectorSubtract(v, v2)); return v; }
		INLINE Vector3 operator*= (const Vector3& v2)	{ v = Vector3(DirectX::XMVectorMultiply(v, v2)); return v; }
		INLINE Vector3 operator/= (const Vector3& v2)	{ v = Vector3(DirectX::XMVectorDivide(v, v2)); return v; }
		INLINE Vector3 operator*= (const Scalar& v2)	{ v = *this * Vector3(v2); return v; }
		INLINE Vector3 operator/= (const Scalar& v2)	{ v = *this / Vector3(v2); return v; }
		INLINE Vector3 operator*= (float v2)			{ v = *this * Scalar(v2); return v; }
		INLINE Vector3 operator/= (float v2)			{ v = *this / Scalar(v2); return v; }

		INLINE bool operator== (const Vector3& v2)		const { if (v2.GetX() == GetX() && v2.GetY() == GetY() && v2.GetZ() == GetZ()) { return true; } return false; }
		INLINE bool operator!= (const Vector3& v2)		const { if (v2.GetX() != GetX() || v2.GetY() != GetY() || v2.GetZ() != GetZ()) { return true; } return false; }

		INLINE Vector3 operator% (const Vector3& v2)	const { return Vector3(DirectX::XMVector3Cross(v, v2)); }
		INLINE Scalar operator& (const Vector3& v2)		const { return Scalar(DirectX::XMVector3Dot(v, v2)); }

		INLINE Vector3 Negate()							const { return -(*this); }
		INLINE Vector3 Add(const Vector3& v2)			const { return *this + v2; }
		INLINE Vector3 Subtract(const Vector3& v2)		const { return *this - v2; }
		INLINE Vector3 Multiply(const Vector3& v2)		const { return *this * v2; }
		INLINE Vector3 Divide(const Vector3& v2)		const { return *this / v2; }
		INLINE Vector3 Scale(const Scalar& v2)			const { return *this * v2; }
		INLINE Vector3 Divide(const Scalar& v2)			const { return *this / v2; }
		INLINE Vector3 Scale(float v2)					const { return *this * v2; }
		INLINE Vector3 Divide(float v2)					const { return *this / v2; }

		INLINE Vector3 Cross(Vector3 v2)				const { return *this % v2; }
		INLINE Scalar Dot(Vector3 v2)					const { return *this & v2; }

		INLINE Vector3 Normalize()						const { return Vector3(DirectX::XMVector3Normalize(v)); } //!< normalize this vector
		INLINE Vector3 NormalizeEst()					const { return Vector3(DirectX::XMVector3NormalizeEst(v)); } //!< get an estimate of this vector normalized
		INLINE Scalar Length()							const { return Scalar(DirectX::XMVector3Length(v)); } //!< get length of this vector
		INLINE Scalar LengthEst()						const { return Scalar(DirectX::XMVector3LengthEst(v)); } //!< get estimated length of this vector
		INLINE Scalar LengthSq()						const { return Scalar(DirectX::XMVector3LengthSq(v)); } //!< get squared length of this vector

        INLINE Vector3 ToDegrees()                      const { return Vector3(DirectX::XMVectorScale(v, 180.0f / DirectX::XM_PI)); }
		INLINE physx::PxVec3 ToPxVec3()					const { return physx::PxVec3(GetX(), GetY(), GetZ()); }
        INLINE physx::PxExtendedVec3 ToPxExtendedVec3()	const { return physx::PxExtendedVec3(GetX(), GetY(), GetZ()); }
		INLINE const DirectX::XMVECTOR& ToDxVec()		const { return v; }
	protected:
		DirectX::XMVECTOR v;
	};
}