#include "scene_loader.h"
#include "core/scene_graph/scene_graph.h"
#include "model_loader.h"
#include "resource_manager.h"
#include "components/physics/rigidbody_static.h"
#include "core/physics/physics_box_geometry.h"
#include "components/physics/rigidbody_dynamic.h"
#include "core/physics/physics_sphere_geometry.h"
#include "../../../game/components/line_rederer.h"
#include "core/physics/physics_convex_mesh_geometry.h"

namespace tremble
{

    //------------------------------------------------------------------------------------------------------
    SceneLoader::SceneLoader() :
        callbacks_(),
        folder_name_("")
    {
    }

    //------------------------------------------------------------------------------------------------------
    SceneLoader::~SceneLoader()
    {
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::RegisterTag(std::function<void(SGNode* node, TagData& tag_data)> callback, std::string tag)
    {
        if (callbacks_.find(tag) == callbacks_.end())
        {
            callbacks_[tag] = callback;
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::LoadScene(const std::string& scene_file, SGNode* scene_root_node)
    {
        std::ifstream stream;

        stream.open(scene_file);
        nlohmann::json scene_data = nullptr;
        if (stream.is_open())
        {
            DetermineScenePath(scene_file);
            stream >> scene_data;
            stream.close();
            if (scene_data["Name"].is_string())
            {
                std::cout << "Scene: " << scene_data["Name"] << " was opened." << std::endl;
            }
        }
        else
        {
            folder_name_ = "";
            std::cout << "Couldn't load scene file" << std::endl;
            return;
        }

        if (!scene_root_node)
        {
            scene_root_node = Get::Scene();
        }

        if(scene_data["SGNodes"].is_array())
        {
            for(nlohmann::json::iterator it = scene_data["SGNodes"].begin(); it != scene_data["SGNodes"].end(); ++it)
            {
                nlohmann::json node_data = *it;
                AttachNewNodeFromJsonData(node_data, scene_root_node);
            }
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::AttachNewNodeFromJsonData(const nlohmann::json& node_data, SGNode* attach_to)
    {
        Vector3 position    = ExtractPositionFromNode(node_data);
        Vector3 rotation    = ExtractRotationFromNode(node_data);
        Vector3 scale       = ExtractScaleFromNode(node_data);
        bool is_static      = node_data["IsStatic"].get<bool>();
        SGNode* sg_node     = attach_to->AddChild(is_static, position, rotation, scale);

        //if this node has children, add them onto the node
        if (HasNode(node_data, "Children") && node_data["Children"].is_array())
        {
            nlohmann::json children = node_data["Children"];
            for (nlohmann::json::iterator child = children.begin(); child != children.end(); ++child)
            {
                AttachNewNodeFromJsonData(*child, sg_node);
            }
        }

        if (HasNode(node_data, "Light") && node_data["Light"].is_object())
        {
            nlohmann::json light_data = node_data["Light"];
            AttachLightOntoNode(light_data, sg_node);
        }

        //if this node has a model, load the model onto the node
        if (HasNode(node_data, "Model") && node_data["Model"].is_string())
        {
            LoadModelOntoNode(node_data, sg_node);
            if(node_data["NameID"].get<std::string>() == "Brush")
            {
                Model* model = sg_node->FindComponentInChildren<Renderable>()->GetModel();
                sg_node->AddComponent<RigidbodyStatic>(model);
            }
            if (HasNode(node_data, "Colliders"))
            {
                LoadCollidersOntoNode(node_data, sg_node);
            }
        }

        ExecuteTagCallbacks(sg_node, node_data);
    }

    void SceneLoader::AttachLightOntoNode(const nlohmann::json& light_data, SGNode* node)
    {
        LightType light_type = light_data["LightType"].get<LightType>();

        float intensity = 1.0f;
        if (HasNode(light_data, "Intensity") && light_type != LightTypeDirectional)
        {
            intensity = light_data["Intensity"].get<float>();
        }

        Vector4 color = {
            light_data["ColorR"].get<float>(),// * intensity/5,
            light_data["ColorG"].get<float>(),// * intensity/5,
            light_data["ColorB"].get<float>(),// * intensity/5,
            1.0f
        };
        Light* light_component = node->AddComponent<Light>();
        light_component->SetLightType(light_type);
        light_component->SetColor(color);
        light_component->SetDirection(node->GetForwardVectorWorld());
        switch(light_type)
        {
        case LightTypeDirectional:
            light_component->SetShadowCasting(true);
            break;
        case LightTypePoint:
            light_component->SetFalloffEnd(light_data["FallofEnd"].get<float>());
            break;
        case LightTypeSpot:
            light_component->SetFalloffEnd(light_data["FallofEnd"].get<float>());
            light_component->SetConeAngle(light_data["Cone"].get<float>());
            break;
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::LoadModelOntoNode(const nlohmann::json& node_data, SGNode* attach_to)
    {
        std::string file_path = folder_name_ + node_data["Model"].get<std::string>();

        Model* model = Get::ResourceManager()->GetModel(file_path, false);
        Renderable* renderable = attach_to->AddComponent<Renderable>();
        renderable->SetModel(model);
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::LoadCollidersOntoNode(const nlohmann::json& node_data, SGNode* sg_node)
    {
        nlohmann::json colliders = node_data["Colliders"];

        bool is_static = sg_node->IsStatic();

        //load boxColliders
        if(HasNode(colliders, "Boxes") && colliders["Boxes"].is_array())
        {
            nlohmann::json boxColliders = colliders["Boxes"];
            for (nlohmann::json::iterator it = boxColliders.begin(); it != boxColliders.end(); ++it)
            {
                nlohmann::json box = *it;
                Vector3 half_extents(
                    box["ExtentX"].get<float>(),
                    box["ExtentY"].get<float>(),
                    box["ExtentZ"].get<float>()
                );
                AttachRigidbodyToNode(is_static, &PhysicsBoxGeometry(half_extents), sg_node);
            }
        }

        //load sphereColliders
        if(HasNode(colliders, "Spheres") && colliders["Spheres"].is_array())
        {
            nlohmann::json boxColliders = colliders["Spheres"];
            for (nlohmann::json::iterator it = boxColliders.begin(); it != boxColliders.end(); ++it)
            {
                nlohmann::json sphere = *it;
                float radius = sphere["Radius"].get<float>();
                AttachRigidbodyToNode(is_static, &PhysicsSphereGeometry(radius), sg_node);
            }
        }

        //load TriangleMeshColliders
        if(HasNode(colliders, "TriangleMesh"))
        {
            AttachTriangleMeshCollider(is_static, sg_node);
        }

        //load convexColliders
        if(HasNode(colliders, "Convex") && colliders["Convex"].is_array())
        {
            nlohmann::json convex_colliders = colliders["Convex"];
            for (nlohmann::json::iterator it = convex_colliders.begin(); it != convex_colliders.end(); ++it)
            {
                nlohmann::json convex_data = *it;
                if(HasNode(convex_data, "Vertices") && convex_data["Vertices"].is_array())
                {
                    nlohmann::json vert_data = convex_data["Vertices"];
                    std::vector<physx::PxVec3> verts(vert_data.size());
                    int counter = 0;
                    for (nlohmann::json::iterator vert_it = vert_data.begin(); vert_it != vert_data.end(); ++vert_it)
                    {
                        nlohmann::json vert = *vert_it;
                        verts[counter] = physx::PxVec3(
                            vert["X"].get<float>(),
                            vert["Y"].get<float>(),
                            vert["Z"].get<float>()
                        );
                        counter++;
                    }
                    AttachRigidbodyToNode(is_static, &ConvexMeshGeometry(verts), sg_node);
                }
            }
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::DetermineScenePath(const std::string& scene_file)
    {
        folder_name_ = scene_file;
        auto lastSlash = folder_name_.find_last_of('/');
		size_t num = ~0;
		if (lastSlash < (~(static_cast<size_t>(0))) - 6969)
		{
			folder_name_.replace(folder_name_.begin() + lastSlash + 1, folder_name_.end(), "");
		}
		else
		{
			lastSlash = folder_name_.find_last_of('\\');
			folder_name_.replace(folder_name_.begin() + lastSlash + 1, folder_name_.end(), "");
		}
		
        std::cout << "path to scene: " << folder_name_ << std::endl;
    }

    //------------------------------------------------------------------------------------------------------
    Vector3 SceneLoader::ExtractPositionFromNode(const nlohmann::json& node_data)
    {
        return Vector3(
            node_data["PositionX"].get<float>(),
            node_data["PositionY"].get<float>(),
            node_data["PositionZ"].get<float>()
        );
    }

    //------------------------------------------------------------------------------------------------------
    Vector3 SceneLoader::ExtractRotationFromNode(const nlohmann::json& node_data)
    {
        return Vector3(
            node_data["EulerX"].get<float>(),
            node_data["EulerY"].get<float>(),
            node_data["EulerZ"].get<float>()
        );
    }

    //------------------------------------------------------------------------------------------------------
    Vector3 SceneLoader::ExtractScaleFromNode(const nlohmann::json& node_data)
    {
        return Vector3(
            node_data["ScaleX"].get<float>(),
            node_data["ScaleY"].get<float>(),
            node_data["ScaleZ"].get<float>()
        );
    }

    //------------------------------------------------------------------------------------------------------
    bool SceneLoader::HasNode(const nlohmann::json& node_data, const std::string& node_name)
    {
        return node_data.find(node_name) != node_data.end();
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::AttachRigidbodyToNode(bool is_static, PhysicsGeometry* physics_geometry, SGNode* node)
    {
        if(is_static)
        {
            node->AddComponent<RigidbodyStatic>(physics_geometry, nullptr, PhysicsGeometry::ScalingType::RELATIVE_TO_PARENT);
        }
        else
        {
            node->AddComponent<RigidbodyDynamic>(physics_geometry, nullptr, PhysicsGeometry::ScalingType::RELATIVE_TO_PARENT);
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::AttachTriangleMeshCollider(bool is_static, SGNode* node)
    {
        Renderable* renderable = node->FindComponentInChildren<Renderable>();
        if (!renderable)
            return;

        Model* model = renderable->GetModel();
        if (!model)
            return;

        if(is_static)
        {
            node->AddComponent<RigidbodyStatic>(model, nullptr, PhysicsGeometry::ScalingType::RELATIVE_TO_PARENT);
        }
        else
        {
            node->AddComponent<RigidbodyDynamic>(model, nullptr, PhysicsGeometry::ScalingType::RELATIVE_TO_PARENT);            
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SceneLoader::ExecuteTagCallbacks(SGNode* node, const nlohmann::json& node_data)
    {
        //if this node has tags and tag-data, add them onto the node
        if (HasNode(node_data, "Tags") && node_data["Tags"].is_array())
        {
            TagData tag_data; //!< instantiate struct to pass to callback function

                              //extract tag data
            if (HasNode(node_data, "TagData"))
            {
                nlohmann::json tag_data_node = node_data["TagData"]; //!< json node holding "TagData"

                //extract floats
                if (HasNode(tag_data_node, "Floats") && tag_data_node["Floats"].is_array())
                {
                    nlohmann::json floats = tag_data_node["Floats"];
                    for (nlohmann::json::iterator it = floats.begin(); it != floats.end(); ++it)
                    {
                        nlohmann::json float_data = *it;
                        std::string key = float_data["Key"].get<std::string>();
                        float value = float_data["Value"].get<float>();
                        tag_data.Floats[key] = value;
                    }
                }

                //extract booleans
                if (HasNode(tag_data_node, "Booleans") && tag_data_node["Booleans"].is_array())
                {
                    nlohmann::json booleans = tag_data_node["Booleans"];
                    for (nlohmann::json::iterator it = booleans.begin(); it != booleans.end(); ++it)
                    {
                        nlohmann::json bool_data = *it;
                        std::string key = bool_data["Key"].get<std::string>();
                        bool value = bool_data["Value"].get<bool>();
                        tag_data.Booleans[key] = value;
                    }
                }

                //extract integers
                if (HasNode(tag_data_node, "Integers") && tag_data_node["Integers"].is_array())
                {
                    nlohmann::json integers = tag_data_node["Integers"];
                    for (nlohmann::json::iterator it = integers.begin(); it != integers.end(); ++it)
                    {
                        nlohmann::json integer_data = *it;
                        std::string key = integer_data["Key"].get<std::string>();
                        int value = integer_data["Value"].get<int>();
                        tag_data.Integers[key] = value;
                    }
                }

                //extract strings
                if (HasNode(tag_data_node, "Strings") && tag_data_node["Strings"].is_array())
                {
                    nlohmann::json strings = tag_data_node["Strings"];
                    for (nlohmann::json::iterator it = strings.begin(); it != strings.end(); ++it)
                    {
                        nlohmann::json string_data = *it;
                        std::string key = string_data["Key"].get<std::string>();
                        std::string value = string_data["Value"].get<std::string>();
                        tag_data.Strings[key] = value;
                    }
                }
            }

            //execute callbacks
            nlohmann::json tags_node = node_data["Tags"]; //!< json node holding "Tags"
            for (nlohmann::json::iterator it = tags_node.begin(); it != tags_node.end(); ++it)
            {
                nlohmann::json tag = *it;
                std::string tag_name = tag["Value"].get<std::string>();
                if (callbacks_.find(tag_name) != callbacks_.end())
                {
                    callbacks_[tag_name](node, tag_data);
                }
            }
        }
    }
}
