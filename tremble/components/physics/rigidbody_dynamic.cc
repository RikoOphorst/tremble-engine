#include "rigidbody_dynamic.h"

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

    void RigidbodyDynamic::Awake(PhysicsGeometry* geometry, PhysicsMaterial* material, PhysicsGeometry::ScalingType scaling_type)
    {
        InitRigidbodyDynamic();
        AttachScaledShape(geometry, material, scaling_type);
        PxRigidBodyExt::updateMassAndInertia(*px_rigid_dynamic_, 10.0f);
        Get::PhysicsManager()->AddPxActor(px_rigid_dynamic_, this);
    }

    void RigidbodyDynamic::Awake(tremble::Model* model, PhysicsMaterial* material, PhysicsGeometry::ScalingType scaling_type)
    {
        InitRigidbodyDynamic();
        for (int i = 0; i < model->GetMeshes().size(); ++i)
        {
            PhysicsGeometry* geometry = model->GetMeshes()[i]->GetPhysicsTriangleMeshGeometry();
            AttachScaledShape(geometry, material, scaling_type);
        }
        PxRigidBodyExt::updateMassAndInertia(*px_rigid_dynamic_, 10.0f);
        Get::PhysicsManager()->AddPxActor(px_rigid_dynamic_, this);
    }

    void RigidbodyDynamic::UpdateBeforePhysics()
	{
        if (GetNode()->WasMovedByUser())
        {
            PxTransform new_transform(
                GetNode()->GetPosition().ToPxVec3(),
                GetNode()->GetRotationQuaternion().ToPxQuat()
            );
            if (!is_kinematic_)
            {
                px_rigid_dynamic_->setGlobalPose(new_transform);
            }
            else
            {
                px_rigid_dynamic_->setKinematicTarget(new_transform);
            }
        }
	}

    void RigidbodyDynamic::UpdateAfterPhysics()
    {
        if (!px_rigid_dynamic_->isSleeping())
        {
            PxTransform get_transform(px_rigid_dynamic_->getGlobalPose());
            GetNode()->SetPositionForPhysics_(Vector3(
                get_transform.p.x,
                get_transform.p.y,
                get_transform.p.z
            ));
            GetNode()->SetRotationQuaternionForPhysics_(
                Quaternion(get_transform.q)
            );
        }
    }

	void RigidbodyDynamic::AddImpulseAtPos(Vector3 force, Vector3 world_pos)
	{
        if (!is_kinematic_)
        {
            PxRigidBodyExt::addForceAtPos(*px_rigid_dynamic_, force.ToPxVec3(), world_pos.ToPxVec3(), physx::PxForceMode::eIMPULSE);
        }
	}

	void RigidbodyDynamic::AddLocalImpulseAtLocalPos(Vector3 force, Vector3 local_pos)
	{
        if (!is_kinematic_)
        {
            PxRigidBodyExt::addLocalForceAtLocalPos(*px_rigid_dynamic_, force.ToPxVec3(), local_pos.ToPxVec3(), physx::PxForceMode::eIMPULSE);
        }
	}

	void RigidbodyDynamic::SetMass(float mass)
	{
        px_rigid_dynamic_->setMass(mass);
	}

    void RigidbodyDynamic::SetDrag(float drag)
    {
        if (!is_kinematic_)
        {
            ASSERT(drag > 0 && "Drag cannot be less than zero");
            px_rigid_dynamic_->setLinearDamping(drag);
        }
    }

    float RigidbodyDynamic::GetDrag()
    {
        return px_rigid_dynamic_->getLinearDamping();
    }

    void RigidbodyDynamic::SetAngularDrag(float angular_drag)
    {
        if (!is_kinematic_)
        {
            px_rigid_dynamic_->setAngularDamping(angular_drag);
        }
    }

    float RigidbodyDynamic::GetAngularDrag()
    {
        return px_rigid_dynamic_->getAngularDamping();
    }

    void RigidbodyDynamic::SetLinearVelocity(Vector3 velocity)
    {
        if (!is_kinematic_)
        {
            px_rigid_dynamic_->setLinearVelocity(velocity.ToPxVec3());
        }
    }

    Vector3 RigidbodyDynamic::GetLinearVelocity()
    {
        return px_rigid_dynamic_->getLinearVelocity();
    }

    void RigidbodyDynamic::SetAngularVelocity(Vector3 angular_velocity)
    {
        if (!is_kinematic_)
        {
            px_rigid_dynamic_->setAngularVelocity(angular_velocity.ToPxVec3());
        }
    }

    Vector3 RigidbodyDynamic::GetAngularVelocity()
    {
        return px_rigid_dynamic_->getAngularVelocity();
    }

	float RigidbodyDynamic::GetMass()
	{
		return px_rigid_dynamic_->getMass();
	}

    void RigidbodyDynamic::SetKinematic(bool is_kinematic)
    {
        px_rigid_dynamic_->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, is_kinematic);
        is_kinematic_ = is_kinematic;
    }

    bool RigidbodyDynamic::GetKinematic()
    {
        return false;
    }

	physx::PxRigidActor* RigidbodyDynamic::GetPxRigidbodyActor()
	{
		return (PxRigidActor*)px_rigid_dynamic_;
	}

    void RigidbodyDynamic::InitRigidbodyDynamic()
    {
        PxTransform transform(
            DirectX::XMVectorGetX(GetNode()->GetPosition()),
            DirectX::XMVectorGetY(GetNode()->GetPosition()),
            DirectX::XMVectorGetZ(GetNode()->GetPosition())
            ,
            GetNode()->GetRotationQuaternion().ToPxQuat());
        px_rigid_dynamic_ = Get::PhysicsManager()->px_physics_->createRigidDynamic(transform);
    }

    void RigidbodyDynamic::AttachShape(physx::PxShape* px_shape)
    {
        px_rigid_dynamic_->attachShape(*px_shape);
    }
}
