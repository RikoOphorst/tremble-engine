#pragma once

namespace tremble
{
    class Player;

    class Grenade : public Component
    {
    public:

        void Awake();

        void Update();

        void Explode();

        static SGNode* Spawn(const Vector3& position, const Vector3& direction);

        RigidbodyDynamic* GetRigidBody() const { return rigidbody_; }

    private:

        void ApplyForcesToObjectsHit(std::vector<OctreeObject*>& objects_hit);
        void DamagePlayer(Player* player, float distance_from_explosion);
        void OnTriggerEnter(const Component& other_component);

        float max_damage_ = 10.0f;
        float time_to_live_ = 3.0f;
        float throwing_force_ = 10.0f;
        float explosion_radius_ = 5.0f;


        float time_elapsed_;
        bool exploded_ = false;


        Model* model_;
        Renderable* renderable_;
        RigidbodyDynamic* rigidbody_;
        DirectX::BoundingSphere explosion_area_;
        AudioSource* audio_;

        AudioClip* grenade_explosion_clip;
        AudioClip* grenade_hit_clip;

        static PhysicsMaterial* physics_material_;
    };

}