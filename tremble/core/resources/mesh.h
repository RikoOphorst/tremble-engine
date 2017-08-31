#pragma once

#include "../rendering/vertex.h"
#include "../rendering/structured_buffer.h"
#include "../rendering/byte_address_buffer.h"
#include "../rendering/graphics_context.h"
#include "../math/math.h"
#include "core/memory/allocators/free_list_allocator.h"

namespace tremble
{
	struct Material;
	class Texture;
	class Renderable;
	class SkinnedRenderable;
    class PhysicsTriangleMeshGeometry;

	/**
	* @class tremble::Mesh
	* @author Riko Ophorst
	* @brief Allows to create meshes given model data
	*/
	class Mesh
	{
	public:
		/**
		* @struct tremble::Mesh::MeshData
		* @author Riko Ophorst
		* @brief Describes & stores the data of a mesh
		*/
		struct MeshData
		{
			std::vector<Vertex> vertices; //!< All vertices in this mesh
			std::vector<uint32_t> indices; //!< All indices in this mesh
			D3D_PRIMITIVE_TOPOLOGY topology; //!< Topology of this mesh
		};

		/**
		* @brief Constructs a mesh based on supplied mesh data
		* @param[in] mesh_data The mesh data the mesh should be build with
		*/
		Mesh(const MeshData& mesh_data);

		~Mesh();

		void SetMeshData(const MeshData& mesh_data) { mesh_data_ = mesh_data; }
		const MeshData& GetMeshData() const { return mesh_data_; }

		void SetVertices(const std::vector<Vertex>& vertices);
		const std::vector<Vertex>& GetVertices() const { return mesh_data_.vertices; }

		void SetIndices(const std::vector<uint32_t>& indices);
		const std::vector<uint32_t>& GetIndices() const { return mesh_data_.indices; }

		size_t GetIndexCount() const { return mesh_data_.indices.size(); }

		void SetTopology(D3D12_PRIMITIVE_TOPOLOGY topology) { topology_ = topology; }
		D3D12_PRIMITIVE_TOPOLOGY GetTopology() const { return topology_; }

		void BuildBuffers();

		void Set(GraphicsContext& context);
		void Draw(GraphicsContext& context);

		bool AreBuffersBuilt() const { return buffers_built_; }

		const DirectX::BoundingSphere& GetBounds() const { return bounds_; }

		void SetMaterial(Material* material) { material_ = material; }
		const Material* GetMaterial() const { return material_; }
		Material* GetMaterial() { return material_; }

		UINT CreateObjectConstantBufferID(Renderable* renderable);
		void SetObjectConstantBufferID(Renderable* renderable, UINT id);
		UINT GetObjectConstantBufferID(Renderable* renderable);

		UINT CreateObjectConstantBufferID(SkinnedRenderable* renderable);
		void SetObjectConstantBufferID(SkinnedRenderable* renderable, UINT id);
		UINT GetObjectConstantBufferID(SkinnedRenderable* renderable);

        PhysicsTriangleMeshGeometry* GetPhysicsTriangleMeshGeometry();

	protected:
        void CookColliderGeometry();

		bool buffers_built_;
		MeshData mesh_data_;

		D3D12_PRIMITIVE_TOPOLOGY topology_;

		StructuredBuffer vertex_buffer_;
		D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view_;

		ByteAddressBuffer index_buffer_;
		D3D12_INDEX_BUFFER_VIEW index_buffer_view_;

		DirectX::BoundingSphere bounds_;
		Material* material_;

		std::unordered_map<Renderable*, UINT> object_constant_buffer_ids_; //!< All object constant buffer IDs per renderable that uses this mesh
		std::unordered_map<SkinnedRenderable*, UINT> skinned_object_constant_buffer_ids_; //!< All object constant buffer IDs per renderable that uses this mesh

        FreeListAllocator* collider_allocator_;
        PhysicsTriangleMeshGeometry* triangle_mesh_geometry_;

	};
}
