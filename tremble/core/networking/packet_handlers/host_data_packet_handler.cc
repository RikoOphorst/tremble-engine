#include "host_data_packet_handler.h"
#include "../../resources/scene_loader.h"
#include "../../get.h"
#include "../network_manager.h"
#include "../game_data_manager.h"
#include "../i_network_event_handler.h"

namespace tremble
{
	HostDataPacketHandler::HostDataPacketHandler()
	{
	}

	HostDataPacketHandler::~HostDataPacketHandler()
	{
	}

	void HostDataPacketHandler::Handle(int packet_id, RakNet::Packet * packet, Peer * sender)
	{
		RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet);
		RakNet::RakString loaded_map;
		HostData host_data;
		bs->Read(loaded_map); //host_data.Deserialize(bs);

		host_data.loaded_map = loaded_map.C_String();
		Get::NetworkManager()->GetGameData()->SetHostData(host_data);
		
		delete bs;

		if (Get::NetworkManager()->HasNetworkEventInterface())
		{
			Get::NetworkManager()->GetNetworkEventInterface()->OnHostDataReceived(host_data);
		}
	}
}