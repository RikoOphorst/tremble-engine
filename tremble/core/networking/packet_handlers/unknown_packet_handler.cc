#include "unknown_packet_handler.h"
#include <iostream>

using namespace tremble;

UnknownPacketHandler::UnknownPacketHandler()
{
}

UnknownPacketHandler::~UnknownPacketHandler()
{
}

void UnknownPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
{
	std::cout << "Received unknown packet id: " << (int) packet_id << "." << std::endl;
}
