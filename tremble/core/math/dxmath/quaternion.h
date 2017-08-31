#pragma once

#ifndef INLINE
#define INLINE __forceinline
#endif

#include "scalar.h"
#include "vector3.h"
#include "mat44.h"

namespace tremble
{
	class Quaternion
	{
	public:
		INLINE Quaternion() { q = DirectX::XMQuaternionIdentity(); }
		INLINE Quaternion(const DirectX::XMVECTOR& q_) { q = q_; }
		INLINE Quaternion(const Scalar& roll, const Scalar& pitch, const Scalar& yaw) { q = DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }
		INLINE Quaternion(const Vector3& axis, const Scalar& angle) { q = DirectX::XMQuaternionRotationAxis(axis, angle); }
		INLINE Quaternion(const Vector3& rotation_radians) { q = DirectX::XMQuaternionRotationRollPitchYaw(rotation_radians.GetX(), rotation_radians.GetY(), rotation_radians.GetZ()); }
		INLINE Quaternion(const Mat44& matrix) { q = DirectX::XMQuaternionRotationMatrix(matrix); }
		INLINE Quaternion(const physx::PxQuat& px_quat) { q = DirectX::XMVectorSet(px_quat.x, px_quat.y, px_quat.z, px_quat.w); }

		INLINE operator DirectX::XMVECTOR()	const { return q; }

		INLINE Scalar GetX() const { return Scalar(DirectX::XMVectorGetX(q)); }
		INLINE Scalar GetY() const { return Scalar(DirectX::XMVectorGetY(q)); }
		INLINE Scalar GetZ() const { return Scalar(DirectX::XMVectorGetZ(q)); }
		INLINE Scalar GetW() const { return Scalar(DirectX::XMVectorGetW(q)); }
		INLINE void SetX(const Scalar& s) { DirectX::XMVectorSetX(q, s.Get()); }
		INLINE void SetY(const Scalar& s) { DirectX::XMVectorSetY(q, s.Get()); }
		INLINE void SetZ(const Scalar& s) { DirectX::XMVectorSetZ(q, s.Get()); }
		INLINE void SetW(const Scalar& s) { DirectX::XMVectorSetW(q, s.Get()); }

        INLINE Quaternion Inverse() const { return DirectX::XMQuaternionInverse(q); }

		INLINE std::string ToString() const { return std::string(GetX()) + "," + std::string(GetY()) + "," + std::string(GetZ()) + "," + std::string(GetW()); }
		INLINE operator std::string() const { return ToString(); }

		INLINE Quaternion operator~ (void) const { return Quaternion(DirectX::XMQuaternionConjugate(q)); }
		INLINE Quaternion operator- (void) const { return Quaternion(DirectX::XMVectorNegate(q)); }

		INLINE Quaternion operator* (Quaternion rhs) const { return Quaternion(DirectX::XMQuaternionMultiply(q, rhs.q)); }
		INLINE Vector3 operator* (Vector3 rhs) const { return Vector3(DirectX::XMVector3Rotate(rhs, q)); }

		INLINE Quaternion& operator= (Quaternion rhs) { q = rhs.q; return *this; }
		INLINE Quaternion& operator*= (Quaternion rhs) { *this = *this * rhs; return *this; }

		INLINE bool operator== (const Quaternion& rhs) const { return (GetX() == rhs.GetX() && GetY() == rhs.GetY() && GetZ() == rhs.GetZ() && GetW() == rhs.GetW()); }
		INLINE bool operator!= (const Quaternion& rhs) const { return !(GetX() == rhs.GetX() && GetY() == rhs.GetY() && GetZ() == rhs.GetZ() && GetW() == rhs.GetW()); }

        INLINE static Quaternion FromTo(const Quaternion& from, const Quaternion& to);
       

		INLINE physx::PxQuat ToPxQuat() const
		{
			physx::PxQuat pxq;

			pxq.x = GetX();
			pxq.y = GetY();
			pxq.z = GetZ();
			pxq.w = GetW();

			return pxq;
		}

		INLINE Vector3 ToEuler() const {
			float sqw;
			float sqx;
			float sqy;
			float sqz;
			
            DirectX::XMFLOAT4 quaternion;
            DirectX::XMStoreFloat4(&quaternion, q);

			sqw = quaternion.w * quaternion.w;
			sqx = quaternion.x * quaternion.x;
			sqy = quaternion.y * quaternion.y;
			sqz = quaternion.z * quaternion.z;
			
            float unit = sqw + sqx + sqy + sqz;
            float test = quaternion.x * quaternion.w - quaternion.y * quaternion.z;

            Vector3 vec;
            if (test > 0.4995f * unit)
            {
                vec.SetY(2.0f * atan2(quaternion.y, quaternion.x));
                vec.SetX(DirectX::XM_PIDIV2);

                return vec;
            }
            if (test < -0.4995f * unit)
            {
                vec.SetY(-2.0f * atan2(quaternion.y, quaternion.x));
                vec.SetX(-DirectX::XM_PIDIV2);

                return vec;
            }

            vec.SetX(asin(2.0f * (quaternion.w * quaternion.x - quaternion.z * quaternion.y)));
            vec.SetY(atan2(2.0f * quaternion.w * quaternion.y + 2.0f * quaternion.z * quaternion.x, 1 - 2.0f * (sqx + sqy)));
            vec.SetZ(atan2(2.0f* quaternion.w * quaternion.z + 2.0f * quaternion.x * quaternion.y, 1 - 2.0f * (sqz + sqx)));

			//rotxrad = (float)atan2l(2.0 * (DirectX::XMVectorGetY(q) * DirectX::XMVectorGetZ(q) + DirectX::XMVectorGetX(q) * DirectX::XMVectorGetW(q)), (-sqx - sqy + sqz + sqw)); 
			//rotyrad = (float)asinl(-2.0 * (DirectX::XMVectorGetX(q) * DirectX::XMVectorGetZ(q) - DirectX::XMVectorGetY(q) * DirectX::XMVectorGetW(q))); //
			//rotzrad = (float)atan2l(2.0 * (DirectX::XMVectorGetX(q) * DirectX::XMVectorGetY(q) + DirectX::XMVectorGetZ(q) * DirectX::XMVectorGetW(q)), (sqx - sqy - sqz + sqw));
			
			return vec;
		};

        INLINE Vector3 ToDegreesEuler() const {
            return ToEuler().ToDegrees();
        };
	private:
		DirectX::XMVECTOR q;
	};
}