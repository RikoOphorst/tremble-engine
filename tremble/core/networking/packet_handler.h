#pragma once

#include "RakPeerInterface.h"
#include "BitStream.h"

namespace tremble
{
	class Peer;

	/**
	 * @brief The packet handler interface.
	 *
	 * @author Simon Kok
	 */
	class IPacketHandler
	{
	public:
		IPacketHandler();
		virtual ~IPacketHandler() {};

		/**
		 * @brief Handles an incoming packet
		 * @param[in] packet_id The packet id.
		 * @param[in] packet The RakNet::Packet object.
		 * @param[in] sender The peer from which we received this packet.
		 */
		virtual void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) = 0;

		template <typename T>
		static inline T ReadPacketStruct(RakNet::BitStream* packet_data)
		{
			T packet_struct; // Create struct
			packet_data->Read(packet_struct); // Write packet data to struct.
			return packet_struct;
		}
	protected:

		/**
		 * @brief Extracts packet data as a RakNet::BitStream.
		 * @param[in] packet The RakNet::Packet object.
		 * @return A bitstream containing the packet data.
		 */
		static RakNet::BitStream* GetPacketData(RakNet::Packet* packet);
	};
}