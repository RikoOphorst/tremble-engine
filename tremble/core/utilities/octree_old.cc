//#include "octree_old.h"
//
//#include "../math/math.h"
//#include "../../components/rendering/renderable.h"
//#include "../resources/mesh.h"
//#include "../rendering/renderer.h"
//#include "../get.h"
//
//namespace tremble
//{
//	std::queue<Renderable*> Octree::pending_insertion_ = std::queue<Renderable*>();
//	bool Octree::tree_ready_ = false;
//	bool Octree::tree_built_ = false;
//	unsigned int Octree::ocs = 0;
//
//	//------------------------------------------------------------------------------------------------------
//	Octree::Octree() :
//		current_life_(-1),
//		has_children_(false)
//	{
//		region_ = DirectX::BoundingBox(Vector3(), Vector3()); 
//		
//		for (int i = 0; i < 8; i++)
//		{
//			children_[i] = nullptr;
//		}
//	}
//	
//	//------------------------------------------------------------------------------------------------------
//	Octree::Octree(const DirectX::BoundingBox& region) :
//		current_life_(-1),
//		region_(region),
//		has_children_(false)
//	{
//		for (int i = 0; i < 8; i++)
//		{
//			children_[i] = nullptr;
//		}
//	}
//	
//	//------------------------------------------------------------------------------------------------------
//	Octree::Octree(const DirectX::BoundingBox& region, const std::vector<Renderable*>& objects) :
//		current_life_(-1),
//		region_(region),
//		objects_(objects),
//		has_children_(false)
//	{
//		int i = 0;
//
//		for (int i = 0; i < 8; i++)
//		{
//			children_[i] = nullptr;
//		}
//	}
//	
//	//------------------------------------------------------------------------------------------------------
//	void Octree::Insert(Renderable* renderable)
//	{
//		pending_insertion_.push(renderable);
//	}
//
//	//------------------------------------------------------------------------------------------------------
//	void Octree::UpdateTree()
//	{
//		if (!tree_built_)
//		{
//			while (pending_insertion_.size() != 0)
//			{
//				objects_.push_back(pending_insertion_.front());
//			}
//
//			BuildTree();
//		}
//		else
//		{
//			while (pending_insertion_.size() != 0)
//			{
//				Insert(pending_insertion_.front());
//				pending_insertion_.pop();
//			}
//		}
//
//		tree_ready_ = true;
//	}
//
//	//------------------------------------------------------------------------------------------------------
//	void Octree::BuildTree()
//	{
//		// Early out if we're a leaf node (i.e. 0-1 objects), it doesn't make sense to further subdivide a node which only contains 1 object
//		if (objects_.size() <= 1)
//			return;
//
//		Vector3 extents = region_.Extents;
//		Vector3 center = region_.Center;
//
//		Vector3 oct_extents = extents / 2.0f;
//
//		DirectX::BoundingBox octants[8];
//		octants[0] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
//		octants[1] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
//		octants[2] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), -oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
//		octants[3] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), -oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
//		octants[4] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);
//		octants[5] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);
//		octants[6] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), -oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);
//		octants[7] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), -oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);
//
//		std::vector<Renderable*> obj_per_octant[8];
//		std::vector<Renderable*> remove_from_self;
//
//		Renderable* obj;
//		for (int i = 0; i < objects_.size(); i++)
//		{
//			obj = objects_[i];
//			
//			for (int o = 0; o < 8; o++)
//			{
//				if (octants[o].Contains(obj->GetBoundingBox()) == DirectX::ContainmentType::CONTAINS)
//				{
//					obj_per_octant[o].push_back(obj);
//					remove_from_self.push_back(obj);
//				}
//			}
//		}
//
//		for (int i = 0; i < remove_from_self.size(); i++)
//		{
//			for (int j = 0; j < objects_.size(); j++)
//			{
//				if (objects_[j] == remove_from_self[i])
//				{
//					objects_.erase(objects_.begin() + j);
//				}
//			}
//		}
//
//		for (int i = 0; i < 8; i++)
//		{
//			if (obj_per_octant[i].size() > 0)
//			{
//				children_[i] = CreateNode(octants[i], obj_per_octant[i]);
//				children_[i]->BuildTree();
//				active_nodes |= static_cast<unsigned char>(1 << i);
//				has_children_ = true;
//			}
//		}
//	}
//	
//	//------------------------------------------------------------------------------------------------------
//	void Octree::FindEnclosingCube()
//	{
//		Vector3 global_min, global_max;
//		GetBoundingBoxMinMax(region_, &global_min, &global_max);
//
//		//go through all the objects in the list and find the extremes for their bounding areas.
//		for (int i = 0; i < objects_.size(); i++)
//		{
//			Renderable* obj = objects_[i];
//
//			Vector3 local_min = Vector3(), local_max = Vector3();
//
//			//GetBoundingSphereMinMax(obj->GetMesh()->GetBounds(), &local_min, &local_max);
//
//			if (local_min.GetX() < global_min.GetX()) global_min.SetX(local_min.GetX());
//			if (local_min.GetY() < global_min.GetY()) global_min.SetY(local_min.GetY());
//			if (local_min.GetZ() < global_min.GetZ()) global_min.SetZ(local_min.GetZ());
//
//			if (local_max.GetX() > global_max.GetX()) global_max.SetX(local_max.GetX());
//			if (local_max.GetY() > global_max.GetY()) global_max.SetY(local_max.GetY());
//			if (local_max.GetZ() > global_max.GetZ()) global_max.SetZ(local_max.GetZ());
//		}
//
//		DirectX::BoundingBox::CreateFromPoints(region_, global_min.ToDxVec(), global_max.ToDxVec());
//	}
//
//	//------------------------------------------------------------------------------------------------------
//	void Octree::DrawTree(DirectX::XMFLOAT4 color)
//	{
//		DirectX::XMFLOAT4 col = DirectX::XMFLOAT4(color.x + 0.5f, color.y, color.z, color.w);
//
//		Get::Renderer()->RenderDebugVolume(DebugVolume::CreateCube(region_.Center, Vector3(region_.Extents) * 2.0f, col));
//
//		for (int flags = active_nodes, index = 0; flags > 0; flags >>= 1, index++)
//		{
//			if ((flags & 1) == 1)
//				children_[index]->DrawTree(col);
//		}
//	}
//
//	//------------------------------------------------------------------------------------------------------
//	void Octree::GetIntersectingObjects(const DirectX::BoundingFrustum& frustum, std::vector<Renderable*>& out)
//	{
//		//terminator for any unnecessary recursion
//		if (objects_.size() == 0 && has_children_ == false)
//			return;
//
//		//test each object in the list for intersection
//		for each(Renderable* obj in objects_)
//		{
//			//test for intersection
//			bool ir = obj->GetBoundingBox().Intersects(frustum);
//			if (ir != false) out.push_back(obj);
//		}
//
//		//test each object in the list for intersection
//		for (int a = 0; a < 8; a++)
//		{
//			if (children_[a] != nullptr && (frustum.Contains(children_[a]->region_) != DirectX::ContainmentType::DISJOINT))
//			{
//				children_[a]->GetIntersectingObjects(frustum, out);
//			}
//		}
//	}
//
//	//------------------------------------------------------------------------------------------------------
//	void Octree::GetIntersectingObjects(const PlaneFrustum& frustum, std::vector<Renderable*>& out)
//	{
//		//terminator for any unnecessary recursion
//		if (objects_.size() == 0 && has_children_ == false)
//			return;
//
//		//test each object in the list for intersection
//		for each(Renderable* obj in objects_)
//		{
//			//test for intersection
//			if (obj->GetBoundingBox().ContainedBy(frustum.near_plane, frustum.far_plane, frustum.right_plane, frustum.left_plane, frustum.top_plane, frustum.bottom_plane) != DirectX::ContainmentType::DISJOINT)
//			{
//				out.push_back(obj);
//			}
//		}
//
//		//test each object in the list for intersection
//		for (int a = 0; a < 8; a++)
//		{
//			if (children_[a] != nullptr && children_[a]->region_.ContainedBy(frustum.near_plane, frustum.far_plane, frustum.right_plane, frustum.left_plane, frustum.top_plane, frustum.bottom_plane) != DirectX::ContainmentType::DISJOINT)
//			{
//				children_[a]->GetIntersectingObjects(frustum, out);
//			}
//		}
//	}
//
//	//------------------------------------------------------------------------------------------------------
//	Octree* Octree::CreateNode(const DirectX::BoundingBox& region, const std::vector<Renderable*>& objects)
//	{
//		if (objects.size() == 0)
//			return nullptr;
//
//		Octree* ret = new Octree(region, objects);
//		ret->parent_ = this;
//
//		return ret;
//	}
//	
//	//------------------------------------------------------------------------------------------------------
//	void GetBoundingBoxMinMax(const DirectX::BoundingBox& bounds, Vector3* out_min, Vector3* out_max)
//	{
//		*out_max = Vector3(bounds.Extents) + Vector3(bounds.Center);
//		*out_min = -(*out_max);
//	}
//	
//	//------------------------------------------------------------------------------------------------------
//	void GetBoundingSphereMinMax(const DirectX::BoundingSphere& bounds, Vector3* out_min, Vector3* out_max)
//	{
//		*out_min = Vector3(
//			bounds.Center.x - bounds.Radius,
//			bounds.Center.y - bounds.Radius,
//			bounds.Center.z - bounds.Radius
//		);
//		*out_max = Vector3(
//			bounds.Center.x + bounds.Radius,
//			bounds.Center.y + bounds.Radius,
//			bounds.Center.z + bounds.Radius
//		);
//	}
//}