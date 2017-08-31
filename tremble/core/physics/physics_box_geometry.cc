#include "physics_box_geometry.h"
#include "../utilities/debug.h"
#include "core/get.h"
#include "core/memory/memory_manager.h"

namespace tremble
{
	using namespace physx;

	physx::PxGeometry* PhysicsBoxGeometry::GetPxGeometry() const
	{
		return (physx::PxGeometry*)&px_box_geometry_;
	}

	PhysicsBoxGeometry::PhysicsBoxGeometry(const Vector3& halfExtents)
		:PhysicsGeometry(PhysicsGeometry::Box)
	{
		px_box_geometry_ = PxBoxGeometry(halfExtents.ToPxVec3());
	}

	PhysicsBoxGeometry::PhysicsBoxGeometry(const physx::PxBoxGeometry& px_box_geometry)
		:PhysicsGeometry(PhysicsGeometry::Box), px_box_geometry_(px_box_geometry)
	{

	}

    PhysicsBoxGeometry::PhysicsBoxGeometry(PhysicsGeometry* box_geometry)
        : PhysicsGeometry(PhysicsGeometry::Box)
    {
        ASSERT(box_geometry->GetType() == PhysicsGeometry::Box);
        px_box_geometry_ = *(PxBoxGeometry*)box_geometry->GetPxGeometry();
    }

	Vector3 PhysicsBoxGeometry::GetHalfExtents()
	{
		return px_box_geometry_.halfExtents;
	}

	void PhysicsBoxGeometry::SetHalfExtents(const Vector3& half_extents)
	{
		px_box_geometry_.halfExtents = half_extents.ToPxVec3();
	}

    void PhysicsBoxGeometry::Scale(const Vector3& scale)
    {
        px_box_geometry_.halfExtents.x *= scale.GetX();
        px_box_geometry_.halfExtents.y *= scale.GetY();
        px_box_geometry_.halfExtents.z *= scale.GetZ();
    }

    PhysicsGeometry* PhysicsBoxGeometry::CreateCopyForScaled() const
    {
        return std::move(static_cast<PhysicsGeometry*>(Get::MemoryManager()->NewTemp<PhysicsBoxGeometry>(*this)));
    }
}
