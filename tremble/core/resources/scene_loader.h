#pragma once

namespace tremble
{
    class PhysicsGeometry;
    class SGNode;
    class Vector3;

    struct TagData
    {
        std::map<std::string, float> Floats;
        std::map<std::string, bool> Booleans;
        std::map<std::string, int> Integers;
        std::map<std::string, std::string> Strings;
    };

    /**
     * @class tremble::SceneLoader
     * @author Tim Sleddens
     * @brief This class exists to load custom exported .tmap (json) files from unreal editor.
     */
    class SceneLoader
    {
    public:
        SceneLoader(); //!< default constructor
        ~SceneLoader(); //!< default destructor

        void RegisterTag(std::function<void(SGNode* node, TagData& tag_data)> callback, std::string tag);

        /**
         * @brief Loads the scene into a given SGNode
         * @param[in] scene_file The path to the .tmap file
         * @param[in] scene_root_node The SGNode you want to attach the scene to
         */
        void LoadScene(const std::string& scene_file, SGNode* scene_root_node = nullptr);

    private:
        /**
         * @brief Attaches a child node on a given node, with given node data.
         * @param[in] node_data A nlohmann::json containing SGNode data
         * @param[in] attach_to The node you want to attach the new data to
         */
        void AttachNewNodeFromJsonData(const nlohmann::json& node_data, SGNode* attach_to);

        void AttachLightOntoNode(const nlohmann::json& light_data, SGNode* node);

        /**
         * @brief Loads a model and attaches it to the node so it's renderable
         * @param[in] node_data A nlohmann::json containing SGNode data
         * @param[in] attach_to The node you want to attach the renderable model to
         */
        void LoadModelOntoNode(const nlohmann::json& node_data, SGNode* attach_to);

        /**
        * @brief Loads a the collider(s) onto the node
        * @param[in] node_data A nlohmann::json containing SGNode data
        * @param[in] attach_to The node you want to attach the collider(s) to
        */
        void LoadCollidersOntoNode(const nlohmann::json& node_data, SGNode* sg_node);

        /**
         * @brief Sets the private folder_name_ variable to the name of the loaded scene's folder
         * @param[in] scene_file A string containing the path to the scene
         */
        void DetermineScenePath(const std::string& scene_file);

        /**
         * @brief Finds and returns a Vector3 position of given node data
         * @param[in] node_data A piece of nlohmann::json data you want to extract a position from
         */
        Vector3 ExtractPositionFromNode(const nlohmann::json& node_data);

        /**
        * @brief Finds and returns a Vector3 rotation of given node data
        * @param[in] node_data A piece of nlohmann::json data you want to extract a rotation from
        */
        Vector3 ExtractRotationFromNode(const nlohmann::json& node_data);

        /**
        * @brief Finds and returns a Vector3 scale of given node data
        * @param[in] node_data A piece of nlohmann::json data you want to extract a scale from
        */
        Vector3 ExtractScaleFromNode(const nlohmann::json& node_data);
        
        /**
         * @brief Checks if a node exists within given node data
         * @param[in] node_data A nlohmann::json piece of data you want to check against
         * @param[in] node_name The name of the node you want to check exists
         */
        bool HasNode(const nlohmann::json& node_data, const std::string& node_name);

        /**
         * @brief Attaches a dynamic or static rigidbody on a node
         * @param[in] is_static Determines if the rigidbody will be dynamic or static
         * @param[in] physics_geometry The PhysicsGeometry that will be added
         * @param[in] node The node you want to attach the rigidbody to
         */
        void AttachRigidbodyToNode(bool is_static, PhysicsGeometry* physics_geometry, SGNode* node);
        void AttachTriangleMeshCollider(bool is_static, SGNode* node);

        /**
        * @brief Loads tags and tagdata if there is any, and executes callbacks
        * @param[in] node The SGNode you want to pass back to the callback function
        * @param[in] node_data A nlohmann::json data structure you want to extract tag-data from
        */
        void ExecuteTagCallbacks(SGNode* node, const nlohmann::json& node_data);

        std::map<std::string, std::function<void(SGNode* node, TagData& tag_data)>> callbacks_;

        std::string folder_name_; //!< The folder name of the last loaded scene
    };

}
