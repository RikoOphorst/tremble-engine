#pragma once

#include "tremble/core/networking/i_network_event_handler.h"

namespace tremble {
    class NetworkEventHandler : public INetworkEventHandler
    {
    public:
        NetworkEventHandler();
        ~NetworkEventHandler();

    private:
        // Inherited via IConnectionEventHandler
        virtual void OnHostingStarted() override;
        virtual void OnConnectedToServer(Peer& server_peer) override;
        virtual void OnConnectionFailed(ConnectionFailedReason reason) override;
        virtual void OnClientConnected(Peer& client_peer) override;
        virtual void OnClientDisconnect(Peer& client_peer) override;
        virtual void OnConnectionLost(Peer& peer) override;
        virtual void OnPlayerAdded(PlayerData& player_data) override;
        virtual void OnPlayerRemoved(Peer& peer) override;
        virtual void OnGameInformationReceived() override;
        virtual void OnHostDataReceived(HostData& host_data) override;
    };
}