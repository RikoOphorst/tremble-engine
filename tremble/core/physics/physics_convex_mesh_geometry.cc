#include "physics_convex_mesh_geometry.h"

#include "core/get.h"
#include "core/memory/memory_manager.h"
#include "core/physics/physics_manager.h"

namespace tremble
{
    using namespace physx;

    //------------------------------------------------------------------------------------------------------
    ConvexMeshGeometry::ConvexMeshGeometry(const std::vector<PxVec3>& verts) :
        PhysicsGeometry(ConvexMesh)
    {
        CookConvexMesh(verts);
    }

    //------------------------------------------------------------------------------------------------------
    physx::PxGeometry* ConvexMeshGeometry::GetPxGeometry() const
    {
        return (PxGeometry*)&px_convex_mesh_geometry_;
    }

    //------------------------------------------------------------------------------------------------------
    void ConvexMeshGeometry::Scale(const Vector3& scale)
    {
        px_convex_mesh_geometry_.scale.scale.x = scale.GetX();
        px_convex_mesh_geometry_.scale.scale.y = scale.GetY();
        px_convex_mesh_geometry_.scale.scale.z = scale.GetZ();
    }

    //------------------------------------------------------------------------------------------------------
    PhysicsGeometry* ConvexMeshGeometry::CreateCopyForScaled() const
    {
        return std::move(static_cast<PhysicsGeometry*>(Get::MemoryManager()->NewTemp<ConvexMeshGeometry>(*this)));
    }

    //------------------------------------------------------------------------------------------------------
    void ConvexMeshGeometry::CookConvexMesh(const std::vector<PxVec3>& verts)
    {
        PxConvexMeshDesc convex_desc;
        convex_desc.points.count    = PxU32(verts.size());
        convex_desc.points.stride   = sizeof(PxVec3);
        convex_desc.points.data     = &verts[0];
        convex_desc.flags           = PxConvexFlag::eCOMPUTE_CONVEX;

        PxDefaultMemoryOutputStream buf;
        PxConvexMeshCookingResult::Enum result;
        PxCooking* cooking = Get::PhysicsManager()->GetCooking();
        if(!cooking->cookConvexMesh(convex_desc, buf, &result))
        {
            ASSERT(false && "Unable to cook convexMesh");
            return;
        }

        PxPhysics* physics = Get::PhysicsManager()->GetPxPhysics();
        PxDefaultMemoryInputData input(buf.getData(), buf.getSize());
        px_convex_mesh_geometry_ = PxConvexMeshGeometry(physics->createConvexMesh(input));
    }
}

