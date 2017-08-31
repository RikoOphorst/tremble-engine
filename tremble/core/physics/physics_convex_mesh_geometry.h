#pragma once

#include "../math/math.h"
#include "physics_geometry.h"

namespace tremble
{
    class ConvexMeshGeometry : public PhysicsGeometry
    {
    public:
        ConvexMeshGeometry(const std::vector<physx::PxVec3>& verts);

        physx::PxGeometry* GetPxGeometry() const override;
        void Scale(const Vector3& scale) override;

        PhysicsGeometry* CreateCopyForScaled() const override;
    private:

        void CookConvexMesh(const std::vector<physx::PxVec3>& verts);

        physx::PxConvexMeshGeometry px_convex_mesh_geometry_;
    };
}