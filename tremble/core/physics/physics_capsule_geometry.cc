#include "physics_capsule_geometry.h"
#include "../math/math.h"
#include "../utilities/debug.h"
#include "core/get.h"
#include "core/memory/memory_manager.h"

namespace tremble
{
    physx::PxGeometry* tremble::PhysicsCapsuleGeometry::GetPxGeometry() const
    {
        return (physx::PxGeometry*)&px_capsule_geometry_;
    }

    PhysicsCapsuleGeometry::PhysicsCapsuleGeometry(float radius, float half_height)
        :PhysicsGeometry(PhysicsGeometry::Capsule)
    {
        px_capsule_geometry_ = physx::PxCapsuleGeometry(radius, half_height);
    }

    PhysicsCapsuleGeometry::PhysicsCapsuleGeometry(const physx::PxCapsuleGeometry & px_capsule_geometry)
        : PhysicsGeometry(PhysicsGeometry::Capsule), px_capsule_geometry_(px_capsule_geometry)
    {

    }

    PhysicsCapsuleGeometry::PhysicsCapsuleGeometry(PhysicsGeometry* capsule_geometry)
        : PhysicsGeometry(PhysicsGeometry::Capsule)
    {
        ASSERT(capsule_geometry->GetType() == PhysicsGeometry::Capsule);
        px_capsule_geometry_ = *(physx::PxCapsuleGeometry*)capsule_geometry->GetPxGeometry();
    }

    float PhysicsCapsuleGeometry::GetRadius()
    {
        return px_capsule_geometry_.radius;
    }

    float PhysicsCapsuleGeometry::GetHalfHeight()
    {
        return px_capsule_geometry_.halfHeight;
    }

    void PhysicsCapsuleGeometry::SetRadius(float radius)
    {
        px_capsule_geometry_.radius = radius;
    }

    void PhysicsCapsuleGeometry::SetHalfHeight(float half_height)
    {
        px_capsule_geometry_.halfHeight = half_height;
    }

    void PhysicsCapsuleGeometry::Scale(const Vector3& scale)
    {
        px_capsule_geometry_.halfHeight *= scale.GetY();
        px_capsule_geometry_.radius *= std::max(scale.GetX(), scale.GetZ());
    }

    PhysicsGeometry* PhysicsCapsuleGeometry::CreateCopyForScaled() const
    {
        return std::move(static_cast<PhysicsGeometry*>(Get::MemoryManager()->NewTemp<PhysicsCapsuleGeometry>(*this)));
    }
}
