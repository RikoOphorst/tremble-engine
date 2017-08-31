#include "rigidbody_static.h"

#include "../../core/get.h"
#include "../../core/scene_graph/scene_graph.h"
#include "../../core/physics/physics_manager.h"
#include "../../core/physics/physics_material.h"
#include "../../core/physics/physics_triangle_mesh_geometry.h"
#include "../../core/resources/model.h"
#include "../../core/resources/mesh.h"

namespace tremble
{
    using namespace physx;

    //------------------------------------------------------------------------------------------------------
    physx::PxRigidActor * RigidbodyStatic::GetPxRigidbodyActor()
    {
        return (PxRigidActor*)px_rigid_static_;
    }

    //------------------------------------------------------------------------------------------------------
    void RigidbodyStatic::Awake(PhysicsGeometry* geometry, PhysicsMaterial* material, PhysicsGeometry::ScalingType scaling_type)
    {
        InitRigidbodyStatic();
        AttachScaledShape(geometry, material, scaling_type);
        Get::PhysicsManager()->AddPxActor(px_rigid_static_, this);
    }

    //------------------------------------------------------------------------------------------------------
    void RigidbodyStatic::Awake(tremble::Model* model, PhysicsMaterial* material, PhysicsGeometry::ScalingType scaling_type)
    {
        InitRigidbodyStatic();
        for (int i = 0; i < model->GetMeshes().size(); ++i)
        {
            PhysicsGeometry* geometry = model->GetMeshes()[i]->GetPhysicsTriangleMeshGeometry();
            AttachScaledShape(geometry, material, scaling_type);
        }
        Get::PhysicsManager()->AddPxActor(px_rigid_static_, this);
    }

    //------------------------------------------------------------------------------------------------------
    void RigidbodyStatic::InitRigidbodyStatic()
    {
        ASSERT(GetNode()->IsStatic() == true);
        PxTransform transform = PxTransform(
            DirectX::XMVectorGetX(GetNode()->GetPosition()),
            DirectX::XMVectorGetY(GetNode()->GetPosition()),
            DirectX::XMVectorGetZ(GetNode()->GetPosition()),
            GetNode()->GetRotationQuaternion().ToPxQuat()
        );
        px_rigid_static_ = Get::PhysicsManager()->GetPxPhysics()->createRigidStatic(transform);
    }

    //------------------------------------------------------------------------------------------------------
    void RigidbodyStatic::AttachShape(physx::PxShape* px_shape)
    {
        px_rigid_static_->attachShape(*px_shape);
    }
}
