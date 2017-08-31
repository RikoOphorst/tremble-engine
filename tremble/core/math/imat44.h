#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

namespace tremble
{
	class Mat44;
	class Scalar;
	class Vector4;
	class Vector3;

	class IMat44
	{
	public:
		virtual INLINE std::string ToString() const = 0;
		virtual INLINE operator std::string() const = 0;

		virtual INLINE Vector4 GetR0()				const = 0;
		virtual INLINE Vector4 GetR1()				const = 0;
		virtual INLINE Vector4 GetR2()				const = 0;
		virtual INLINE Vector4 GetR3()				const = 0;
		virtual INLINE void SetR0(const Vector4& v) = 0;
		virtual INLINE void SetR1(const Vector4& v) = 0;
		virtual INLINE void SetR2(const Vector4& v) = 0;
		virtual INLINE void SetR3(const Vector4& v) = 0;

		virtual INLINE Mat44 operator- ()					const = 0;
		virtual INLINE Mat44 operator+ (const Mat44& m2)	const = 0;
		virtual INLINE Mat44 operator- (const Mat44& m2)	const = 0;
		virtual INLINE Mat44 operator* (const Mat44& m2)	const = 0;
		virtual INLINE Mat44 operator/ (const Scalar& s)	const = 0;
		virtual INLINE Mat44 operator* (const Scalar& s)	const = 0;
		virtual INLINE Vector3 operator* (const Vector3& v)	const = 0;
		virtual INLINE Vector4 operator* (const Vector4& v)	const = 0;

		virtual INLINE Mat44 operator+= (const Mat44& m2) = 0;
		virtual INLINE Mat44 operator-= (const Mat44& m2) = 0;
		virtual INLINE Mat44 operator*= (const Mat44& m2) = 0;
		virtual INLINE Mat44 operator/= (const Scalar& s) = 0;
		virtual INLINE Mat44 operator*= (const Scalar& s) = 0;

		virtual INLINE Scalar Determinant() const = 0;
		virtual INLINE Mat44 Inverse() const = 0;
		virtual INLINE Mat44 Transpose() const = 0;
	};
}