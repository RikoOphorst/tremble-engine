#pragma once

namespace tremble
{
    class Player;

    /**
     * @class tremble::DeathScreen
     * @author Tim Sleddens
     * @brief This class exists to show a timer on the screen when the player dies.
     * And respawn him when the timer is finished. Also it disables the player.
     */
    class DeathScreen : public Component
    {
    public:

        /**
         * @brief Initialize private variables and components
         * @param[in] player a pointer to the Player owning the DeathScreen
         */
        void Awake(Player* player);

        /**
         * @brief Update the death_timer_
         */
        void Update();

        /**
         * @brief Enables the DeathScreen
         */
        void Enable();

        /**
         * @brief Disables the DeathScreen and reset all variables
         */
        void Reset();

        /**
         * @brief Returns if the DeathScreen is enabled
         */
        bool IsEnabled() const;

    private:

        /**
         * @brief Returns time in this format: %f.2
         */
        std::string GetTimeLeft() const;

        Player* player_; //!< Pointer to the player owning this

        TextComponent* death_text_; //!< Text renderer component

        const float death_time_ = 3.0f; //!< The amount of seconds the DeathScreen will be visible for

        float death_timer_; //!< Variable that keeps track of the time passed

        bool enabled_; //!< Is this component enabled?

    };

}