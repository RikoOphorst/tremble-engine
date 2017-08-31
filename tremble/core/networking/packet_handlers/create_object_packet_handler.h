#pragma once
#include "../packet_handler.h"

namespace tremble
{

	/**
	* @brief Handles incoming 'create object' packets.
	*
	* @author Simon Kok
	*/
	class CreateObjectPacketHandler : public IPacketHandler
	{
	public:
		CreateObjectPacketHandler();
		~CreateObjectPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
	};
}