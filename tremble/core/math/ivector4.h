#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

namespace tremble
{
	class Vector4;
	class Vector3;
	class Scalar;

	class IVector4
	{
	public:
		virtual INLINE Scalar GetX() const = 0;
		virtual INLINE Scalar GetY() const = 0;
		virtual INLINE Scalar GetZ() const = 0;
		virtual INLINE Scalar GetW() const = 0;
		virtual INLINE void SetX(const Scalar& s) = 0;
		virtual INLINE void SetY(const Scalar& s) = 0;
		virtual INLINE void SetZ(const Scalar& s) = 0;
		virtual INLINE void SetW(const Scalar& s) = 0;
		virtual INLINE std::string ToString() const = 0;
		virtual INLINE operator std::string() const = 0;

		virtual INLINE Vector4 operator- ()	const = 0; //!< negate a vector
		virtual INLINE Vector4 operator+ (const Vector4& v2) const = 0; //!< add two vectors
		virtual INLINE Vector4 operator- (const Vector4& v2) const = 0; //!< subtract two vectors
		virtual INLINE Vector4 operator* (const Vector4& v2) const = 0; //!< multiply two vectors
		virtual INLINE Vector4 operator/ (const Vector4& v2) const = 0; //!< divide two vectors
		virtual INLINE Vector4 operator* (const Scalar&  v2) const = 0; //!< multiply vec by scalar
		virtual INLINE Vector4 operator/ (const Scalar&  v2) const = 0; //!< device vec by scalar
		virtual INLINE Vector4 operator* (float  v2) const = 0; //!< multiply vec by float
		virtual INLINE Vector4 operator/ (float  v2) const = 0; //!< divide vec by float

		virtual INLINE Vector4 operator+= (const Vector4& v2) = 0;
		virtual INLINE Vector4 operator-= (const Vector4& v2) = 0;
		virtual INLINE Vector4 operator*= (const Vector4& v2) = 0;
		virtual INLINE Vector4 operator/= (const Vector4& v2) = 0;
		virtual INLINE Vector4 operator*= (const Scalar& v2) = 0;
		virtual INLINE Vector4 operator/= (const Scalar& v2) = 0;
		virtual INLINE Vector4 operator*= (float v2) = 0;
		virtual INLINE Vector4 operator/= (float v2) = 0;

		virtual INLINE Vector4 operator% (const Vector4& v2) const = 0; //!< cross two vectors
		virtual INLINE Scalar operator& (const Vector4& v2) const = 0; //!< dot two vectors

		virtual INLINE Vector4 Negate() const = 0;
		virtual INLINE Vector4 Add(const Vector4& v2) const = 0;
		virtual INLINE Vector4 Subtract(const Vector4& v2) const = 0;
		virtual INLINE Vector4 Multiply(const Vector4& v2) const = 0;
		virtual INLINE Vector4 Divide(const Vector4& v2) const = 0;
		virtual INLINE Vector4 Scale(const Scalar& v2) const = 0;
		virtual INLINE Vector4 Divide(const Scalar& v2) const = 0;
		virtual INLINE Vector4 Scale(float v2) const = 0;
		virtual INLINE Vector4 Divide(float v2) const = 0;

		virtual INLINE Vector4 Cross(Vector4 v2) const = 0; //!< cross two vectors
		virtual INLINE Scalar Dot(Vector4 v2) const = 0; //!< dot two vectors

		virtual INLINE Vector4 Normalize4D() const = 0; //!< normalize this vector
		virtual INLINE Vector4 NormalizeEst4D() const = 0; //!< normalize this vector
		virtual INLINE Scalar Length4D() const = 0; //!< get length of this vector
		virtual INLINE Scalar LengthEst4D() const = 0; //!< get length of this vector
		virtual INLINE Scalar LengthSq4D() const = 0; //!< get length of this vector

		virtual INLINE Vector4 Normalize3D() const = 0; //!< normalize this vector
		virtual INLINE Vector4 NormalizeEst3D() const = 0; //!< normalize this vector
		virtual INLINE Scalar Length3D() const = 0; //!< get length of this vector
		virtual INLINE Scalar LengthEst3D() const = 0; //!< get length of this vector
		virtual INLINE Scalar LengthSq3D() const = 0; //!< get length of this vector
	};
}