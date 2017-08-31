#pragma once

#include "../math/math.h"
#include "physics_geometry.h"

namespace tremble
{
	class PhysicsBoxGeometry : public PhysicsGeometry
	{
	public:
		physx::PxGeometry* GetPxGeometry() const override;
		PhysicsBoxGeometry(const Vector3& halfExtents = Vector3(0.5f, 0.5f, 0.5f));
        PhysicsBoxGeometry(const physx::PxBoxGeometry& px_box_geometry);
        PhysicsBoxGeometry(PhysicsGeometry* box_geometry);
		Vector3 GetHalfExtents();
		void SetHalfExtents(const Vector3& half_extents);
        void Scale(const Vector3& scale) override;

        PhysicsGeometry* CreateCopyForScaled() const override;
	private:
		physx::PxBoxGeometry px_box_geometry_;
	};
}