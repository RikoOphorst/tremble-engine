#pragma once
#include "player.h"
#include "invoke_timer.h"

namespace tremble
{
    class DeathBox : public Component
    {
    public:
        void Awake()
        {
            SetOnTriggerEnter(&DeathBox::OnTriggerEnter, this);
        }

        void Update()
        {
        }

        static void AddDeaathBox(SGNode* node, TagData& tagData)
        {
            node->AddComponent<DeathBox>();
            node->AddComponent<TriggerCollider>(&PhysicsBoxGeometry(Vector3(1, 1, 1)));
        }

        void OnTriggerEnter(const Component& other_component)
        {
            if (other_component.GetType() == typeid(CharacterController))
            {
                other_component.GetNode()->AddComponent<InvokerTimer>()->SetOnComplete(other_component, [](const Component& other_component)
                {
                    other_component.GetNode()->FindComponent<Player>()->Spawn();
                });
            }
        }
    };
}