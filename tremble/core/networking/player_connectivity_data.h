#pragma once
#include "peer.h"
#include "BitStream.h"
#include <string>

namespace tremble
{

    struct PlayerData
    {
        void Serialize(RakNet::BitStream* bit_stream) const;
        void Deserialize(RakNet::BitStream* bit_stream);
        bool operator== (const PlayerData& other)
        {
            return other.nickname == nickname && other.peer_id == peer_id;
        }
        //Peer* peer = nullptr; //!< Peer, associated with player
		PeerID peer_id;
        std::string nickname; //!< Player's nickname
    };

    struct HostData
    {
        void Serialize(RakNet::BitStream* bit_stream) const;
        void Deserialize(RakNet::BitStream* bit_stream);
        ~HostData() {};
        std::string loaded_map;
    };
}