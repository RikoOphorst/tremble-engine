#pragma once

#include "RakPeerInterface.h"
#include "RakString.h"
#include <string>

namespace tremble
{
	typedef unsigned int PeerID;

	/*
	 * @brief 
	 *
	 * @author Simon Kok
	 */
	class Peer
	{
	public:
		friend class NetworkManager;
		friend class TransformSerializationComponent;
		friend class ConnectivityPacketHandler;

		Peer(PeerID index, RakNet::RakNetGUID guid, bool is_host = false);
		Peer(PeerID index, bool is_host = false);
		~Peer();

		void SetPeerIndex(PeerID peer_id); //!< Set the peer's index.
        PeerID GetPeerIndex(); //!< @return This peer's (server) index.

		RakNet::RakNetGUID& GetGuid();

		bool IsHost();
		bool IsVirtual();
	protected:
	private:
        PeerID peer_index_; //!< The unique index for this peer (this is used for identification by the users instead of the raknet guid, shared over the network).
		RakNet::RakNetGUID guid_; //!< Raknet's unique identifier for this peer.
		bool is_host_; //!< Whether this peer is a host or a client peer.
		bool is_virtual_; //!< Whether this peer is actually connected to us or just our representation of another peer connected to the server. If true this means the peer has no guid.
	};
}