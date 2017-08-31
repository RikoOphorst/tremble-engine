#pragma once

#include "../../core/scene_graph/component.h"
#include "../../core/rendering/graphics_context.h"

namespace tremble
{
	class Mesh;
	class Texture;
	struct Material;
	class Model;
	class Camera;
	struct OctreeObject;
	class FreeListAllocator;

	class Renderable : public Component
	{
		friend class FBXLoader;
	public:
		Renderable();
		~Renderable();

		void Start();
		void Update();
		void Shutdown();

		void Draw(GraphicsContext& context, Camera* camera);
		void Draw(GraphicsContext& context, OctreeObject* node, Camera* camera);
		void DrawBasic(GraphicsContext& context, const Mat44& view, const Mat44& projection);
		void DrawBasic(GraphicsContext& context, Camera* camera);
		void DrawBasic(GraphicsContext& context, OctreeObject* node, Camera* camera);

		void SetModel(Model* model);
		Model* GetModel();

		void SetActive(bool active) { active_ = active; }
		const bool& GetActive() { return active_; }

		const std::vector<OctreeObject*>& GetOctreeObjects() const { return octree_nodes_; }

		void ComputeBounds();
		void UpdateBounds();
	private:
		bool active_;
		Model* model_;
		FreeListAllocator* octree_node_allocator_;
		std::vector<OctreeObject*> octree_nodes_;
		std::vector<std::pair<Mesh*, Mat44>> cached_mesh_transforms_;
	};
}