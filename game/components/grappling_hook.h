#pragma once
#include "player.h"

namespace tremble
{
    class GrapplePoint : public Component
    {

    };

    class GrapplingHook : public Component
    {
    public:
        bool enabled = true; //can you use the hook

        bool grappling = false;

        float GetGrappling() { return grappling; }//!< returns the grappling state

        float GetAttachedToWall() { return attached_to_wall; }//!< returns the grappling state

        void RelaseWall(bool value)
        {
            grappling = false;
            attached_to_wall = false;
            character_controller->SetGravity(-20.0f);
        }//!< Release the player from the wall


        bool wait_for_ground_touch = true;//!< Needs to be on the ground before you can grapple again

        float grapple_distance = 50.0f;//!< Maximum grappling distance
        float grapple_speed = 50.0f;//!< Speed of grapple 

        float minimum_cancel_grapple_range = 3.0f;//!< The closest you have to be to your target before you attach to the wall
        //--------------------------------------------

        void Awake()
        {
            character_controller = GetNode()->FindComponent<CharacterController>();

            Camera* camera_component = GetNode()->FindComponentInChildren<Camera>();
            camera_node_ = camera_component->GetNode();

            current_pos = Vector3(0.0f, 0.0f, 0.0f);
            target = Vector3(0.0f, 0.0f, 0.0f);

            player = GetNode()->FindComponent<Player>();

            //grapple_speed *= 1000;

            source = player->gun_node_->FindComponent<AudioSource>();

            grapple_clip = Get::ResourceManager()->GetAudioClip("grapple_sound.mp3");
            grapple_clip_end = Get::ResourceManager()->GetAudioClip("grapple_sound_fin.mp3");

        }

        void Update()
        {
            if (enabled)
            {
                if (wait_for_ground_touch)
                {
                    if (Get::InputManager()->GetMousePressed(MOUSE_BUTTON_RIGHT) && character_controller->IsOnGround())// only for if you need to be on the ground before it registers
                    {
                        target = ShootHookRaycast();
                        if (target != Vector3())// Checks to see if the target is actually valid
                        {
                            current_pos = GetNode()->GetPosition();
                            PlayAudio(true);
                            grappling = true;
                            character_controller->SetGravity(0);// Sets the gravity to 0 so you don't fall
                        }
                    }
                }
                else
                {
                    if (Get::InputManager()->GetMousePressed(MOUSE_BUTTON_RIGHT))// Registers anywhere but doesn't work that well
                    {
                        target = ShootHookRaycast();
                        if (target != Vector3())
                        {
                            current_pos = GetNode()->GetPosition();
                            PlayAudio(true);
                            grappling = true;
                            character_controller->SetGravity(0);
                        }
                    }
                }

                if (grappling)// If this is true you're moving
                {
                    current_pos = GetNode()->GetPosition();
                    character_controller->Move((target - current_pos).Normalize() * grapple_speed);// The actual movement
                    float distance_to_target = GetDistance(GetNode()->GetPosition(), target);

                    if (distance_to_target <= minimum_cancel_grapple_range && distance_to_target >= 0.0f)
                    {
                        target = Vector3();
                        source->Stop();
                        PlayAudio(false);
                        attached_to_wall = true;// Stays attached to the wall at this point
                        grappling = false;
                    }
                }

                if (Get::InputManager()->GetMouseReleased(MOUSE_BUTTON_RIGHT))// Releasing the mouse will allow you to stop the grapple and fall whenever
                {
                    if (grappling)
                    {
                        Vector3 currentpos = character_controller->GetNode()->GetPosition();
                        Vector3 velocity = currentpos - last_pos;

                        //character_controller->Jump(velocity.GetY() * Scalar(10.0f));
                        //character_controller->ForwardHop(abs(velocity.GetX() + velocity.GetZ()* Scalar(50.0f)));

                        character_controller->DirectionalHop(velocity, 60);

                        source->Stop();
                        PlayAudio(false);
                    }
                    grappling = false;
                    attached_to_wall = false;
                    character_controller->SetGravity(-20.0f);
                }

                last_pos = character_controller->GetNode()->GetPosition();
            }
        }

        static void AddGrapplePoints(SGNode* node, TagData& tagData)
        {
            node->AddComponent<GrapplePoint>();
        }

    private:
        CharacterController* character_controller;
        SGNode* camera_node_;

        Vector3 target;
        Vector3 current_pos;

        bool attached_to_wall = false;

        AudioClip* grapple_clip;
        AudioClip* grapple_clip_end;

        AudioSource* source;

        Player* player;

        Vector3 last_pos;
        //--------------------------------------------

        Vector3 ShootHookRaycast()
        {
            std::vector<Component*> raycast_ignore;
            raycast_ignore.push_back(camera_node_->GetParent()->FindComponentInChildren<CharacterController>());

            for (int i = 0; i < GetNode()->FindAllComponents<TriggerCollider>().size(); i++)
            {
                raycast_ignore.push_back(GetNode()->FindAllComponents<TriggerCollider>()[i]);
            }

            Vector3 direction = camera_node_->GetForwardVectorWorld().Normalize();
            physx::PxRaycastBuffer raycast_buffer;

            if (Get::PhysicsManager()->Raycast(&raycast_buffer, camera_node_->GetPosition() + direction * 0.8f, direction, grapple_distance, &raycast_ignore))
            {
                Component* component = Get::PhysicsManager()->GetComponentFromRaycast(&raycast_buffer);

                Rigidbody* rigidbody = static_cast<Rigidbody*>(component);
                if (rigidbody != nullptr)
                {
                    if (component->GetNode()->FindComponent<GrapplePoint>())
                    {
                        return (Vector3)raycast_buffer.block.position;
                    }
                    return Vector3();
                }
                return Vector3();
            }
            return Vector3();
        }

        float GetDistance(Vector3 a, Vector3 b)
        {
            Vector3 holder = Vector3();

            holder.SetX(a.GetX() - b.GetX());
            holder.SetY(a.GetY() - b.GetY());
            holder.SetZ(a.GetZ() - b.GetZ());

            return sqrt(holder.GetX()*holder.GetX() + holder.GetY()*holder.GetY() + holder.GetY()*holder.GetY());
        }

        void PlayAudio(bool start)
        {
            if (start)
            {
                source->SetAudioClip(grapple_clip);
                source->Play();
            }
            else
            {
                source->SetAudioClip(grapple_clip_end);
                source->Play();
            }
        }
    };
}
