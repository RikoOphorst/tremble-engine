#include "peer.h"

namespace tremble
{
	Peer::Peer(PeerID index, RakNet::RakNetGUID rakNetGuid, bool is_host)
		: peer_index_(index)
		, guid_(rakNetGuid)
		, is_host_(is_host)
		, is_virtual_(false)
	{
	}

	Peer::Peer(PeerID index, bool is_host)
		: peer_index_(index)
		, is_host_(is_host)
		, is_virtual_(true)
	{
	}

	Peer::~Peer()
	{
	}

	void Peer::SetPeerIndex(PeerID peer_id)
	{
		peer_index_ = peer_id;
	}

	PeerID Peer::GetPeerIndex()
	{
		return peer_index_;
	}

	RakNet::RakNetGUID& Peer::GetGuid()
	{
		return guid_;
	}

	bool Peer::IsHost()
	{
		return is_host_;
	}

	bool Peer::IsVirtual()
	{
		return is_virtual_;
	}
}