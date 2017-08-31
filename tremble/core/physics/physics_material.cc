#include "physics_material.h"
#include "../get.h"

namespace tremble
{
	//PhysicsMaterial::PhysicsMaterial(float static_friction, float dynamic_friction, float restitution)
	//{
	//}

	physx::PxMaterial* PhysicsMaterial::GetPxMaterial()
	{
		return px_material_;
	}

	void PhysicsMaterial::SetStaticFriction(float static_friction)
	{
		px_material_->setStaticFriction(static_friction);
	}

	void PhysicsMaterial::SetDynamicFriction(float dynamic_friction)
	{
		px_material_->setDynamicFriction(dynamic_friction);
	}

	void PhysicsMaterial::SetRestitution(float restitution)
	{
		px_material_->setRestitution(restitution);
	}

	float PhysicsMaterial::GetStaticFriction()
	{
		return px_material_->getStaticFriction();
	}

	float PhysicsMaterial::GetDynamicFriction()
	{
		return px_material_->getDynamicFriction();;
	}

	float PhysicsMaterial::GetRestitution()
	{
		return px_material_->getRestitution();
	}

	PhysicsMaterial::PhysicsMaterial(physx::PxMaterial* px_material)
		:px_material_(px_material)
	{
		
	}
}