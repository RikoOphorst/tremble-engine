#include "packet_receiver.h"
#include "../memory/memory_includes.h"
#include "../core/get.h"
#include "network_manager.h"
#include "peer_factory.h"
#include "packet_handler.h"
#include "packet_handler_includes.h"
#include "packet_layout_defs.h"
#include "packet_identifiers.h"

using namespace tremble;

PacketReceiver::PacketReceiver()
{
	pr_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(10000);
	CreatePacketHandlers();
}

PacketReceiver::~PacketReceiver()
{
	std::vector<IPacketHandler*>::iterator it;
	for (it = packet_handlers_.begin(); it != packet_handlers_.end(); it++)
	{
		pr_allocator_->Delete(*it);
	}
	
	Get::MemoryManager()->DeleteAllocator(pr_allocator_);
}

void PacketReceiver::CreatePacketHandlers()
{
    packet_handlers_.reserve(10);
    //Unknown packet handler
    UnknownPacketHandler* unknown_packet_handler = pr_allocator_->New<UnknownPacketHandler>();
    packet_handlers_.push_back(unknown_packet_handler);
    registered_packets_.insert({ ID_UNKNOWN_USER_PACKET, unknown_packet_handler });
    //Connectivity packet handler
	ConnectivityPacketHandler* connectivity_packet_handler = pr_allocator_->New<ConnectivityPacketHandler>();
    packet_handlers_.push_back(connectivity_packet_handler);
	registered_packets_.insert({ ID_CONNECTION_REQUEST_ACCEPTED, connectivity_packet_handler });
	registered_packets_.insert({ ID_CONNECTION_ATTEMPT_FAILED, connectivity_packet_handler });
	registered_packets_.insert({ ID_ALREADY_CONNECTED, connectivity_packet_handler });
	registered_packets_.insert({ ID_CONNECTION_BANNED, connectivity_packet_handler });
	registered_packets_.insert({ ID_INVALID_PASSWORD, connectivity_packet_handler });
	registered_packets_.insert({ ID_DISCONNECTION_NOTIFICATION, connectivity_packet_handler });
	registered_packets_.insert({ ID_CONNECTION_LOST, connectivity_packet_handler });
	registered_packets_.insert({ ID_NEW_INCOMING_CONNECTION, connectivity_packet_handler });
	registered_packets_.insert({ ID_NO_FREE_INCOMING_CONNECTIONS, connectivity_packet_handler });
    //Ping packet handler
    PingPacketHandler* ping_packet_handler = pr_allocator_->New<PingPacketHandler>();
    packet_handlers_.push_back(ping_packet_handler);
	registered_packets_.insert({ ID_CONNECTED_PING, ping_packet_handler });
    //Test packet handler
    TestPacketHandler* test_packet_handler = pr_allocator_->New<TestPacketHandler>();
    packet_handlers_.push_back(test_packet_handler);
    registered_packets_.insert({ ID_TEST_PACKET, test_packet_handler });
    //Serialization packet handler (components syncronization) (hosts only)
    SerializationPacketHandler* serialization_packet_handler = pr_allocator_->New<SerializationPacketHandler>();
    packet_handlers_.push_back(serialization_packet_handler);
    registered_packets_.insert({ ID_SERIALIZATION_PACKET, serialization_packet_handler });
    //Input packet handler (hosts only)
    InputPacketHandler* input_packet_handler = pr_allocator_->New<InputPacketHandler>();
    packet_handlers_.push_back(input_packet_handler);
    registered_packets_.insert({ ID_INPUT_PACKET, input_packet_handler });
	//Player data packet handler (hosts only)
	PlayerDataPacketHandler* player_data_packet_handler = pr_allocator_->New<PlayerDataPacketHandler>();
	packet_handlers_.push_back(player_data_packet_handler);
	registered_packets_.insert({ ID_PLAYER_DATA_PACKET, player_data_packet_handler });
	//Host data packet handler (clients only)
	HostDataPacketHandler* host_data_packet_handler = pr_allocator_->New<HostDataPacketHandler>();
	packet_handlers_.push_back(host_data_packet_handler);
	registered_packets_.insert({ ID_HOST_DATA_PACKET, host_data_packet_handler });
	//New player packet handler (clients only)
	NewPlayerPacketHandler* new_peer_packet_handler = pr_allocator_->New<NewPlayerPacketHandler>();
	packet_handlers_.push_back(new_peer_packet_handler);
	registered_packets_.insert({ ID_NEW_PLAYER_PACKET, new_peer_packet_handler });
	//Remove player packet handler (clients only)
	RemovePlayerPacketHandler* remove_peer_packet_handler = pr_allocator_->New<RemovePlayerPacketHandler>();
	packet_handlers_.push_back(remove_peer_packet_handler);
	registered_packets_.insert({ ID_REMOVE_PLAYER_PACKET, remove_peer_packet_handler });
	//Player information packet handler (clients only)
	GameDataPacketHandler* peer_information_packet_handler = pr_allocator_->New<GameDataPacketHandler>();
	packet_handlers_.push_back(peer_information_packet_handler);
	registered_packets_.insert({ ID_PLAYER_INFORMATION_PACKET, peer_information_packet_handler });
	//Create object packet handler (clients only, for now)
	CreateObjectPacketHandler* create_object_packet_handler = pr_allocator_->New<CreateObjectPacketHandler>();
	packet_handlers_.push_back(create_object_packet_handler);
	registered_packets_.insert({ ID_CREATE_OBJECT_PACKET, create_object_packet_handler });
}

void PacketReceiver::AddPacketHandler(int packet_id, IPacketHandler* handler)
{
	if(packet_id != ID_UNKNOWN_USER_PACKET) // Not allowed to add a custom handler for.
		registered_packets_.insert({ packet_id, handler });
}

void PacketReceiver::Handle(RakNet::Packet* packet)
{
	// Read the packet id.
	unsigned char packet_id = GetPacketIdentifier(packet);
	// Identify the sender.
	Peer* sender = Get::NetworkManager()->GetPeerFactory()->FindPeer(packet->guid);

	// Pass on to the handler(s) associated with this packet id.
	std::unordered_map<int, IPacketHandler*>::iterator it = registered_packets_.find(packet_id);

	if (it == registered_packets_.end())
	{
		// Pass on to the unknown packet handler.
		std::unordered_map<int, IPacketHandler*>::iterator unknown_it = registered_packets_.find(ID_UNKNOWN_USER_PACKET);
		unknown_it->second->Handle(packet_id, packet, sender);
	}
	else
	{
		// Pass on the designated packet handler.
		it->second->Handle(packet_id, packet, sender);
	}
}

// Taken from http://www.raknet.net/raknet/manual/receivingpackets.html
unsigned char PacketReceiver::GetPacketIdentifier(RakNet::Packet * packet)
{
	if (packet == 0)
		return 255;

	if ((unsigned char)packet->data[0] == ID_TIMESTAMP)
	{
		RakAssert(packet->length > sizeof(RakNet::MessageID) + sizeof(RakNet::Time));
		return (unsigned char)packet->data[sizeof(RakNet::MessageID) + sizeof(RakNet::Time)];
	}
	else
		return (unsigned char)packet->data[0];
}
