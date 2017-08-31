#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

namespace tremble
{
	class Scalar;

	class IScalar
	{
	public:
		virtual INLINE Scalar operator- ()	const = 0; //!< negate a scalar
		virtual INLINE Scalar operator+ (const Scalar& v2) const = 0; //!< add two vectors
		virtual INLINE Scalar operator- (const Scalar& v2) const = 0; //!< subtract two scalars
		virtual INLINE Scalar operator* (const Scalar& v2) const = 0; //!< multiply two scalars
		virtual INLINE Scalar operator/ (const Scalar& v2) const = 0; //!< divide two scalars
		virtual INLINE std::string ToString() const = 0;
		virtual INLINE operator std::string() const = 0;
		virtual INLINE operator float() const = 0;
		virtual INLINE float Get() const = 0;
	};
}