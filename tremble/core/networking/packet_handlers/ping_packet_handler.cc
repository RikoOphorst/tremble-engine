#include "ping_packet_handler.h"
#include "../packet_identifiers.h"
#include "../packet_layout_defs.h"
#include "../network_manager.h"
#include <iostream>

using namespace tremble;

PingPacketHandler::PingPacketHandler()
{
}

PingPacketHandler::~PingPacketHandler()
{
}

void PingPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
{
	RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); // Extract packet data as RakNet::BitStream.
	TestPacket packet_struct = IPacketHandler::ReadPacketStruct<TestPacket>(bs); // Read packet as struct.

	delete bs;
}