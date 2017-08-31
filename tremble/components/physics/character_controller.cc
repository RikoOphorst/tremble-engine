#include "character_controller.h"
#include "../../core/scene_graph/scene_graph.h"
#include "../../core/physics/physics_material.h"

namespace tremble
{
	using namespace physx;

	//------------------------------------------------------------------------------------------------------
	void CharacterController::Awake()
	{
		velocity_ = Vector3(0, 0, 0);
		gravity_ = Get::PhysicsManager()->GetGravity().GetY();
		PxCapsuleControllerDesc desc;
		desc.height = 1.0f;
		desc.radius = 0.75f;

		desc.position = PxExtendedVec3(GetNode()->GetPosition().GetX(), GetNode()->GetPosition().GetY(), GetNode()->GetPosition().GetZ());
		
		desc.nonWalkableMode = physx::PxControllerNonWalkableMode::ePREVENT_CLIMBING_AND_FORCE_SLIDING;
		desc.slopeLimit = cos(
			(Scalar(40.0f).ToRadians()));

		is_controllable_ = true;
		
		PhysicsManager* manager = Get::PhysicsManager();

		desc.material = manager->GetMaterial("default")->GetPxMaterial();
		if (desc.isValid())
		{
			px_controller_ = manager->px_controller_manager_->createController(desc);
            px_controller_->getActor()->userData = ((Component*)this);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void CharacterController::Move(const Vector3& movement)
	{
		movements_ += movement;
	}

	//------------------------------------------------------------------------------------------------------
	void CharacterController::ForwardHop(float forward_velocity)
	{
		velocity_ += GetNode()->GetForwardVectorWorld() * forward_velocity;
	}

	//------------------------------------------------------------------------------------------------------
	void CharacterController::DirectionalHop(Vector3 velocity, float strength)
	{
		velocity_ += velocity * Scalar(strength);
	}
	//------------------------------------------------------------------------------------------------------
	void CharacterController::Jump(float velocity)
	{
		velocity_.SetY(velocity);
	}

	//------------------------------------------------------------------------------------------------------
	bool CharacterController::IsOnGround()
	{
		return is_on_ground_;
	}

    //------------------------------------------------------------------------------------------------------
    void CharacterController::UpdateBeforePhysics()
    {
        if (GetNode()->WasMovedByUser())
        {
            px_controller_->setPosition((PxExtendedVec3)GetNode()->GetPosition().ToPxExtendedVec3());
        }
    }

	//------------------------------------------------------------------------------------------------------
	void CharacterController::Update()
	{
		if(is_controllable_)
		{
			velocity_.SetY(velocity_.GetY() + Scalar(gravity_ * Get::DeltaT()));

			//only slow forward velocity if it's greater than 0
			velocity_ *= drag_;

			PxControllerCollisionFlags flags = px_controller_->move(((movements_ + velocity_) * Get::DeltaT()).ToPxVec3(), 0, Get::DeltaT(), PxControllerFilters());


			if (flags == PxControllerCollisionFlag::eCOLLISION_DOWN)
			{
				velocity_ *= 0.0f;
				velocity_.SetY(0);
				is_on_ground_ = true;
			}

			else
			{
				is_on_ground_ = false;
			}
			GetNode()->SetLocalPosition(Vector3(px_controller_->getPosition()));
			movements_ = Vector3(0, 0, 0);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void CharacterController::SetCrouchHeight(float height)
	{
		px_controller_->resize(height);
	}
	//------------------------------------------------------------------------------------------------------
	void CharacterController::SetFreezePosition(bool is_frozen)
	{
		is_controllable_ = !is_frozen;
	}

	//------------------------------------------------------------------------------------------------------
	void CharacterController::ZeroVelocities()
	{
		velocity_ = Vector3(0, 0, 0);
	}
}