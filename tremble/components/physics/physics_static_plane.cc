#include "physics_static_plane.h"

#include "../../core/get.h"
#include "../../core/physics/physics_manager.h"
#include "../../core/physics/physics_material.h"

namespace tremble
{
	using namespace physx;

	void PhysicsStaticPlane::Awake(const Vector3 angle, float offset, PhysicsMaterial* physics_material)
	{
		if (physics_material == nullptr)
		{
			physics_material = Get::PhysicsManager()->GetMaterial("default");
		}
		angle_ = angle;
		offset_ = offset;
		px_rigid_static_ = Get::PhysicsManager()->CreatePxPlaneRigidStatic(angle_, offset_, physics_material);
		Get::PhysicsManager()->AddPxActor(px_rigid_static_, this);
	}
}