#pragma once
#include <vector>
#include "player_connectivity_data.h"
#include "peer.h"

namespace tremble
{

	/**
	* @brief Manages hostdata and playerdata.
	*
	* @author Simon Kok
	*/
	class GameDataManager
	{
	public:
		GameDataManager();
		~GameDataManager();

		void SetHostData(HostData host_data);
		HostData& GetHostData();

		void SetTemporaryPlayerData(PlayerData player_data);
		PlayerData& GetTemporaryPlayerData();

		bool HasPlayerData(PeerID peer_id);
		PlayerData GetPlayerData(PeerID peer_id);
		void RemovePlayerData(PeerID peer_id);
		void AddPlayerData(PlayerData player_data);

		std::vector<PlayerData>& GetAllPlayerData();

		void LogPlayerData();
	private:
		HostData host_data_;
		PlayerData temporary_player_data_;
		std::vector<PlayerData> player_data_;
	};
}