#include "test_packet_handler.h"
#include "../packet_identifiers.h"
#include "../packet_layout_defs.h"
#include "../network_manager.h"
#include <iostream>

using namespace tremble;

TestPacketHandler::TestPacketHandler()
{
}

TestPacketHandler::~TestPacketHandler()
{
}

void TestPacketHandler::Handle(int packet_id, RakNet::Packet* packet, Peer* sender)
{
	RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); // Extract packet data as RakNet::BitStream.
	TestPacket packet_struct = IPacketHandler::ReadPacketStruct<TestPacket>(bs); // Read packet as struct.

	// Handle received data.
	std::cout << "testpacket some_int: " << packet_struct.some_int << ", some_float: " << packet_struct.some_float << std::endl;

	delete bs;
}