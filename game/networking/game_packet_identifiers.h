#pragma once

#include "tremble/core/networking/packet_identifiers.h"

/*
* @brief Use this to define custom packet ID's.
*/
enum GamePacketIdentifiers
{
    ID_CUSTOM_PACKET = ID_GAME_PACKETS_START, // Add game packet ID's below.
    ID_CREATE_PLAYER_PACKET,
    ID_PLAYER_READY_PACKET
};