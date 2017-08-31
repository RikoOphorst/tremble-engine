#pragma once

#include "NetworkIDObject.h"

namespace tremble
{
	class Serializable : public RakNet::NetworkIDObject
	{
	public:
		friend class SerializationManager;

		Serializable();
		~Serializable();

		virtual void Serialize(RakNet::BitStream& packet) = 0;
		virtual void Deserialize(RakNet::BitStream& packet) = 0;
	};
}