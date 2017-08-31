#pragma once
namespace tremble
{
	class Rigidbody;
	class Component;

	struct ContactData
	{
		Vector3 position;
		float separation;
		Vector3 normal;
		Vector3 impulse;
	};

	struct CollisionData
	{
		Component* other_component; //!< Other rigidbody or plane, that participates in the collision
		Vector3 impulse; //!< Overall impulse, applied to resolve collision
		const std::vector<ContactData>* contacts; //!< Contacts of this collision
	};

}