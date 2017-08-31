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

	class SkinnedRenderable : public Component
	{
		friend class FBXLoader;
	public:
		SkinnedRenderable();
		~SkinnedRenderable();

		void Start();
		void Update();
		void Shutdown();

		void Draw(GraphicsContext& context, Camera* camera);
		void DrawBasic(GraphicsContext& context, const Mat44& view, const Mat44& projection);
		void DrawBasic(GraphicsContext& context, Camera* camera);

		void PlayAnimation(const std::string& name);
		void PlayAnimation(const unsigned int& anim_index);

		void SetModel(Model* model);
		Model* GetModel();

		void SetActive(bool active) { active_ = active; }
		const bool& GetActive() { return active_; }
	private:
		bool active_;
		Model* model_;
		std::vector<std::pair<Mesh*, Mat44>> cached_mesh_transforms_;

		bool is_playing_animation_;
		int current_animation_;
		float current_animation_time_;
		float current_animation_time_normalized_;
		float current_animation_time_in_ticks_;
		std::unordered_map<std::string, int> name_to_animation_mapping_;
		UploadBuffer bone_upload_buffer_;
		StructuredBuffer bone_buffer_;
	};
}