#pragma once

namespace tremble
{
	/**
	 * @brief An enum of the different types of serializable components. 
	 * This is included in every serialization block after the NetworkID,
	 * so that if the client doesn't have a serializable component with
	 * the NetworkID, it knows what kind of object to create from the
	 * object's state snapshot.
	 */
	//enum SerializableComponentTypes
	//{
	//	PLAYER
	//};
}