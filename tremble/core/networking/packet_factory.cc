#include "packet_factory.h"
#include "../get.h"
#include "../memory/memory_includes.h"
#include "packet_identifiers.h"

using namespace tremble;

PacketFactory::PacketFactory(size_t memory_size)
{
	pf_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(memory_size);
}

PacketFactory::~PacketFactory()
{
	Get::MemoryManager()->DeleteAllocator(pf_allocator_);
}

RakNet::BitStream* PacketFactory::CreatePacket(int packet_id, void* packet_contents, uint32_t content_size_bits) //TODO: this doens't seem to work
{
	RakNet::BitStream* packet_data = new RakNet::BitStream();//pf_allocator_->New<RakNet::BitStream>(); // Create a new bitstream.
	
	packet_data->Write((RakNet::MessageID) packet_id); // Write the packet_id.
	
	if (packet_contents != nullptr)
	{
		//packet_data->Write(packet_contents, size); // Write the packet content.
		packet_data->WriteBits((unsigned char*) packet_contents, content_size_bits);
	}

	return packet_data;
}

RakNet::BitStream * PacketFactory::CreatePacket(void * packet_contents, uint32_t content_size_bits)
{
	RakNet::BitStream* packet_data = new RakNet::BitStream();

	if (packet_contents != nullptr)
	{
		packet_data->WriteBits((unsigned char*)packet_contents, content_size_bits);
	}

	return packet_data;
}

void PacketFactory::DeletePacket(RakNet::BitStream* packet)
{
	delete packet;
	//pf_allocator_->Delete(packet);
}