#include "model.h"

#include "../get.h"
#include "../core/rendering/texture.h"
#include "../core/rendering/material.h"
#include "../core/resources/mesh.h"
#include "../core/rendering/renderer.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Model::Model() :
		root_node_(nullptr),
		node_allocator_(nullptr),
		mesh_allocator_(nullptr),
		material_allocator_(nullptr),
		texture_allocator_(nullptr)
	{
		
	}
	
	//------------------------------------------------------------------------------------------------------
	Model::~Model()
	{
		if (node_allocator_ != nullptr)
		{
			for (int i = 0; i < nodes_.size(); i++)
			{
				node_allocator_->Delete(nodes_[i]);
				nodes_[i] = nullptr;
			}
			Get::MemoryManager()->DeleteAllocator(node_allocator_);


			for (int i = 0; i < textures_.size(); i++)
			{
				texture_allocator_->Delete(textures_[i]);
				textures_[i] = nullptr;
			}
			Get::MemoryManager()->DeleteAllocator(texture_allocator_);


			for (int i = 0; i < materials_.size(); i++)
			{
				material_allocator_->Delete(materials_[i]);
				materials_[i] = nullptr;
			}
			Get::MemoryManager()->DeleteAllocator(material_allocator_);


			for (int i = 0; i < meshes_.size(); i++)
			{
				mesh_allocator_->Delete(meshes_[i]);
				meshes_[i] = nullptr;
			}
			Get::MemoryManager()->DeleteAllocator(mesh_allocator_);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Model::Iterate(ModelNode* node, std::vector<std::pair<Mesh*, Mat44>>& out_pairs)
	{
		for (int i = 0; i < node->meshes.size(); i++)
		{
			out_pairs.push_back(std::make_pair(node->meshes[i], node->GetTransform()));
		}

		for (int i = 0; i < node->children.size(); i++)
		{
			Iterate(node->children[i], out_pairs);
		}
	}

	//------------------------------------------------------------------------------------------------------
	std::vector<std::pair<Mesh*, Mat44>> Model::GetMeshesWithTransforms()
	{
		std::vector<std::pair<Mesh*, Mat44>> pairs;
		Iterate(root_node_, pairs);

		return pairs;
	}
	
	//------------------------------------------------------------------------------------------------------
	void Model::GetBoneTransforms(float animation_time_in_seconds, const Animation& animation, const DirectX::XMMATRIX& world_transform, std::vector<Mat44>& output)
	{
		float anim_time = fmod(animation.ticks_per_second * animation_time_in_seconds, animation.duration);

		ReadNodeHierarchy(anim_time, animation, root_node_, world_transform);

		for (int i = 0; i < bones_.size(); i++)
		{
			output.push_back(bones_[i].final_transformation);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Model::ReadNodeHierarchy(float animation_time, const Animation& animation, Model::ModelNode* node, const DirectX::XMMATRIX& parent_transform)
	{
		std::string node_name(node->name);
		DirectX::XMMATRIX node_transform = node->transform;
		DirectX::XMMATRIX global_transform = DirectX::XMMatrixIdentity();

		if (animation.node_name_to_channel_mapping.find(node_name) != animation.node_name_to_channel_mapping.end())
		{
			const AnimationChannel& channel = animation.channels[animation.node_name_to_channel_mapping.find(node_name)->second];

			Vector3 scaling;
			ComputeInterpolatedScaling(channel, animation_time, scaling);
			Quaternion rotation;
			ComputeInterpolatedRotation(channel, animation_time, rotation);
			Vector3 position;
			ComputeInterpolatedPosition(channel, animation_time, position);

			DirectX::XMMATRIX scaling_matrix		= DirectX::XMMatrixScalingFromVector(scaling);
			DirectX::XMMATRIX rotation_matrix		= DirectX::XMMatrixRotationQuaternion(rotation);
			DirectX::XMMATRIX translation_matrix	= DirectX::XMMatrixTranslation(position.GetX(), position.GetY(), position.GetZ());

			node_transform = scaling_matrix * rotation_matrix * translation_matrix;
		}

		global_transform = node_transform * parent_transform;

		if (name_to_bone_mapping_.find(node_name) != name_to_bone_mapping_.end())
		{
			bones_[name_to_bone_mapping_[node_name]].final_transformation = bones_[name_to_bone_mapping_[node_name]].bone_offset * global_transform * root_node_->GetTransform().Inverse();
		}

		for (unsigned int i = 0; i < node->children.size(); i++)
		{
			ReadNodeHierarchy(animation_time, animation, node->children[i], global_transform);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Model::OutputChildren(int num, Model::ModelNode* node)
	{
		for (int i = 0; i < node->children.size(); i++)
		{
			for (int j = 0; j < num; j++)
			{
				std::cout << "  ";
			}

			std::cout << node->children[i]->name << std::endl;

			OutputChildren(num + 1, node->children[i]);
		}
	}
	
	//------------------------------------------------------------------------------------------------------
	void Model::ComputeInterpolatedScaling(const AnimationChannel& channel, float animation_time, Vector3& output)
	{
		if (channel.scaling_keys.size() == 1)
		{
			output = channel.scaling_keys[0].value;
			return;
		}

		unsigned int scaling_index = FindScalingKeyIndex(channel, animation_time);
		unsigned int next_scaling_index = (scaling_index + 1);
		assert(next_scaling_index < channel.scaling_keys.size());
		
		float delta_time = channel.scaling_keys[next_scaling_index].time - channel.scaling_keys[scaling_index].time;
		float factor = (animation_time - channel.scaling_keys[scaling_index].time) / delta_time;
		assert(factor >= 0.0f && factor <= 1.0f);

		const Vector3& start = channel.scaling_keys[scaling_index].value;
		const Vector3& end = channel.scaling_keys[next_scaling_index].value;
		Vector3 delta = end - start;

		output = start + (delta * factor);
	}
	
	//------------------------------------------------------------------------------------------------------
	unsigned int Model::FindScalingKeyIndex(const AnimationChannel& channel, float animation_time)
	{
		assert(channel.scaling_keys.size() > 0);

		for (uint i = 0; i < channel.scaling_keys.size() - 1; i++) {
			if (animation_time < channel.scaling_keys[i + 1].time) {
				return i;
			}
		}

		return 0;
	}
	
	//------------------------------------------------------------------------------------------------------
	void Model::ComputeInterpolatedPosition(const AnimationChannel& channel, float animation_time, Vector3& output)
	{
		if (channel.position_keys.size() == 1)
		{
			output = channel.position_keys[0].value;
			return;
		}

		unsigned int position_index = FindPositionKeyIndex(channel, animation_time);
		unsigned int next_position_index = (position_index + 1);
		assert(next_position_index < channel.position_keys.size());

		float delta_time = channel.position_keys[next_position_index].time - channel.position_keys[position_index].time;
		float factor = (animation_time - channel.position_keys[position_index].time) / delta_time;
		assert(factor >= 0.0f && factor <= 1.0f);

		const Vector3& start = channel.position_keys[position_index].value;
		const Vector3& end = channel.position_keys[next_position_index].value;
		Vector3 delta = end - start;

		output = start + (delta * factor);
	}

	//------------------------------------------------------------------------------------------------------
	unsigned int Model::FindPositionKeyIndex(const AnimationChannel& channel, float animation_time)
	{
		assert(channel.position_keys.size() > 0);

		for (uint i = 0; i < channel.position_keys.size() - 1; i++) {
			if (animation_time < channel.position_keys[i + 1].time) {
				return i;
			}
		}

		return 0;
	}

	//------------------------------------------------------------------------------------------------------
	void Model::ComputeInterpolatedRotation(const AnimationChannel& channel, float animation_time, Quaternion& output)
	{
		if (channel.rotation_keys.size() == 1) {
			output = channel.rotation_keys[0].value;
			return;
		}

		unsigned int rotation_index = FindRotationKeyIndex(channel, animation_time);
		unsigned int next_rotation_index = (rotation_index + 1);
		assert(next_rotation_index < channel.rotation_keys.size());
		
		float delta_time = (float)(channel.rotation_keys[next_rotation_index].time - channel.rotation_keys[rotation_index].time);
		float factor = (animation_time - (float)channel.rotation_keys[rotation_index].time) / delta_time;
		assert(factor >= 0.0f && factor <= 1.0f);

		const Quaternion& start_rotation = channel.rotation_keys[rotation_index].value;
		const Quaternion& end_rotation = channel.rotation_keys[next_rotation_index].value;
		output = DirectX::XMQuaternionSlerp(start_rotation, end_rotation, factor);
		output = DirectX::XMQuaternionNormalize(output);
	}

	//------------------------------------------------------------------------------------------------------
	unsigned int Model::FindRotationKeyIndex(const AnimationChannel& channel, float animation_time)
	{
		assert(channel.position_keys.size() > 0);

		for (uint i = 0; i < channel.position_keys.size() - 1; i++) {
			if (animation_time < channel.position_keys[i + 1].time) {
				return i;
			}
		}

		return 0;
	}
}