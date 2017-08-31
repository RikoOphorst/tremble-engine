#include "pick_up.h"
#include "player.h"

namespace tremble
{
    void PickUp::OnTriggerEnter(const Component& other_component)
    {

        if (other_component.GetType() == typeid(CharacterController))
        {
            other_component.GetNode()->FindComponent<Player>()->OnPickUp(amount, pick_up_type_);
            renderable_->SetActive(false);
        }
    }

    void PickUp::OnTriggerExit(const Component& other_component)
    {
        std::cout << "Trigger exit " << other_component.GetType().name() << std::endl;
    }

    void PickUp::Start()
    {
        SetOnTriggerEnter(&PickUp::OnTriggerEnter, this);
        SetOnTriggerExit(&PickUp::OnTriggerExit, this);
        //load model and set renderable
        renderable_ = GetNode()->FindComponentInChildren<Renderable>();
        renderable_->SetActive(true);
    }

    void PickUp::Update()
    {
        //Only check for distance if the renderable is active
        //else start respawn time
        if (renderable_->GetActive() == false)
        {
            if (respawn_timer_ >= respawn_time_)
            {
                renderable_->SetActive(true);
                respawn_timer_ = 0;
            }
            else
            {
                respawn_timer_ += Get::DeltaT();
            }
        }


        GetNode()->SetRotationDegrees(GetNode()->GetRotationDegrees() + Vector3(0, rotation_speed_y_ * Get::DeltaT(), 0));
    }

}
