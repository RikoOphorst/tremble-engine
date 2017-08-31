#include "game_data_manager.h"
#include <limits>

namespace tremble
{
	GameDataManager::GameDataManager()
	{

	}

	GameDataManager::~GameDataManager()
	{

	}

	void GameDataManager::SetHostData(HostData host_data)
	{
		host_data_ = host_data;
	}

	HostData & GameDataManager::GetHostData()
	{
		return host_data_;
	}

	void GameDataManager::SetTemporaryPlayerData(PlayerData player_data)
	{
		temporary_player_data_ = player_data;
	}

	PlayerData& GameDataManager::GetTemporaryPlayerData()
	{
		return temporary_player_data_;
	}

	bool GameDataManager::HasPlayerData(PeerID peer_id)
	{
		for each(PlayerData player_data in player_data_)
		{
			if (player_data.peer_id == peer_id)
				return true;
		}
		return false;
	}

	PlayerData GameDataManager::GetPlayerData(PeerID peer_id)
	{
		for each(PlayerData player_data in player_data_)
		{
			if (player_data.peer_id == peer_id)
				return player_data;
		}

		PlayerData dummy;
		dummy.peer_id = std::numeric_limits<PeerID>::max();
		dummy.nickname = std::string("invalid player data");

		LogPlayerData();
		return dummy;
	}

	void GameDataManager::RemovePlayerData(PeerID peer_id)
	{
		player_data_.erase(
			std::remove_if(player_data_.begin(), player_data_.end(), [&](const PlayerData& player_data)
			{
				return player_data.peer_id == peer_id;
			})
			, player_data_.end());
		LogPlayerData();
	}

	void GameDataManager::AddPlayerData(PlayerData player_data)
	{
		player_data_.push_back(player_data);
		LogPlayerData();
	}

	std::vector<PlayerData>& GameDataManager::GetAllPlayerData()
	{
		return player_data_;
	}

	void GameDataManager::LogPlayerData()
	{
		std::cout << "player_data count: " << player_data_.size() << std::endl;

		for each(PlayerData player_data in player_data_)
		{
			std::cout << "----------------" << std::endl;
			std::cout << "id: " << player_data.peer_id << std::endl;
			std::cout << "nickname: " << player_data.nickname << std::endl;
			std::cout << "----------------" << std::endl;
		}
	}
}