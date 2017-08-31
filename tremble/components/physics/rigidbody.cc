#include "rigidbody.h"
#include "../../core/scene_graph/scene_graph.h"
#include "../../core/physics/physics_manager.h"
#include "../../core/physics/physics_material.h"

namespace tremble
{
	using namespace physx;

    //------------------------------------------------------------------------------------------------------
	PhysicsMaterial* Rigidbody::GetMaterial()
	{
		PxShape* px_shape;
		PxMaterial* px_material;
		GetPxRigidbodyActor()->getShapes(&px_shape, sizeof(PxShape*));
		px_shape->getMaterials(&px_material, sizeof(PxMaterial*));
		return static_cast<PhysicsMaterial*>(px_material->userData);
	}

    //------------------------------------------------------------------------------------------------------
	PhysicsGeometryHolder Rigidbody::GetGeometry()
	{
		PxShape* shape;
        GetPxRigidbodyActor()->getShapes(&shape, sizeof(PxShape*));
		PxGeometryHolder px_geometry_holder = shape->getGeometry();
		return PhysicsGeometryHolder(px_geometry_holder);
	}

    //------------------------------------------------------------------------------------------------------
	void Rigidbody::SetMaterial(PhysicsMaterial* physics_material)
	{
		PxShape* shape;
        GetPxRigidbodyActor()->getShapes(&shape, sizeof(PxShape*));
		PxMaterial* px_material = physics_material->GetPxMaterial();
		shape->setMaterials(&px_material, 1);
	}

    //------------------------------------------------------------------------------------------------------
	void Rigidbody::SetGeometry(PhysicsGeometry* physics_geometry)
	{
		PxShape* shape;
        GetPxRigidbodyActor()->getShapes(&shape, sizeof(PxShape*));
		shape->setGeometry(*physics_geometry->GetPxGeometry());
	}

    //------------------------------------------------------------------------------------------------------
	void Rigidbody::Shutdown()
	{
		//Get::PhysicsManager()->RemovePxActor(px_rigidbody_);
	}

    //------------------------------------------------------------------------------------------------------
    void Rigidbody::DoCollisionCallbacks(CollisionData collision_data) const
    {
        for each (auto i in collision_callbacks_)
        {
            i(collision_data);
        }
    }

    //------------------------------------------------------------------------------------------------------
    void Rigidbody::AttachScaledShape(PhysicsGeometry* geometry, PhysicsMaterial* material, PhysicsGeometry::ScalingType scaling_type)
    {
        PhysicsGeometry* scaled = geometry->CreateCopyForScaled();
        if (material == nullptr)
        {
            material = Get::PhysicsManager()->GetMaterial("default");
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
        physx::PxShape* px_shape = Get::PhysicsManager()->CreatePxShape(scaled, material);
        AttachShape(px_shape);
        px_shape->release();
    }
}
