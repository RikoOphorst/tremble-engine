#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

#include "../imat44.h"
#include "scalar.h"
#include "vector4.h"
#include "vector3.h"

namespace tremble
{
	class Mat44 : public IMat44
	{
		friend class Vector4;
	public:
		INLINE Mat44() { m = DirectX::XMMATRIX(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f); }
		INLINE Mat44(const DirectX::XMMATRIX& mat) { m = mat; }
		INLINE Mat44(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) { m = DirectX::XMMATRIX(r0.v, r1.v, r2.v, r3.v); }

		INLINE operator DirectX::XMMATRIX()			const { return m; }

		INLINE Vector4 GetR0()						const { return Vector4(m.r[0]); }
		INLINE Vector4 GetR1()						const { return Vector4(m.r[1]); }
		INLINE Vector4 GetR2()						const { return Vector4(m.r[2]); }
		INLINE Vector4 GetR3()						const { return Vector4(m.r[3]); }
		INLINE void SetR0(const Vector4& v)			{ m.r[0] = v.v; }
		INLINE void SetR1(const Vector4& v)			{ m.r[1] = v.v; }
		INLINE void SetR2(const Vector4& v)			{ m.r[2] = v.v; }
		INLINE void SetR3(const Vector4& v)			{ m.r[3] = v.v; }
		INLINE std::string ToString()				const { std::string out; for (int y = 0; y < 4; y++) { out += std::to_string(DirectX::XMVectorGetX(m.r[y])); out += ","; out += std::to_string(DirectX::XMVectorGetY(m.r[y])); out += ","; out += std::to_string(DirectX::XMVectorGetZ(m.r[y])); out += ","; out += std::to_string(DirectX::XMVectorGetW(m.r[y])); out += "\n"; } return out; }
		INLINE operator std::string()				const { return ToString(); }

		INLINE bool operator== (const Mat44 m2) const {
			return 
				GetR0().GetX() == m2.GetR0().GetX() &&
				GetR0().GetY() == m2.GetR0().GetY() &&
				GetR0().GetZ() == m2.GetR0().GetZ() &&
				GetR0().GetW() == m2.GetR0().GetW() && 
				GetR1().GetX() == m2.GetR1().GetX() &&
				GetR1().GetY() == m2.GetR1().GetY() &&
				GetR1().GetZ() == m2.GetR1().GetZ() &&
				GetR1().GetW() == m2.GetR1().GetW() && 
				GetR2().GetX() == m2.GetR2().GetX() &&
				GetR2().GetY() == m2.GetR2().GetY() &&
				GetR2().GetZ() == m2.GetR2().GetZ() &&
				GetR2().GetW() == m2.GetR2().GetW() && 
				GetR3().GetX() == m2.GetR3().GetX() &&
				GetR3().GetY() == m2.GetR3().GetY() &&
				GetR3().GetZ() == m2.GetR3().GetZ() &&
				GetR3().GetW() == m2.GetR3().GetW();
		}

		INLINE Mat44 operator- ()					const { return Mat44(m * -1); }
		INLINE Mat44 operator+ (const Mat44& m2)	const { return Mat44(m + m2.m); }
		INLINE Mat44 operator- (const Mat44& m2)	const { return Mat44(m - m2.m); }
		INLINE Mat44 operator* (const Mat44& m2)	const { return Mat44(m * m2.m); }
		INLINE Mat44 operator/ (const Scalar& s)	const { return Mat44(m / s); }
		INLINE Mat44 operator* (const Scalar& s)	const { return Mat44(m * s); }
		INLINE Vector3 operator* (const Vector3& v)	const { return Vector3(DirectX::XMVector3Transform(v.v, m)); }
		INLINE Vector4 operator* (const Vector4& v)	const { return Vector4(DirectX::XMVector4Transform(v.v, m)); }

		INLINE Mat44 operator+= (const Mat44& m2)	{ m = m + m2.m; return *this; }
		INLINE Mat44 operator-= (const Mat44& m2)	{ m = m - m2.m; return *this; }
		INLINE Mat44 operator*= (const Mat44& m2)	{ m = m * m2.m; return *this; }
		INLINE Mat44 operator/= (const Scalar& s)	{ m = m / s; return *this; }
		INLINE Mat44 operator*= (const Scalar& s)	{ m = m * s; return *this; }

		INLINE Scalar Determinant() const { return Scalar(DirectX::XMMatrixDeterminant(m)); }
		INLINE Mat44 Inverse() const { return Mat44(DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(m), m)); }
		INLINE Mat44 Transpose() const { return Mat44(DirectX::XMMatrixTranspose(m)); }

		INLINE physx::PxMat44 ToPxMat44() const {
			physx::PxMat44 pxm;
			Mat44 t = Transpose();

			pxm.column0 = physx::PxVec4(DirectX::XMVectorGetX(t.m.r[0]), DirectX::XMVectorGetY(t.m.r[1]), DirectX::XMVectorGetZ(t.m.r[2]), DirectX::XMVectorGetW(t.m.r[3]));
			pxm.column1 = physx::PxVec4(DirectX::XMVectorGetX(t.m.r[0]), DirectX::XMVectorGetY(t.m.r[1]), DirectX::XMVectorGetZ(t.m.r[2]), DirectX::XMVectorGetW(t.m.r[3]));
			pxm.column2 = physx::PxVec4(DirectX::XMVectorGetX(t.m.r[0]), DirectX::XMVectorGetY(t.m.r[1]), DirectX::XMVectorGetZ(t.m.r[2]), DirectX::XMVectorGetW(t.m.r[3]));
			pxm.column3 = physx::PxVec4(DirectX::XMVectorGetX(t.m.r[0]), DirectX::XMVectorGetY(t.m.r[1]), DirectX::XMVectorGetZ(t.m.r[2]), DirectX::XMVectorGetW(t.m.r[3]));

			return pxm;
		}
	protected:
		DirectX::XMMATRIX m;
	};
}