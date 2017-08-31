#pragma once

#include "../scene_graph/scene_graph.h"
#include "../rendering/material.h"
#include "../resources/mesh.h"
#include "../memory/memory_includes.h"
#include "../resources/model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing fla
#include <assimp/version.h>

#include <string>

#define GET_COLOR(key, map, use_map, game_man, tx_type, mat_key) \
if (material->GetTextureCount(tx_type) > 0) \
{\
	aiString path;\
	aiReturn returned = material->GetTexture(tx_type, 0, &path);\
	assert(returned == aiReturn_SUCCESS);\
\
	map = new Texture(path.C_Str());\
	use_map = true;\
}\
else\
{\
	aiColor4D color;\
	assert(material->Get(mat_key, color) == aiReturn_SUCCESS);\
	key = DirectX::XMFLOAT3(color.r, color.g, color.b);\
}

namespace tremble
{
	/**
	* @class tremble::ModelLoader
	* @author Riko Ophorst
	* @brief This class exists only to encapsulate the static LoadModel functions
	*/
	class ModelLoader
	{
	private:
		ModelLoader(); //!< Default constructor
		~ModelLoader(); //!< Default destructor
		
	public:
		/**
		* @brief Loads a model into memory using the supplied model allocator
		* @param[in] model_path The path to the model you wish to load
		* @param[in] model_allocator The allocator that should be used to allocate the actual Model structure
		*/
		static Model* LoadModel(const std::string& model_path, Allocator* model_allocator);

	protected:
		/**
		* @brief Processes an array of meshes - outputting an array of tremble-compatible meshes
		* @param[in] meshes An array of meshes loaded in by Assimp
		* @param[in] num_meshes The number of meshes in the meshes array
		* @param[in] allocator The allocator that should be used to allocate the tremble-compatible meshes
		* @param[out] out_meshes An array of tremble-compatible meshes
		*/
		static void ProcessMeshes(aiMesh** meshes, unsigned int num_meshes, Allocator* allocator, std::vector<Mesh*>& out_meshes, std::vector<Model::Bone>& out_bones, std::unordered_map<std::string, unsigned int>& name_to_bone_mapping);
		
		/**
		* @brief Processes an array of materials - outputting an array of tremble-compatible materials
		* @param[in] rel_path The path to which all the materials' textures are relative
		* @param[in] materials An array of materials loaded in by Assimp
		* @param[in] num_materials The number of materials in the materials array
		* @param[in] material_allocator The allocator that should be used to allocate all the tremble-compatible materials
		* @param[in] texture_allocator The allocator that should be used to allocate all the tremble-compatible textures
		* @param[out] out_materials An array of tremble-compatible materials
		* @param[out] out_textures An array of tremble-compatible textures
		*/
		static void ProcessMaterials(const std::string& rel_path, aiMaterial** materials, unsigned int num_materials, Allocator* material_allocator, Allocator* texture_allocator, std::vector<Material*>& out_materials, std::vector<Texture*>& out_textures);
		
		/**
		* @brief Processes a hierarchy of nodes - outputting a hierarchy of model nodes
		* @param[in] root_node The root node of the hierarchy from which the algorithm will traverse down
		* @param[in] meshes An array of meshes of which the indices match up with the aiMesh array in the aiScene that was used to load in the model
		* @param[in] model_node_allocator The allocator that should be used to allocate all the Model::ModelNode structs
		* @param[in] out_root_node The root node of the outputted node hierarchy
		* @param[in] out_model_nodes A flat array of all model nodes in the hierarchy
		*/
		static void ProcessNodes(aiNode* root_node, const std::vector<Mesh*>& meshes, Allocator* model_node_allocator, Model::ModelNode** out_root_node, std::vector<Model::ModelNode*>& out_model_nodes);
		

		/**
		* @brief Recursively iterates the node hierarchy and builds the correct node structure
		* @param[in] node The node that should be iterated on
		* @param[in] parent_model_node The parent of the node that we're iterating on (if any)
		* @param[in] meshes An array of meshes of which the indices match up with the aiMesh array in the aiScene that was used to load in the model
		* @param[in] model_node_allocator The allocator that should be used to allocate all the Model::ModelNode structs
		* @param[in] out_model_node The node of the outputted node hierarchy
		* @param[in] out_model_nodes A flat array of all model nodes in the hierarchy
		*/
		static void IterateNodes(aiNode* node, Model::ModelNode* parent_model_node, const std::vector<Mesh*>& meshes, Allocator* model_node_allocator, Model::ModelNode** out_model_node, std::vector<Model::ModelNode*>& out_model_nodes);

		static void ProcessAnimations(aiAnimation** animations, unsigned int num_animations, const std::vector<Model::ModelNode*>& model_nodes, std::vector<Animation>& out_animations);

		/**
		* @brief Counts all the nodes that live in the scene hierarchy
		* @param[in] node The node from which all child nodes should be counted
		* @param[in] current_count The count at which we should start
		*/
		static int CountNodes(aiNode* node, int current_count);
	};
}