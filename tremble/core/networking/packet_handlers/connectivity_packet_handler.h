#pragma once

#include "../packet_handler.h"
#include "../player_connectivity_data.h"

namespace tremble
{
	class NetworkManager;

	/**
	 * @brief Handles incoming packets related to connectivity.
	 * 
	 * @author Simon Kok
	 */
	class ConnectivityPacketHandler : public IPacketHandler
	{
	public:
		ConnectivityPacketHandler();
		~ConnectivityPacketHandler();

		void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;

	private:

		/**
		 * Send our nickname to the host.
		 */
		void SendOurPlayerData();

		/**
		* Send host data to a peer.
		*/
		void SendHostData(Peer& peer);

		/**
		 * Called when our connection request has been accepted.
		 */
		void HandleAcceptedRequest(RakNet::Packet* packet, Peer* sender);
		/**
		 * Called when the connection attempt has failed.
		 */
		void HandleFailedRequest(RakNet::Packet* packet, Peer* sender);
		/**
		 * Called when we're trying to connect to a peer we're already connected to.
		 */
		void HandleAlreadyConnected(RakNet::Packet* packet, Peer* sender);
		/**
		 * Called when we've been banned by the host.
		 */
		void HandleConnectionBanned(RakNet::Packet* packet, Peer* sender);
		/**
		 * Called when the password we specified doesn't match the host's.
		 */
		void HandleInvalidPassword(RakNet::Packet* packet, Peer* sender);
		/**
		 * Called when a peer terminates the connection.
		 */
		void HandleDisconnectNotification(RakNet::Packet* packet, Peer* sender);
		/**
		 * Called when the connection is unexpectedly lost.
		 */
		void HandleConnectionLost(RakNet::Packet* packet, Peer* sender);
		/**
		 * Called in the host when a client tries to connect with us.
		 */
		void HandleNewIncomingConnection(RakNet::Packet* packet, Peer* sender);
		/**
		 * This is called when the peer we're trying to connect to has hit the maximum 
		 * amount of connections that they specified in the NetworkController constructor.
		 */
		void HandleNoFreeIncomingConnections(RakNet::Packet* packet, Peer* sender);
	};
}