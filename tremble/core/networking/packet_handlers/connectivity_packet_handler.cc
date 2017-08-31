#include "connectivity_packet_handler.h"
#include "../core/get.h"
#include "../packet_identifiers.h"
#include "../packet_factory.h"
#include "../network_manager.h"
#include "../i_network_event_handler.h"
#include "../peer.h"
#include "../peer_factory.h"
#include "../game_data_manager.h"
#include "../../config/config_manager.h"
#include "../../input/input_manager.h"
#include <iostream>
#include <string>

using namespace tremble;

ConnectivityPacketHandler::ConnectivityPacketHandler()
{
}

ConnectivityPacketHandler::~ConnectivityPacketHandler()
{
}

void ConnectivityPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
{
	switch (packet_id)
	{
	case ID_CONNECTION_REQUEST_ACCEPTED:
		HandleAcceptedRequest(packet, sender);
		break;
	case ID_CONNECTION_ATTEMPT_FAILED:
		HandleFailedRequest(packet, sender);
		break;
	case ID_ALREADY_CONNECTED:
		HandleAlreadyConnected(packet, sender);
		break;
	case ID_CONNECTION_BANNED:
		HandleConnectionBanned(packet, sender);
		break;
	case ID_INVALID_PASSWORD:
		HandleInvalidPassword(packet, sender);
		break;
	case ID_DISCONNECTION_NOTIFICATION:
		HandleDisconnectNotification(packet, sender);
		break;
	case ID_CONNECTION_LOST:
		HandleConnectionLost(packet, sender);
		break;
	case ID_NEW_INCOMING_CONNECTION:
		HandleNewIncomingConnection(packet, sender);
		break;
	case ID_NO_FREE_INCOMING_CONNECTIONS:
		HandleNoFreeIncomingConnections(packet, sender);
	}
}

void ConnectivityPacketHandler::SendOurPlayerData()
{
	RakNet::BitStream* player_data_packet = Get::PacketFactory()->CreatePacket(ID_PLAYER_DATA_PACKET);

	player_data_packet->Write(RakNet::RakString(Get::NetworkManager()->GetGameData()->GetTemporaryPlayerData().nickname.c_str()));

	Get::NetworkManager()->SendPacket(Get::NetworkManager()->GetPeerFactory()->GetHostingPeer(), PacketReliability::RELIABLE_ORDERED, player_data_packet);
	Get::PacketFactory()->DeletePacket(player_data_packet);
}

void ConnectivityPacketHandler::SendHostData(Peer & peer)
{
	RakNet::BitStream* player_data_packet = Get::PacketFactory()->CreatePacket(ID_HOST_DATA_PACKET);

	Get::NetworkManager()->GetGameData()->GetHostData().Serialize(player_data_packet);

	Get::NetworkManager()->SendPacket(Get::NetworkManager()->GetPeerFactory()->GetHostingPeer(), PacketReliability::RELIABLE_ORDERED, player_data_packet);
	Get::PacketFactory()->DeletePacket(player_data_packet);
}

void ConnectivityPacketHandler::HandleAcceptedRequest(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "Accepted connection request (guid: " << packet->guid.ToString() << ")" << std::endl;
	Peer& p = Get::NetworkManager()->GetPeerFactory()->CreatePeer(0, packet->guid, true);

	SendOurPlayerData();
	Get::InputManager()->SetCanSendInput(true);

	if (Get::NetworkManager()->HasNetworkEventInterface())
	{
		Get::NetworkManager()->GetNetworkEventInterface()->OnConnectedToServer(p);
	}
}

void ConnectivityPacketHandler::HandleFailedRequest(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "Failed connection request (guid: " << packet->guid.ToString() << ")" << std::endl;
	if (Get::NetworkManager()->HasNetworkEventInterface())
		Get::NetworkManager()->GetNetworkEventInterface()->OnConnectionFailed(ATTEMPT_FAILED);
}

void ConnectivityPacketHandler::HandleAlreadyConnected(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "Raknet instance already connected (guid: " << packet->guid.ToString() << ")" << std::endl;
	if (Get::NetworkManager()->HasNetworkEventInterface())
		Get::NetworkManager()->GetNetworkEventInterface()->OnConnectionFailed(ALREADY_CONNECTED);
}

void ConnectivityPacketHandler::HandleConnectionBanned(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "User banned (guid: " << packet->guid.ToString() << ")" << std::endl;
	if (Get::NetworkManager()->HasNetworkEventInterface())
		Get::NetworkManager()->GetNetworkEventInterface()->OnConnectionFailed(CONNECTION_BANNED);
}

void ConnectivityPacketHandler::HandleInvalidPassword(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "Invalid password (guid: " << packet->guid.ToString() << ")" << std::endl;
	if (Get::NetworkManager()->HasNetworkEventInterface())
		Get::NetworkManager()->GetNetworkEventInterface()->OnConnectionFailed(INVALID_PASSWORD);
}

void ConnectivityPacketHandler::HandleDisconnectNotification(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "Disconnect notification (guid: " << packet->guid.ToString() << ")" << std::endl;
	Peer* p = Get::NetworkManager()->GetPeerFactory()->FindPeer(packet->guid);
	Get::InputManager()->SetCanSendInput(false);

	if (p != nullptr)
	{
		if (Get::NetworkManager()->HasNetworkEventInterface())
		{
			Get::NetworkManager()->GetNetworkEventInterface()->OnClientDisconnect(*p);
		}
		Get::NetworkManager()->GetPeerFactory()->DestroyPeer(*p);
	}
}

void ConnectivityPacketHandler::HandleConnectionLost(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "Connection lost (guid: " << packet->guid.ToString() << ")" << std::endl;
	Peer* p = Get::NetworkManager()->GetPeerFactory()->FindPeer(packet->guid);
	Get::InputManager()->SetCanSendInput(true);

	if (p != nullptr)
	{
		if (Get::NetworkManager()->HasNetworkEventInterface())
		{
			Get::NetworkManager()->GetNetworkEventInterface()->OnConnectionLost(*p);
		}
		Get::NetworkManager()->GetPeerFactory()->DestroyPeer(*p);
	}
}

void ConnectivityPacketHandler::HandleNewIncomingConnection(RakNet::Packet * packet, Peer* sender)
{
	if (sender != nullptr)
	{
		std::cout << "Connection from already existing peer (guid: " << packet->guid.ToString() << ")" << std::endl;
	}
	else
	{
		std::cout << "New incoming connection (guid: " << packet->guid.ToString() << ")" << std::endl;
		Peer& p = Get::NetworkManager()->GetPeerFactory()->CreatePeer(packet->guid, false);

		// Send the new peer our host data.
		SendHostData(p);

		if (Get::NetworkManager()->HasNetworkEventInterface())
		{
			Get::NetworkManager()->GetNetworkEventInterface()->OnClientConnected(p);
		}
	}
}

void ConnectivityPacketHandler::HandleNoFreeIncomingConnections(RakNet::Packet * packet, Peer* sender)
{
	std::cout << "No free incoming connection (guid: " << packet->guid.ToString() << ")" << std::endl;
	if (Get::NetworkManager()->HasNetworkEventInterface())
		Get::NetworkManager()->GetNetworkEventInterface()->OnConnectionFailed(HOST_NOT_ACCEPTING_CONNECTIONS);
}
