#include "network_object_creator.h"
#include "network_objects.h"
#include "game_packet_defs.h"
#include "components\player.h"
#include "components\grappling_hook.h"
#include "components\flying.h"
#include "components\score_system.h"

namespace tremble
{
    void NetworkObjectCreator::OnNewNetworkObjectReceived(int networked_object_type, PeerID owner_id, RakNet::BitStream& bit_stream)
    {
        std::cout << "Create new object type: " << networked_object_type << ", owner id: " << owner_id << std::endl;

        switch (networked_object_type)
        {
        case NetworkedObjectTypes::PLAYER:
            PlayerCreationPacketData player_creation_data = IPacketHandler::ReadPacketStruct<PlayerCreationPacketData>(&bit_stream);
            if (owner_id == Get::NetworkManager()->GetPeerFactory()->GetMyPeer()->GetPeerIndex())
            {
                std::cout << "Creating new 'our player'." << std::endl;
                CreateOurPlayer(player_creation_data); //TODO: Maybe create a separate function for creating a player as client
            }
            else
            {
                std::cout << "Creating new 'other player'. (owner: " << owner_id << ")" << std::endl;
                CreateOtherPlayer(owner_id, player_creation_data);
            }
            break;

        case NetworkedObjectTypes::SCORE_SYSTEM:
            ScoreSystemCreationPacket packet_data = IPacketHandler::ReadPacketStruct<ScoreSystemCreationPacket>(&bit_stream);
            CreateScoreSystem(owner_id, packet_data);
        }
    }

    void NetworkObjectCreator::CreateScoreSystem()
    {
        ScoreSystem* score_system = Get::Scene()->AddComponent<ScoreSystem>();
        SendScoreSystemCreationPacket(ScoreSystemCreationPacket{ score_system->GetNetworkID() });
        std::cout << "Sending score system with id: " << std::to_string(score_system->GetNetworkID()) << std::endl;
    }

    void NetworkObjectCreator::CreateScoreSystem(PeerID owner, ScoreSystemCreationPacket & packet_data)
    {
        ScoreSystem* score_system = Get::Scene()->AddComponent<ScoreSystem>();
        score_system->SetNetworkID(packet_data.score_system_id);
        std::cout << "Created received score system with id: " << std::to_string(packet_data.score_system_id) << std::endl;
    }

    void NetworkObjectCreator::SendScoreSystemCreationPacket(ScoreSystemCreationPacket & packet_data)
    {
        RakNet::BitStream* score_system_creation_packet = Get::PacketFactory()->CreatePacket(ID_CREATE_OBJECT_PACKET);

        ObjectCreationPacketHeader packet_header = {
            Get::NetworkManager()->GetPeerFactory()->GetMyPeer()->GetPeerIndex(),
            SCORE_SYSTEM
        };
        score_system_creation_packet->Write(packet_header);
        score_system_creation_packet->Write(packet_data);

        std::cout << "Sending score system with id: " << std::to_string(packet_data.score_system_id) << std::endl;

        for each(Peer* peer in Get::NetworkManager()->GetPeerFactory()->GetAllPeers())
        {
            if (!peer->IsHost())
            {
                Get::NetworkManager()->SendPacket(peer, PacketReliability::RELIABLE_ORDERED, score_system_creation_packet);
            }
        }

        Get::PacketFactory()->DeletePacket(score_system_creation_packet);
    }


    void NetworkObjectCreator::CreateOurPlayer(PlayerCreationPacketData& packet_data)
    {
        //Set up the player
        SGNode* player_node = Get::Scene()->AddChild();
        player_node->SetLocalPosition(Vector3(0, 1, 0));

        SGNode* player_camera = player_node->AddChild(false, Vector3(0.0f, 0.4f, 0.0f));
        Camera* player_camera_component = player_camera->AddComponent<Camera>();
        player_camera_component->SetFOV(60.0f * 0.0174533f);
        player_camera_component->SetNearZ(0.01f);

        Player* player = nullptr;

        if (!Get::Config().god_mode)
        {
            player_node->AddComponent<CharacterController>();
            player = player_node->AddComponent<Player>();
            GrapplingHook* grappling_hook = player_node->AddComponent<GrapplingHook>();

            TransformSerializationComponent* serializer = player_node->AddComponent<TransformSerializationComponent>();
            serializer->SetIsMe(true);

            PeerID my_peer_id = Get::NetworkManager()->GetPeerFactory()->GetMyPeer()->GetPeerIndex();

            //std::cout << "Associating 'our player' with owner: " << my_peer_id << std::endl;
            player->SetupHUD();

            if (Get::NetworkManager()->IsHost())
            {
                std::cout << "Sending player creation packet" << std::endl;
                SendPlayerCreationPacket(Get::NetworkManager()->GetPeerFactory()->GetMyPeer()->GetPeerIndex(), serializer, player, player->all_guns_);
            }
            else
            {
                // Set component network ID's.
                std::cout << "Setting our players components network id: " << packet_data.transform_component_id << std::endl;
                serializer->SetNetworkID(packet_data.transform_component_id);
                player->SetNetworkID(packet_data.health_component_id);
                player->all_guns_[0]->SetNetworkID(packet_data.base_guns[0]);
                player->all_guns_[1]->SetNetworkID(packet_data.base_guns[1]);
                player->all_guns_[2]->SetNetworkID(packet_data.base_guns[2]);
                player->all_guns_[3]->SetNetworkID(packet_data.base_guns[3]);
            }

            Get::InputManager()->LockCursor();
        }
        else
        {
            player_node->SetLocalPosition(Vector3(-7.664f, 16.7168f, -23.4085f));
            player_node->SetLocalRotationDegrees(Vector3(29.2163f, 11.425f, -0.000011057f));
            Flying* flying = player_node->AddComponent<Flying>();
        }

        Get::Renderer()->SetCamera(player_camera_component);
    }

    void NetworkObjectCreator::CreateOtherPlayer(PeerID owner, PlayerCreationPacketData& packet_data)
    {
        //Set up the player
        SGNode* other_player_node = Get::Scene()->AddChild();
        other_player_node->AssociateWithPeer(owner);
        other_player_node->SetLocalPosition(Vector3(0, 1, 0));

        SGNode* other_player_camera = other_player_node->AddChild(false, Vector3(0.0f, 0.4f, 0.0f));
        Camera* our_camera = Get::Renderer()->GetCamera();
        Camera* player_camera_component = other_player_camera->AddComponent<Camera>();
        Get::Renderer()->SetCamera(our_camera);
        player_camera_component->SetFOV(60.0f * 0.0174533f);
        player_camera_component->SetNearZ(0.01f);

        SGNode* other_player_model = other_player_node->AddChild(false, Vector3(0, -1.0f, 0), Vector3(), Vector3(0.01f, 0.01f, 0.01f));
        other_player_model->AddComponent<Renderable>()->SetModel(Get::ResourceManager()->GetModel("ArmyPilot/ArmyPilot.obj", true));

        if (!Get::Config().god_mode)
        {
            other_player_node->AddComponent<CharacterController>();
            Player* other_player = other_player_node->AddComponent<Player>();
            GrapplingHook* grappling_hook = other_player_node->AddComponent<GrapplingHook>();

            TransformSerializationComponent* serializer = other_player_node->AddComponent<TransformSerializationComponent>();

            other_player->AssociateWithPeer(owner);
            grappling_hook->AssociateWithPeer(owner);

            std::cout << "Associating 'other player' with owner: " << owner << std::endl;
            other_player->other_player_ = true;

            if (Get::NetworkManager()->IsHost())
            {
                other_player->Spawn();
                other_player_node->Move(Vector3(0, 10, 0));
                std::cout << "Sending player creation packet" << std::endl;
                SendPlayerCreationPacket(owner, serializer, other_player, other_player->all_guns_);
            }
            else
            {
                // Set component network ID's.
                std::cout << "Setting other players components network id: " << packet_data.transform_component_id << std::endl;
                serializer->SetNetworkID(packet_data.transform_component_id);
                other_player->SetNetworkID(packet_data.health_component_id);
                other_player->all_guns_[0]->SetNetworkID(packet_data.base_guns[0]);
                other_player->all_guns_[1]->SetNetworkID(packet_data.base_guns[1]);
                other_player->all_guns_[2]->SetNetworkID(packet_data.base_guns[2]);
                other_player->all_guns_[3]->SetNetworkID(packet_data.base_guns[3]);
            }
        }
        else
        {
            other_player_camera->SetLocalPosition(Vector3(-7.664f, 16.7168f, -23.4085f));
            other_player_camera->SetLocalRotationDegrees(Vector3(29.2163f, 11.425f, -0.000011057f));
            Flying* flying = other_player_camera->AddComponent<Flying>();
        }
    }

    void NetworkObjectCreator::SendPlayerCreationPacket(PeerID owner, Serializable* transform_component, Serializable* health_component, std::vector<BaseGun*> base_guns)
    {
        RakNet::BitStream* player_creation_packet = Get::PacketFactory()->CreatePacket(ID_CREATE_OBJECT_PACKET);

        ObjectCreationPacketHeader packet_header = {
            owner,
            PLAYER
        };
        player_creation_packet->Write(packet_header);

        PlayerCreationPacketData packet_data = {
            transform_component->GetNetworkID(),
            health_component->GetNetworkID(),
            base_guns[0]->GetNetworkID(),
            base_guns[1]->GetNetworkID(),
            base_guns[2]->GetNetworkID(),
            base_guns[3]->GetNetworkID()
        };

        player_creation_packet->Write(packet_data);
        for each (Serializable* i in base_guns)
        {
            player_creation_packet->Write(i->GetNetworkID());
        }

        for each(Peer* peer in Get::NetworkManager()->GetPeerFactory()->GetAllPeers())
        {
            if (!peer->IsHost())
            {
                Get::NetworkManager()->SendPacket(peer, PacketReliability::RELIABLE_ORDERED, player_creation_packet);
            }
        }

        Get::PacketFactory()->DeletePacket(player_creation_packet);
    }

    void NetworkObjectCreator::CreateNewPlayerAsHost(PeerID owner)
    {

    }

    void NetworkObjectCreator::CreateNewPlayer(PeerID owner, RakNet::BitStream& bit_stream)
    {
        std::cout << "Attempting to create new player for peer: " << owner << std::endl;
        //TODO: Think of what of the create code should be split between host/client or between for ourselves/for someone else if any.

        //Set up the player
        SGNode* player_node = Get::Scene()->AddChild();
        player_node->SetLocalPosition(Vector3(0, 1, 0));

        //if (owner == Get::NetworkManager()->GetPeerFactory()->GetMyPeer()->GetPeerIndex()) //Create player for yourself.
        //{
        SGNode* player_camera = player_node->AddChild(false, Vector3(0.0f, 0.4f, 0.0f));
        Camera* player_camera_component = player_camera->AddComponent<Camera>();
        player_camera_component->SetFOV(60.0f * 0.0174533f);
        player_camera_component->SetNearZ(0.01f);

        Player* player = nullptr;

        if (!Get::Config().god_mode)
        {
            player_node->AddComponent<CharacterController>();
            player = player_node->AddComponent<Player>();
            player_node->AddComponent<GrapplingHook>();
            player->Spawn();
            TransformSerializationComponent* serializer = player_node->AddComponent<TransformSerializationComponent>();
            Get::InputManager()->LockCursor();
        }
        else
        {
            player_node->SetLocalPosition(Vector3(-7.664f, 16.7168f, -23.4085f));
            player_node->SetLocalRotationDegrees(Vector3(29.2163f, 11.425f, -0.000011057f));
            Flying* flying = player_node->AddComponent<Flying>();
        }

        if (player != nullptr)
        {
            player->AssociateWithPeer(owner);
        }

        if (Get::NetworkManager()->IsHost())
        {

        }
        //}
        //else //Create networked player.
        //{

        //}
    }
}