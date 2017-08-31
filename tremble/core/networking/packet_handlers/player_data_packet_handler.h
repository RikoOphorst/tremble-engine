#pragma once

#include "../packet_handler.h"
#include "../player_connectivity_data.h"

namespace tremble
{

	/**
	* @brief Handles player data packets from the client containing the nickname.
	*
	* @author Simon Kok
	*/
	class PlayerDataPacketHandler : public IPacketHandler
	{
	public:
		PlayerDataPacketHandler();
		~PlayerDataPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;

	private:
		/**
		* Generate and send the player information packet to all peers.
		*/
		void SendPlayerInformation(Peer& new_peer, PlayerData player_data);
	};
}