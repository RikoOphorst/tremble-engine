#include "input_packet_handler.h"
#include "../../input/input_manager.h"
#include "../../get.h"

namespace tremble
{
    InputPacketHandler::InputPacketHandler()
    {

    }

    InputPacketHandler::~InputPacketHandler()
    {

    }

    void InputPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
    {
        RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); // Extract packet data as RakNet::BitStream.
        InputState received_input(*bs);
        //std::cout << "Input received " << received_input.key_states.size() << std::endl;
        //for each (std::pair<KEY_TYPE, KeyState> i in received_input.key_states)
        //{
            //std::cout << i.first << " down " << i.second.down << std::endl;
        //}
        //std::cout << std::endl;
        Get::InputManager()->StoreInput(sender->GetPeerIndex(), received_input);
    }
}