#include "create_object_packet_handler.h"
#include "../../get.h"
#include "../network_manager.h"
#include "../i_network_object_creator.h"

namespace tremble
{
	CreateObjectPacketHandler::CreateObjectPacketHandler()
	{

	}

	CreateObjectPacketHandler::~CreateObjectPacketHandler()
	{

	}

	void CreateObjectPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
	{
		if (Get::NetworkManager()->IsHost())
		{
			return;
		}

		RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); //<! Extract packet data as RakNet::BitStream.
		ObjectCreationPacketHeader create_object_header = IPacketHandler::ReadPacketStruct<ObjectCreationPacketHeader>(bs); //<! The peer information block for our peer.

		if (Get::NetworkManager()->HasNetworkObjectCreator())
		{
			Get::NetworkManager()->GetNetworkObjectCreator()->OnNewNetworkObjectReceived(create_object_header.networked_object_type, create_object_header.owner_peer_id, *bs);
		}

		delete bs;
	}
}