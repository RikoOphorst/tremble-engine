#pragma once

namespace tremble
{
	class PhysicsMaterial
	{
	public:
		friend class PhysicsManager;
		physx::PxMaterial* GetPxMaterial();

		void SetStaticFriction(float static_friction);
		void SetDynamicFriction(float dynamic_friction);
		void SetRestitution(float restitution);

		float GetStaticFriction();
		float GetDynamicFriction();
		float GetRestitution();

	private:
		PhysicsMaterial(physx::PxMaterial* px_material); //!< Constructor, used only by the physics manager
		physx::PxMaterial* px_material_; //!< Physics material that PhysX uses
	};
}