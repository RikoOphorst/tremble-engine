#pragma once
#include "player.h"

namespace tremble
{
    class JumpPad : public Component
    {
    public:

        void Awake()
        {
            SetOnTriggerEnter(&JumpPad::OnTriggerEnter, this);
        }

        void Update()
        {
        }

        static void AddJumpPads(SGNode* node, TagData& tagData)
        {
            node->AddComponent<JumpPad>();
            node->AddComponent<TriggerCollider>(&PhysicsBoxGeometry(Vector3(0.5f, 0.5f, 0.5f)));
        }

        void OnTriggerEnter(const Component& other_component)
        {
            if (other_component.GetType() == typeid(CharacterController))
            {
                other_component.GetNode()->FindComponent<CharacterController>()->ZeroVelocities();
                other_component.GetNode()->FindComponent<CharacterController>()->Jump(23);
            }
        }
    };
}