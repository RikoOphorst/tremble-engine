#include "packet_handler.h"
#include "peer.h"
#include "packet_identifiers.h"

using namespace tremble;

IPacketHandler::IPacketHandler()
{
	
}

RakNet::BitStream* IPacketHandler::GetPacketData(RakNet::Packet * packet)
{
	RakNet::BitStream* bs = new RakNet::BitStream(packet->data, packet->length, false);
	bs->IgnoreBytes(sizeof(RakNet::MessageID));
	return bs;
}