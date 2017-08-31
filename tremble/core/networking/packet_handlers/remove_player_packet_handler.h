#pragma once
#include "../packet_handler.h"

namespace tremble
{

	/**
	* @brief Handles incoming new peer packets, this packet contains just the peer id and whether it's yours or not.
	*
	* @author Simon Kok
	*/
	class RemovePlayerPacketHandler : public IPacketHandler
	{
	public:
		RemovePlayerPacketHandler();
		~RemovePlayerPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
	};
}