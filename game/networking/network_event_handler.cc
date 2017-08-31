#include "networking_demo.h"
#include "networking/network_event_handler.h"
#include "networking/network_object_creator.h"
#include "networking/game_packet_identifiers.h"
#include "networking/game_packet_defs.h"
#include "components/player.h"
#include "components/cube_movement.h"
#include "BitStream.h"

namespace tremble
{
    NetworkEventHandler::NetworkEventHandler()
    {
    }

    NetworkEventHandler::~NetworkEventHandler()
    {

    }

    void NetworkEventHandler::OnHostingStarted()
    {

    }

    void NetworkEventHandler::OnConnectedToServer(Peer& server_peer)
    {
    }

    void NetworkEventHandler::OnClientConnected(Peer& client_peer)
    {
    }

    void NetworkEventHandler::OnConnectionFailed(ConnectionFailedReason reason)
    {
    }

    void NetworkEventHandler::OnClientDisconnect(Peer& client_peer)
    {
    }

    void NetworkEventHandler::OnConnectionLost(Peer& peer)
    {
    }

    void NetworkEventHandler::OnPlayerAdded(PlayerData& player_data)
    {
        NetworkingDemo::GetNetworkingDemo().IncrementPlayersReady();
    }

    void NetworkEventHandler::OnPlayerRemoved(Peer & peer)
    {
        NetworkingDemo::GetNetworkingDemo().IncrementPlayersReady();
    }

    void NetworkEventHandler::OnGameInformationReceived()
    {
        //RakNet::BitStream* bs = Get::PacketFactory()->CreatePacket(ID_PLAYER_READY_PACKET);
        //Get::NetworkManager()->SendPacket(Get::NetworkManager()->GetPeerFactory()->GetHostingPeer(), PacketReliability::RELIABLE_ORDERED, bs);
        //Get::PacketFactory()->DeletePacket(bs);
    }

    void NetworkEventHandler::OnHostDataReceived(HostData & host_data)
    {
        Get::SceneLoader()->LoadScene(host_data.loaded_map);
    }
}