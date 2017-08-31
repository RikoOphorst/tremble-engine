#pragma once
#include "game_packet_defs.h"

namespace tremble
{
    class Serializable;
    class BaseGun;

    class NetworkObjectCreator : public INetworkObjectCreator
    {
    public:
        void OnNewNetworkObjectReceived(int networked_object_type, PeerID owner, RakNet::BitStream& bit_stream) override;

        void CreateScoreSystem();
        void CreateScoreSystem(PeerID owner, ScoreSystemCreationPacket& packet_data);
        void SendScoreSystemCreationPacket(ScoreSystemCreationPacket& packet_data);

        void CreateOurPlayer(PlayerCreationPacketData& packet_data);
        void CreateOtherPlayer(PeerID owner, PlayerCreationPacketData& packet_data);
        void SendPlayerCreationPacket(PeerID owner, Serializable* transform_component, Serializable* health_component, std::vector<BaseGun*> base_guns);

        void CreateNewPlayerAsHost(PeerID owner);
        void CreateNewPlayer(PeerID owner, RakNet::BitStream& bit_stream);
    };
}