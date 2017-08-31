#pragma once

namespace tremble
{
    /**
    * @brief This packet is used by a client to notify the host when a client is ready to start.
    *
    * @author Simon Kok
    */
    class PlayerReadyPacketHandler : public IPacketHandler
    {
    public:
        PlayerReadyPacketHandler();
        ~PlayerReadyPacketHandler();

        void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
    };
}