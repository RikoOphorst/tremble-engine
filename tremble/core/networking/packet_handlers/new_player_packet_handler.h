#pragma once
#include "../packet_handler.h"
#include "../player_connectivity_data.h"
#include "BitStream.h"

namespace tremble
{
	class Peer;

	/**
	* @brief Handles incoming new peer packets, this packet contains just the peer id and whether it's yours or not.
	*
	* @author Simon Kok
	*/
	class NewPlayerPacketHandler : public IPacketHandler
	{
	public:
		NewPlayerPacketHandler();
		~NewPlayerPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
	};
}