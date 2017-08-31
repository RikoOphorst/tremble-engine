#pragma once

#include <map>
#include "RakPeerInterface.h"

namespace tremble
{
	class FreeListAllocator;
	class IPacketHandler;

	/**
	 * The packet receiver makes sure all incoming packets are handled by the correct packet handler.
	 *
	 * @author Simon Kok
	 */
	class PacketReceiver
	{
	public:
		PacketReceiver();
		~PacketReceiver();

		/**
		 * @brief Map a packet handler to a certain packet identifier.
		 * @param[in] packet_id The packet identifier associated with this handler.
		 */
		void AddPacketHandler(int packet_id, IPacketHandler* handler);

		void Handle(RakNet::Packet* packet);

		static unsigned char GetPacketIdentifier(RakNet::Packet* p); //!< Taken from http://www.raknet.net/raknet/manual/receivingpackets.html

	protected:
		FreeListAllocator* pr_allocator_; //!< Pointer to the packetreceiver's allocator.

	private:
		std::unordered_map<int, IPacketHandler*> registered_packets_; //!< Packet handlers mapped to their designated packet identifier.
        std::vector<IPacketHandler*> packet_handlers_;
		void CreatePacketHandlers();
	};
}