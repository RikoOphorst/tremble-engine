#include "octree.h"

#include "../get.h"
#include "../math/math.h"
#include "../resources/mesh.h"
#include "../rendering/renderer.h"
#include "../memory/memory_includes.h"
#include "../../components/rendering/renderable.h"

namespace tremble
{
	using namespace DirectX;

	//------------------------------------------------------------------------------------------------------
	Octree::Octree(const DirectX::BoundingBox& region)
	{
		region_ = region;
		max_lifespan_ = 8;
		cur_lifespan_ = -1;
		allocator_ = Get::MemoryManager()->GetNewAllocator<PoolAllocator, Octree>(sizeof(Octree) * 4096);

		parent_ = nullptr;
		active_child_nodes_ = 0;
		for (int i = 0; i < 8; i++)
		{
			child_nodes_[i] = nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------
	Octree::Octree(const DirectX::BoundingBox& region, const std::vector<OctreeObject*>& objects, Octree* parent)
	{
		objects_ = objects;
		region_ = region;
		max_lifespan_ = 8;
		cur_lifespan_ = -1;
		allocator_ = nullptr;

		parent_ = parent;
		active_child_nodes_ = 0;
		for (int i = 0; i < 8; i++)
		{
			child_nodes_[i] = nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------
	Octree::~Octree()
	{
		for (int i = 0; i < 8; i++)
		{
			if (child_nodes_[i] != nullptr)
			{
				GetAllocator()->Delete(child_nodes_[i]);
			}
		}

		if (allocator_ != nullptr)
		{
			Get::MemoryManager()->DeleteAllocator(allocator_);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Octree::Update()
	{
		// Octree lifetime updates
		if (objects_.size() == 0 && active_child_nodes_ == 0)
		{
			if (cur_lifespan_ == -1)
			{
				cur_lifespan_ = max_lifespan_;
			}
			else if (cur_lifespan_ > 0)
			{
				cur_lifespan_--;
			}
		}
		else
		{
			if (max_lifespan_ <= 64)
			{
				max_lifespan_ *= 2;
			}
		}
		
		// Find all of this octree's objects that moved over the last frame & erase ones that are no longer alive
		std::vector<OctreeObject*> moved_objects;
		for (int i = static_cast<int>(objects_.size()) - 1; i >= 0; i--)
		{
			if (objects_[i]->alive)
			{
				if (objects_[i]->updated)
				{
					moved_objects.push_back(objects_[i]);
				}
			}
			else
			{
				objects_.erase(objects_.begin() + i);
			}
		}

		// Recursively update any child nodes
		for (int i = 0; i < 8; i++)
		{
			if (child_nodes_[i] != nullptr)
			{
				child_nodes_[i]->Update();
			}
		}

		// For all moved objects, attempt to find the octree region in which they fit
		// If one octree node is found to be able to contain the moved object, assign the object to that node
		for (int i = static_cast<int>(moved_objects.size()) - 1; i >= 0; i--)
		{
			Octree* current = this;

			// Recursively iterate parent octree nodes whether we they are able to contain the moved object
			while (current->region_.Contains(moved_objects[i]->bounds) != ContainmentType::CONTAINS)
			{
				if (current->parent_ != nullptr)
				{
					current = current->parent_;
				}
				else
				{
					break;
				}
			}

			// Remove the moved object from this octree's objects
			for (int j = static_cast<int>(objects_.size()) - 1; j >= 0; j--)
			{
				if (objects_[j] == moved_objects[i])
				{
					objects_.erase(objects_.begin() + j);
				}
			}

			// Add the moved object to the octree that /is/ able to contain the moved object
			current->Insert(moved_objects[i]);
		}

		// Prune any dead child nodes (i.e. no more lifespan)
		for (int i = 0; i < 8; i++)
		{
			if (child_nodes_[i] != nullptr && child_nodes_[i]->cur_lifespan_ == 0)
			{
				GetAllocator()->Delete(child_nodes_[i]);
				child_nodes_[i] = nullptr;
				active_child_nodes_ ^= (unsigned char)(1U << i);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Octree::Draw()
	{
		Get::Renderer()->RenderDebugVolume(
			DebugVolume::CreateCube(
				region_.Center,
				DirectX::XMFLOAT3(
					region_.Extents.x * 2.0f,
					region_.Extents.y * 2.0f,
					region_.Extents.z * 2.0f
				)
			)
		);

		for (int i = 0; i < 8; i++)
		{
			if (child_nodes_[i] != nullptr)
			{
				child_nodes_[i]->Draw();
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Octree::Insert(OctreeObject* object)
	{
		/*make sure we're not inserting an object any deeper into the tree than we have to.
		-if the current node is an empty leaf node, just insert and leave it.*/
		if (objects_.size() <= 1 && active_child_nodes_ == 0)
		{
			objects_.push_back(object);
			return;
		}

		Vector3 extents = region_.Extents;
		Vector3 center = region_.Center;

		if (extents.LengthSq() == 0)
		{
			FindEnclosingCube();
			extents = region_.Extents;
			center = region_.Center;
		}

		if (extents.GetX() * Scalar(2.0f) <= MIN_SIZE && extents.GetY() * Scalar(2.0f) <= MIN_SIZE && extents.GetZ() * Scalar(2.0f) <= MIN_SIZE)
		{
			objects_.push_back(object);
			return;
		}

		Vector3 oct_extents = extents / 2.0f;

		DirectX::BoundingBox octants[8];
		octants[0] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
		octants[1] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
		octants[2] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), -oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
		octants[3] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), -oct_extents.GetY(), oct_extents.GetZ()), oct_extents);
		octants[4] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);
		octants[5] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);
		octants[6] = DirectX::BoundingBox(center + Vector3(-oct_extents.GetX(), -oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);
		octants[7] = DirectX::BoundingBox(center + Vector3(oct_extents.GetX(), -oct_extents.GetY(), -oct_extents.GetZ()), oct_extents);

		if (region_.Contains(object->bounds) == ContainmentType::CONTAINS)
		{
			bool found = false;
			for (int a = 0; a < 8; a++)
			{
				if (octants[a].Contains(object->bounds) == ContainmentType::CONTAINS)
				{
					if (child_nodes_[a] != nullptr)
					{
						child_nodes_[a]->Insert(object);
					}
					else
					{
						child_nodes_[a] = CreateNode(octants[a], object);
						active_child_nodes_ |= (unsigned char)(1 << a);
					}
					found = true;
				}
			}

			if (!found)
			{
				objects_.push_back(object);
			}
		}
		else
		{
			//assert(false && "Tried to insert an object into an octree region while the object doesn't fit.");
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Octree::GetContainedObjects(const PlaneFrustum& frustum, std::vector<OctreeObject*>& out)
	{
		//terminator for any unnecessary recursion
		if (objects_.size() == 0 && active_child_nodes_ == 0)
			return;

		//test each object in the list for intersection
		for each(OctreeObject* obj in objects_)
		{
			//test for intersection
			if (obj->bounds.ContainedBy(frustum.near_plane, frustum.far_plane, frustum.right_plane, frustum.left_plane, frustum.top_plane, frustum.bottom_plane) != DirectX::ContainmentType::DISJOINT)
			{
				out.push_back(obj);
			}
		}

		//test each object in the list for intersection
		for (int a = 0; a < 8; a++)
		{
			if (child_nodes_[a] != nullptr && child_nodes_[a]->region_.ContainedBy(frustum.near_plane, frustum.far_plane, frustum.right_plane, frustum.left_plane, frustum.top_plane, frustum.bottom_plane) != DirectX::ContainmentType::DISJOINT)
			{
				child_nodes_[a]->GetContainedObjects(frustum, out);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	void Octree::GetContainedObjects(const BoundingSphere& sphere, std::vector<OctreeObject*>& out) const
	{
		//terminator for any unnecessary recursion
		if (objects_.size() == 0 && active_child_nodes_ == 0)
			return;

		//test each object in the list for intersection
		for each(OctreeObject* obj in objects_)
		{
			//test for intersection
			if (obj->bounds.Contains(sphere) != ContainmentType::DISJOINT)
			{
				out.push_back(obj);
			}
		}

		//test each object in the list for intersection
		for (int a = 0; a < 8; a++)
		{
			if (child_nodes_[a] != nullptr && child_nodes_[a]->region_.Contains(sphere) != ContainmentType::DISJOINT)
			{
				child_nodes_[a]->GetContainedObjects(sphere, out);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------
	int Octree::GetNodeCount()
	{
		int temp = 0;

		for (int i = 0; i < objects_.size(); i++)
		{
			temp++;
		}

		for (int i = 0; i < 8; i++)
		{
			if (child_nodes_[i] != nullptr)
			{
				temp += child_nodes_[i]->GetNodeCount();
			}
		}

		return temp;
	}

	//------------------------------------------------------------------------------------------------------
	Octree* Octree::CreateNode(const DirectX::BoundingBox& region, const std::vector<OctreeObject*>& objects)
	{
		if (objects.size() == 0)
		{
			return nullptr;
		}

		Octree* ret = GetAllocator()->New<Octree>(region, objects, this);

		return ret;
	}

	//------------------------------------------------------------------------------------------------------
	Octree* Octree::CreateNode(const DirectX::BoundingBox& region, OctreeObject* object)
	{
		if (object == nullptr)
		{
			return nullptr;
		}

		std::vector<OctreeObject*> objects;
		objects.push_back(object);
		Octree* ret = GetAllocator()->New<Octree>(region, objects, this);

		return ret;
	}

	//------------------------------------------------------------------------------------------------------
	void Octree::FindEnclosingCube()
	{
		Vector3 global_min, global_max;
		GetBoundingBoxMinMax(region_, &global_min, &global_max);

		//go through all the objects in the list and find the extremes for their bounding areas.
		for (int i = 0; i < objects_.size(); i++)
		{
			OctreeObject* obj = objects_[i];

			Vector3 local_min = Vector3(), local_max = Vector3();

			GetBoundingBoxMinMax(obj->bounds, &local_min, &local_max);

			if (local_min.GetX() < global_min.GetX()) global_min.SetX(local_min.GetX());
			if (local_min.GetY() < global_min.GetY()) global_min.SetY(local_min.GetY());
			if (local_min.GetZ() < global_min.GetZ()) global_min.SetZ(local_min.GetZ());

			if (local_max.GetX() > global_max.GetX()) global_max.SetX(local_max.GetX());
			if (local_max.GetY() > global_max.GetY()) global_max.SetY(local_max.GetY());
			if (local_max.GetZ() > global_max.GetZ()) global_max.SetZ(local_max.GetZ());
		}

		DirectX::BoundingBox::CreateFromPoints(region_, global_min.ToDxVec(), global_max.ToDxVec());
	}

	//------------------------------------------------------------------------------------------------------
	PoolAllocator* Octree::GetAllocator()
	{
		if (parent_ != nullptr)
		{
			return parent_->GetAllocator();
		}
		else
		{
			return allocator_;
		}
	}

	//------------------------------------------------------------------------------------------------------
	void GetBoundingBoxMinMax(const DirectX::BoundingBox& bounds, Vector3* out_min, Vector3* out_max)
	{
		*out_max = Vector3(bounds.Extents) + Vector3(bounds.Center);
		*out_min = -(*out_max);
	}

	//------------------------------------------------------------------------------------------------------
	void GetBoundingSphereMinMax(const DirectX::BoundingSphere& bounds, Vector3* out_min, Vector3* out_max)
	{
		*out_min = Vector3(
			bounds.Center.x - bounds.Radius,
			bounds.Center.y - bounds.Radius,
			bounds.Center.z - bounds.Radius
		);
		*out_max = Vector3(
			bounds.Center.x + bounds.Radius,
			bounds.Center.y + bounds.Radius,
			bounds.Center.z + bounds.Radius
		);
	}
}