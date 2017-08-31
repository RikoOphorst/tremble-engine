#pragma once

namespace tremble
{
    class StartGameListener : public Component
    {
    public:
        void Start();
        void Update();

    private:
        bool has_pressed_start_ = false;
    };
}