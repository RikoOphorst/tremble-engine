#include "renderable.h"

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
	Renderable::Renderable() :
		model_(nullptr),
		octree_node_allocator_(nullptr),
		active_(true)
	{
		
	}

	//------------------------------------------------------------------------------------------------------
	Renderable::~Renderable()
	{
	}
	
	//------------------------------------------------------------------------------------------------------
	void Renderable::Start()
	{
		
	}
	
	//------------------------------------------------------------------------------------------------------
	void Renderable::Update()
	{
		if (GetNode()->WasMoved())
		{
			UpdateBounds();
		}
		else
		{
			for (int i = 0; i < octree_nodes_.size(); i++)
			{
				if (octree_nodes_[i]->alive == true)
				{
					octree_nodes_[i]->updated = false;
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderable::Shutdown()
	{
        for (int i = 0; i < octree_nodes_.size(); i++)
        {
            octree_nodes_[i]->alive = false;
        }

        Get::Octree()->Update();

        for (int i = 0; i < octree_nodes_.size(); i++)
        {
            octree_node_allocator_->Delete(octree_nodes_[i]);
        }

        Get::MemoryManager()->DeleteAllocator(octree_node_allocator_);
	}

	//------------------------------------------------------------------------------------------------------
	void Renderable::Draw(GraphicsContext& context, Camera* camera)
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
	void Renderable::Draw(GraphicsContext& context, OctreeObject* node, Camera* camera)
	{
		if (active_)
		{
			Mesh* mesh = cached_mesh_transforms_[node->renderable_mesh_id].first;
			Mat44& transform = cached_mesh_transforms_[node->renderable_mesh_id].second;

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

	//------------------------------------------------------------------------------------------------------
	void Renderable::DrawBasic(GraphicsContext& context, const Mat44& view, const Mat44& projection)
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

				mesh->Draw(context);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderable::DrawBasic(GraphicsContext& context, Camera* camera)
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
				constants.world_view = constants.world * camera->GetView();
				constants.world_view_projection = constants.world * camera->GetViewProjection();

				Get::Renderer()->GetObjectConstantsBuffer().InsertDataByElement(mesh->GetObjectConstantBufferID(this), &constants);

				context.SetConstantBuffer(0, Get::Renderer()->GetObjectConstantsBuffer().GetAddressByElement(mesh->GetObjectConstantBufferID(this)));

				mesh->Draw(context);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderable::DrawBasic(GraphicsContext& context, OctreeObject* node, Camera* camera)
	{
		if (active_)
		{
			Mesh* mesh = cached_mesh_transforms_[node->renderable_mesh_id].first;
			Mat44& transform = cached_mesh_transforms_[node->renderable_mesh_id].second;

			if (!mesh->AreBuffersBuilt())
			{
				mesh->BuildBuffers();
			}

			ObjectConstants constants;
			constants.world = transform * GetNode()->GetWorldTransform();
			constants.world_view = constants.world * camera->GetView();
			constants.world_view_projection = constants.world * camera->GetViewProjection();

			Get::Renderer()->GetObjectConstantsBuffer().InsertDataByElement(mesh->GetObjectConstantBufferID(this), &constants);

			context.SetConstantBuffer(0, Get::Renderer()->GetObjectConstantsBuffer().GetAddressByElement(mesh->GetObjectConstantBufferID(this)));

			mesh->Draw(context);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderable::SetModel(Model* model)
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

			ComputeBounds();
		}
	}

	//------------------------------------------------------------------------------------------------------
	Model* Renderable::GetModel()
	{
		return model_;
	}
	
	//------------------------------------------------------------------------------------------------------
	void Renderable::ComputeBounds()
	{
		if (octree_nodes_.size() != 0)
		{
			for (int i = 0; i < octree_nodes_.size(); i++)
			{
				octree_nodes_[i]->alive = false;
			}
		}

		//@TODO this entire algorithm should be multithreaded
		if (octree_node_allocator_ == nullptr)
		{
			octree_node_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(OctreeObject), static_cast<unsigned int>(cached_mesh_transforms_.size())));
		}

		std::vector<DirectX::XMFLOAT3> verts;

		for (int i = 0; i < cached_mesh_transforms_.size(); i++)
		{
			std::vector<DirectX::XMFLOAT3> verts;

			const std::vector<Vertex>& mesh_verts = cached_mesh_transforms_[i].first->GetMeshData().vertices;
			for (int j = 0; j < mesh_verts.size(); j++)
			{
				verts.push_back(cached_mesh_transforms_[i].second * Vector4(mesh_verts[j].position, Scalar(1.0f)));
			}

			OctreeObject* octree_node = octree_node_allocator_->New<OctreeObject>();
			octree_node->renderable = this;
			octree_node->renderable_mesh_id = i;
			DirectX::BoundingBox::CreateFromPoints(octree_node->bounds, verts.size(), &verts[0], sizeof(DirectX::XMFLOAT3));
			octree_node->original_bounds = octree_node->bounds;
			octree_node->updated = true;
			octree_node->alive = true;
			octree_node->original_bounds.Transform(octree_node->bounds, GetNode()->GetWorldTransform());
			octree_nodes_.push_back(octree_node);

			Get::Octree()->Insert(octree_node);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Renderable::UpdateBounds()
	{
		for (int i = 0; i < octree_nodes_.size(); i++)
		{
			if (!octree_nodes_[i]->alive)
			{
				continue;
			}
			else
			{
				octree_nodes_[i]->updated = true;
				octree_nodes_[i]->original_bounds.Transform(octree_nodes_[i]->bounds, GetNode()->GetWorldTransform());
			}
		}
	}
}