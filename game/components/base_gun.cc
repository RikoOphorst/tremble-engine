#include "base_gun.h"
#include "player.h"
#include "pick_up.h"

namespace tremble
{
    ParticleSystem* BaseGun::bullet_hole_;
    void BaseGun::Start()
    {
        //particle emitter set up
        pe = GetNode()->FindComponentInChildren<ParticleSystem>()->GetNode()->AddComponent<ParticleEmitter>();

        pe->AssignToSystem(GetNode()->FindComponentInChildren<ParticleSystem>());
        ParticleType p;
        p.color = { 0,0,0,1 };
        p.color_random = { 0,0,0,0 };
        p.colorEnd = { 1,1,1,0 };
        p.colorEnd_random = { 0,0,0,0 };
        p.lifetime = 0.1f;
        p.lifetime_random = 1;
        p.size = .01f;
        p.sizeEnd = .05f;
        p.size_random = 0;
        p.sizeEnd_random = 0;
        p.velocity = { 0,0.2f,0.1f };
        p.velocity_random = { 0.02f,0,0.2f };
        pe->SetSpawnArea({ 0.1f,0.1f,0.1f });

        pe->SetParticle(p);
        pe->SetMode(Passive);


        camera_node_ = GetNode()->GetParent()->FindComponentInChildren<Camera>()->GetNode();
        gun_model_renderable_ = GetNode()->AddComponent<Renderable>();
        //my_predefined_type_ = LaserGun;

        if (my_predefined_type_ == Pistol)
        {
            //set all variables here
            ammo_capacity_ = 20;

            ammo_ = ammo_capacity_;

            reload_speed_ = 0.5f;
            fire_rate_ = 0.0f;

            can_fire_ = true;
            can_fire_timer_ = 0;

            reload_time_ = 1;
            current_reload_time_ = 0;
            reloading_ = false;

            recoil_ = 0.01f;
            recoil_return_speed_ = 2.5f;
            recoil_effect_camera_ = 0.5f;

            gun_fire_types_ = Trigger;

            spread_amount_ = 0.02f;

            bullet_travel_distance_ = 30;

            spray_amount_ = 1;

            model_path_name_ = "pistol/pistol.obj";

            gun_start_position_ = Vector3(0.4f, -0.4f, 0.5f);

            damage_ = 10;

            //load the gun shot sound
            shooting_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/pistol_fire.mp3");

            //load the reload shot sound
            reload_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/pistol_reload.mp3");
        }
        else if (my_predefined_type_ == Shotgun)
        {
            //set all variables here
            ammo_capacity_ = 10;

            ammo_ = ammo_capacity_;

            reload_speed_ = 0.5f;
            fire_rate_ = 0.25f;

            can_fire_ = true;
            can_fire_timer_ = 0;

            reload_time_ = 1;
            current_reload_time_ = 0;
            reloading_ = false;

            gun_fire_types_ = Trigger;

            recoil_ = 0.5f;
            recoil_return_speed_ = 2.5f;
            recoil_effect_camera_ = 0.5f;

            spread_amount_ = 0.1f;

            bullet_travel_distance_ = 30;

            spray_amount_ = 70;

            model_path_name_ = "shotgun/shotgun.obj";
            gun_start_position_ = Vector3(0.4f, -0.4f, 0.5f);

            damage_ = 1;

            //load the gun shot sound
            shooting_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/shotgun_fire.mp3");

            //load the reload shot sound
            reload_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/shotgun_reload.mp3");
        }
        else if (my_predefined_type_ == LaserGun)
        {
            //set all variables here
            ammo_capacity_ = 500;

            ammo_ = ammo_capacity_;

            reload_speed_ = 0.5f;
            fire_rate_ = 0.0f;

            can_fire_ = true;
            can_fire_timer_ = 0;

            reload_time_ = 1;
            current_reload_time_ = 0;
            reloading_ = false;

            gun_fire_types_ = Laser;

            recoil_ = 0.01f;
            recoil_return_speed_ = 2.5f;
            recoil_effect_camera_ = 0.5f;

            spread_amount_ = 0.0f;

            bullet_travel_distance_ = 100;

            spray_amount_ = 1;

            model_path_name_ = "heavy_gun/heavy_gun.obj";
            gun_start_position_ = Vector3(0.4f, -0.4f, 0.0f);

            damage_ = 1;

            //load the gun shot sound
            shooting_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/laser_firing_sound.mp3");

            end_shooting_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/laser_end_sound.mp3");

            //load the reload shot sound
            reload_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/laser_reload.mp3");
        }
        else if (my_predefined_type_ == AssualtRifle)
        {
            //set all variables here
            ammo_capacity_ = 120;

            ammo_ = ammo_capacity_;

            reload_speed_ = 0.5f;
            fire_rate_ = 0.1f;

            can_fire_ = true;
            can_fire_timer_ = 0;

            reload_time_ = 1;
            current_reload_time_ = 0;
            reloading_ = false;

            recoil_ = 0.05f;
            recoil_return_speed_ = 2.5f;
            recoil_effect_camera_ = 0.5f;

            gun_fire_types_ = Automatic;

            spread_amount_ = 0.02f;

            bullet_travel_distance_ = 30;

            spray_amount_ = 1;

            model_path_name_ = "marauder/marauder.obj";
            gun_start_position_ = Vector3(0.4f, -0.4f, 0.5f);

            damage_ = 5;

            //load the gun shot sound
            shooting_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/machinegun_fire.mp3");

            //load the reload shot sound
            reload_clip_ = Get::ResourceManager()->GetAudioClip("GunSounds/pistol_reload.mp3");
        }

        gun_model_ = Get::ResourceManager()->GetModel(model_path_name_);

        SetModel();


        GetNode()->FindComponentInChildren<ParticleSystem>()->SetTexture(Get::ResourceManager()->GetTexture("smoke.png"));
        //load the reload shot sound
        //reload_clip_ = Get::ResourceManager()->GetAudioClip("gun_reload.mp3");
    }


    void BaseGun::RepositionMuzzle()
    {
        if (enabled == true)
        {
            SGNode* muzzleNode = GetNode()->FindComponentInChildren<ParticleSystem>()->GetNode();
            if (my_predefined_type_ == Pistol)
            {
                muzzleNode->SetLocalPosition(gun_start_position_ + Vector3(-0.1, 0.75f, 0.5f));
            }
            else if (my_predefined_type_ == Shotgun)
            {
                muzzleNode->SetLocalPosition(gun_start_position_ + Vector3(0, 0.5f, 1.7f));
            }
            else if (my_predefined_type_ == LaserGun)
            {
                muzzleNode->SetLocalPosition(gun_start_position_ + Vector3(0, 1.0f, 4.0f));
            }
            else if (my_predefined_type_ == AssualtRifle)
            {
                muzzleNode->SetLocalPosition(gun_start_position_ + Vector3(0, 0.75f, 1.70f));
            }
        }
    }
    void BaseGun::Update()
    {
        if (enabled == true)
        {
            //Set all text of hud elements
            //Display ammo and full ammo clip size
            if (GetNode()->GetParent()->GetParent()->FindComponentInChildren<Player>()->ammo_counter_)
            {
                GetNode()->GetParent()->GetParent()->FindComponentInChildren<Player>()->ammo_counter_->SetText(std::to_string(ammo_));
            }

            Reloading();
            RecoilCoolDown();
            FireRate();

            if (reloading_ == false)
            {
                if (ammo_ != 0)
                {
                    if (gun_fire_types_ == Automatic)
                    {
                        if (Get::InputManager()->GetMouseDown(MOUSE_BUTTON_LEFT) && can_fire_ == true)
                        {
                            Shooting(true);
                        }
                    }
                    else if (gun_fire_types_ == Trigger)
                    {
                        if (Get::InputManager()->GetMouseReleased(MOUSE_BUTTON_LEFT) && can_fire_ == true)
                        {
                            Shooting(true);
                        }
                    }
                    else if (gun_fire_types_ == Laser)
                    {
                        if (Get::InputManager()->GetMouseDown(MOUSE_BUTTON_LEFT))
                        {
                            Shooting(false);
                        }
                        else if (Get::InputManager()->GetMouseReleased(MOUSE_BUTTON_LEFT))
                        {
                            GetNode()->FindComponent<AudioSource>()->Stop();
                            GetNode()->FindComponent<AudioSource>()->SetAudioClip(end_shooting_clip_);
                            GetNode()->FindComponent<AudioSource>()->PlayOnce();
                        }
                    }
                }
            }
        }
        gun_model_renderable_->SetActive(enabled);
        RepositionMuzzle();
    }

    void BaseGun::SetModel()
    {
        gun_model_renderable_->SetModel(gun_model_);


        //set scale of the gun
        GetNode()->SetLocalScale(Vector3(0.5f, 0.5f, 0.5f));
    }

    void BaseGun::RecoilCoolDown()
    {
        //moves the gun according to the applied recoil offset
        GetNode()->SetLocalPosition(gun_start_position_ - Vector3(0, 0, -1)*current_recoil_offset_);


        Vector3 current_camera_pos_ = camera_node_->GetLocalPosition();
        //moves the camera according to the applied recoil offset plus an amount float
        camera_node_->SetLocalPosition(Vector3((float)current_camera_pos_.GetX(), (float)current_camera_pos_.GetY(), -1 * current_recoil_offset_*recoil_effect_camera_));

        //cooldown recoil
        current_recoil_offset_ = Lerp(current_recoil_offset_, 0, Get::DeltaT()*recoil_return_speed_);
    }

    float BaseGun::Lerp(float a, float b, float t)
    {
        //return the interpolation  value between a and b
        return (1 - t)*a + t*b;
    }

    void BaseGun::Shooting(bool soundLoop)
    {
        if (Get::NetworkManager()->IsHost())
        {//if the left mouse button is pressed fire
            can_fire_ = false;


            //set the shooting clip to the gun's audiosource
            GetNode()->FindComponent<AudioSource>()->SetAudioClip(shooting_clip_);

            ammo_--;

            //play shot sound
            if (soundLoop)
            {
                GetNode()->FindComponent<AudioSource>()->Play();
            }
            else
            {
                GetNode()->FindComponent<AudioSource>()->PlayOnce();
            }

            //add recoil to the gun offset
            current_recoil_offset_ -= recoil_;

            //spray bullets
            for (int i = 0; i < spray_amount_; i++)
            {
                //get the forward direction of the camera to use for raycasting
                Vector3 direction = (camera_node_->GetForwardVectorWorld() + Vector3(RandomRange(-spread_amount_, spread_amount_), RandomRange(-spread_amount_, spread_amount_), RandomRange(-spread_amount_, spread_amount_))).Normalize();


                //buffer which stores all the data for the raycast
                physx::PxRaycastBuffer raycast_buffer;


                Vector3 orgin_for_ray_ = camera_node_->GetPosition();

                //ignored raycast layer
                std::vector<Component*> raycast_ignore;

                std::vector<CharacterController*> all_characters = camera_node_->GetParent()->FindAllComponents<CharacterController>();
                for each(CharacterController* character in all_characters)
                {
                    raycast_ignore.push_back(character);
                }

                std::vector<TriggerCollider*>all_trigggers_ = Get::Scene()->FindAllComponents<TriggerCollider>();

                for each (TriggerCollider* trigger in all_trigggers_)
                {
                    //GetNode()->GetParent()->FindComponent<Player>()->player_head_hitbox_;
                    if (trigger == GetNode()->GetParent()->GetParent()->FindComponent<Player>()->player_head_hitbox_)
                    {
                        raycast_ignore.push_back(trigger);
                        continue;
                    }

                    if (trigger == GetNode()->GetParent()->GetParent()->FindComponent<Player>()->player_body_hitbox_)
                    {
                        raycast_ignore.push_back(trigger);
                        continue;
                    }
                }

                //raycasting in front + spread offset of the character
                if (Get::PhysicsManager()->Raycast(&raycast_buffer, orgin_for_ray_, direction, bullet_travel_distance_, &raycast_ignore))
                {

                    //Returns a component that was hit by the raycast
                    Component* component = Get::PhysicsManager()->GetComponentFromRaycast(&raycast_buffer);


                    if (component != nullptr)
                    {
                        if (component->GetType() == typeid(TriggerCollider))
                        {
                            TriggerCollider* trigger_collider_ = static_cast<TriggerCollider*>(component);

                            if (trigger_collider_->tag == "Head")
                            {
                                DLOG("HIT THEM IN THE HEAD");
                                component->GetNode()->GetParent()->FindComponent<Player>()->OnDamage(damage_ * 2);
                            }
                            else if (trigger_collider_->tag == "Body")
                            {
                                DLOG("HIT THEM IN THE BODY");
                                component->GetNode()->GetParent()->FindComponent<Player>()->OnDamage(damage_);
                            }
                        }

                        if (component->GetType() == typeid(RigidbodyStatic))
                        {
                            //data to spawn a single particle 
                            ParticleDescription p;
                            p.lifetime = 30;
                            p.size = 0.15f;
                            p.sizeEnd = p.size;
                            p.color = { 1, 1, 1, 1 };
                            p.colorEnd = { 1, 1, 1, 0 };
                            p.velocity = { 0, 0, 0 };
                            //set the position of the particl on the position of the raycast hit
                            p.position = raycast_buffer.block.position + raycast_buffer.block.normal*p.size / 2;
                            BaseGun::bullet_hole_->Spawn(p);
                            bullet_holes_.push_back(p.position);
                        }
                    }
                }
            }
        }
    }

    void BaseGun::Reloading()
    {
        //check if the reload key is pressed and only when the gun is not reloading
        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_R) && reloading_ == false && ammo_ != ammo_capacity_)
        {
            reloading_ = true;


            //set the reload clip to the gun's audiosource
            GetNode()->FindComponent<AudioSource>()->SetAudioClip(reload_clip_);

            //play relaod sound
            GetNode()->FindComponent<AudioSource>()->Play();
        }
        else if (reloading_ == true)
        {
            //check if the current time it take to reach has reached the reload time
            if (current_reload_time_ <= reload_time_)
            {
                //add time to the reloading time
                current_reload_time_ += Get::DeltaT();


                //set rotation of the gun based on the current reload time
                GetNode()->SetLocalRotationDegrees(Vector3(Scalar(45 * std::sin(current_reload_time_) + 1 / 2), Scalar(0), Scalar(0)));
            }
            else
            {
                std::cout << "Reloading" << std::endl;
                //if the reload time has been reached reset the timer
                //and set the ammo to full
                //and stop reloading
                current_reload_time_ = 0;
                ammo_ = ammo_capacity_;
                reloading_ = false;
            }
        }
    }

    void BaseGun::FireRate()
    {
        //fire rate handling
        if (can_fire_ == false)
        {
            if (can_fire_timer_ >= fire_rate_)
            {
                can_fire_timer_ = 0;
                can_fire_ = true;
            }
            can_fire_timer_ += Get::DeltaT();
        }
    }

    float BaseGun::RandomRange(float low, float high)
    {
        //std::cout <<  (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (high - low) << std::endl;

        //return a float between low and high
        return  low + (static_cast <float> (rand()) / static_cast <float> (RAND_MAX)) * (high - low);
    }

    void BaseGun::Serialize(RakNet::BitStream& packet)
    {
        packet.Write(bullet_holes_.size());
        for (int i = 0; i < bullet_holes_.size(); i++)
        {
            packet.Write(bullet_holes_[i]);
        }
        bullet_holes_.clear();
    }

    void BaseGun::Deserialize(RakNet::BitStream & packet)
    {
        Vector3 bullet_hole;
        ParticleDescription p;
        p.lifetime = 30;
        p.size = 0.15f;
        p.sizeEnd = p.size;
        p.color = { 1, 1, 1, 1 };
        p.colorEnd = { 1, 1, 1, 0 };
        p.velocity = { 0, 0, 0 };
        size_t amount_of_holes;
        packet.Read(amount_of_holes);
        for (int i = 0; i < amount_of_holes; i++)
        {
            packet.Read(bullet_hole);
            //set the position of the particl on the position of the raycast hit
            p.position = bullet_hole;
            BaseGun::bullet_hole_->Spawn(p);
        }
    }
}
