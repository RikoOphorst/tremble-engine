#pragma once

#include "../packet_handler.h"

namespace tremble
{

	/**
	 * @brief Handles some custom incoming packet.
	 *
	 * @author Simon Kok
	 */
	class TestPacketHandler : public IPacketHandler
	{
	public:
		TestPacketHandler();
		~TestPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
	};
}