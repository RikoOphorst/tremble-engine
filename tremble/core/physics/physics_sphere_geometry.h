#pragma once

#include "../math/math.h"
#include "physics_geometry.h"

namespace tremble
{
	class PhysicsSphereGeometry : public PhysicsGeometry
	{
	public:
		physx::PxGeometry* GetPxGeometry() const override;
		PhysicsSphereGeometry(float radius);
		PhysicsSphereGeometry(const physx::PxSphereGeometry& px_sphere_geometry);
        PhysicsSphereGeometry(PhysicsGeometry* sphere_geometry);
		float GetRadius();
		void SetRadius(float radius);
        void Scale(const Vector3& scale) override;

        PhysicsGeometry* CreateCopyForScaled() const override;
	private:
		physx::PxSphereGeometry px_sphere_geometry_;
	};
}