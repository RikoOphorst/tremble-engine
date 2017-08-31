#pragma once

#include "BitStream.h"

namespace tremble
{
	class FreeListAllocator;

	/**
	 * @brief Saves you some code when creating a packet.
	 * 
	 * @author Simon Kok
	 */
	class PacketFactory
	{
	public:
		PacketFactory(size_t memory_size);
		~PacketFactory();

		/**
		 * @brief Creates a packet from a struct.
		 *
		 * Writes the packet_id to a RakNet::BitStream and then writes the data
		 * at the void pointer to the RakNet::BitStream aswell.
		 *
		 * @param[in] packet_id The packet identifier (-1 for packet without an identifier).
		 * @param[in] packet_contents A pointer to the packet data to be written to the BitStream.
		 * @return the created RakNet::BitStream.
		 */
		static RakNet::BitStream* CreatePacket(int packet_id, void* packet_contents = nullptr, uint32_t content_size_bits = 0);

		static RakNet::BitStream* CreatePacket(void* packet_contents = nullptr, uint32_t content_size_bits = 0);

		/**
		 * @brief Deallocate packet data.
		 *
		 * @param[in] packet Pointer to the BitStream object to delete.
		 */
		void DeletePacket(RakNet::BitStream* packet);

		/**
		* @brief Creates a packet from a struct
		*
		* Writes the packet_id to a RakNet::BitStream and then writes the contents
		* of the struct to the RakNet::BitStream aswell.

		* @param[in] packet_id The packet identifier.
		* @param[in] packet_struct The struct containing the packet data.
		* @return the created RakNet::BitStream
		*/
		template <typename T>
		static inline RakNet::BitStream* CreatePacket(int packet_id, T packet_struct) {
			RakNet::BitStream* packet_data = new RakNet::BitStream(); // Create a new bitstream.
			packet_data->Write((RakNet::MessageID) packet_id); // Write the packet_id.
			packet_data->Write(packet_struct); // Write the contents of the struct.

			return packet_data;
		}

	protected:
		FreeListAllocator* pf_allocator_;
	};
}