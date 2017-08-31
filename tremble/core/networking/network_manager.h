#pragma once

#include "RakPeerInterface.h"
#include "peer.h"
#include "i_network_event_handler.h"
#include "player_connectivity_data.h"
#include <string>
#include <vector>

#define DEFAULT_PORT 43555

namespace tremble
{
	class FreeListAllocator;
	class PacketReceiver;
	class PacketFactory;
	class PeerFactory;
	class IPacketHandler;
	class INetworkObjectCreator;
	class SerializationManager;
	class GameDataManager;
	class RakNet::NetworkIDManager;

	/**
	 * From this class you can control the core networking including setting
	 * up a host, connecting/disconnecting and managing of connected peers.
	 *
	 * @author Simon Kok
	 */
	class NetworkManager
	{
	public:
		NetworkManager();
		NetworkManager(size_t memory_size);
		~NetworkManager();


		/**
		 * @brief Start hosting a game.
		 *
		 * @param[in] max_connections The maximum amount of connections this instance allows.
		 * @param[in] port The port to listen on (default port across the library is 43555).
		 * @param[in] password A password that clients have to specify to connect to us. Leave blank for no password.
		 */
		void Host(
			const HostData& host_data,
			std::string nickname = "",
			INetworkEventHandler* network_event_handler = nullptr,
			int max_connections = 12, 
			int port = DEFAULT_PORT, 
			std::string password = "");

		/**
		 * @brief Attempt to connect to the specified host.
		 * The user is responsible for attempting again after a failure to connect.
		 *
		 * @param[in] host_address The address of the host we're connecting to.
		 * @param[in] port The port the host is listening on (default port across the library is 43555).
		 * @param[in] password The host's password needed to connect to it. Leave blank for no password.
		 * @return A RakNet::ConnectionAttemptResult.
		 */
		RakNet::ConnectionAttemptResult AttemptConnect(
			std::string host_address = "127.0.0.1",
			std::string nickname = "",
			INetworkEventHandler* network_event_handler = nullptr,
			int port = DEFAULT_PORT,
			std::string password = "");

		/**
		 * @brief Check if there is new incoming data available, passes any incoming data to the packet receiver.
		 */
		void Listen();

		/**
		 * @brief Shutdown the network controller.
		 */
		void Shutdown();

		/**
		 * @brief Send a packet to another peer.
		 * @param[in] recipient The GUID or system address associated with the desired recipient.
		 * @param[in] packet The bitstream object containing the packet you want to send.
		 */
		void SendPacket(RakNet::RakNetGUID recipient, RakNet::BitStream* packet_data);

		/**
		* @brief Send a packet to another peer.
		* @param[in] recipient Pointer to the peer to send this packet to.
		* @param[in] packet The bitstream object containing the packet you want to send.
		*/
		void SendPacket(Peer* recipient, RakNet::BitStream* packet_data);

		/**
		* @brief Send a packet to another peer.
		* @param[in] recipient Pointer to the peer to send this packet to
		* @param[in] realiability_mode What level of reliability to send this packet with.
		* @param[in] packet The bitstream object containing the packet you want to send.
		*/
		void SendPacket(Peer* recipient, PacketReliability reliability_mode, RakNet::BitStream* packet_data);

		/**
		 * @return The class in charge of handling incoming data.
		 */
		PacketReceiver& GetPacketReceiver() { return *packet_receiver_; }

		/**
		* @return The class in charge of creating and managing peers.
		*/
		PeerFactory* GetPeerFactory() { return peer_factory_; }

		/**
		* @return Raknets internal 'network ID' manager. Manages object ID's shared over the network.
		*/
		RakNet::NetworkIDManager& GetNetworkIdManager() { return *network_id_manager_; }

		/**
		* @return The manager responsible for serializing and deserializing components.
		*/
		SerializationManager& GetSerializationManager() { return *serialization_manager_; }

		/**
		* @return The manager responsible for game data (nicknames, current map etc).
		*/
		GameDataManager* GetGameData() { return game_data_manager_; }

		bool IsHost() { return is_host_; } //<! @return Whether this networking instance is a host.

		bool IsRunning() { return is_running_; } //<! @return Whether this network manager is running.

		RakNet::RakNetGUID GetGuid() { return guid_; }

		/**
		 * @brief The connection event functions (OnClientConnected etc) are called in the NetworkInterface you specify.
		 */
		void SetNetworkEventInterface(INetworkEventHandler* net_interface) { network_event_interface_ = net_interface; }
		INetworkEventHandler* GetNetworkEventInterface() { return network_event_interface_; } //!< @return The network interface if the user has specified one (can return nullptr).
		bool HasNetworkEventInterface();

		void SetNetworkObjectCreator(INetworkObjectCreator* net_object_creator) { network_object_creator_ = net_object_creator; }
		INetworkObjectCreator* GetNetworkObjectCreator() { return network_object_creator_; } //!< @return The network obect creator if the user has specified one (can return nullptr).
		bool HasNetworkObjectCreator();

	protected:
		FreeListAllocator* nm_allocator_; //!< Pointer to the allocator
	private:
		/**
		 * @brief Start the RakPeerInterface.
		 */
		void Startup(bool is_host, int port = DEFAULT_PORT, INetworkEventHandler* network_event_handler = nullptr);

		RakNet::RakPeerInterface* client_; //!< The RakPeer associated with this instance.
		RakNet::RakNetGUID guid_; //!< The unique identifier generated by RakNet for this instance.

		INetworkEventHandler* network_event_interface_; //!< The user can use this to receive updates on peer connections.
		INetworkObjectCreator* network_object_creator_; //!< The user can implement this class to create networked objects.

		bool is_host_; //!< Whether this instance will serve as host for the other peers.
		int max_connections_ = 2; //!< The maximum amount of connections RakNet will allow.
		std::string password_; //!< The password needed to connect to the host.

		std::string host_address_; //!< The host address we're connected to.
		int port_; //!< The port the host is listening on.

		RakNet::Packet* packet_; //!< The pointer used for incoming packets before they are handled and deleted.
		PacketReceiver* packet_receiver_; //!< The class in charge of handling incoming data.
		//PacketFactory* packet_factory_; //!< Factory class for creating and deleting packets.

		PeerFactory* peer_factory_; //!< The class in charge of creating and managing peers.

		RakNet::NetworkIDManager* network_id_manager_; // !< Raknets internal 'network ID' manager. Manages object ID's shared over the network.
		SerializationManager* serialization_manager_; //!< The manager responsible for serializing and deserializing components.
		GameDataManager* game_data_manager_; //!< The manager responsible for game data (nicknames, current map etc).

		bool is_running_ = false; //!< Whether the network controller is running (used to shutdown rakpeerinterface)
	};
}