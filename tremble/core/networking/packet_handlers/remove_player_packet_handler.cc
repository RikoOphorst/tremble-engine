#include "remove_player_packet_handler.h"
#include "../../get.h"
#include "../network_manager.h"
#include "core/scene_graph/component_manager.h"
#include "../game_data_manager.h"
#include "../peer_factory.h"

namespace tremble
{
	RemovePlayerPacketHandler::RemovePlayerPacketHandler()
	{

	}

	RemovePlayerPacketHandler::~RemovePlayerPacketHandler()
	{

	}

	void RemovePlayerPacketHandler::Handle(int packet_id, RakNet::Packet * packet, Peer * sender)
	{
		if (Get::NetworkManager()->IsHost())
			return;

		RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); //<! Extract packet data as RakNet::BitStream.
		RemovePlayerPacket remove_player_packet = IPacketHandler::ReadPacketStruct<RemovePlayerPacket>(bs); //<! The peer information block for our peer.
		
		Peer* peer_to_remove = Get::NetworkManager()->GetPeerFactory()->FindPeer(remove_player_packet.peer_id);

		if (peer_to_remove != nullptr) //<! Make sure the peer we're trying to remove exists.
		{
			if (Get::NetworkManager()->GetGameData()->HasPlayerData(peer_to_remove->GetPeerIndex()))
			{
				Get::ComponentManager()->OnPlayerDisconnect(Get::NetworkManager()->GetGameData()->GetPlayerData(peer_to_remove->GetPeerIndex()));
				Get::NetworkManager()->GetGameData()->RemovePlayerData(peer_to_remove->GetPeerIndex());
			}
			if (Get::NetworkManager()->HasNetworkEventInterface())
			{
				Get::NetworkManager()->GetNetworkEventInterface()->OnPlayerRemoved(*peer_to_remove);
			}

			Get::NetworkManager()->GetPeerFactory()->DestroyPeer(*peer_to_remove);
		}
	}
}