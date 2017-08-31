#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

namespace tremble
{
	class Vector3;
	class Vector4;
	class Scalar;

	class IVector3
	{
	public:
		virtual INLINE Scalar GetX() const = 0;
		virtual INLINE Scalar GetY() const = 0;
		virtual INLINE Scalar GetZ() const = 0;
		virtual INLINE void SetX(const Scalar& s) = 0;
		virtual INLINE void SetY(const Scalar& s) = 0;
		virtual INLINE void SetZ(const Scalar& s) = 0;
		virtual INLINE std::string ToString() const = 0;
		virtual INLINE operator std::string() const = 0;

		virtual INLINE Vector3 operator- ()	const = 0; //!< negate a vector
		virtual INLINE Vector3 operator+ (const Vector3& v2) const = 0; //!< add two vectors
		virtual INLINE Vector3 operator- (const Vector3& v2) const = 0; //!< subtract two vectors
		virtual INLINE Vector3 operator* (const Vector3& v2) const = 0; //!< multiply two vectors
		virtual INLINE Vector3 operator/ (const Vector3& v2) const = 0; //!< divide two vectors
		virtual INLINE Vector3 operator* (const Scalar&  v2) const = 0; //!< multiply vec by scalar
		virtual INLINE Vector3 operator/ (const Scalar&  v2) const = 0; //!< device vec by scalar
		virtual INLINE Vector3 operator* (float  v2) const = 0; //!< multiply vec by float
		virtual INLINE Vector3 operator/ (float  v2) const = 0; //!< divide vec by float

		virtual INLINE Vector3 operator+= (const Vector3& v2) = 0;
		virtual INLINE Vector3 operator-= (const Vector3& v2) = 0;
		virtual INLINE Vector3 operator*= (const Vector3& v2) = 0;
		virtual INLINE Vector3 operator/= (const Vector3& v2) = 0;
		virtual INLINE Vector3 operator*= (const Scalar& v2) = 0;
		virtual INLINE Vector3 operator/= (const Scalar& v2) = 0;
		virtual INLINE Vector3 operator*= (float v2) = 0;
		virtual INLINE Vector3 operator/= (float v2) = 0;

		virtual INLINE Vector3 operator% (const Vector3& v2) const = 0; //!< cross two vectors
		virtual INLINE Scalar operator& (const Vector3& v2) const = 0; //!< dot two vectors

		virtual INLINE Vector3 Negate() const = 0;
		virtual INLINE Vector3 Add(const Vector3& v2) const = 0;
		virtual INLINE Vector3 Subtract(const Vector3& v2) const = 0;
		virtual INLINE Vector3 Multiply(const Vector3& v2) const = 0;
		virtual INLINE Vector3 Divide(const Vector3& v2) const = 0;
		virtual INLINE Vector3 Scale(const Scalar& v2) const = 0;
		virtual INLINE Vector3 Divide(const Scalar& v2) const = 0;
		virtual INLINE Vector3 Scale(float v2) const = 0;
		virtual INLINE Vector3 Divide(float v2) const = 0;

		virtual INLINE Vector3 Cross(Vector3 v2) const = 0; //!< cross two vectors
		virtual INLINE Scalar Dot(Vector3 v2) const = 0; //!< dot two vectors

		virtual INLINE Vector3 Normalize() const = 0; //!< normalize this vector
		virtual INLINE Vector3 NormalizeEst() const = 0; //!< normalize this vector
		virtual INLINE Scalar Length() const = 0; //!< get length of this vector
		virtual INLINE Scalar LengthEst() const = 0; //!< get length of this vector
		virtual INLINE Scalar LengthSq() const = 0; //!< get length of this vector
	};
}