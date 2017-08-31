#pragma once

#include <vector>
#include <unordered_map>
#include "networking/game_packet_defs.h"

namespace tremble
{
    //OLD SIMON's CODE
    class NetworkEventHandler;
    class NetworkObjectCreator;
    class PlayerReadyPacketHandler;

    class NetworkingDemo
    {
    public:
        static NetworkingDemo& GetNetworkingDemo()
        {
            static NetworkingDemo instance; // Guaranteed to be destroyed.
                                            // Instantiated on first use.
            return instance;
        }
        NetworkingDemo();
        NetworkingDemo(NetworkingDemo const&) = delete;
        void operator=(NetworkingDemo const&) = delete;
        ~NetworkingDemo();

        bool GameHasStarted() { return game_started_; }

        bool StartGame();
        void CreateStartGameListenerNode();

        void IncrementPlayersReady() { players_ready_++; }
        void DecrementPlayersReady() { players_ready_--; }
        unsigned int GetPlayersReady() { return players_ready_; }

        NetworkObjectCreator* GetNetworkObjectCreator() { return network_object_creator_; }
    protected:
        FreeListAllocator* nd_allocator_;

    private:
        NetworkEventHandler* network_event_handler_;
        NetworkObjectCreator* network_object_creator_;
        PlayerReadyPacketHandler* player_ready_packet_handler_;
        SGNode* start_game_listener_node_;

        bool game_started_;
        unsigned int players_ready_;

        void SetupPacketHandlers();
        void DestroyStartGameListenerNode();
        //void SetupPlayers();
    };


}