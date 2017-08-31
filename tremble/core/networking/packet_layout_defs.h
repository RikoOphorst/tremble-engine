#pragma once
#include "RakString.h"

#pragma pack(push, 1) //!< Force the compiler to pack the structs byte-aligned.
struct TestPacket
{
	int some_int;
	float some_float;
};

struct TransformSerializationPacket
{
	float x_pos, y_pos, z_pos, y_rot, camera_x_rot;
};

struct PlayerInformationPacketHeader
{
	unsigned short amount_of_players; //!< This is the total amount of player information blocks contained in this packet.
};

/*struct PlayerInformationPacket
{
	unsigned int peer_id; //TODO: probably should be a short instead.
};*/

struct RemovePlayerPacket
{
	unsigned int peer_id; //TODO: probably should be a short instead.
};

struct ObjectCreationPacketHeader
{
	unsigned int owner_peer_id;
	unsigned char networked_object_type;
};

struct SerializationPacketBlockHeader
{
	RakNet::NetworkID transform_component_id;
	unsigned int serialization_block_size;
};

#pragma pack(pop)