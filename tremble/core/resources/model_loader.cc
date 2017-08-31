#include "model_loader.h"

#include "../rendering/texture.h"
#include "../resources/model.h"
#include "../resources/resource_manager.h"
#include "../get.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	ModelLoader::ModelLoader()
	{
		
	}
	
	//------------------------------------------------------------------------------------------------------
	ModelLoader::~ModelLoader()
	{

	}
	
	//------------------------------------------------------------------------------------------------------
	Model* ModelLoader::LoadModel(const std::string& model_path, Allocator* model_allocator)
	{
		std::cout << "Loading model \"" << model_path << "\" using Assimp (v" << aiGetVersionMajor() << "." << aiGetVersionMinor() << "." << aiGetVersionRevision() << ").." << std::endl;

		std::string rel_path = model_path;
		
		while (rel_path[rel_path.size() - 1] != '/' && rel_path[rel_path.size() - 1] != '\\')
		{
			rel_path.pop_back();
		}

		Assimp::Importer importer;
		importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 4);
		const aiScene* scene = importer.ReadFile(model_path,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_FlipUVs |
			aiProcess_JoinIdenticalVertices |
			aiProcess_LimitBoneWeights |
			aiProcess_GenNormals);

		if (scene == nullptr)
		{
			DLOG(importer.GetErrorString());
			system("PAUSE");
			exit(-1);
		}

		assert(scene);
		assert(scene->HasMeshes());

		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const aiColor4D ZeroColor(0.0f, 0.0f, 0.0f, 1.0f);

		FreeListAllocator* mesh_allocator = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(Mesh), scene->mNumMeshes));
		std::vector<Mesh*> meshes;
		std::vector<Model::Bone> bones;
		std::unordered_map<std::string, unsigned int> name_to_bone_mapping;
		ProcessMeshes(scene->mMeshes, scene->mNumMeshes, mesh_allocator, meshes, bones, name_to_bone_mapping);

		FreeListAllocator* material_allocator = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(Material), scene->mNumMaterials));
		FreeListAllocator* texture_allocator = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(Texture), scene->mNumMaterials * 6));
		std::vector<Material*> materials;
		std::vector<Texture*> textures;
		ProcessMaterials(rel_path, scene->mMaterials, scene->mNumMaterials, material_allocator, texture_allocator, materials, textures);

		// link the meshes to the correct materials
		for (int i = 0; i < meshes.size(); i++)
		{
			meshes[i]->SetMaterial(materials[scene->mMeshes[i]->mMaterialIndex]);
		}

		Model* model = model_allocator->New<Model>();
		model->SetMeshes(meshes);
		model->SetMaterials(materials);
		model->SetTextures(textures);

		int num_nodes = CountNodes(scene->mRootNode, 0);

		Model::ModelNode* root_model_node = nullptr;
		FreeListAllocator* node_allocator = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(Model::ModelNode), num_nodes));
		
		ProcessNodes(
			scene->mRootNode, 
			meshes, 
			node_allocator, 
			&root_model_node, 
			model->GetModelNodes()
		);

		model->SetRootModelNode(root_model_node);

		model->SetNodeAllocator(node_allocator);
		model->SetTextureAllocator(texture_allocator);
		model->SetMeshAllocator(mesh_allocator);
		model->SetMaterialAllocator(material_allocator);

		std::vector<Animation> animations;
		ProcessAnimations(scene->mAnimations, scene->mNumAnimations, model->GetModelNodes(), animations);

		model->SetAnimations(animations);
		model->SetBones(bones);
		model->SetNameToBoneMapping(name_to_bone_mapping);

		return model;
	}
	
	//------------------------------------------------------------------------------------------------------
	void ModelLoader::ProcessMeshes(aiMesh** meshes, unsigned int num_meshes, Allocator* allocator, std::vector<Mesh*>& out_meshes, std::vector<Model::Bone>& out_bones, std::unordered_map<std::string, unsigned int>& name_to_bone_mapping)
	{
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		const aiColor4D ZeroColor(0.0f, 0.0f, 0.0f, 1.0f);

		for (unsigned int i = 0; i < num_meshes; i++)
		{
			aiMesh* mesh = meshes[i];

			Mesh::MeshData mesh_data;
			mesh_data.topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

			for (unsigned int j = 0; j < mesh->mNumVertices; j++)
			{
				Vertex vert;

				const aiVector3D* position = &(mesh->mVertices[j]);
				const aiVector3D* normal = mesh->HasNormals() ? &(mesh->mNormals[j]) : &Zero3D;
				const aiVector3D* bitangent = mesh->HasTangentsAndBitangents() ? &(mesh->mBitangents[j]) : &Zero3D;
				const aiVector3D* tangent = mesh->HasTangentsAndBitangents() ? &(mesh->mTangents[j]) : &Zero3D;
				const aiVector3D* tex_coord = mesh->HasTextureCoords(0) ? &(mesh->mTextureCoords[0][j]) : &Zero3D;
				const aiColor4D* color = mesh->HasVertexColors(0) ? mesh->mColors[0] : &ZeroColor;

				vert.position = DirectX::XMFLOAT3(position->x, position->y, position->z);
				vert.color = DirectX::XMFLOAT4(color->r, color->g, color->b, color->a);
				vert.normal = DirectX::XMFLOAT3(normal->x, normal->y, normal->z);
				vert.bitangent = DirectX::XMFLOAT3(bitangent->x, bitangent->y, bitangent->z);
				vert.tangent = DirectX::XMFLOAT3(tangent->x, tangent->y, tangent->z);
				vert.uv = DirectX::XMFLOAT2(tex_coord->x, tex_coord->y);
				vert.bone_ids[0] = 0;
				vert.bone_ids[1] = 0; 
				vert.bone_ids[2] = 0;
				vert.bone_ids[3] = 0;
				vert.bone_weights[0] = 0.0f;
				vert.bone_weights[1] = 0.0f;
				vert.bone_weights[2] = 0.0f;
				vert.bone_weights[3] = 0.0f;

				mesh_data.vertices.push_back(vert);
			}

			for (unsigned int j = 0; j < mesh->mNumFaces; j++)
			{
				aiFace face = mesh->mFaces[j];

				assert(face.mNumIndices == 3);

				for (int k = 0; k < static_cast<int>(face.mNumIndices); k++)
				{
					mesh_data.indices.push_back(face.mIndices[k]);
				}
			}

			if (mesh->HasBones())
			{
				assert(mesh->mNumBones <= 256);
				for (unsigned int j = 0; j < mesh->mNumBones; j++)
				{
					unsigned int	bone_index = 0;
					std::string		bone_name = mesh->mBones[j]->mName.C_Str();
					aiBone*			imported_bone = mesh->mBones[j];

					if (name_to_bone_mapping.find(bone_name) == name_to_bone_mapping.end())
					{
						out_bones.push_back({});
						bone_index = out_bones.size() - 1;
					}
					else
					{
						bone_index = name_to_bone_mapping[bone_name];
					}

					name_to_bone_mapping[bone_name] = bone_index;

					Model::Bone& model_bone = out_bones[bone_index];

					model_bone.ai_mat = imported_bone->mOffsetMatrix;
					model_bone.bone_offset.SetR0(Vector4(imported_bone->mOffsetMatrix.a1, imported_bone->mOffsetMatrix.a2, imported_bone->mOffsetMatrix.a3, imported_bone->mOffsetMatrix.a4));
					model_bone.bone_offset.SetR1(Vector4(imported_bone->mOffsetMatrix.b1, imported_bone->mOffsetMatrix.b2, imported_bone->mOffsetMatrix.b3, imported_bone->mOffsetMatrix.b4));
					model_bone.bone_offset.SetR2(Vector4(imported_bone->mOffsetMatrix.c1, imported_bone->mOffsetMatrix.c2, imported_bone->mOffsetMatrix.c3, imported_bone->mOffsetMatrix.c4));
					model_bone.bone_offset.SetR3(Vector4(imported_bone->mOffsetMatrix.d1, imported_bone->mOffsetMatrix.d2, imported_bone->mOffsetMatrix.d3, imported_bone->mOffsetMatrix.d4));
					model_bone.bone_offset = model_bone.bone_offset.Transpose();

					for (unsigned int k = 0; k < imported_bone->mNumWeights; k++)
					{
						Vertex& vert = mesh_data.vertices[imported_bone->mWeights[k].mVertexId];
						uint8_t bone_id = static_cast<uint8_t>(bone_index);

						for (int l = 0; l < 4; l++)
						{
							if (vert.bone_weights[l] == 0.0f)
							{
								vert.bone_ids[l] = bone_id;
								vert.bone_weights[l] = imported_bone->mWeights[k].mWeight;
								break;
							}
						}
					}
				}
			}

			for (int j = 0; j < mesh_data.vertices.size(); j++)
			{
				Vertex& vert = mesh_data.vertices[j];
				
				float total = vert.bone_weights[0] +
					vert.bone_weights[1] +
					vert.bone_weights[2] +
					vert.bone_weights[3];

				if (total != 1.0f)
				{
					float w0 = vert.bone_weights[0] / total;
					float w1 = vert.bone_weights[1] / total;
					float w2 = vert.bone_weights[2] / total;
					float w3 = vert.bone_weights[3] / total;

					vert.bone_weights[0] = w0;
					vert.bone_weights[1] = w1;
					vert.bone_weights[2] = w2;
					vert.bone_weights[3] = w3;
				}
			}

			out_meshes.push_back(allocator->New<Mesh>(mesh_data));
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	void ModelLoader::ProcessMaterials(const std::string& rel_path, aiMaterial** materials, unsigned int num_materials, Allocator* material_allocator, Allocator* texture_allocator, std::vector<Material*>& out_materials, std::vector<Texture*>& out_textures)
	{
		for (unsigned int i = 0; i < num_materials; i++)
		{
			Material* mat = material_allocator->New<Material>();

			aiMaterial* material = materials[i];

			for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_AMBIENT); j++)
			{
				aiString path;
				material->GetTexture(aiTextureType_AMBIENT, j, &path);

				if (std::string(path.C_Str()) != "") {
					mat->ambient_map = texture_allocator->New<Texture>(rel_path + path.C_Str());
					mat->use_ambient_map = true;
					out_textures.push_back(mat->ambient_map);
				}
			}

			for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++)
			{
				aiString path;
				material->GetTexture(aiTextureType_DIFFUSE, j, &path);

				if (std::string(path.C_Str()) != "") {
					mat->diffuse_map = texture_allocator->New<Texture>(rel_path + path.C_Str());
					mat->use_diffuse_map = true;
					out_textures.push_back(mat->diffuse_map);
				}
			}

			for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_DIFFUSE); j++)
			{
				aiString path;
				material->GetTexture(aiTextureType_EMISSIVE, j, &path);

				if (std::string(path.C_Str()) != "") {
					mat->emissive_map = texture_allocator->New<Texture>(rel_path + path.C_Str());
					mat->use_emissive_map = true;
					out_textures.push_back(mat->emissive_map);
				}
			}

			for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_NORMALS); j++)
			{
				aiString path;
				material->GetTexture(aiTextureType_NORMALS, j, &path);

				if (std::string(path.C_Str()) != "") {
					mat->normal_map = texture_allocator->New<Texture>(rel_path + path.C_Str());
					mat->use_normal_map = true;
					out_textures.push_back(mat->normal_map);
				}
			}

			for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_SPECULAR); j++)
			{
				aiString path;
				material->GetTexture(aiTextureType_SPECULAR, j, &path);

				if (std::string(path.C_Str()) != "") {
					mat->specular_map = texture_allocator->New<Texture>(rel_path + path.C_Str());
					mat->use_specular_map = true;
					out_textures.push_back(mat->specular_map);
				}
			}

			for (unsigned int j = 0; j < material->GetTextureCount(aiTextureType_SHININESS); j++)
			{
				aiString path;
				material->GetTexture(aiTextureType_SHININESS, j, &path);

				if (std::string(path.C_Str()) != "") {
					mat->shininess_map = texture_allocator->New<Texture>(rel_path + path.C_Str());
					mat->use_shininess_map = true;
					out_textures.push_back(mat->shininess_map);
				}
			}

			out_materials.push_back(mat);
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	void ModelLoader::ProcessNodes(aiNode* root_node, const std::vector<Mesh*>& meshes, Allocator* model_node_allocator, Model::ModelNode** out_root_model_node, std::vector<Model::ModelNode*>& out_model_nodes)
	{
		IterateNodes(root_node, nullptr, meshes, model_node_allocator, out_root_model_node, out_model_nodes);
	}
	
	//------------------------------------------------------------------------------------------------------
	void ModelLoader::IterateNodes(aiNode* node, Model::ModelNode* parent_model_node, const std::vector<Mesh*>& meshes, Allocator* model_node_allocator, Model::ModelNode** out_model_node, std::vector<Model::ModelNode*>& out_model_nodes)
	{
		Model::ModelNode* model_node = nullptr;

		model_node = model_node_allocator->New<Model::ModelNode>();

		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			model_node->meshes.push_back(meshes[node->mMeshes[i]]);
		}

		Mat44 transform;
		transform.SetR0(Vector4(node->mTransformation.a1, node->mTransformation.a2, node->mTransformation.a3, node->mTransformation.a4));
		transform.SetR1(Vector4(node->mTransformation.b1, node->mTransformation.b2, node->mTransformation.b3, node->mTransformation.b4));
		transform.SetR2(Vector4(node->mTransformation.c1, node->mTransformation.c2, node->mTransformation.c3, node->mTransformation.c4));
		transform.SetR3(Vector4(node->mTransformation.d1, node->mTransformation.d2, node->mTransformation.d3, node->mTransformation.d4));

		transform = transform.Transpose();

		model_node->transform = transform;
		model_node->parent = parent_model_node;
		model_node->name = node->mName.C_Str();

		if (parent_model_node != nullptr)
		{
			parent_model_node->children.push_back(model_node);
		}

		if (out_model_node != nullptr)
		{
			*out_model_node = model_node;
		}

		out_model_nodes.push_back(model_node);

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			IterateNodes(node->mChildren[i], model_node, meshes, model_node_allocator, nullptr, out_model_nodes);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void ModelLoader::ProcessAnimations(aiAnimation** animations, unsigned int num_animations, const std::vector<Model::ModelNode*>& model_nodes, std::vector<Animation>& out_animations)
	{
		for (unsigned int i = 0; i < num_animations; i++)
		{
			aiAnimation* imp_animation = animations[i];
			
			Animation anim;
			anim.name = imp_animation->mName.C_Str();
			anim.duration = static_cast<float>(imp_animation->mDuration);
			anim.ticks_per_second = static_cast<float>(imp_animation->mTicksPerSecond);

			for (unsigned int j = 0; j < imp_animation->mNumChannels; j++)
			{
				aiNodeAnim* imp_channel = imp_animation->mChannels[j];
				AnimationChannel channel;
				channel.bone_name = imp_channel->mNodeName.C_Str();

				for (int k = 0; k < model_nodes.size(); k++)
				{
					if (model_nodes[k]->name == channel.bone_name)
					{
						anim.node_name_to_channel_mapping[channel.bone_name] = j;
						break;
					}
				}

				for (unsigned int k = 0; k < imp_channel->mNumPositionKeys; k++)
				{
					aiVectorKey imp_position_key = imp_channel->mPositionKeys[k];
					AnimationVectorKey position_key;
					position_key.time = imp_position_key.mTime;
					position_key.value = Vector3(imp_position_key.mValue.x, imp_position_key.mValue.y, imp_position_key.mValue.z);
					channel.position_keys.push_back(position_key);
				}

				for (unsigned int k = 0; k < imp_channel->mNumRotationKeys; k++)
				{
					aiQuatKey imp_rotation_key = imp_channel->mRotationKeys[k];
					AnimationQuatKey rotation_key;
					rotation_key.time = imp_rotation_key.mTime;
					rotation_key.value = Quaternion(
						DirectX::XMVectorSet(
							imp_rotation_key.mValue.x, 
							imp_rotation_key.mValue.y,
							imp_rotation_key.mValue.z,
							imp_rotation_key.mValue.w)
					);
					channel.rotation_keys.push_back(rotation_key);
				}

				for (unsigned int k = 0; k < imp_channel->mNumScalingKeys; k++)
				{
					aiVectorKey imp_scaling_key = imp_channel->mScalingKeys[k];
					AnimationVectorKey scaling_key;
					scaling_key.time = imp_scaling_key.mTime;
					scaling_key.value = Vector3(imp_scaling_key.mValue.x, imp_scaling_key.mValue.y, imp_scaling_key.mValue.z);
					channel.scaling_keys.push_back(scaling_key);
				}

				anim.channels.push_back(channel);
			}

			out_animations.push_back(anim);
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	int ModelLoader::CountNodes(aiNode* node, int current_count)
	{
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			current_count = CountNodes(node->mChildren[i], current_count);
		}

		return current_count + 1;
	}
}