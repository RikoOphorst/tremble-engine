#include "game_data_packet_handler.h"
#include "../../get.h"
#include "../network_manager.h"
#include "core/scene_graph/component_manager.h"
#include "../game_data_manager.h"
#include "../peer.h"
#include "../peer_factory.h"
#include "../player_connectivity_data.h"

namespace tremble
{
	GameDataPacketHandler::GameDataPacketHandler()
	{

	}

	GameDataPacketHandler::~GameDataPacketHandler()
	{

	}

	void GameDataPacketHandler::Handle(int packet_id, RakNet::Packet * packet, Peer * sender)
	{
		if (Get::NetworkManager()->IsHost())
			return;

		RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); //<! Extract packet data as RakNet::BitStream.
		
		//HostData host_data;
		RakNet::RakString loaded_map;
		size_t amount_of_players;
		PlayerData our_player_data;
		RakNet::RakString our_nickname;

		//host_data.Deserialize(bs);
		bs->Read(loaded_map);
		bs->Read(amount_of_players);
		bs->Read(our_player_data.peer_id);
		bs->Read(our_nickname);
		our_player_data.nickname = our_nickname.C_String();
		//our_player_data.Deserialize(bs); //<! Read our player data from the bitstream.

		std::cout << "Received player " << our_player_data.peer_id << ": " << our_player_data.nickname << std::endl;
		std::cout << "Loaded map: " << loaded_map.C_String() << std::endl;

		Get::NetworkManager()->GetPeerFactory()->SetMyPeer(Get::NetworkManager()->GetPeerFactory()->CreatePeer(our_player_data.peer_id, Get::NetworkManager()->GetGuid(), false));
		Get::NetworkManager()->GetGameData()->AddPlayerData(our_player_data);

		if (amount_of_players > 1)
		{
			for (int i = 0; i < amount_of_players - 1; i++) //<! -1 because we've already read the first entry, which was our own player data.
			{
				PlayerData player_data;
				RakNet::RakString nickname;

				bs->Read(player_data.peer_id);
				bs->Read(nickname);
				player_data.nickname = nickname.C_String();
				//player_data.Deserialize(bs); //<! Read the player data from the bitstream.
				
				std::cout << "Received player " << player_data.peer_id << ": " << player_data.nickname << std::endl;

				Peer* peer = Get::NetworkManager()->GetPeerFactory()->FindPeer(player_data.peer_id);
				if (peer == nullptr) //<! Make sure we're not creating a duplicate peer.
				{
					Get::NetworkManager()->GetPeerFactory()->CreatePeer(player_data.peer_id);
					Get::NetworkManager()->GetGameData()->AddPlayerData(player_data);
				}
				else
				{
					Get::NetworkManager()->GetGameData()->AddPlayerData(player_data);
				}

				Get::ComponentManager()->OnPlayerConnect(player_data);
				if (Get::NetworkManager()->HasNetworkEventInterface())
				{
					Get::NetworkManager()->GetNetworkEventInterface()->OnPlayerAdded(player_data);
				}
			}
		}

		if (Get::NetworkManager()->HasNetworkEventInterface())
		{
			Get::NetworkManager()->GetNetworkEventInterface()->OnGameInformationReceived();
			Get::NetworkManager()->GetNetworkEventInterface()->OnHostDataReceived(HostData{ loaded_map.C_String() });
		}

		delete bs;
	}
}