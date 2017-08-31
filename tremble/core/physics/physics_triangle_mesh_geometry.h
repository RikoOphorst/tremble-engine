#pragma once

#include "../math/math.h"
#include "physics_geometry.h"

namespace tremble
{
    class Mesh;
    class Model;

    class PhysicsTriangleMeshGeometry : public PhysicsGeometry
	{
	public:
        PhysicsTriangleMeshGeometry(tremble::Mesh* mesh);

		physx::PxGeometry* GetPxGeometry() const override;
        void Scale(const Vector3& scale) override;

        PhysicsGeometry* CreateCopyForScaled() const override;

        static std::vector<physx::PxVec3> GetVertices(tremble::Mesh* mesh);
        static std::vector<physx::PxU32> GetIndices(tremble::Mesh* mesh);
        static std::vector<physx::PxU32> CreateFakeIndices(int size);
	private:

        void cookTriangleMeshGeometry(tremble::Mesh* mesh);

		physx::PxTriangleMeshGeometry px_triangle_mesh_geometry_;
	};
}
