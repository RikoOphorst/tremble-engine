#include "player_data_packet_handler.h"
#include "../../get.h"
#include "../network_manager.h"
#include "../game_data_manager.h"
#include "../peer_factory.h"
#include "../packet_factory.h"
#include "../packet_identifiers.h"
#include "../peer.h"
#include "../player_connectivity_data.h"

using namespace tremble;

PlayerDataPacketHandler::PlayerDataPacketHandler()
{
}

PlayerDataPacketHandler::~PlayerDataPacketHandler()
{
}

void PlayerDataPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
{
	if (sender == nullptr || !Get::NetworkManager()->IsHost())
	{
		return;
	}

	RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); // Extract packet data as RakNet::BitStream.
	RakNet::RakString nickname;
	bs->Read(nickname);
	delete bs;

	PlayerData player_data = { sender->GetPeerIndex(), nickname };
	Get::NetworkManager()->GetGameData()->AddPlayerData(player_data);

	SendPlayerInformation(*sender, player_data);

	if (Get::NetworkManager()->HasNetworkEventInterface())
	{
		Get::NetworkManager()->GetNetworkEventInterface()->OnPlayerAdded(player_data);
	}
}

void PlayerDataPacketHandler::SendPlayerInformation(Peer& new_peer, PlayerData player_data)
{
	// Used to tell a new peer about the map to load and already existing peers (including, and first, itself).
	RakNet::BitStream* game_data_packet = Get::PacketFactory()->CreatePacket(ID_PLAYER_INFORMATION_PACKET);
	// Used to notify the other peers of the new peer.
	RakNet::BitStream* new_player_packet = Get::PacketFactory()->CreatePacket(ID_NEW_PLAYER_PACKET);

	player_data.Serialize(new_player_packet); //Write the new player data to the packet meant for the already existing peers.

	//HostData host_data = Get::NetworkManager()->GetGameData()->GetHostData();
	RakNet::RakString loaded_map = RakNet::RakString(Get::NetworkManager()->GetGameData()->GetHostData().loaded_map.c_str());

	size_t amount_of_players = Get::NetworkManager()->GetGameData()->GetAllPlayerData().size();

	game_data_packet->Write(loaded_map);
	//host_data.Serialize(game_data_packet); // Write the host data (loaded map) to the game data packet.
	game_data_packet->Write(amount_of_players); //Write the amount of peers contained in the game data packet.

	std::cout << "Writing player data (" << std::to_string(player_data.peer_id) << ": " << player_data.nickname.c_str() << std::endl;
	player_data.Serialize(game_data_packet); //Write the players own player data to the game data packet.

	for each(PlayerData other_player_data in Get::NetworkManager()->GetGameData()->GetAllPlayerData())
	{
		if (other_player_data.peer_id != player_data.peer_id) //TODO: is this right?
		{
			Peer* associated_peer = Get::NetworkManager()->GetPeerFactory()->FindPeer(other_player_data.peer_id);

			if (associated_peer != nullptr)
			{
				std::cout << "Writing player data (" << std::to_string(other_player_data.peer_id) << ": " << other_player_data.nickname.c_str() << std::endl;
				player_data.Serialize(game_data_packet); // Write this existing players data to the game data packet.

				// Send this existing player a new player packet.
				if(!associated_peer->IsHost()) {
					Get::NetworkManager()->SendPacket(associated_peer, PacketReliability::RELIABLE_ORDERED, new_player_packet);
					std::cout << "Sent new player packet to peer " << other_player_data.peer_id << std::endl;
				}
			}
		}
	}

	Get::NetworkManager()->SendPacket(&new_peer, game_data_packet); //Send the new peer the peer information packet.
	std::cout << "Sent game information packet to peer " << new_peer.GetPeerIndex() << std::endl;

	Get::PacketFactory()->DeletePacket(game_data_packet);
	Get::PacketFactory()->DeletePacket(new_player_packet);
}