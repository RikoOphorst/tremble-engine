#include "mesh.h"

#include "../rendering/renderer.h"
#include "../utilities/utilities.h"
#include "../game_manager.h"
#include "../get.h"
#include "../rendering/upload_buffer.h"
#include "../rendering/descriptor_heap.h"
#include "../rendering/material.h"
#include "../rendering/texture.h"
#include "../../components/rendering/renderable.h"
#include "../memory/memory_includes.h"
#include "../physics/physics_triangle_mesh_geometry.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Mesh::Mesh(const MeshData& mesh_data) :
		buffers_built_(false),
		topology_(mesh_data.topology),
        material_(nullptr),
        collider_allocator_(nullptr),
        triangle_mesh_geometry_(nullptr)
	{
		mesh_data_ = std::move(mesh_data);
	}

	//------------------------------------------------------------------------------------------------------
	Mesh::~Mesh()
	{
        if (triangle_mesh_geometry_ != nullptr)
        {
            collider_allocator_->Delete(triangle_mesh_geometry_);
        }

        if (collider_allocator_ != nullptr)
        {
            Get::MemoryManager()->DeleteAllocator(collider_allocator_);
        }
	}

	//------------------------------------------------------------------------------------------------------
	void Mesh::SetVertices(const std::vector<Vertex>& vertices)
	{
		mesh_data_.vertices = std::move(vertices);
	}

	//------------------------------------------------------------------------------------------------------
	void Mesh::SetIndices(const std::vector<uint32_t>& indices)
	{
		mesh_data_.indices = std::move(indices);
	}

	//------------------------------------------------------------------------------------------------------
	void Mesh::BuildBuffers()
	{
		vertex_buffer_.Create(L"VertexBuffer", static_cast<UINT>(mesh_data_.vertices.size()), sizeof(Vertex), &mesh_data_.vertices[0], false);

		vertex_buffer_view_ = {};
		vertex_buffer_view_.StrideInBytes = sizeof(Vertex);
		vertex_buffer_view_.SizeInBytes = static_cast<UINT>(mesh_data_.vertices.size() * sizeof(Vertex));
		vertex_buffer_view_.BufferLocation = vertex_buffer_->GetGPUVirtualAddress();

		if (mesh_data_.indices.size() > 0)
		{
			index_buffer_.Create(L"IndexBuffer", static_cast<UINT>(mesh_data_.indices.size()), static_cast<UINT>(sizeof(uint32_t)), &mesh_data_.indices[0], false);

			index_buffer_view_ = {};
			index_buffer_view_.Format = DXGI_FORMAT_R32_UINT;
			index_buffer_view_.SizeInBytes = static_cast<UINT>(mesh_data_.indices.size() * sizeof(uint32_t));
			index_buffer_view_.BufferLocation = index_buffer_->GetGPUVirtualAddress();
		}

		DirectX::BoundingSphere::CreateFromPoints(bounds_, mesh_data_.vertices.size(), &mesh_data_.vertices[0].position, sizeof(Vertex));

		UINT obj_cb_byte_size = (sizeof(ObjectConstants) + 255) & ~255;

		if (material_ != nullptr)
		{
			if (material_->ambient_map != nullptr	&& !material_->ambient_map->AreBuffersBuilt())	{ material_->ambient_map->BuildBuffers(); }
			if (material_->diffuse_map != nullptr	&& !material_->diffuse_map->AreBuffersBuilt())	{ material_->diffuse_map->BuildBuffers(); }
			if (material_->emissive_map != nullptr	&& !material_->emissive_map->AreBuffersBuilt())	{ material_->emissive_map->BuildBuffers(); }
			if (material_->normal_map != nullptr	&& !material_->normal_map->AreBuffersBuilt())	{ material_->normal_map->BuildBuffers(); }
			if (material_->shininess_map != nullptr && !material_->shininess_map->AreBuffersBuilt()){ material_->shininess_map->BuildBuffers(); }
			if (material_->specular_map != nullptr	&& !material_->specular_map->AreBuffersBuilt())	{ material_->specular_map->BuildBuffers(); }

			int material_constant_buffer_id_ = Get::Renderer()->GetNewMaterialConstantBufferID();

			D3D12_CONSTANT_BUFFER_VIEW_DESC mat_cbv_desc;
			mat_cbv_desc.BufferLocation = Get::Renderer()->GetMaterialConstantsBuffer().GetAddressByElement(material_constant_buffer_id_);
			mat_cbv_desc.SizeInBytes = obj_cb_byte_size;

			material_->renderer_material_cb_id = material_constant_buffer_id_;
			material_->renderer_material_descriptor_id = Get::CbvSrvUavHeap().CreateConstantBufferView(&mat_cbv_desc);
		}

		buffers_built_ = true;
	}
	
	//------------------------------------------------------------------------------------------------------
	void Mesh::Set(GraphicsContext& context)
	{
		if (!AreBuffersBuilt())
		{
			BuildBuffers();
		}

		context.SetPrimitiveTopology(topology_);
		context.SetVertexBuffer(0, vertex_buffer_view_);
		if (mesh_data_.indices.size() > 0)
		{
			context.SetIndexBuffer(index_buffer_view_);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Mesh::Draw(GraphicsContext& context)
	{
		if (!AreBuffersBuilt())
		{
			BuildBuffers();
		}

		context.SetPrimitiveTopology(topology_);
		context.SetVertexBuffer(0, vertex_buffer_view_);

		if (mesh_data_.indices.size() > 0)
		{
			context.SetIndexBuffer(index_buffer_view_);
			context.DrawIndexedInstanced(static_cast<UINT>(mesh_data_.indices.size()), 1, 0, 0, 0);
		}
		else
		{
			context.DrawInstanced(static_cast<UINT>(mesh_data_.vertices.size()), 1);
		}
	}

	//------------------------------------------------------------------------------------------------------
	UINT Mesh::CreateObjectConstantBufferID(Renderable* renderable)
	{
		UINT id = Get::Renderer()->GetNewObjectConstantBufferID();
		object_constant_buffer_ids_[renderable] = id;
		return id;
	}

	//------------------------------------------------------------------------------------------------------
	void Mesh::SetObjectConstantBufferID(Renderable* renderable, UINT id)
	{
		object_constant_buffer_ids_[renderable] = id;
	}

	//------------------------------------------------------------------------------------------------------
	UINT Mesh::GetObjectConstantBufferID(Renderable* renderable)
	{
		auto& find = object_constant_buffer_ids_.find(renderable);
		if (find == object_constant_buffer_ids_.end())
		{
			return DESCRIPTOR_ID_UNKNOWN;
		}
		else
		{
			return find->second;
		}
	}

	//------------------------------------------------------------------------------------------------------
	UINT Mesh::CreateObjectConstantBufferID(SkinnedRenderable* renderable)
	{
		UINT id = Get::Renderer()->GetNewObjectConstantBufferID();
		skinned_object_constant_buffer_ids_[renderable] = id;
		return id;
	}

	//------------------------------------------------------------------------------------------------------
	void Mesh::SetObjectConstantBufferID(SkinnedRenderable* renderable, UINT id)
	{
		skinned_object_constant_buffer_ids_[renderable] = id;
	}

	//------------------------------------------------------------------------------------------------------
	UINT Mesh::GetObjectConstantBufferID(SkinnedRenderable* renderable)
	{
		auto& find = skinned_object_constant_buffer_ids_.find(renderable);
		if (find == skinned_object_constant_buffer_ids_.end())
		{
			return DESCRIPTOR_ID_UNKNOWN;
		}
		else
		{
			return find->second;
		}
	}

    //------------------------------------------------------------------------------------------------------
    PhysicsTriangleMeshGeometry* Mesh::GetPhysicsTriangleMeshGeometry()
    {
        if(triangle_mesh_geometry_ == nullptr)
        {
            CookColliderGeometry();
        }
        return triangle_mesh_geometry_;
    }

    //------------------------------------------------------------------------------------------------------
    void Mesh::CookColliderGeometry()
    {
        if (triangle_mesh_geometry_ != nullptr)
            return;

        unsigned int allocation_size = FreeListAllocator::GetRecommendedMemoryPoolSize(sizeof(PhysicsTriangleMeshGeometry), 1);

        ASSERT(collider_allocator_ == nullptr);
        collider_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(allocation_size);
        triangle_mesh_geometry_ = collider_allocator_->New<PhysicsTriangleMeshGeometry>(this);
        ASSERT(triangle_mesh_geometry_ != nullptr);
    }
}
