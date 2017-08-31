#include "trigger_collider.h"
#include "../../core/scene_graph/scene_graph.h"
#include "../../core/physics/physics_manager.h"
#include "../../core/physics/physics_box_geometry.h"
#include "../../core/physics/physics_capsule_geometry.h"
#include "../../core/physics/physics_sphere_geometry.h"

namespace tremble
{
    using namespace physx;

    PhysicsGeometryHolder TriggerCollider::GetGeometry()
    {
        PxShape* shape_buffer;
        px_rigid_static_->getShapes(&shape_buffer, 1);
        PhysicsGeometryHolder returning_holder(shape_buffer->getGeometry());
        return returning_holder;
    }

    void TriggerCollider::SetGeometry(PhysicsGeometry* physics_geometry)
    {
        PxShape* shape;
        (PxRigidActor*)px_rigid_static_->getShapes(&shape, sizeof(PxShape*));
        shape->setGeometry(*physics_geometry->GetPxGeometry());
    }

    void TriggerCollider::Shutdown()
    {
        //Get::PhysicsManager()->RemovePxActor(px_rigidbody_);
    }

    physx::PxRigidActor* TriggerCollider::GetPxRigidbodyActor()
    {
        return (PxRigidActor*)px_rigid_static_;
    }

    void TriggerCollider::Awake(PhysicsGeometry* geometry, PhysicsGeometry::ScalingType scaling_type)
    {
        PxTransform transform(
            DirectX::XMVectorGetX(GetNode()->GetPosition()),
            DirectX::XMVectorGetY(GetNode()->GetPosition()),
            DirectX::XMVectorGetZ(GetNode()->GetPosition())
            ,
            GetNode()->GetRotationQuaternion().ToPxQuat());
        PhysicsGeometry* scaled = nullptr;
        switch (geometry->GetType())
        {
        case PhysicsGeometry::Box:
            scaled = (PhysicsGeometry*)Get::MemoryManager()->NewTemp<PhysicsBoxGeometry>(geometry);
            break;
        case PhysicsGeometry::Sphere:
            scaled = (PhysicsGeometry*)Get::MemoryManager()->NewTemp<PhysicsSphereGeometry>(geometry);
            break;
        case PhysicsGeometry::Capsule:
            scaled = (PhysicsGeometry*)Get::MemoryManager()->NewTemp<PhysicsCapsuleGeometry>(geometry);
            break;
        default:
            ASSERT(false);
            break;
        }
        switch (scaling_type)
        {
        case PhysicsGeometry::ScalingType::GLOBAL_SCALE:
            break;
        case PhysicsGeometry::ScalingType::RELATIVE_TO_NODE:
            scaled->Scale(GetNode()->GetScale());
            break;
        case PhysicsGeometry::ScalingType::RELATIVE_TO_PARENT:
            scaled->Scale(GetNode()->GetParent()->GetScale());
            break;
        }
        px_rigid_static_ = Get::PhysicsManager()->px_physics_->createRigidStatic(transform);
        physx::PxShape* px_shape_ = Get::PhysicsManager()->CreatePxShape(scaled, Get::PhysicsManager()->GetMaterial("default"));
        px_shape_->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
        px_shape_->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);
        px_rigid_static_->attachShape(*px_shape_);
        px_shape_->release();
        Get::PhysicsManager()->AddPxActor(px_rigid_static_, this);
    }

    void TriggerCollider::UpdateBeforePhysics()
    {
        if (GetNode()->WasMovedByUser())
        {
            px_rigid_static_->setGlobalPose(
                PxTransform(
                    GetNode()->GetPosition().ToPxVec3(),
                    GetNode()->GetRotationQuaternion().ToPxQuat()
                ));
        }
    }

    void TriggerCollider::Update()
    {
        PhysicsGeometryHolder holder = GetGeometry();
        PhysicsBoxGeometry box_geometry = holder.GetBoxGeometry();
        //Get::Renderer()->RenderDebugVolume(
        //    DebugVolume::CreateCube(
        //        GetNode()->GetPosition(),
        //        DirectX::XMFLOAT3(
        //            box_geometry.GetHalfExtents().GetX() * Scalar(2),
        //            box_geometry.GetHalfExtents().GetY() * Scalar(2),
        //            box_geometry.GetHalfExtents().GetZ() * Scalar(2)
        //        )
        //    )
        //);
    }
}