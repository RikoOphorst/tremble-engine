#include "player.h"
#include "components/grappling_hook.h"
#include "components/base_gun.h"
#include "grenade.h"
#include "death_screen.h"

namespace tremble
{
    PhysicsMaterial* Player::physics_material_ = nullptr;


    std::vector<SGNode*> Player::spawnpoints;
    void Player::Awake()
    {
        enabled_ = true;
        death_screen_ = GetNode()->AddComponent<DeathScreen>(this);
        character_controller_ = GetNode()->FindComponent<CharacterController>();
        if (character_controller_ != nullptr)
        {
            ///set height of character controller capsule
            normal_height_ = 1.0f;
            character_controller_->SetCrouchHeight(normal_height_);
            current_height_ = normal_height_;
            character_controller_->SetGravity(-20.0f);
        }
        else
        {
            DLOG("Character controller not found for player");
        }
        Camera* camera_component = GetNode()->FindComponentInChildren<Camera>();
        if (camera_component == nullptr)
        {
            const std::vector<SGNode*>& children = GetNode()->GetChildren();
            if (children.size() > 0)
            {
                camera_node_ = children[0];
            }
            else
            {
                camera_node_ = GetNode()->AddChild(false, Vector3(0.0f, 0.5f, 0.0f));
            }
        }
        else
        {
            camera_node_ = camera_component->GetNode();
        }

        Get::Renderer()->SetVirtualSize(1920, 1080);

        listener = GetNode()->FindComponent<AudioListener>();

        player_hitbox_head_node = GetNode()->AddChild(false);
        player_hitbox_head_node->SetLocalPosition(Vector3(0, 0.5, 0));
        player_head_hitbox_ = player_hitbox_head_node->AddComponent<TriggerCollider>(&PhysicsBoxGeometry(Vector3(0.25f, 0.3f, 0.25f)));
        player_head_hitbox_->tag = "Head";

        player_hitbox_body_node = GetNode()->AddChild(false);
        player_hitbox_body_node->SetLocalPosition(Vector3(0, -0.5, 0));
        player_body_hitbox_ = player_hitbox_body_node->AddComponent<TriggerCollider>(&PhysicsBoxGeometry(Vector3(0.5f, 0.7f, 0.5f)));
        player_body_hitbox_->tag = "Body";


        //Adds a gun to the camera
        gun_node_ = camera_node_->AddChild(false, camera_node_->GetPosition(), camera_node_->GetRotationRadians(), Vector3(1, 1, 1));


        //add an audiosource to the gun
        gun_node_->AddComponent<AudioSource>();
        SGNode* gun_muzzle_node_ = gun_node_->AddChild(false);
        gun_muzzle_node_->SetLocalPosition(gun_muzzle_node_->GetForwardVectorWorld()*0.5);
        gun_muzzle_node_->SetLocalPosition(gun_muzzle_node_->GetLocalPosition() + gun_muzzle_node_->GetUpVectorWorld() * 0.25);
        gun_muzzle_node_->AddComponent<ParticleSystem>()->SetSpace(System);

        //set the rotation of the camera
        camera_pitch_ = 0;
        camera_node_->SetLocalRotationDegrees(Vector3(Scalar(camera_pitch_), Scalar(0.0f), Scalar(0.0f)));


        //add a base gun types to the player
        pistol_ = gun_node_->AddComponent<BaseGun>();
        pistol_->my_predefined_type_ = pistol_->Pistol;
        pistol_->enabled = true;

        shot_gun_ = gun_node_->AddComponent<BaseGun>();
        shot_gun_->my_predefined_type_ = shot_gun_->Shotgun;

        laser_gun_ = gun_node_->AddComponent<BaseGun>();
        laser_gun_->my_predefined_type_ = laser_gun_->LaserGun;

        assualt_rifle_ = gun_node_->AddComponent<BaseGun>();
        assualt_rifle_->my_predefined_type_ = assualt_rifle_->AssualtRifle;

        //add all guns to an array
        all_guns_.push_back(pistol_);
        all_guns_.push_back(shot_gun_);
        all_guns_.push_back(laser_gun_);
        all_guns_.push_back(assualt_rifle_);
    }

    void Player::Start()
    {
        renderable_ = GetNode()->FindComponentInChildren<Renderable>();
        walk_source_ = GetNode()->AddComponent<AudioSource>();
        walking_clip_ = Get::ResourceManager()->GetAudioClip("footsteps.mp3");
        walk_source_->SetAudioClip(walking_clip_);
        walk_source_->SetVolume(0.3f);
        //set all weapons to unlocked for testing weapon scrolling
        pistol_->unlocked = true;
        assualt_rifle_->unlocked = true;
        shot_gun_->unlocked = true;
        laser_gun_->unlocked = true;


        current_gun_ = pistol_;
        current_gun_->enabled = true;
        Spawn();
    }

    void Player::SetupHUD()
    {
        ImageComponent* crosshair_ = camera_node_->AddComponent<ImageComponent>();
        Texture* cross_hair_texture_ = Get::ResourceManager()->GetTexture("cross.png");
        crosshair_->SetTexture(cross_hair_texture_);
        crosshair_->SetSize({ 50, 50 , 0 });
        crosshair_->SetPosition({ (float)Get::Renderer()->GetVirtualSizeX() / 2.0f, (float)Get::Renderer()->GetVirtualSizeY() / 2.0f, 0 });
        crosshair_->SetCenter({ 0.5f, 0.5f, 0 });
        crosshair_->SetColor({ 1,1,1,0.5 });



        //add text for all the player information
        ammo_counter_ = GetNode()->AddComponent<TextComponent>();
        ammo_counter_->SetFont("../../font/5thagent.ttf");
        ammo_counter_->SetCenter({ 0.5f, 0.5f, 0 });
        ammo_counter_->SetPosition(Vector3(Get::Renderer()->GetVirtualSizeX() - 50, Get::Renderer()->GetVirtualSizeY() - 25, 0));
        ammo_counter_->SetSize(50);
        ammo_counter_->SetLayer(1);

        shield_display_text_ = GetNode()->AddComponent<TextComponent>();
        shield_display_text_->SetFont("../../font/5thagent.ttf");
        shield_display_text_->SetSize(50);
        shield_display_text_->SetCenter({ 0.5f, 0.5f, 0 });
        shield_display_text_->SetPosition(Vector3(Get::Renderer()->GetVirtualSizeX() / 2.0f + 100, Get::Renderer()->GetVirtualSizeY() / 2.0f + 250, 0));
        shield_display_text_->SetColor({ 1, 1, 0 ,1 });
        shield_display_text_->SetLayer(1);

        health_display_text_ = GetNode()->AddComponent<TextComponent>();
        health_display_text_->SetFont("../../font/5thagent.ttf");
        health_display_text_->SetSize(50);
        health_display_text_->SetCenter({ 0.5f, 0.5f, 0 });
        health_display_text_->SetPosition(Vector3(Get::Renderer()->GetVirtualSizeX() / 2.0f - 100, Get::Renderer()->GetVirtualSizeY() / 2.0f + 250, 0));
        health_display_text_->SetLayer(1);

        ImageComponent* health_cross_ = GetNode()->AddComponent<ImageComponent>();
        Texture* health_cross_texuture_ = Get::ResourceManager()->GetTexture("health_cross.png");
        health_cross_->SetTexture(health_cross_texuture_);
        health_cross_->SetPosition(health_display_text_->GetPosition() - Vector3(100, 0, 0));
        health_cross_->SetCenter({ 0.5f,0.5f,0 });
        health_cross_->SetSize({ (float)health_cross_->GetSize().GetX() / 2.0f,(float)health_cross_->GetSize().GetY() / 2.0f ,0 });


        ImageComponent* shield_icon_ = GetNode()->AddComponent<ImageComponent>();
        Texture* shield_icon_texuture_ = Get::ResourceManager()->GetTexture("shield_icon.png");
        shield_icon_->SetTexture(shield_icon_texuture_);
        shield_icon_->SetPosition(shield_display_text_->GetPosition() + Vector3(100, 0, 0));
        shield_icon_->SetCenter({ 0.5f,0.5f,0 });
        shield_icon_->SetSize({ (float)shield_icon_->GetSize().GetX() / 2.0f,(float)shield_icon_->GetSize().GetY() / 2.0f ,0 });

        ImageComponent* ammo_icon_ = GetNode()->AddComponent<ImageComponent>();
        Texture* ammo_icon_texuture_ = Get::ResourceManager()->GetTexture("ammo_icon.png");
        ammo_icon_->SetTexture(ammo_icon_texuture_);
        ammo_icon_->SetPosition(ammo_counter_->GetPosition() - Vector3(100, 0, 0));
        ammo_icon_->SetCenter({ 0.5f,0.5f,0 });
        ammo_icon_->SetSize({ (float)ammo_icon_->GetSize().GetX() / 2.0f,(float)ammo_icon_->GetSize().GetY() / 2.0f ,0 });


        //text_background
        //add a background to the text displays
        ImageComponent* text_display_1_background_ = camera_node_->AddComponent<ImageComponent>();
        Texture* text_background_ = Get::ResourceManager()->GetTexture("text_background.png");
        text_display_1_background_->SetTexture(text_background_);
        text_display_1_background_->SetSize({ 150, 50 , 0 });
        text_display_1_background_->SetPosition(health_display_text_->GetPosition());
        text_display_1_background_->SetCenter({ 0.5f, 0.5f, 0 });
        text_display_1_background_->SetLayer(2);
        text_display_1_background_->SetColor({ 1, 1, 1, 0.25f
        });

        ImageComponent* text_display_2_background_ = camera_node_->AddComponent<ImageComponent>();
        text_display_2_background_->SetTexture(text_background_);
        text_display_2_background_->SetSize({ 150, 50 , 0 });
        text_display_2_background_->SetPosition(shield_display_text_->GetPosition());
        text_display_2_background_->SetCenter({ 0.5f, 0.5f, 0 });
        text_display_2_background_->SetLayer(2);
        text_display_2_background_->SetColor({ 1, 1, 1, 0.25f
        });

        ImageComponent* text_display_3_background_ = camera_node_->AddComponent<ImageComponent>();
        text_display_3_background_->SetTexture(text_background_);
        text_display_3_background_->SetSize({ 150, 50 , 0 });
        text_display_3_background_->SetPosition(ammo_counter_->GetPosition());
        text_display_3_background_->SetCenter({ 0.5f, 0.5f, 0 });
        text_display_3_background_->SetLayer(2);
        text_display_3_background_->SetColor({ 1, 1, 1, 0.25f
        });
    }



    void Player::Update()
    {
        if (!enabled_)
        {
            return;
        }


        listener->Set3DListener(camera_node_->GetPosition(), Vector3(0, 0, 0), camera_node_->GetForwardVectorWorld(), camera_node_->GetUpVectorWorld());

        if (GetNode()->FindComponent<GrapplingHook>()->GetGrappling() == false && GetNode()->FindComponent<GrapplingHook>()->GetAttachedToWall() == false)
        {
            if (Locomotion() == true)
            {
                HeadBobbing();
            }
        }


        Aiming();

        //handle Weapon Switching on input
        WeaponSwitching();


        Crouching();

        //reset position
        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_Q))
        {
            Spawn();
        }

        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_T))
        {
            OnPickUp(10, "Health");
            OnPickUp(10, "Ammo");
            OnPickUp(10, "Shield");
        }

        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_V) && character_controller_->IsOnGround())
        {
            ///forward hop when on the ground
            character_controller_->Jump(5);
            character_controller_->ForwardHop(20);
        }

        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_V) && GetNode()->FindComponent<GrapplingHook>()->GetAttachedToWall() == true)
        {
            ///forward hop when player is attached to wall
            GetNode()->FindComponent<GrapplingHook>()->RelaseWall(false);
            character_controller_->Jump(5);
            character_controller_->ForwardHop(20);
        }

        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_F))
        {
            Vector3 forward = camera_node_->GetForwardVectorWorld();
            Grenade::Spawn(GetNode()->GetPosition() + forward * 2.0f, forward);
        }


        //display health
        if (health_display_text_)
        {
            health_display_text_->SetText(std::to_string(health_points_));
        }

        //display shield
        if (shield_display_text_)
        {
            shield_display_text_->SetText(std::to_string(shield_points_));
        }
    }



    void Player::Crouching()
    {
        if (Get::InputManager()->GetKeyDown(TYPE_KEY_C))
        {
            current_height_ = Lerp(current_height_, crouching_height_, Get::DeltaT() * crouching_speed_);
            character_controller_->SetCrouchHeight(current_height_);
        }
        else
        {
            current_height_ = Lerp(current_height_, normal_height_, Get::DeltaT() * crouching_speed_);
            character_controller_->SetCrouchHeight(current_height_);
        }
    }

    void Player::HeadBobbing()
    {
        // Check if the player is on ground before head bobbing
        if (character_controller_->IsOnGround() == true)
        {
            // increase headbob value
            headbob_time_ += (headbob_speed_*sprint_headbob_multiplier_)*Get::DeltaT();

            Vector3 current_camera_pos_ = camera_node_->GetLocalPosition();
            // sin creates a value between minus 1 and 1 based on the value passed to it
            // +1/2 make sure it's between 0 and 1
            // mutliplied bt the the headbobbiness_ for the amount of movement
            camera_node_->SetLocalPosition(Vector3(float(current_camera_pos_.GetX()), ((std::sin(headbob_time_) + 1) / 2)*headbobbiness_, float(current_camera_pos_.GetZ())));
        }
    }
    void Player::WeaponSwitching()
    {
        //keyboard weapon switching
        for (int i = 0; i < all_guns_.size(); i++)
        {
            //before switcing check if the player unlocked the gun
            if (all_guns_[i]->unlocked == true)
            {
                //get 48+ the key the gun is assigned to on the keyboard
                if (Get::InputManager()->GetKeyPressed((KEY_TYPE)(49 + i)))
                {
                    ChangeWeapon(i);
                    return;
                }
            }
        }

        //scroll wheel weapon selecting
        float scroll_value_ = Get::InputManager()->GetMouseDeltaWheel();

        //check if the user is scrolling the wheel
        if (std::abs(scroll_value_) > 0.0f)
        {
            //find out what the new selected gun should be
            int i = current_selected_gun_ + Sign(scroll_value_);

            while (true)
            {
                //check if the index is out of range
                if (i < 0) i = all_guns_.size() - 1;
                if (i >= all_guns_.size()) i = 0;

                //check if the gun is unlocked
                if (all_guns_[i]->unlocked == true)
                {
                    ChangeWeapon(i);
                    return;
                }

                //if the index is eqaul to the selected gun you looped through all of them and could'nt find anything
                if (i == current_selected_gun_)
                {
                    return;
                }

                //increase i
                i += +Sign(scroll_value_);
            }
        }
    }
    void Player::ChangeWeapon(int changed_weapon_index_)
    {
        //set new gun
        current_gun_->enabled = false;
        current_gun_ = all_guns_.at(changed_weapon_index_);
        current_gun_->enabled = true;
        current_selected_gun_ = changed_weapon_index_;
    }

    void Player::Show()
    {
        renderable_->SetActive(true);
    }

    void Player::Hide()
    {
        renderable_->SetActive(false);
    }

    void Player::Enable()
    {
        enabled_ = true;
        Show();
        current_gun_->enabled = true;
    }

    void Player::Disable()
    {
        enabled_ = false;
        Hide();
        current_gun_->enabled = false;
    }

    void Player::GunSwaying()
    {
        //2d vector with the change of mouse position between the frames
        DirectX::XMINT2 mouse_delta = Get::InputManager()->GetMouseDeltaPos();

        //Get the new rotation y of the gun by multiplying the amount by the delta of the mouse so the larger the mouse delta value is the more sway happens
        float factor_y = mouse_delta.x*sway_amount_factor_yaw;
        float factor_x = mouse_delta.y*sway_amount_factor_pitch;

        //std::cout << mouse_delta.y << std::endl;

        //check if they don't go over there limits
        if (factor_x > max_sway_amount_factor_pitch)
        {
            factor_x = max_sway_amount_factor_pitch;
        }

        if (factor_x < -max_sway_amount_factor_pitch)
        {
            factor_x = -max_sway_amount_factor_pitch;
        }

        if (factor_y > max_sway_amount_factor_yaw)
        {
            factor_y = max_sway_amount_factor_yaw;
        }

        if (factor_y < -max_sway_amount_factor_yaw)
        {
            factor_y = -max_sway_amount_factor_yaw;
        }

        //Lerp between the old and new rotations
        gun_node_->SetLocalRotationDegrees(Vector3(Scalar(Lerp(gun_node_->GetLocalRotationDegrees().GetX(), -factor_x, Get::DeltaT()*sway_amount_factor_lerp_speed)), Scalar(Lerp(gun_node_->GetLocalRotationDegrees().GetY(), -factor_y, Get::DeltaT()*sway_amount_factor_lerp_speed)), Scalar(0)));
    }
    bool Player::Locomotion()
    {
        bool isWalking = false;
        float current_speed_ = movement_speed_;

        //Move the character forward only use sprint multiplier on forward movement or backwards
        if (Get::InputManager()->GetKeyDown(TYPE_KEY_W))
        {
            //increase speed when the player continues to walk forward
            current_sprint_multiplier += increase_amount_sprint_multiplier;
            current_sprint_multiplier = std::min(sprint_multiplier_, current_sprint_multiplier);

            isWalking = true;

            character_controller_->Move(GetNode()->GetForwardVectorWorld()*current_speed_*current_sprint_multiplier);
        }
        else if (Get::InputManager()->GetKeyDown(TYPE_KEY_S))
        {
            isWalking = true;
            character_controller_->Move(-GetNode()->GetForwardVectorWorld()*current_speed_);
        }

        //Move the character right or left
        if (Get::InputManager()->GetKeyDown(TYPE_KEY_D))
        {
            isWalking = true;
            character_controller_->Move(GetNode()->GetRightVectorWorld()*current_speed_);
        }
        else if (Get::InputManager()->GetKeyDown(TYPE_KEY_A))
        {
            isWalking = true;
            character_controller_->Move(-GetNode()->GetRightVectorWorld()*(current_speed_));
        }

        //makes the character jump
        if (Get::InputManager()->GetKeyPressed(TYPE_KEY_SPACE))
        {
            if (character_controller_->IsOnGround())
            {
                character_controller_->Jump(jump_height_);
            }
        }


        if (isWalking == true)
        {
            //Decrease speed when the player is not walking
            current_sprint_multiplier -= increase_amount_sprint_multiplier;
            current_sprint_multiplier = std::max(current_sprint_multiplier, 1.0f);

            if (character_controller_->IsOnGround())
            {
                walk_source_->PlayOnce();
            }
        }

        if (!character_controller_->IsOnGround() || !isWalking)
        {
            walk_source_->Stop();
        }



        return isWalking;
    }
    void Player::Aiming()
    {
        camera_pitch_ = camera_node_->GetLocalRotationDegrees().GetX();
        //2d vector with the change of mouse position between the frames
        DirectX::XMINT2 mouse_delta = Get::InputManager()->GetMouseDeltaPos();


        //TODO fix for camera looking at the ground on start because of mouse delta being high on start up
        //Temp fix
        if (mouse_delta.y > 128)
        {
            mouse_delta.y = 0;
            mouse_delta.x = 0;
        }

        //Rotates the object on it's yaw axis in radians
        GetNode()->RotateYRadians(Scalar(mouse_sensitivity_x_ * mouse_delta.x * static_cast<float>(Get::Config().mouse_sensitivity) / 100.0f));

        //increase the pitch by the delta of the mouse y and multiplied by its sensitivity
        camera_pitch_ += Scalar(mouse_sensitivity_y_ * mouse_delta.y * static_cast<float>(Get::Config().mouse_sensitivity) / 100.0f);

        //clamp camera to 90/-70
        if (camera_pitch_ > 0)
        {
            //returns the smallest number and sets that as the camera pitch
            camera_pitch_ = std::min((float)90, (float)camera_pitch_);
        }
        else
        {
            //returns the biggest number and sets that as the camera pitch
            camera_pitch_ = std::max((float)-70, (float)camera_pitch_);
        }

        //set the rotation of the camera
        camera_node_->SetLocalRotationDegrees(Vector3(Scalar(camera_pitch_), Scalar(0.0f), Scalar(0.0f)));

        GunSwaying();
    }

    void Player::Spawn()
    {
        if (!Get::Config().host)
        {
            return;
        }

        health_points_ = 100;
        shield_points_ = 100;

        if (spawnpoints.size() == 0)
        {
            GetNode()->SetPosition(Vector3(0, 2, 0));
            return;
        }

        //vector to store all object that are in the sphere
        std::vector<OctreeObject*> objects_hit;

        //sphere setup
        DirectX::BoundingSphere check_area_;
        check_area_.Radius = 15;

        int randomSpawnpoint = (int)std::rand() % spawnpoints.size();
        while (true)
        {
            //reset to zero if none were found
            if (randomSpawnpoint > spawnpoints.size() - 1)
            {
                check_area_.Radius--;
                randomSpawnpoint = 0;
            }

            //sphere setup
            check_area_.Center = spawnpoints[randomSpawnpoint]->GetPosition();

            //get all the object contained withing the  sphere
            objects_hit.clear();
            Get::Octree()->GetContainedObjects(check_area_, objects_hit);


            bool blocked = false;

            //loop through all object and check if there is a player
            for each (OctreeObject* object in objects_hit)
            {
                if (object->renderable->GetNode()->FindComponent<BaseGun>() != NULL)
                {
                    blocked = true;
                    break;
                }
            }

            //player was blocked
            if (blocked == true)
            {
                randomSpawnpoint++;
                continue;
            }

            //set position of the player
            GetNode()->SetPosition(spawnpoints[randomSpawnpoint]->GetPosition());
            GetNode()->SetLocalRotationQuaternion(spawnpoints[randomSpawnpoint]->GetLocalRotationQuaternion());
            return;
        }
    }


    float Player::Lerp(float a, float b, float t)
    {
        //return the interpolation  value between a and b
        return (1 - t)*a + t*b;
    }

    int Player::Sign(float value)
    {
        if (value > 0)
        {
            return 1;
        }
        else
        {
            return -1;
        }
    }


    void Player::OnDamage(int value)
    {
        //add damage to the shield
        shield_points_ -= value;

        //if the sheild hit points are negative add it to the player and reset the shield hit points to zero
        if (shield_points_ < 0)
        {
            health_points_ += shield_points_;
            shield_points_ = 0;
        }

        //if hp is below zero or zero game over
        if (health_points_ <= 0)
        {
            death_screen_->Enable();
            //			Spawn();
        }
    }

    void Player::OnPickUp(int value, std::string tag)
    {
        //add value to the correct container variable based on the tag
        if (tag == "Health")
        {
            health_points_ = std::min(health_points_ + value, 100);
        }
        else if (tag == "Shield")
        {
            shield_points_ = std::min(shield_points_ + value, 100);
        }
        else if (tag == "Ammo")
        {
            //Add ammo to all guns on pick up
            pistol_->ammo_ = std::min(pistol_->ammo_ + value, pistol_->ammo_capacity_);
            shot_gun_->ammo_ = std::min(shot_gun_->ammo_ + value, shot_gun_->ammo_capacity_);
            laser_gun_->ammo_ = std::min(laser_gun_->ammo_ + value, laser_gun_->ammo_capacity_);
            assualt_rifle_->ammo_ = std::min(assualt_rifle_->ammo_ + value, assualt_rifle_->ammo_capacity_);
        }
        else if (tag == "Weapon")
        {
            all_guns_[value]->unlocked = true;
        }

    }


    void Player::Serialize(RakNet::BitStream & packet)
    {
        if (Get::NetworkManager()->IsHost())
        {
            packet.Write(
                PlayerHealthData
            {
                health_points_,
                shield_points_
            }
            );
            //            packet.Write(
            //                enabled_
            //            );
        }
    }

    void Player::Deserialize(RakNet::BitStream & packet)
    {
        PlayerHealthData serialization_data = IPacketHandler::ReadPacketStruct<PlayerHealthData>(&packet);

        health_points_ = serialization_data.hp;
        shield_points_ = serialization_data.sp;

        //        bool enabled;
        //        packet.Read(enabled);
        //        if(enabled != enabled_)
        //        {
        //            enabled_ = enabled;
        //            if (enabled_)
        //                Enable();
        //            else
        //                Disable();
        //        }
    }
}
