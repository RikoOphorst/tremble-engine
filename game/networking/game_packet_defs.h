#pragma once

#include "NetworkIDObject.h"

#pragma pack(push, 1) //!< Force the compiler to pack the structs byte-aligned.

struct PlayerCreationPacketData
{
    RakNet::NetworkID transform_component_id;
    RakNet::NetworkID health_component_id;
    RakNet::NetworkID base_guns[4];
};

struct ScoreSystemCreationPacket
{
    RakNet::NetworkID score_system_id;
};

struct PlayerSerializationPacket
{
    float x, y, z;
};

#pragma pack(pop)