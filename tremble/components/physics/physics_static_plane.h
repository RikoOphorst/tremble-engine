#pragma once

#include "../../core/scene_graph/component.h"

namespace tremble
{
	class PhysicsMaterial;
	class PhysicsStaticPlane : public Component
	{
	public:
		void Awake(const Vector3 angle = Vector3(0, 1, 0), float offset = 0.0f, PhysicsMaterial* physics_material = nullptr);
	private:
		Vector3 angle_;
		Scalar offset_;
		physx::PxRigidStatic* px_rigid_static_;
	};
}