#pragma once

namespace tremble
{
    class CollisionTest : public Component
    {
    public:
        CollisionData fake_data;

        std::function<void(const CollisionData&)> stdfunc;

        void OnCollision(const CollisionData& collision_data)
        {
            std::cout << "wow, collision callback " << collision_data.other_component->GetType().name() << std::endl;
        }

        void Start()
        {
            SetCollisionCallback(&CollisionTest::OnCollision, this);
        }

    };
}