#pragma once

namespace tremble
{
    struct ScoreData
    {
        PlayerData player_data;
        size_t score;
        size_t kills;
        size_t deaths;
    };

    class ScoreSystem : public Component, public Serializable
    {
    public:
        void OnPlayerConnect(const PlayerData& player_data) //!< Happens when a new player is connected
        {
            player_scores_[player_data.peer_id] = ScoreData();
            player_scores_[player_data.peer_id].player_data = player_data;
            scores_changed_ = true;
        }

        void Serialize(RakNet::BitStream& bs) override; //!< Serialization function (happens on a host)
        void Deserialize(RakNet::BitStream& bs) override; //!< Deserialization function (happens on a client)
        void AddScore(int score, Peer* peer); //!< Add score to a player
        void SubstractScore(int score, Peer* peer); //!< Substract score from a player
        void AddKill(Peer* peer); //!< Add a kill to a player
        void AddDeath(Peer* peer); //!< Add a death to a player

        const std::unordered_map<PeerID, ScoreData>& GetScores() { return player_scores_; } //!< Get all the player's scores
        const ScoreData& GetPlayersScoreData(Peer* peer); //!< Get a score of a particular player
    private:
        std::unordered_map<PeerID, ScoreData> player_scores_; //!< Player's scores, assotiated with peer id 
        bool scores_changed_ = false; //!< A dirty flag, indicating if the data needs to be syncronized or not

    };
}