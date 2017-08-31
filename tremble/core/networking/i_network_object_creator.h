#pragma once

namespace tremble
{
	class RakNet::BitStream;

	class INetworkObjectCreator
	{
	public:
		virtual void OnNewNetworkObjectReceived(int networked_object_type, PeerID owner, RakNet::BitStream& bit_stream) = 0;
	};
}