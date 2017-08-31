#pragma once

namespace tremble
{
	class Renderable;
	class Vector3;
	class PoolAllocator;

	/**
	* @brief A set of planes which represent a frustum
	* @struct tremble::PlaneFrustum
	* @author Riko Ophorst
	*/
	struct PlaneFrustum
	{
		DirectX::XMVECTOR near_plane; //!< The near plane of the frustum
		DirectX::XMVECTOR far_plane; //!< The far plane of the frustum
		DirectX::XMVECTOR right_plane; //!< The right plane of the frustum
		DirectX::XMVECTOR left_plane; //!< The left plane of the frustum
		DirectX::XMVECTOR top_plane; //!< The top plane of the frustum
		DirectX::XMVECTOR bottom_plane; //!< The bottom plane of the frustum
	};

	/**
	* @brief A structure that represents an object inside of the Octree.
	* @struct tremble::OctreeObject
	* @author Riko Ophorst
	*/
	struct OctreeObject
	{
		DirectX::BoundingBox original_bounds; //!< The original bounds for this object (used for boundingbox transformations)
		DirectX::BoundingBox bounds; //!< The current bounds for this object (accurately represents the bounding box for this node, even after it has been repositioned)
		Renderable* renderable; //!< The renderable that is attached to this object
		int renderable_mesh_id; //!< The mesh id that corresponds to this renderable (relative to Renderable::cached_mesh_transforms_)
		bool alive; //!< Whether this object is alive or not
		bool updated; //!< Whether this object has updated over the past frame or not
	};

	/**
	* @brief Represents an octree spatial data structure
	* @class tremble::Octree
	* @author Riko Ophorst
	*/
	class Octree
	{
	public:		
		/**
		* @brief Default constructor based on a certain pre-defined region.
		* @param[in] region The region that this octree "owns"
		*/
		Octree(const DirectX::BoundingBox& region);

		/**
		* @brief Constructs an octree with a given list of objects, and a parent
		* @param[in] region The region this octree owns
		* @param[in] objects The list of objects that should be automatically added to this octree
		* @param[in] parent The parent of this octree
		*/
		Octree(const DirectX::BoundingBox& region, const std::vector<OctreeObject*>& objects, Octree* parent);

		/**
		* @brief Destructs the octree
		*/
		~Octree();

		void Update(); //!< Updates the octree
		void Draw(); //!< Draws the octree

		/**
		* @brief Inserts a node into the octree
		* @param[in] object The object your want to insert into the octree
		*/
		void Insert(OctreeObject* object);

		/**
		* @brief Get a list of objects that are contained within a PlaneFrustum
		* @param[in] frustum The frustum you are testing with
		* @param[out] out List of OctreeNode objects that are contained within the given PlaneFrustum
		*/
		void GetContainedObjects(const PlaneFrustum& frustum, std::vector<OctreeObject*>& out);
	    void GetContainedObjects(const DirectX::BoundingSphere& sphere, std::vector<OctreeObject*>& out) const;

	    int GetNodeCount(); //!< Count the number of nodes that live in the Octree

	protected:
		/**
		* @brief Factory constructor for creating a node with a list of objects
		* @param[in] region The region the new octree owns
		* @param[in] objects The list of objects that should be automatically added to the new octree
		*/
		Octree* CreateNode(const DirectX::BoundingBox& region, const std::vector<OctreeObject*>& objects);
		
		/**
		* @brief Factory constructor for creating a node with an object
		* @param[in] region The region the new octree owns
		* @param[in] objects The object that should be automatically added to the new octree
		*/
		Octree* CreateNode(const DirectX::BoundingBox& region, OctreeObject* object);

		/**
		* @brief Recomputes this octree's bounding region to be a snug fit for the objects it contains
		*/
		void FindEnclosingCube();

		/**
		* @brief Query for the octree's allocator
		*/
		PoolAllocator* GetAllocator();

	private:
		DirectX::BoundingBox region_; //!< The bounding region this octree owns
		std::vector<OctreeObject*> objects_; //!< The objects this octree owns

		Octree* parent_; //!< The parent of this octree
		Octree* child_nodes_[8]; //!< The children (octants) of this octree
		unsigned char active_child_nodes_; //!< Bit mask for the alive child nodes

		int max_lifespan_; //!< The maximum lifespan of this octree
		int cur_lifespan_; //!< The current lifespan of this octree

		PoolAllocator* allocator_; //!< Pool allocator

		const int MIN_SIZE = 1; //!< The minimum uniform size of the octree region
	};

	/**
	* @brief Get the min and max for a bounding box
	* @param[in] bounds The bounds of the box for which you want to find the min & max
	* @param[out] out_min Outputs the minimum point of the bounding box
	* @param[out] out_max Outputs the maximum point of the bounding box
	*/
	void GetBoundingBoxMinMax(const DirectX::BoundingBox& bounds, Vector3* out_min, Vector3* out_max);

	/**
	* @brief Get the min and max for a bounding sphere
	* @param[in] bounds The bounds of the sphere for which you want to find the min & max
	* @param[out] out_min Outputs the minimum point of the bounding box
	* @param[out] out_max Outputs the maximum point of the bounding box
	*/
	void GetBoundingSphereMinMax(const DirectX::BoundingSphere& bounds, Vector3* out_min, Vector3* out_max);
}