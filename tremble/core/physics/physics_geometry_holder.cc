#include "physics_geometry_holder.h"
#include "../utilities/debug.h"
#include "physics_geometry.h"
#include "physics_box_geometry.h"
#include "physics_capsule_geometry.h"
#include "physics_sphere_geometry.h"
#include "../get.h"
#include "physics_manager.h"
#include "../memory/allocators/free_list_allocator.h"

namespace tremble
{
	using namespace physx;

	PhysicsGeometryHolder::PhysicsGeometryHolder()
	{

	}

    PhysicsGeometryHolder::PhysicsGeometryHolder(const PhysicsGeometryHolder& other)
    {
        switch (other.GetGeometryType())
        {
        case PhysicsGeometry::Box:
            m_physics_geometry_ = Get::PhysicsManager()->physics_allocator_->New<PhysicsBoxGeometry>(other.m_physics_geometry_);
            break;
        case PhysicsGeometry::Capsule:
            m_physics_geometry_ = Get::PhysicsManager()->physics_allocator_->New<PhysicsCapsuleGeometry>(other.m_physics_geometry_);
            break;
        case PhysicsGeometry::Sphere:
            m_physics_geometry_ = Get::PhysicsManager()->physics_allocator_->New<PhysicsSphereGeometry>(other.m_physics_geometry_);
            break;
        }
    }

    PhysicsGeometryHolder::PhysicsGeometryHolder(const physx::PxGeometry& px_geometry)
    {
        SetGeometry(px_geometry);
    }

    PhysicsGeometryHolder::PhysicsGeometryHolder(const physx::PxGeometryHolder& px_geometry)
    {
        SetGeometry(px_geometry.any());
    }

    void PhysicsGeometryHolder::SetGeometry(const physx::PxGeometry& px_geometry)
    {
        Clear();
        switch (px_geometry.getType())
        {
        case PxGeometryType::eBOX:
            m_physics_geometry_ = Get::PhysicsManager()->physics_allocator_->New<PhysicsBoxGeometry>((const physx::PxBoxGeometry&)px_geometry);
            break;
        case PxGeometryType::eCAPSULE:
            m_physics_geometry_ = Get::PhysicsManager()->physics_allocator_->New<PhysicsCapsuleGeometry>((const physx::PxCapsuleGeometry&)px_geometry);
            break;
        case PxGeometryType::eSPHERE:
            m_physics_geometry_ = Get::PhysicsManager()->physics_allocator_->New<PhysicsSphereGeometry>((const physx::PxSphereGeometry&)px_geometry);
            break;
        default:
            ASSERT(false && "Geometry type not supported");
        }
    }

    PhysicsGeometryHolder::~PhysicsGeometryHolder()
    {
        Clear();
    }

    void PhysicsGeometryHolder::Clear()
    {
        if (m_physics_geometry_ != nullptr)
        {
            Get::PhysicsManager()->physics_allocator_->Delete(m_physics_geometry_);
            m_physics_geometry_ = nullptr;
        }
    }

    PhysicsGeometry* PhysicsGeometryHolder::GetGeometry()
    {
        return m_physics_geometry_;
    }

    PhysicsGeometry::GeometryType PhysicsGeometryHolder::GetGeometryType() const
	{
        return m_physics_geometry_->GetType();
	}

	PhysicsBoxGeometry* PhysicsGeometryHolder::GetBoxGeometry()
	{
		return (PhysicsBoxGeometry*)m_physics_geometry_;
	}

    PhysicsSphereGeometry* PhysicsGeometryHolder::GetSphereGeometry()
	{
        return (PhysicsSphereGeometry*)m_physics_geometry_;
	}

	PhysicsCapsuleGeometry* PhysicsGeometryHolder::GetCapsuleGeometry()
	{
        return (PhysicsCapsuleGeometry*)m_physics_geometry_;
	}

}
