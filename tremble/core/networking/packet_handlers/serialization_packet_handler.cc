#include "serialization_packet_handler.h"
#include "../packet_identifiers.h"
#include "../packet_layout_defs.h"
#include "../network_manager.h"
#include "../serialization_manager.h"

#include "../core/get.h"

namespace tremble
{
	SerializationPacketHandler::SerializationPacketHandler()
	{
	}

	SerializationPacketHandler::~SerializationPacketHandler()
	{
	}

	void SerializationPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
	{
		if (Get::NetworkManager()->IsHost())
			return;

		RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); // Extract packet data as RakNet::BitStream.

		Get::NetworkManager()->GetSerializationManager().Deserialize(*bs);

		delete bs;
	}
}