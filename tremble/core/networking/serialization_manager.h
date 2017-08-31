#pragma once

#include "serializable.h"
#include <vector>

namespace tremble
{

	class SerializationManager
	{
	public:
		SerializationManager();
		~SerializationManager();
		
		void Register(Serializable& serializable_component);
		void Unregister(Serializable& serializable_component);

		void Deserialize(RakNet::BitStream& serialization_data);
		void Serialize();
	private:
		std::vector<Serializable*> serializable_components_;
	};
}