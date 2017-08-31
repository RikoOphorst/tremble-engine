#include "score_system.h"
namespace tremble
{
    void HostCheck()
    {
#ifdef _DEBUG
        if (Get::NetworkManager()->IsHost() != true)
        {
            std::cout << "wrong networking.. don't do this on the client" << std::endl;
        }
#endif
    }

    void ScoreSystem::Serialize(RakNet::BitStream& bs)
    {
        if (scores_changed_)
        {
            bs.Write(player_scores_.size());
            std::unordered_map<PeerID, ScoreData>::iterator iter = player_scores_.begin();
            while (iter != player_scores_.end())
            {
                bs.Write(iter->second.player_data.peer_id);
                bs.Write(iter->second.kills);
                bs.Write(iter->second.deaths);
                bs.Write(iter->second.score);
            }
            scores_changed_ = false;
        }
    }
    void ScoreSystem::Deserialize(RakNet::BitStream& bs)
    {
        size_t number_of_players;
        PeerID peer_id;
        bs.Read(number_of_players);
        for (size_t i = 0; i < number_of_players; i++)
        {
            bs.Read(peer_id);
            bs.Read(player_scores_[peer_id].kills);
            bs.Read(player_scores_[peer_id].deaths);
            bs.Read(player_scores_[peer_id].score);
        }
    }

    void ScoreSystem::AddScore(int score, Peer * peer)
    {
        player_scores_[peer->GetPeerIndex()].score += score;
        scores_changed_ = true;
        HostCheck();
    }

    void ScoreSystem::SubstractScore(int score, Peer * peer)
    {
        player_scores_[peer->GetPeerIndex()].score -= score;
        scores_changed_ = true;
        HostCheck();
    }

    void ScoreSystem::AddKill(Peer * peer)
    {
        player_scores_[peer->GetPeerIndex()].kills += 1;
        scores_changed_ = true;
        HostCheck();
    }

    void ScoreSystem::AddDeath(Peer * peer)
    {
        player_scores_[peer->GetPeerIndex()].deaths += 1;
        scores_changed_ = true;
        HostCheck();
    }

    const ScoreData& ScoreSystem::GetPlayersScoreData(Peer * peer)
    {
        return player_scores_[peer->GetPeerIndex()];
    }

}