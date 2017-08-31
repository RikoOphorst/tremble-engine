#include "peer_factory.h"
#include "peer.h"
#include "../memory/memory_includes.h"
#include "../get.h"
#include <iostream>

namespace tremble
{
	PeerFactory::PeerFactory()
	{
		pf_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(10000);
	}

	PeerFactory::~PeerFactory()
	{
		for each(Peer* p in all_peers_)
		{
			pf_allocator_->Delete(p);
		}

		Get::MemoryManager()->DeleteAllocator(pf_allocator_);
	}

	Peer & PeerFactory::CreatePeer(RakNet::RakNetGUID guid, bool is_host)
	{
		Peer* peer = pf_allocator_->New<Peer>(peer_index_++, guid, is_host);
		all_peers_.push_back(peer);

		if (is_host)
			SetHostingPeer(*peer);

		LogPeers();
		return *peer;
	}

	Peer & PeerFactory::CreatePeer(PeerID server_index, RakNet::RakNetGUID guid, bool is_host)
	{
		Peer* peer = pf_allocator_->New<Peer>(server_index, guid, is_host);
		all_peers_.push_back(peer);

		if (is_host)
			SetHostingPeer(*peer);

		LogPeers();
		return *peer;
	}

	Peer & PeerFactory::CreatePeer(PeerID server_index, bool is_host)
	{
		Peer* peer = pf_allocator_->New<Peer>(server_index, is_host);
		all_peers_.push_back(peer);

		if (is_host)
			SetHostingPeer(*peer);

		LogPeers();
		return *peer;
	}

	void PeerFactory::DestroyPeer(Peer& peer)
	{
		std::vector<Peer*>::iterator it = all_peers_.begin();
		while (it != all_peers_.end())
		{
			if (*it == &peer)
			{
				pf_allocator_->Delete<Peer>((*it));
				all_peers_.erase(it);
				return;
			}
			else
				it++;
		}
		LogPeers();
	}

	void PeerFactory::DestroyPeer(PeerID peer_index)
	{
		auto it = find_if(all_peers_.begin(), all_peers_.end(),
			[=](Peer* p)
		{
			return p->GetPeerIndex() == peer_index;
		});

		if (it != all_peers_.end())
		{
			pf_allocator_->Delete<Peer>((*it));
			all_peers_.erase(it);
		}
		LogPeers();
	}

	Peer * PeerFactory::FindPeer(PeerID peer_index)
	{
		for each(Peer* peer in all_peers_)
		{
			if (peer->GetPeerIndex() == peer_index)
			{
				return peer;
			}
		}

		return nullptr;
	}

	Peer * PeerFactory::FindPeer(RakNet::RakNetGUID guid)
	{
		for each(Peer* peer in all_peers_)
		{
			if (peer->GetGuid() == guid)
			{
				return peer;
			}
		}

		return nullptr;
	}

	void PeerFactory::LogPeers()
	{
		std::cout << "--------------------" << std::endl;
		std::cout << "amount of peers: " << all_peers_.size() << std::endl;
		for each(Peer* peer in all_peers_)
		{
			std::cout << "peer (index: " << peer->GetPeerIndex() << ", guid: " << peer->GetGuid().ToString() << ", is host:" << peer->IsHost() << ")" << std::endl;
		}
	}
}