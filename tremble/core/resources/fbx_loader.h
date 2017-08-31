#pragma once

#include "mesh.h"

namespace fbxsdk
{
	class FbxScene;
	class FbxMesh;
	class FbxNode;
	class FbxSurfaceMaterial;
}

namespace tremble
{
	class SGNode;
	struct Material;

	class FBXLoader
	{
		friend class ResourceManager;
	public:
		static bool LoadFBXScene(const std::string& file_path_to_fbx, SGNode* node_to_load_into, bool load_as_static = true);
		static bool LoadFBXMesh(const std::string& file_path_to_fbx, Mesh::MeshData* mesh_data);
	private:
		static bool Setup(fbxsdk::FbxScene** output_scene, std::string file_path_to_fbx);
		static void ImportFBXNodeIntoNode(fbxsdk::FbxNode* node, SGNode* node_to_load_into, const std::string& file_path, bool load_as_static = true);

		static void ImportFBXMesh(fbxsdk::FbxMesh* mesh, Mesh::MeshData* mesh_data);
		static bool ImportFBXNode(fbxsdk::FbxNode* node, Mesh::MeshData* mesh_data);

		static void ImportFBXMaterial(fbxsdk::FbxSurfaceMaterial* fbx_material, Material* out_material, ResourceManager* resource_manager, const std::string& file_path);
	};
}