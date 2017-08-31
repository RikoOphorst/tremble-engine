#include "grenade.h"
#include "player.h"

namespace tremble {

    PhysicsMaterial* Grenade::physics_material_ = nullptr;

    //------------------------------------------------------------------------------------------------------
    void Grenade::Awake()
    {
        if (!physics_material_)
        {
            physics_material_ = Get::PhysicsManager()->CreateMaterial("Grenade", 0.0f, 1.0f, 1.0f);
        }

        model_ = Get::ResourceManager()->GetModel("grenade/grenade.obj");
        renderable_ = GetNode()->AddComponent<Renderable>();
        renderable_->SetModel(model_);
        audio_ = GetNode()->AddComponent<AudioSource>();
        //GetNode()->AddComponent<TriggerCollider>(&PhysicsBoxGeometry(Vector3(1, 1, 1)));

        //create complex collider till we have mesh colliders
        std::vector<DirectX::XMFLOAT3> points;
        for (auto& herro : renderable_->GetOctreeObjects())
        {
            DirectX::XMFLOAT3 corners[8];
            herro->bounds.GetCorners(corners);

            for (int i = 0; i < 8; i++)
            {
                points.push_back(corners[i]);
            }
        }

        DirectX::BoundingSphere sphere;
        DirectX::BoundingSphere::CreateFromPoints(sphere, points.size(), &points[0], sizeof(DirectX::XMFLOAT3));

        rigidbody_ = GetNode()->AddComponent<RigidbodyDynamic>(&PhysicsSphereGeometry(sphere.Radius), physics_material_, PhysicsGeometry::ScalingType::RELATIVE_TO_PARENT);
        explosion_area_.Radius = explosion_radius_;

        grenade_explosion_clip = Get::ResourceManager()->GetAudioClip("grenade_explosion.mp3");
        grenade_hit_clip = Get::ResourceManager()->GetAudioClip("grenade_hit.mp3");
        SetOnTriggerEnter(&Grenade::OnTriggerEnter, this);
    }

    //------------------------------------------------------------------------------------------------------
    void Grenade::Update()
    {
        time_elapsed_ += Get::DeltaT();
        if (time_elapsed_ >= time_to_live_ && !exploded_)
        {
            Explode();
        }
    }

    //------------------------------------------------------------------------------------------------------
    void Grenade::Explode()
    {
        std::vector<OctreeObject*> objects_hit;
        explosion_area_.Center = GetNode()->GetPosition();
        Get::Octree()->GetContainedObjects(explosion_area_, objects_hit);
        ApplyForcesToObjectsHit(objects_hit);

        audio_->SetAudioClip(grenade_explosion_clip);
        audio_->Play();

        renderable_->SetActive(false);
        exploded_ = true;
    }

    //------------------------------------------------------------------------------------------------------
    SGNode* Grenade::Spawn(const Vector3& position, const Vector3& direction)
    {
        SGNode* grenade = Get::Scene()->AddChild(false, position, Vector3());
        Grenade* grenadeComponent = grenade->AddComponent<Grenade>();
        grenadeComponent->GetRigidBody()->AddImpulseAtPos(direction * 10.0f, position);
        return grenade;
    }

    //------------------------------------------------------------------------------------------------------
    void Grenade::ApplyForcesToObjectsHit(std::vector<OctreeObject*>& objects_hit)
    {
        if (objects_hit.size() == 0)
            return;

        //see if objects_hit contain rigidbodies, and save them
        std::vector<RigidbodyDynamic*> rigidbodies;
        for (auto& obj : objects_hit)
        {
            RigidbodyDynamic* rigidbody = obj->renderable->GetNode()->FindComponentInChildren<RigidbodyDynamic>();
            if (rigidbody != nullptr)
            {
                rigidbodies.push_back(rigidbody);
            }
        }

        //add impulses to all rigidbodies, and if rigidbodies have a Player component, damage them
        Vector3 grenade_pos = GetNode()->GetPosition();
        for (auto& rigidbody : rigidbodies)
        {
            if (rigidbody == rigidbody_)
                continue;

            Vector3 rigidbody_pos = rigidbody->GetNode()->GetPosition();
            Vector3 explosion_direction = (rigidbody_pos - grenade_pos).Normalize();
            Vector3 force = explosion_direction * 75.0f;
            rigidbody->AddImpulseAtPos(force, grenade_pos);

            Player* player = rigidbody->GetNode()->FindComponentInChildren<Player>();
            if (player != nullptr)
            {
                DamagePlayer(player, (rigidbody_pos - grenade_pos).Length());
            }

        }
    }

    //------------------------------------------------------------------------------------------------------
    void Grenade::DamagePlayer(Player* player, float distance_from_explosion)
    {
        float damage_percentage = distance_from_explosion / explosion_radius_;
        player->OnDamage(max_damage_ * damage_percentage);
    }

    //------------------------
    void Grenade::OnTriggerEnter(const Component& other_component)
    {
        if (!audio_->IsPlaying())
        {
            audio_->SetAudioClip(grenade_hit_clip);
            audio_->Play();
        }
    }
}
