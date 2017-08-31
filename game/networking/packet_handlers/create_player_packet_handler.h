#pragma once

namespace tremble
{
    /**
    * @brief Handles remote creation of a player object (this packet is only handled by the client,
    * as the clients have no say in when (network) objects are created.
    *
    * @author Simon Kok
    */
    class CreatePlayerPacketHandler : public IPacketHandler
    {
    public:
        CreatePlayerPacketHandler();
        ~CreatePlayerPacketHandler();

        void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
    };
}