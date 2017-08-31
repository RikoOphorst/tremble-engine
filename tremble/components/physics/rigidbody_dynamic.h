#pragma once

#include "rigidbody.h"

namespace tremble
{
    class Model;

    class RigidbodyDynamic : public Rigidbody
	{
	public:
		void Awake(PhysicsGeometry* geometry, PhysicsMaterial* = nullptr, PhysicsGeometry::ScalingType scaling_type = PhysicsGeometry::ScalingType::RELATIVE_TO_NODE);
		void Awake(tremble::Model* model, PhysicsMaterial* = nullptr, PhysicsGeometry::ScalingType scaling_type = PhysicsGeometry::ScalingType::RELATIVE_TO_NODE);
	    void UpdateBeforePhysics();
		void UpdateAfterPhysics();
		void AddImpulseAtPos(Vector3 force, Vector3 world_pos);
		void AddLocalImpulseAtLocalPos(Vector3 force, Vector3 local_pos);
		void SetMass(float mass);
        float GetMass();
        void SetDrag(float drag);
        float GetDrag();
        void SetAngularDrag(float angular_drag);
        float GetAngularDrag();
        void SetLinearVelocity(Vector3 velocity);
        Vector3 GetLinearVelocity();
        void SetAngularVelocity(Vector3 angular_velocity);
        Vector3 GetAngularVelocity();
        void SetKinematic(bool is_kinematic);
        bool GetKinematic();
		physx::PxRigidActor* GetPxRigidbodyActor() override;
	private:

        void InitRigidbodyDynamic();
        void AttachShape(physx::PxShape* px_shape) override;

        bool is_kinematic_ = false;
        physx::PxRigidDynamic* px_rigid_dynamic_;
	};
}