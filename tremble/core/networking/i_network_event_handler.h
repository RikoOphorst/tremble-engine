#pragma once

#include "RakPeerInterface.h"
#include "packet_layout_defs.h"
#include "player_connectivity_data.h"

namespace tremble
{
	class Peer;

	/*
	* @brief Possible reasons a connection attempt has failed.
	*
	* @author Simon Kok
	*/
	enum ConnectionFailedReason
	{
		ATTEMPT_FAILED,
		ALREADY_CONNECTED,
		HOST_NOT_ACCEPTING_CONNECTIONS,
		CONNECTION_BANNED,
		INVALID_PASSWORD,
	};

	/*
	* @brief Event functions related to networked connection to other peers.
	*
	* You can implement this to do something when a connection event happens,
	* These events are also handled inside the networking api, for managing
	* connected peers etc.
	*
	* @author Simon Kok
	*/
	class INetworkEventHandler
	{
	public:
		virtual void OnHostingStarted() = 0;
		virtual void OnConnectedToServer(Peer& server_peer) = 0;
		virtual void OnConnectionFailed(ConnectionFailedReason reason) = 0;
		virtual void OnClientConnected(Peer& client_peer) = 0;
		virtual void OnClientDisconnect(Peer& client_peer) = 0;
		virtual void OnConnectionLost(Peer& peer) = 0;
		virtual void OnPlayerAdded(PlayerData& player_data) = 0;
		virtual void OnPlayerRemoved(Peer& peer) = 0;
		virtual void OnGameInformationReceived() = 0;
		virtual void OnHostDataReceived(HostData& host_data) = 0;
	};
}