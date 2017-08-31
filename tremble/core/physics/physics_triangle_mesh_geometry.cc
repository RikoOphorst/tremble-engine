#include "physics_triangle_mesh_geometry.h"

#include "core/resources/model.h"
#include "core/resources/mesh.h"
#include "core/physics/physics_manager.h"
#include "core/rendering/renderer.h"

namespace tremble
{
	using namespace physx;

    //------------------------------------------------------------------------------------------------------
    PhysicsTriangleMeshGeometry::PhysicsTriangleMeshGeometry(tremble::Mesh* mesh) :
        PhysicsGeometry(TriangleMesh)
    {
        cookTriangleMeshGeometry(mesh);
    }

    //------------------------------------------------------------------------------------------------------
    PxGeometry* PhysicsTriangleMeshGeometry::GetPxGeometry() const
	{
		return (PxGeometry*)&px_triangle_mesh_geometry_;
	}

    //------------------------------------------------------------------------------------------------------
    void PhysicsTriangleMeshGeometry::Scale(const Vector3& scale)
    {
        px_triangle_mesh_geometry_.scale.scale.x = scale.GetX();
        px_triangle_mesh_geometry_.scale.scale.y = scale.GetY();
        px_triangle_mesh_geometry_.scale.scale.z = scale.GetZ();
    }

    //------------------------------------------------------------------------------------------------------
    PhysicsGeometry* PhysicsTriangleMeshGeometry::CreateCopyForScaled() const
    {
        return std::move(static_cast<PhysicsGeometry*>(Get::MemoryManager()->NewTemp<PhysicsTriangleMeshGeometry>(*this)));
    }

    //------------------------------------------------------------------------------------------------------
    void PhysicsTriangleMeshGeometry::cookTriangleMeshGeometry(tremble::Mesh* mesh)
    {
        std::vector<PxVec3> verts = GetVertices(mesh);
        std::vector<PxU32> indices32 = GetIndices(mesh);

        PxTriangleMeshDesc mesh_desc;        
        mesh_desc.points.count          = PxU32(verts.size());
        mesh_desc.points.stride         = sizeof(PxVec3);
        mesh_desc.points.data           = &verts[0];

        mesh_desc.triangles.count       = PxU32(indices32.size()) / 3;
        mesh_desc.triangles.stride      = 3 * sizeof(PxU32);
        mesh_desc.triangles.data        = &indices32[0];

        PxDefaultMemoryOutputStream writeBuffer;
        PxCooking* the_cooking = Get::PhysicsManager()->GetCooking();
        bool status = the_cooking->cookTriangleMesh(mesh_desc, writeBuffer);
        if (!status)
        {
            ASSERT(false && "Failed to cook TriangleMesh");
            return;
        }

        PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());

        PxPhysics* the_physics = Get::PhysicsManager()->GetPxPhysics();
        px_triangle_mesh_geometry_ = PxTriangleMeshGeometry(the_physics->createTriangleMesh(readBuffer));
    }

    //------------------------------------------------------------------------------------------------------
    std::vector<PxVec3> PhysicsTriangleMeshGeometry::GetVertices(tremble::Mesh* mesh)
    {
        std::vector<PxVec3> verts(mesh->GetVertices().size());
        for (int i = 0; i < verts.size(); ++i)
        {
            verts[i].x = PxReal(mesh->GetVertices()[i].position.x);
            verts[i].y = PxReal(mesh->GetVertices()[i].position.y);
            verts[i].z = PxReal(mesh->GetVertices()[i].position.z);
        }
        return verts;
    }

    //------------------------------------------------------------------------------------------------------
    std::vector<physx::PxU32> PhysicsTriangleMeshGeometry::GetIndices(tremble::Mesh* mesh)
    {
        if(mesh->GetIndexCount() == 0)
        {
            return CreateFakeIndices(int(mesh->GetVertices().size()));
        }

        std::vector<PxU32> indices(mesh->GetIndices().size());
        for(int i = 0; i < indices.size(); ++i)
        {
            indices[i] = PxU32(mesh->GetIndices()[i]);
        }
        return indices;
    }

    //------------------------------------------------------------------------------------------------------
    std::vector<PxU32> PhysicsTriangleMeshGeometry::CreateFakeIndices(int size)
    {
        std::vector<PxU32> indices(size);
        for (int i = 0; i < size; ++i)
        {
            indices[i] = PxU32(i);
        }
        return indices;
    }
}
