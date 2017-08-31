#pragma once
#include "../packet_handler.h"

namespace tremble
{

    /**
    * @brief Handles incoming input packets.
    *
    * @author Anton Gavrilov
    */
    class InputPacketHandler : public IPacketHandler
    {
    public:
        InputPacketHandler();
        ~InputPacketHandler();

        void Handle(int packet_id, RakNet::Packet* packet, Peer* sender) override;
    };
}