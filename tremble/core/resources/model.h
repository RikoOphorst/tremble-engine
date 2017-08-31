#pragma once

#include "../math/math.h"
#include "../memory/memory_includes.h"
#include "animation.h"

namespace tremble
{
	class Mesh;
	class Texture;
	struct Material;

	class Model
	{
	public:
		struct ModelNode {
			ModelNode() {
				transform = Mat44();
				parent = nullptr;
			}

			std::string name;
			Mat44 transform;
			ModelNode* parent;
			std::vector<ModelNode*> children;
			std::vector<Mesh*> meshes;

			Mat44 GetTransform()
			{
				if (parent != nullptr)
					return parent->GetTransform() * transform;
				else
					return transform;
			}

			Mat44 GetTransform() const
			{
				if (parent != nullptr)
					return parent->GetTransform() * transform;
				else
					return transform;
			}
		};

		struct Bone
		{
			aiMatrix4x4 ai_mat;
			Mat44 bone_offset;
			Mat44 final_transformation;
		};

	public:
		Model();
		~Model();

		std::vector<std::pair<Mesh*, Mat44>> GetMeshesWithTransforms();

		void GetBoneTransforms(float animation_time_in_seconds, const Animation& animation, const DirectX::XMMATRIX& world_transform, std::vector<Mat44>& output);

		void ReadNodeHierarchy(float animation_time, const Animation& animation, Model::ModelNode* node, const DirectX::XMMATRIX& parent_transform);

		void OutputChildren(int num, Model::ModelNode* node);

		void ComputeInterpolatedScaling(const AnimationChannel& channel, float animation_time, Vector3& output);
		unsigned int FindScalingKeyIndex(const AnimationChannel& channel, float animation_time);

		void ComputeInterpolatedPosition(const AnimationChannel& channel, float animation_time, Vector3& output);
		unsigned int FindPositionKeyIndex(const AnimationChannel& channel, float animation_time);

		void ComputeInterpolatedRotation(const AnimationChannel& channel, float animation_time, Quaternion& output);
		unsigned int FindRotationKeyIndex(const AnimationChannel& channel, float animation_time);

		void SetBones(const std::vector<Bone>& bones) { bones_ = bones; };
		
		void SetNameToBoneMapping(const std::unordered_map<std::string, unsigned int>& name_to_bone_mapping) { name_to_bone_mapping_ = name_to_bone_mapping; };

		void SetMeshes(const std::vector<Mesh*>& meshes) { meshes_ = meshes; }
		const std::vector<Mesh*>& GetMeshes() const { return meshes_; }

		void SetMaterials(const std::vector<Material*>& materials) { materials_ = materials; }
		const std::vector<Material*> GetMaterials() const { return materials_; }

		void SetTextures(const std::vector<Texture*>& textures) { textures_ = textures; }
		const std::vector<Texture*> GetTextures() const { return textures_; }

		void SetAnimations(const std::vector<Animation>& animations) { animations_ = animations; }
		const std::vector<Animation>& GetAnimations() const { return animations_; }

		std::vector<ModelNode*>& GetModelNodes() { return nodes_; }

		void SetRootModelNode(ModelNode* model_node) { root_node_ = model_node; }
		ModelNode* GetRootModelNode() { return root_node_; }

		void SetNodeAllocator(FreeListAllocator* allocator) { node_allocator_ = allocator; }
		FreeListAllocator* GetModelNodeAllocator() { return node_allocator_; }

		void SetMeshAllocator(FreeListAllocator* allocator) { mesh_allocator_ = allocator; }
		FreeListAllocator* GetMeshAllocator() { return mesh_allocator_; }

		void SetMaterialAllocator(FreeListAllocator* allocator) { material_allocator_ = allocator; }
		FreeListAllocator* GetMaterialAllocator() { return material_allocator_; }

		void SetTextureAllocator(FreeListAllocator* allocator) { texture_allocator_ = allocator; }
		FreeListAllocator* GetTextureAllocator() { return texture_allocator_; }

	protected:
		void Iterate(ModelNode* node, std::vector<std::pair<Mesh*, Mat44>>& out_pairs);

	private:
		std::vector<Mesh*> meshes_;
		std::vector<Material*> materials_;
		std::vector<Texture*> textures_;

		ModelNode* root_node_;
		std::vector<ModelNode*> nodes_;
		std::vector<Animation> animations_;
		std::unordered_map<std::string, unsigned int> name_to_bone_mapping_;
		std::vector<Bone> bones_;

		FreeListAllocator* node_allocator_;
		FreeListAllocator* mesh_allocator_;
		FreeListAllocator* material_allocator_;
		FreeListAllocator* texture_allocator_;
	};
}