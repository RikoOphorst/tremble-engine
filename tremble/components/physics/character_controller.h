#pragma once

#include "../../core/scene_graph/component.h"
#include "../../core/game_manager.h"
#include "../../core/scene_graph/component_manager.h"
#include "../../core/physics/physics_manager.h"
#include "../../core/get.h"

namespace tremble
{
	class CharacterController : public Component
	{
	public:
		void Awake(); //!< Creates the character controller
		void Update(); //!< Automatically adds gravity to the made moves, and moves the character
        void UpdateBeforePhysics(); //!< Set position of the character controller if someone moved it
		void Move(const Vector3& movement); //!< Move the character (doesn't actually move it. The movement itself happens in the update). No need to specify gravity
		void Jump(float velocity); //!< Make the character jump in the air(Doesn't check whether it is on the ground)
		void SetGravity(float gravity) { gravity_ = gravity; } //!< Set the gravity of this character
		void ForwardHop(float forward_velocity);
		void DirectionalHop(Vector3 velocity, float strength);
		void SetControl(bool is_controllable) { is_controllable_ = is_controllable; }//!< sets wether the character can be controlled
		bool IsOnGround();
		void SetCrouchHeight(float height);
		void SetFreezePosition(bool is_frozen);
		void ZeroVelocities();
		float GetGravity() { return gravity_; }//!< returns the gravity
	private:
		bool is_on_ground_; //!< Is the character standing on the ground?
		bool is_controllable_;//!< can you control the player? (aka move him)
		float gravity_;
		float drag_ = 1.0f;
		Vector3 movements_; //!< Movements, piled up during the update
		Vector3 position_holder_; //!< used to store position for the movement freeze
		Vector3 velocity_;
		physx::PxController* px_controller_; //!< Pointer to physx character controller
	};
}