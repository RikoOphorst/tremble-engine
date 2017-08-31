#pragma once
namespace tremble
{
    class PickUp : public Component
    {
    public:
        void OnTriggerEnter(const Component& other_component);
        void OnTriggerExit(const Component& other_component);
        void Start();
        void Update();

        Renderable* renderable_;
        std::string pick_up_type_ = "Health";
        float amount = 10;

        float rotation_speed_y_ = 50;

        float respawn_timer_ = 0;
        float respawn_time_ = 2.5f;

    };
}