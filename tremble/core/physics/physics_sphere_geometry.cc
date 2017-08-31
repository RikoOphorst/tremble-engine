#include "physics_sphere_geometry.h"
#include "../utilities/debug.h"
#include "core/get.h"
#include "core/memory/memory_manager.h"

namespace tremble
{
	using namespace physx;

	physx::PxGeometry* PhysicsSphereGeometry::GetPxGeometry() const
	{
		return (physx::PxGeometry*)&px_sphere_geometry_;
	}

	PhysicsSphereGeometry::PhysicsSphereGeometry(float radius)
		:PhysicsGeometry(PhysicsGeometry::Sphere)
	{
		px_sphere_geometry_ = PxSphereGeometry(radius);
	}

	PhysicsSphereGeometry::PhysicsSphereGeometry(const PxSphereGeometry& px_sphere_geometry)
		:PhysicsGeometry(PhysicsGeometry::Sphere), px_sphere_geometry_(px_sphere_geometry)
	{

	}

    PhysicsSphereGeometry::PhysicsSphereGeometry(PhysicsGeometry* sphere_geometry)
        :PhysicsGeometry(PhysicsGeometry::Capsule)
    {
        ASSERT(sphere_geometry->GetType() == PhysicsGeometry::Capsule);
        px_sphere_geometry_ = *(physx::PxSphereGeometry*)sphere_geometry->GetPxGeometry();
    }

	float PhysicsSphereGeometry::GetRadius()
	{
		return px_sphere_geometry_.radius;
	}

	void PhysicsSphereGeometry::SetRadius(float radius)
	{
		px_sphere_geometry_.radius = radius;
	}

    void PhysicsSphereGeometry::Scale(const Vector3& scale)
    {
        px_sphere_geometry_.radius *= std::max(std::max(scale.GetX(), scale.GetY()), scale.GetZ());
    }

    PhysicsGeometry* PhysicsSphereGeometry::CreateCopyForScaled() const
    {
        return std::move(static_cast<PhysicsGeometry*>(Get::MemoryManager()->NewTemp<PhysicsSphereGeometry>(*this)));
    }
}
