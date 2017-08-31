#pragma once
#include "../packet_handler.h"
#include "../player_connectivity_data.h"

namespace tremble
{

	/**
	* @brief Handles incoming host data packets, contains information about the host (current map)
	*
	* @author Simon Kok
	*/
	class HostDataPacketHandler : public IPacketHandler
	{
	public:
		HostDataPacketHandler();
		~HostDataPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
	};
}