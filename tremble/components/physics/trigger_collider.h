#pragma once

#include "../../core/scene_graph/component.h"
#include "../../core/game_manager.h"
#include "../../core/scene_graph/component_manager.h"
#include "../../core/physics/physics_geometry.h"
#include "../../core/physics/physics_geometry_holder.h"
#include "../../core/get.h"

namespace tremble
{
    class TriggerCollider : public Component
    {
    protected:
    public:
        void Awake(PhysicsGeometry* geometry, PhysicsGeometry::ScalingType scaling_type = PhysicsGeometry::ScalingType::RELATIVE_TO_NODE);
        void Update();
        void UpdateBeforePhysics();
        PhysicsGeometryHolder GetGeometry();
        void SetGeometry(PhysicsGeometry* physics_geometry);

		std::string tag;

        void Shutdown() override;
        virtual physx::PxRigidActor* GetPxRigidbodyActor();
    protected:
        physx::PxRigidStatic* px_rigid_static_;
        physx::PxShape* px_shape_;
    };
}