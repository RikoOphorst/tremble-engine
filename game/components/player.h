#pragma once
#include "components/target_dummy.h"
#include "components/base_gun.h"

namespace tremble
{
    class Player : public Component, public Serializable
    {
    public:
        void Awake();
        void Start();

        void Player::Update();

        void Player::OnDamage(int value);
        void Player::OnPickUp(int value, std::string tag);

        void Player::Crouching();

        void Player::HeadBobbing();

        void Player::WeaponSwitching();

        void Player::GunSwaying();

        bool Player::Locomotion();

        void Player::Aiming();

        void Player::Spawn();

        void Player::SetupHUD();

        void Player::ChangeWeapon(int changed_weapon_index_);

        void Show();
        void Hide();
        void Enable();
        void Disable();

        float Player::Lerp(float a, float b, float t);
        int Player::Sign(float value);

        void Player::Serialize(RakNet::BitStream& packet);
        void Player::Deserialize(RakNet::BitStream& packet);

        SGNode* camera_node_;
        SGNode* gun_node_;

        SGNode* player_hitbox_head_node;
        SGNode* player_hitbox_body_node;

        TriggerCollider* player_head_hitbox_;
        TriggerCollider* player_body_hitbox_;

        //text component for player hud
        TextComponent* ammo_counter_;
        TextComponent* health_display_text_;
        TextComponent* shield_display_text_;

        BaseGun* current_gun_;
        BaseGun* pistol_;
        BaseGun* shot_gun_;
        BaseGun* laser_gun_;
        BaseGun* assualt_rifle_;


        std::vector<BaseGun*> all_guns_;

        int current_selected_gun_ = 0;

        static std::vector<SGNode*> spawnpoints;

        bool other_player_ = false;
    private:
        Scalar camera_pitch_ = -70;

        float mouse_sensitivity_x_ = 0.005f;
        float mouse_sensitivity_y_ = 0.3f;

        //walking/sprinting variable
        float movement_speed_ = 12;
        float crouching_movement_speed_ = 3;
        float sprint_multiplier_ = 1.5f;
        float current_sprint_multiplier = 1;
        float increase_amount_sprint_multiplier = 0.001f;

        //variables for head bobbing
        float headbob_time_ = 0;
        float headbobbiness_ = 0.5;
        float headbob_speed_ = 10;
        float sprint_headbob_multiplier_ = 1.5f;

        //weapon sway
        float sway_amount_factor_pitch = 2.25f;
        float sway_amount_factor_yaw = 2.5f;

        float max_sway_amount_factor_pitch = 10;
        float max_sway_amount_factor_yaw = 10;

        float sway_amount_factor_lerp_speed = 1.5f;

        //Player height
        float current_height_ = 0;
        float normal_height_ = 0.15f;
        float crouching_height_ = 0.20f;
        float crouching_speed_ = 7.5f;

        float jump_height_ = 10;

        AudioListener* listener;
        CharacterController* character_controller_;

        //health and shield variable's
        int health_points_ = 100;
        int shield_points_ = 100;



        bool enabled_;

        bool flying = false;


        //audioclips
        AudioClip* reload_clip_;
        AudioClip* shooting_clip_;
        AudioClip* walking_clip_;

        AudioSource* walk_source_;

        class DeathScreen* death_screen_;
        Renderable* renderable_;

        static PhysicsMaterial* physics_material_;

        struct PlayerHealthData
        {
            int hp;
            int sp;
        };
    };
}
