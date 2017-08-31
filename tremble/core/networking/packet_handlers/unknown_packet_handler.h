#pragma once

#include "../packet_handler.h"

namespace tremble
{

	class UnknownPacketHandler : public IPacketHandler
	{
	public:
		UnknownPacketHandler();
		~UnknownPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
	};
}