#pragma once
#include "../packet_handler.h"

namespace tremble
{
	class Peer;

	/**
	* @brief Handles incoming game data packets, the game data packet contains
	* the map to load, and the player data for all existing players, including
	* yourself.
	*
	* @author Simon Kok
	*/
	class GameDataPacketHandler : public IPacketHandler
	{
	public:
		GameDataPacketHandler();
		~GameDataPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;

	private:

	};
}