#include "new_player_packet_handler.h"
#include "../../get.h"
#include "../network_manager.h"
#include "core/scene_graph/component_manager.h"
#include "../game_data_manager.h"
#include "../peer_factory.h"

namespace tremble
{
	NewPlayerPacketHandler::NewPlayerPacketHandler()
	{

	}

	NewPlayerPacketHandler::~NewPlayerPacketHandler()
	{

	}

	void NewPlayerPacketHandler::Handle(int packet_id, RakNet::Packet * packet, Peer * sender)
	{
		std::cout << "New player packet received." << std::endl;
		if (Get::NetworkManager()->IsHost())
			return;

		RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); //<! Extract packet data as RakNet::BitStream.

		PlayerData player_data;
		player_data.Deserialize(bs); // Read player data from bitstream.

		Get::NetworkManager()->GetGameData()->AddPlayerData(player_data);

		if (Get::NetworkManager()->GetPeerFactory()->FindPeer(player_data.peer_id) == nullptr) //<! Make sure we're not creating a duplicate peer.
		{
			Peer& new_peer = Get::NetworkManager()->GetPeerFactory()->CreatePeer(player_data.peer_id);

			Get::ComponentManager()->OnPlayerConnect(player_data);
			if (Get::NetworkManager()->HasNetworkEventInterface())
			{
				Get::NetworkManager()->GetNetworkEventInterface()->OnPlayerAdded(player_data);
			}
		}

		delete bs;
	}
}