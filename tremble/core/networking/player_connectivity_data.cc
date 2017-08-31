#include "player_connectivity_data.h"
#include "RakString.h"

namespace tremble
{

    void HostData::Serialize(RakNet::BitStream* bit_stream) const
    {
		bit_stream->Write(RakNet::RakString(loaded_map.c_str()));
    }

    void HostData::Deserialize(RakNet::BitStream* bit_stream)
    {
		RakNet::RakString loaded_map;
		bit_stream->Read(loaded_map);
    }

    void PlayerData::Serialize(RakNet::BitStream* bit_stream) const
    {
		bit_stream->Write(peer_id);
		bit_stream->Write(RakNet::RakString(nickname.c_str()));
    }

    void PlayerData::Deserialize(RakNet::BitStream* bit_stream)
    {
		PeerID peer_id;
		RakNet::RakString nickname;

		bit_stream->Read(peer_id);
		bit_stream->Read(nickname);
    }
}