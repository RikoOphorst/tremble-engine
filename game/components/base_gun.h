#pragma once
#include "components/target_dummy.h"

namespace tremble
{
    class BaseGun : public Component, public Serializable
    {
    public:
        static ParticleSystem* bullet_hole_;

        ParticleEmitter* pe;

        enum predefined_gun_types
        {
            Pistol, Shotgun, LaserGun, AssualtRifle
        };

        enum fire_types
        {
            Automatic, Trigger, Laser
        };

        predefined_gun_types my_predefined_type_ = Shotgun;

        fire_types gun_fire_types_;

        SGNode* camera_node_;
        int ammo_capacity_ = 100;

        int ammo_ = 100;

        int damage_ = 10;

        float reload_speed_ = 0.5f;
        float fire_rate_ = 0.5f;

        bool can_fire_ = true;
        float can_fire_timer_ = 0;

        float reload_time_ = 1;
        float current_reload_time_ = 0;
        bool reloading_ = false;

        float recoil_ = 0.5f;
        float recoil_return_speed_ = 2.5f;
        float recoil_effect_camera_ = 0.5f;


        float spread_amount_ = 0.1f;
        int spray_amount_ = 5;


        float bullet_travel_distance_ = 1000;

        bool unlocked = false;



        Model* gun_model_;
        Renderable* gun_model_renderable_;

        Vector3 gun_start_position_ = Vector3(0, 0, 0);
        float current_recoil_offset_ = 0;

        bool enabled = false;

        std::string model_path_name_ = "heavy_gun/heavy_gun.obj";

        void BaseGun::Start();

        void BaseGun::Update();

        void BaseGun::RepositionMuzzle();

        void BaseGun::SetModel();

        void BaseGun::RecoilCoolDown();

        float BaseGun::Lerp(float a, float b, float t);

        void BaseGun::Shooting(bool soundLoop);

        void BaseGun::Reloading();

        void BaseGun::FireRate();

        float RandomRange(float low, float high);

        void BaseGun::Serialize(RakNet::BitStream& packet);
        void BaseGun::Deserialize(RakNet::BitStream& packet);
    private:
        //audioclips
        AudioClip* reload_clip_;
        AudioClip* shooting_clip_;
        AudioClip* end_shooting_clip_;

        //bullet holes, stacked before serialization
        std::vector<Vector3> bullet_holes_;
        //AudioClip* laser_end_clip_;
    };
}