#pragma once

#include "../../core/scene_graph/component.h"
#include "../../core/game_manager.h"
#include "../../core/scene_graph/component_manager.h"
#include "../../core/physics/physics_geometry.h"
#include "../../core/physics/physics_geometry_holder.h"
#include "../../core/get.h"

namespace tremble
{
	class PhysicsMaterial;

	class Rigidbody : public Component
	{
	protected:
	public:
		PhysicsMaterial* GetMaterial();
		PhysicsGeometryHolder GetGeometry();

		void SetMaterial(PhysicsMaterial* physics_material);
		void SetGeometry(PhysicsGeometry* physics_geometry);

		void Shutdown() override;
		virtual physx::PxRigidActor* GetPxRigidbodyActor() = 0;
        void DoCollisionCallbacks(CollisionData collision_data) const;

        template <class T>
        void RegisterNewCollisionCallback(void (T::*collision_callback)(const CollisionData&), T* this_pointer) //!< Add a collision callback to this rigidbody
        {
            collision_callbacks_.push_back(std::bind(collision_callback, this_pointer, std::placeholders::_1));
        }
        
        template <class T>
        void DeregisterCollisionCallback(void (T::*collision_callback)(const CollisionData&), T* this_pointer) //!< Remove a collision callback from this rigidbody
        {
            std::find(collision_callbacks_.begin(), collision_callbacks_.end(), std::bind(collision_callback, this_pointer, std::placeholders::_1));
        }
	protected:

        void AttachScaledShape(PhysicsGeometry* geometry, PhysicsMaterial* material, PhysicsGeometry::ScalingType scaling_type);
        virtual void AttachShape(physx::PxShape* px_shape) = 0;

        std::vector<std::function<void(const CollisionData&)>> collision_callbacks_; //!< Collision callback functions
	};
}