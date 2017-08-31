#pragma once
#include "physics_geometry.h"

namespace tremble
{
	class PhysicsCapsuleGeometry : public PhysicsGeometry
	{
	public:
		physx::PxGeometry* GetPxGeometry() const override;
		PhysicsCapsuleGeometry(float radius, float half_height);
		PhysicsCapsuleGeometry(const physx::PxCapsuleGeometry& px_capsule_geometry);
        PhysicsCapsuleGeometry(PhysicsGeometry* capsule_geometry);
		float GetRadius();
		float GetHalfHeight();
		void SetRadius(float radius);
		void SetHalfHeight(float half_height);
        void Scale(const Vector3& scale) override;

        PhysicsGeometry* CreateCopyForScaled() const override;
	private:
		physx::PxCapsuleGeometry px_capsule_geometry_;
	};
}