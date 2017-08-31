#include "serialization_manager.h"
#include "../get.h"
#include "network_manager.h"
#include "peer_factory.h"
#include "packet_factory.h"
#include "packet_identifiers.h"
#include "packet_handler.h"
#include "serializable_component_types.h"
#include "NetworkIDManager.h"
#include "NetworkIDObject.h"

using namespace tremble;

SerializationManager::SerializationManager()
{
	serializable_components_ = std::vector<Serializable*>();
}

SerializationManager::~SerializationManager()
{
}

void SerializationManager::Register(Serializable & serializable_component)
{
	serializable_components_.push_back(&serializable_component);
}

void SerializationManager::Unregister(Serializable & serializable_component)
{
	serializable_components_.erase(std::find(serializable_components_.begin(), serializable_components_.end(), &serializable_component));
}

void SerializationManager::Deserialize(RakNet::BitStream & serialization_data)
{
	unsigned char serialization_blocks_amount;
	serialization_data.Read(serialization_blocks_amount);
	//std::cout << "Deserializing " << std::to_string(serialization_blocks_amount) << " blocks. Total size: " << serialization_data.GetNumberOfBitsUsed() << " bits. Read offset: " << serialization_data.GetReadOffset() << std::endl;

	for (int i = 0; i < serialization_blocks_amount; i++)
	{
		SerializationPacketBlockHeader serialization_block_header = IPacketHandler::ReadPacketStruct<SerializationPacketBlockHeader>(&serialization_data);

		//std::cout << "Block " << std::to_string(i) << " size: " << std::to_string(serialization_block_header.serialization_block_size) << ", network id: " << std::to_string(serialization_block_header.network_id) << std::endl;

		Serializable* designated_component = Get::NetworkManager()->GetNetworkIdManager().GET_OBJECT_FROM_ID<Serializable*>(serialization_block_header.transform_component_id);

		if (designated_component != nullptr)
		{
			RakNet::BitSize_t offset_before = serialization_data.GetReadOffset();
			designated_component->Deserialize(serialization_data);
			RakNet::BitSize_t offset_after = serialization_data.GetReadOffset();
			assert(offset_after - offset_before == serialization_block_header.serialization_block_size);
		}
		else
		{
			serialization_data.SetReadOffset(serialization_data.GetReadOffset() + serialization_block_header.serialization_block_size);
			std::cout << "Serialization object does not exist, network id: " << serialization_block_header.transform_component_id << std::endl;
		}
	}
}

void SerializationManager::Serialize()
{
	if (!Get::NetworkManager()->IsHost())
	{
		return;
	}

	if (serializable_components_.size() == 0/* || Get::NetworkManager()->GetPeerFactory()->GetAllPeers().size() < 2*/)
	{
		return;
	}
	RakNet::BitStream* serialization_packet = Get::PacketFactory()->CreatePacket(ID_SERIALIZATION_PACKET);

    unsigned char written_elements = 0;

	RakNet::BitStream serializable_blocks;
	for each(Serializable* serializable in serializable_components_)
	{
		RakNet::BitStream serializable_block;

		// Add serialization data from serializable component.
		serializable->Serialize(serializable_block);
		RakNet::BitSize_t number_of_bits_in_block = serializable_block.GetNumberOfBitsUsed();
		
		if (number_of_bits_in_block > 0)
		{
			SerializationPacketBlockHeader block_header;
			block_header.transform_component_id = serializable->GetNetworkID();
			block_header.serialization_block_size = number_of_bits_in_block;

			serializable_blocks.Write(block_header);
			//std::cout << "Wrote serialization block header. network id: " << block_header.network_id << ", number of bits in block: " << block_header.serialization_block_size << std::endl;

			RakNet::BitSize_t bits_before = serializable_blocks.GetNumberOfBitsUsed();
			serializable_blocks.WriteBits(serializable_block.GetData(), serializable_block.GetNumberOfBitsUsed());

			//std::cout << "Number of bits in all blocks before adding this block: " << bits_before << " and after: " << serializable_blocks.GetNumberOfBitsUsed() << std::endl;
            written_elements++;
		}
	}

    serialization_packet->Write(written_elements);
    serialization_packet->WriteBits(serializable_blocks.GetData(), serializable_blocks.GetNumberOfBitsUsed());

	for each(Peer* peer in Get::NetworkManager()->GetPeerFactory()->GetAllPeers())
	{
		if (!peer->IsHost())
		{
			Get::NetworkManager()->SendPacket(peer, PacketReliability::RELIABLE_SEQUENCED, serialization_packet);
		}
	}

	Get::PacketFactory()->DeletePacket(serialization_packet);
}
