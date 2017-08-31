#pragma once

#include "MessageIdentifiers.h"

/**
 * @brief The packet ID's used by the networking inside the engine.
 *
 * If you want to implement custom packets you can simply repeat what is done in this class, include this file, 
 * create an enum and fill it with your custom packet id's.
 * 
 * The first value in your custom packets enum should equal the last value of this enum (ID_CUSTOM_PACKETS_START).

 * @author Simon Kok
 */
enum TremblePacketIdentifiers
{
	ID_UNKNOWN_USER_PACKET = -1,
	ID_TEST_PACKET = ID_USER_PACKET_ENUM,
	ID_SERIALIZATION_PACKET,
    ID_INPUT_PACKET,
	ID_PLAYER_DATA_PACKET,
	ID_HOST_DATA_PACKET,
	ID_PLAYER_INFORMATION_PACKET,
	ID_NEW_PLAYER_PACKET,
	ID_REMOVE_PLAYER_PACKET,
	ID_CREATE_OBJECT_PACKET,
	ID_GAME_PACKETS_START
};