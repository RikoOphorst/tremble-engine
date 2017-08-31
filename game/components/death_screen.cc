#include "death_screen.h"
#include "player.h"

namespace tremble
{
    void DeathScreen::Awake(Player* player)
    {
        death_timer_ = 0.0f;
        enabled_ = false;

        player_ = player;
        death_text_ = GetNode()->AddComponent<TextComponent>();
        death_text_->SetFont("../../font/5thagent.ttf");
        death_text_->SetSize(50.0f);
        death_text_->SetCenter({ 0.0f, 0.0f, 0 });
        death_text_->SetPosition(Vector3(250.0f, 100.0f, 0));
        death_text_->SetColor({ 1, 1, 1 ,1 });
        death_text_->SetLayer(1);
    }

    void DeathScreen::Update()
    {
        if (!enabled_)
            return;

        death_timer_ += Get::DeltaT();

        death_text_->SetText("You have died! \nYou will respawn in: " + GetTimeLeft());

        if (death_timer_ >= death_time_)
        {
            Reset();
        }
    }

    void DeathScreen::Enable()
    {
        enabled_ = true;
        player_->Disable();
    }

    void DeathScreen::Reset()
    {
        enabled_ = false;
        death_timer_ = 0.0f;
        death_text_->SetText("");

        player_->Enable();
        player_->Spawn();
    }

    bool DeathScreen::IsEnabled() const
    {
        return enabled_;
    }

    std::string DeathScreen::GetTimeLeft() const
    {
        std::string time_left = std::to_string(death_time_ - death_timer_);
        size_t dot_index = 0;
        for (size_t i = 0; i < time_left.size(); ++i)
        {
            if (time_left[i] == '.')
            {
                dot_index = i;
                break;
            }
        }
        time_left.erase(time_left.begin() + dot_index + 3, time_left.end());
        return time_left;
    }
}
