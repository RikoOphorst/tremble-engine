#include "skinned_renderable.h"

#include "../../core/resources/mesh.h"
#include "../../core/rendering/material.h"
#include "../../core/rendering/texture.h"
#include "../../core/scene_graph/scene_graph.h"
#include "../../core/resources/model.h"
#include "../../core/rendering/upload_buffer.h"
#include "../../core/rendering/material.h"
#include "../../components/rendering/camera.h"
#include "../../core/utilities/octree.h"
#include "../../core/memory/memory_includes.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	SkinnedRenderable::SkinnedRenderable() :
		model_(nullptr),
		active_(true),
		is_playing_animation_(false),
		current_animation_(-1),
		current_animation_time_(0.0f),
		current_animation_time_normalized_(0.0f),
		current_animation_time_in_ticks_(0.0f)
	{

	}

	//------------------------------------------------------------------------------------------------------
	SkinnedRenderable::~SkinnedRenderable()
	{

	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::Start()
	{
		bone_upload_buffer_.Create(L"BoneUploadBuffer", 256, sizeof(DirectX::XMMATRIX), nullptr);
		bone_buffer_.Create(L"BoneBuffer", 256, sizeof(DirectX::XMMATRIX));
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::Update()
	{
		if (is_playing_animation_)
		{
			const Animation& anim = model_->GetAnimations()[current_animation_];

			current_animation_time_ += Get::DeltaT();

			std::vector<Mat44> bone_transforms;
			model_->GetBoneTransforms(current_animation_time_, anim, DirectX::XMMatrixIdentity(), bone_transforms);

			for (int i = 0; i < bone_transforms.size(); i++)
			{
				bone_upload_buffer_.InsertDataByElement(i, &static_cast<DirectX::XMMATRIX>(bone_transforms[i]));
			}

			GraphicsContext& context = GraphicsContext::Begin(L"bone_upload");
			context.CopyBuffer(bone_buffer_, bone_upload_buffer_);
			context.TransitionResource(bone_buffer_, D3D12_RESOURCE_STATE_GENERIC_READ);
			context.Finish();
		}
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::Shutdown()
	{
		
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::Draw(GraphicsContext& context, Camera* camera)
	{
		if (active_)
		{
			if (model_ != nullptr)
			{
				for (int i = 0; i < cached_mesh_transforms_.size(); i++)
				{
					Mesh* mesh = cached_mesh_transforms_[i].first;
					Mat44& transform = cached_mesh_transforms_[i].second;

					if (!mesh->AreBuffersBuilt())
					{
						mesh->BuildBuffers();
					}

					Material* material = mesh->GetMaterial();
					MaterialConstants mat_constants;

					mat_constants.global_ambient = DirectX::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
					mat_constants.ambient_color = DirectX::XMFLOAT4(material->ambient_reflectance.x, material->ambient_reflectance.y, material->ambient_reflectance.z, 1.0f);
					mat_constants.emissive_color = DirectX::XMFLOAT4(material->emissive.x, material->emissive.y, material->emissive.z, 1.0f);
					mat_constants.diffuse_color = DirectX::XMFLOAT4(material->diffuse.x, material->diffuse.y, material->diffuse.z, 1.0f);
					mat_constants.specular_color = DirectX::XMFLOAT4(material->specular.x, material->specular.y, material->specular.z, 1.0f);
					mat_constants.reflectance = DirectX::XMFLOAT4(material->ambient_reflectance.x, material->ambient_reflectance.y, material->ambient_reflectance.z, 1.0f);
					mat_constants.opacity = 1.0f;
					mat_constants.specular_power = 5.0f;
					mat_constants.index_of_refraction = 1.0f;
					mat_constants.has_ambient_texture = false;
					mat_constants.has_emissive_texture = false;
					mat_constants.has_diffuse_texture = material->use_diffuse_map;
					mat_constants.has_specular_texture = false;
					mat_constants.has_specular_power_texture = false;
					mat_constants.has_normal_texture = material->use_normal_map;
					mat_constants.has_bump_texture = false;
					mat_constants.has_opacity_texture = false;
					mat_constants.bump_intensity = 1.0f;
					mat_constants.specular_scale = 1.0f;
					mat_constants.alpha_threshold = 0.05f;
					mat_constants.padding[0] = mat_constants.padding[1] = 0.0f;

					Get::Renderer()->GetMaterialConstantsBuffer().InsertDataByElement(mesh->GetMaterial()->renderer_material_cb_id, &mat_constants);

					ObjectConstants constants;
					constants.world = transform * GetNode()->GetWorldTransform();
					constants.world_view = constants.world * camera->GetView();
					constants.world_view_projection = constants.world * camera->GetViewProjection();

					Get::Renderer()->GetObjectConstantsBuffer().InsertDataByElement(mesh->GetObjectConstantBufferID(this), &constants);

					context.SetConstantBuffer(0, Get::Renderer()->GetObjectConstantsBuffer().GetAddressByElement(mesh->GetObjectConstantBufferID(this)));
					context.SetConstantBuffer(2, Get::Renderer()->GetMaterialConstantsBuffer().GetAddressByElement(mesh->GetMaterial()->renderer_material_cb_id));
					context.SetBufferSRV(15, bone_buffer_);

					Material* mat = mesh->GetMaterial();
					DescriptorHeap& srv_heap = Get::CbvSrvUavHeap();

					if (mat != nullptr)
					{
						if (mat->use_ambient_map == true)
						{
							context.SetDescriptorTable(4, srv_heap.GetGPUDescriptorById(mat->ambient_map->GetSRV()));
						}

						if (mat->use_emissive_map == true)
						{
							context.SetDescriptorTable(5, srv_heap.GetGPUDescriptorById(mat->emissive_map->GetSRV()));
						}

						if (mat->use_diffuse_map == true)
						{
							context.SetDescriptorTable(6, srv_heap.GetGPUDescriptorById(mat->diffuse_map->GetSRV()));
						}

						if (mat->use_specular_map == true)
						{
							context.SetDescriptorTable(7, srv_heap.GetGPUDescriptorById(mat->specular_map->GetSRV()));
						}

						if (mat->use_shininess_map == true)
						{
							context.SetDescriptorTable(8, srv_heap.GetGPUDescriptorById(mat->shininess_map->GetSRV()));
						}

						if (mat->use_normal_map == true)
						{
							context.SetDescriptorTable(9, srv_heap.GetGPUDescriptorById(mat->normal_map->GetSRV()));
						}
					}

					mesh->Draw(context);
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::DrawBasic(GraphicsContext& context, const Mat44& view, const Mat44& projection)
	{
		if (active_)
		{
			for (int i = 0; i < cached_mesh_transforms_.size(); i++)
			{
				Mesh* mesh = cached_mesh_transforms_[i].first;
				Mat44& transform = cached_mesh_transforms_[i].second;

				if (!mesh->AreBuffersBuilt())
				{
					mesh->BuildBuffers();
				}

				ObjectConstants constants;
				constants.world = transform * GetNode()->GetWorldTransform();
				constants.world_view = constants.world * view;
				constants.world_view_projection = constants.world * view * projection;

				Get::Renderer()->GetObjectConstantsBuffer().InsertDataByElement(mesh->GetObjectConstantBufferID(this), &constants);

				context.SetConstantBuffer(0, Get::Renderer()->GetObjectConstantsBuffer().GetAddressByElement(mesh->GetObjectConstantBufferID(this)));
				context.SetBufferSRV(1, bone_buffer_);

				mesh->Draw(context);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::DrawBasic(GraphicsContext& context, Camera* camera)
	{
		DrawBasic(context, camera->GetView(), camera->GetProjection());
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::PlayAnimation(const std::string& name)
	{
		PlayAnimation(name_to_animation_mapping_[name]);
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::PlayAnimation(const unsigned int& anim_index)
	{
		assert(anim_index < model_->GetAnimations().size());

		is_playing_animation_ = true;
		current_animation_ = anim_index;
		current_animation_time_ = 0.1f;
		current_animation_time_in_ticks_ = 0.0f;
		current_animation_time_normalized_ = 0.0f;
	}

	//------------------------------------------------------------------------------------------------------
	void SkinnedRenderable::SetModel(Model* model)
	{
		if (model != model_)
		{
			// cache the mesh/transforms to save a little bit of computational power
			cached_mesh_transforms_ = model->GetMeshesWithTransforms();

			const std::vector<Mesh*>& meshes = model->GetMeshes();
			for (int i = 0; i < model->GetMeshes().size(); i++)
			{
				meshes[i]->CreateObjectConstantBufferID(this);
			}

			model_ = model;

			name_to_animation_mapping_.clear();
			const std::vector<Animation>& animations = model_->GetAnimations();
			for (int i = 0; i < animations.size(); i++)
			{
				const Animation& animation = animations[i];
				name_to_animation_mapping_[animation.name] = i;
			}

			is_playing_animation_ = false;
			current_animation_ = -1;
			current_animation_time_ = 0.0f;
			current_animation_time_in_ticks_ = 0.0f;
			current_animation_time_normalized_ = 0.0f;
		}
	}

	//------------------------------------------------------------------------------------------------------
	Model* SkinnedRenderable::GetModel()
	{
		return model_;
	}
}