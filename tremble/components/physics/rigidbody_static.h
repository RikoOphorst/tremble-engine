#pragma once

#include "rigidbody.h"

namespace tremble
{
    class Model;

    class RigidbodyStatic : public Rigidbody
    {
    public:
        physx::PxRigidActor* GetPxRigidbodyActor() override;
        void Awake(PhysicsGeometry* geometry, PhysicsMaterial* material = nullptr, PhysicsGeometry::ScalingType scaling_type = PhysicsGeometry::ScalingType::RELATIVE_TO_NODE);
        void Awake(tremble::Model* model, PhysicsMaterial* material = nullptr, PhysicsGeometry::ScalingType scaling_type = PhysicsGeometry::ScalingType::RELATIVE_TO_NODE);
    private:

        void InitRigidbodyStatic();
        void AttachShape(physx::PxShape* px_shape) override;

        physx::PxRigidStatic* px_rigid_static_;
        physx::PxShape* px_shape_;
    };
}