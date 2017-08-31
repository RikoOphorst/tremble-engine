#pragma once

#include "../packet_handler.h"

namespace tremble
{

	/**
	* @brief Handler for serialization packets, passes incoming serialization data on to serialization_manager.
	*
	* @author Simon Kok
	*/
	class SerializationPacketHandler : public IPacketHandler
	{
	public:
		SerializationPacketHandler();
		~SerializationPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
	};
}