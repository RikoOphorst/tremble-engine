//#pragma once
//
//namespace tremble
//{
//	class Renderable;
//	class Vector3;
//
//	void GetBoundingBoxMinMax(const DirectX::BoundingBox& bounds, Vector3* out_min, Vector3* out_max);
//	void GetBoundingSphereMinMax(const DirectX::BoundingSphere& bounds, Vector3* out_min, Vector3* out_max);
//
//	class Octree
//	{
//	public:
//		struct PlaneFrustum
//		{
//			DirectX::XMVECTOR near_plane;
//			DirectX::XMVECTOR far_plane;
//			DirectX::XMVECTOR right_plane;
//			DirectX::XMVECTOR left_plane;
//			DirectX::XMVECTOR top_plane;
//			DirectX::XMVECTOR bottom_plane;
//		};
//
//		Octree();
//		Octree(const DirectX::BoundingBox& region);
//		Octree(const DirectX::BoundingBox& region, const std::vector<Renderable*>& objects);
//
//		void Insert(Renderable* renderable);
//		void UpdateTree();
//		void BuildTree();
//		void FindEnclosingCube();
//		void DrawTree(DirectX::XMFLOAT4 color);
//
//		void GetIntersectingObjects(const DirectX::BoundingFrustum& frustum, std::vector<Renderable*>& out);
//		void GetIntersectingObjects(const PlaneFrustum& frustum, std::vector<Renderable*>& out);
//
//		Octree* CreateNode(const DirectX::BoundingBox& region, const std::vector<Renderable*>& objects);
//
//	protected:
//		DirectX::BoundingBox region_;
//
//		std::vector<Renderable*> objects_;
//
//		static std::queue<Renderable*> pending_insertion_;
//
//		Octree* children_[8];
//		unsigned char active_nodes = 0;
//		static const unsigned int MINIMAL_SIZE = 10;
//
//		int max_life_span_;
//		int current_life_;
//		bool has_children_;
//
//		Octree* parent_;
//
//		static bool tree_ready_;
//		static bool tree_built_;
//		static unsigned int ocs;
//	};
//}