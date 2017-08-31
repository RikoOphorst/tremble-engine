#pragma once

namespace tremble
{
    struct dummy_info
    {
        int ID = -1;
        Vector3 position = Vector3(0, 0, 0);
        Vector3 size = Vector3(1, 1, 1);
        Vector3 rotation = Vector3();
        bool is_static = false;
    };

    class TargetDummy : public Component
    {
    public:
        std::vector<dummy_info*> dummyHolder;

        void Start()
        {
            //----------------------------------Small ones

            //Right Row
            AddTarget(Vector3(-10, 1.5f, -4));
            AddTarget(Vector3(-10, 1.5f, -9));
            AddTarget(Vector3(-10, 1.5f, -14));
            AddTarget(Vector3(-10, 1.5f, -19));
            AddTarget(Vector3(-10, 1.5f, -24));

            //Left Row
            AddTarget(Vector3(5, 1.5f, -4));
            AddTarget(Vector3(5, 1.5f, -9));
            AddTarget(Vector3(5, 1.5f, -14));
            AddTarget(Vector3(5, 1.5f, -19));
            AddTarget(Vector3(5, 1.5f, -24));

            //----------------------------------Shooting Range Ones

            //Row1
            AddTarget(Vector3(11, 2.5f, -24));
            AddTarget(Vector3(22, 2.5f, -24));
            AddTarget(Vector3(33, 2.5f, -24));

            //Row 2
            AddTarget(Vector3(16, 4.5f, -19));
            AddTarget(Vector3(28, 4.5f, -19));

            //Row 3
            AddTarget(Vector3(18.5f, 6.6f, -14));
            AddTarget(Vector3(25, 6.6f, -14));

            //Row 4
            AddTarget(Vector3(11, 8.5f, -9));
            AddTarget(Vector3(33, 8.5f, -9));

            //Row 5
            AddTarget(Vector3(16, 10.5f, -4));
            AddTarget(Vector3(22, 10.5f, -4));
            AddTarget(Vector3(28, 10.5f, -4));
        }

    private:
        SGNode* AddBox(Vector3 position, Vector3 size, Vector3 rotation, bool is_static)
        {
            SGNode* target = GetNode()->AddChild(is_static, position, rotation, size);
            Renderable* renderable = target->AddComponent<Renderable>();
            Model* model = Get::ResourceManager()->GetModel("../scenes/Max_Scene/models/Cube_4CAE760E46CEADAC6AA436B945B06A6C.obj", true);
            renderable->SetModel(model);
            if (is_static)
            {
                target->AddComponent<RigidbodyStatic>(&PhysicsBoxGeometry(size*0.5f), Get::PhysicsManager()->GetMaterial("default"));
            }
            else
            {
                target->AddComponent<RigidbodyDynamic>(&PhysicsBoxGeometry(size*0.5f), Get::PhysicsManager()->GetMaterial("default"));
            }
            SetDummyInfo(position, size, rotation, is_static);
            return target;
        };

        void AddTarget(Vector3 position)
        {
            AddBox(position, Vector3(0.25f, 0.75f, 0.25f), Vector3(), true);
            position.SetY(position.GetY().Get() + 1.4f);
            AddBox(position, Vector3(1.0f, 2.0f, 1.0f), Vector3(), false);
            position.SetY(position.GetY().Get() + 1.5f);
            AddBox(position, Vector3(0.75f, 0.75f, 0.75f), Vector3(), false);
        }

        void SetDummyInfo(Vector3 position, Vector3 size = Vector3(1, 1, 1), Vector3 rotation = Vector3(), bool is_static = false)
        {
            dummy_info* temp = new dummy_info();
            temp->position = position;
            temp->size = size;
            temp->rotation = rotation;
            temp->is_static = is_static;
            temp->ID = (int)dummyHolder.size();
            dummyHolder.push_back(temp);
        }
    };
};