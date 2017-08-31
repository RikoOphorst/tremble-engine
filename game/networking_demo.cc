#include "networking_demo.h"
#include "networking/network_event_handler.h"
#include "networking/network_object_creator.h"
#include "networking/game_packet_defs.h"
#include "networking/game_packet_identifiers.h"
#include "networking/packet_handlers/player_ready_packet_handler.h"
#include "components/player.h"
#include "components/grappling_hook.h"
#include "components/flying.h"
#include "components/start_game_listener.h"

namespace tremble
{
    NetworkingDemo::NetworkingDemo()
    {
        nd_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(10000);

        network_event_handler_ = nd_allocator_->New<NetworkEventHandler>();

        network_object_creator_ = nd_allocator_->New<NetworkObjectCreator>();
        Get::NetworkManager()->SetNetworkObjectCreator(network_object_creator_);

        if (Get::Config().host)
        {
            HostData host_data;
            host_data.loaded_map = Get::ConfigManager()->GetScenes()[Get::Config().scene];
            Get::NetworkManager()->Host(host_data, Get::Config().nickname, network_event_handler_);

            TextComponent* text;
            text = Get::Scene()->AddComponent<TextComponent>();
            text->SetFont("../../font/5thagent.ttf");
            text->SetSize(50.0f);
            text->SetCenter({ 0.0f, 0.0f, 0 });
            text->SetPosition(Vector3(1000.0f, 0.0f, 0));
            text->SetColor({ 1, 1, 0 ,1 });
            text->SetLayer(1);
            text->SetText("HOST");
        }
        else
        {
            std::cout << "Attempting to connect: " << Get::NetworkManager()->AttemptConnect(Get::Config().host_ip_address, Get::Config().nickname, network_event_handler_);
        }

        SetupPacketHandlers();
    }

    NetworkingDemo::~NetworkingDemo()
    {
        nd_allocator_->Delete(player_ready_packet_handler_);
        nd_allocator_->Delete(network_object_creator_);
        nd_allocator_->Delete(network_event_handler_);
        Get::MemoryManager()->DeleteAllocator(nd_allocator_);
    }

    bool NetworkingDemo::StartGame()
    {
        if (GameHasStarted())
        {
            return true;
        }

        for each (Peer* peer in Get::NetworkManager()->GetPeerFactory()->GetAllPeers())
        {
            if (!Get::NetworkManager()->GetGameData()->HasPlayerData(peer->GetPeerIndex()))
            {
                return false;
            }
        }

        for each(Peer* peer in Get::NetworkManager()->GetPeerFactory()->GetAllPeers())
        {
            PlayerCreationPacketData packet_data = { (RakNet::NetworkID) 0 };

            if (peer == Get::NetworkManager()->GetPeerFactory()->GetMyPeer())
            {
                NetworkingDemo::GetNetworkingDemo().GetNetworkObjectCreator()->CreateOurPlayer(packet_data);
            }
            else
            {
                NetworkingDemo::GetNetworkingDemo().GetNetworkObjectCreator()->CreateOtherPlayer(peer->GetPeerIndex(), packet_data);
            }
        }

        //GetNetworkObjectCreator()->CreateScoreSystem();
        DestroyStartGameListenerNode();
        game_started_ = true;
        return true;
    }

    void NetworkingDemo::SetupPacketHandlers()
    {
        player_ready_packet_handler_ = nd_allocator_->New<PlayerReadyPacketHandler>();
        Get::NetworkManager()->GetPacketReceiver().AddPacketHandler(ID_PLAYER_READY_PACKET, player_ready_packet_handler_);
    }

    void NetworkingDemo::CreateStartGameListenerNode()
    {
        start_game_listener_node_ = Get::Scene()->AddChild();
        start_game_listener_node_->AddComponent<StartGameListener>();
    }

    void NetworkingDemo::DestroyStartGameListenerNode()
    {
        //start_game_listener_node_->Destroy();
    }

} //namespace tremble