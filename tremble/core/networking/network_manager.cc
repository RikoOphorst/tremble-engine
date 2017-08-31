#include "network_manager.h"
#include "../memory/memory_includes.h"
#include "../get.h"
#include "../game_manager.h"
#include "packet_layout_defs.h"
#include "packet_identifiers.h"
#include "packet_handler.h"
#include "packet_receiver.h"
#include "packet_factory.h"
#include "peer_factory.h"
#include "serialization_manager.h"
#include "game_data_manager.h"
#include "i_network_object_creator.h"
#include "NetworkIDManager.h"
#include <iostream>
#include <algorithm>
#include <limits>

using namespace tremble;
using namespace RakNet;


NetworkManager::NetworkManager()
{
	nm_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(10000);
}

NetworkManager::NetworkManager(size_t memory_size)
{
	nm_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(memory_size);
}

NetworkManager::~NetworkManager()
{
	if(is_running_)
		Shutdown();

	Get::MemoryManager()->DeleteAllocator(nm_allocator_);
}

void NetworkManager::Startup(bool is_host, int port, INetworkEventHandler* network_event_handler)
{
	is_host_ = is_host;

	// Initialize rakpeerinterface.
	client_ = RakNet::RakPeerInterface::GetInstance();
	SocketDescriptor sd = is_host ? SocketDescriptor(port, 0) : SocketDescriptor();
	client_->Startup(max_connections_, &sd, 1);

	if (is_host) // Allow incoming connections if we're hosting.
		client_->SetMaximumIncomingConnections(max_connections_);

	// Get guid and system address.
	guid_ = client_->GetGuidFromSystemAddress(UNASSIGNED_SYSTEM_ADDRESS);

	// Start peer factory.
	peer_factory_ = nm_allocator_->New<PeerFactory>();

	// Add our own peer to the peer connections.
	if(is_host)
		peer_factory_->SetMyPeer(peer_factory_->CreatePeer(guid_, true));

	// Set occasional pinging on (for better timestamping accuracy).
	client_->SetOccasionalPing(true);

	// Start the network ID manager.
	network_id_manager_ = nm_allocator_->New<NetworkIDManager>();

	// Start the serialization manager.
	serialization_manager_ = nm_allocator_->New<SerializationManager>();

	// Start the packet receiver.
	packet_receiver_ = nm_allocator_->New<PacketReceiver>();

	// Start the game data manager.
	game_data_manager_ = nm_allocator_->New<GameDataManager>();

	network_event_interface_ = network_event_handler;
	is_running_ = true;
}

RakNet::ConnectionAttemptResult NetworkManager::AttemptConnect(std::string host_address, std::string nickname, INetworkEventHandler * network_event_handler, int port, std::string password)
{
	host_address_ = host_address;
	port_ = port;
	password_ = password;

	if (!is_running_)
	{
		Startup(false, port, network_event_handler);
		GetGameData()->SetTemporaryPlayerData(PlayerData{ std::numeric_limits<PeerID>::max(), nickname });
	}

	return client_->Connect(host_address_.c_str(), port_, 0, 0);
}

void NetworkManager::Host(const HostData & host_data, std::string nickname, INetworkEventHandler * network_event_handler, int max_connections, int port, std::string password)
{
	if (!is_running_)
	{
		max_connections_ = max_connections;
		port_ = port;
		password_ = password;

		Startup(true, port, network_event_handler);
		GetGameData()->SetHostData(host_data);
		GetGameData()->SetTemporaryPlayerData(PlayerData{ 0, nickname }); // Probably unnecessary.
		GetGameData()->AddPlayerData(PlayerData{ 0, nickname });
	}
}

void NetworkManager::Shutdown()
{
	// Set timeout to max 500ms.
	client_->Shutdown(500);
	RakNet::RakPeerInterface::DestroyInstance(client_);

	nm_allocator_->Delete(game_data_manager_);
	nm_allocator_->Delete(serialization_manager_);
	nm_allocator_->Delete(packet_receiver_);
	nm_allocator_->Delete(network_id_manager_);
	nm_allocator_->Delete(peer_factory_);

	is_running_ = false;
}

void NetworkManager::Listen()
{
	if (!is_running_)
		return;

    while (true)
    {
        packet_ = client_->Receive();
        if (packet_ == 0) // Continue game loop if no data received.
            return;

        packet_receiver_->Handle(packet_); // Pass packet on to packet receiver.
        client_->DeallocatePacket(packet_); // Let raknet deallocate the packet.
    }
}

void NetworkManager::SendPacket(RakNet::RakNetGUID recipient, RakNet::BitStream * packet_data)
{
	if (recipient == guid_)
		return;

    int identifier = client_->Send(packet_data, MEDIUM_PRIORITY, RELIABLE_ORDERED, 1, recipient, false);
	ASSERT(identifier != 0);
}

void NetworkManager::SendPacket(Peer* recipient, RakNet::BitStream * packet_data)
{
	if (recipient == nullptr)
		return;
	if (recipient->GetGuid() == guid_)
		return;

	int identifier = client_->Send(packet_data, MEDIUM_PRIORITY, RELIABLE_ORDERED, 1, recipient->GetGuid(), false);
	ASSERT(identifier != 0);
}

void NetworkManager::SendPacket(Peer * recipient, PacketReliability reliability_mode, RakNet::BitStream * packet_data)
{
	if (recipient == nullptr)
		return;
	if (recipient->GetGuid() == guid_)
		return;

	//TODO: make this shit nice.
	int identifier = client_->Send(packet_data, MEDIUM_PRIORITY, reliability_mode, 1, recipient->GetGuid(), false);
	ASSERT(identifier != 0);
}

bool NetworkManager::HasNetworkEventInterface()
{
	return network_event_interface_ != nullptr;
}

bool NetworkManager::HasNetworkObjectCreator()
{
	return network_object_creator_ != nullptr;
}
