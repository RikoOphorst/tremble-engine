#include "scene_graph.h"
#include "../utilities/debug.h"
#include "../game_manager.h"
#include "../memory/memory_includes.h"
#include "../math/math.h"

namespace tremble
{
    std::queue<SGNode*> SGNode::deletion_queue_;
    FreeListAllocator* SGNode::sg_allocator_ = nullptr;

    //------------------------------------------------------------------------------------------------------
	void SGNode::DoOnCollisionCallbacks(const CollisionData& collision_data)
	{
		for each (Component* i in components_)
		{
			if (i->collision_callback_)
			{
				i->collision_callback_(collision_data);
			}
		}
	}

    //------------------------------------------------------------------------------------------------------
    void SGNode::DoTriggerEnterCallbacks(const Component& other_component)
    {
        for each (Component* i in components_)
        {
            if (i->trigger_enter_callback_)
            {
                i->trigger_enter_callback_(other_component);
            }
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::DoTriggerExitCallbacks(const Component& other_component)
    {
        for each (Component* i in components_)
        {
            if (i->trigger_exit_callback_)
            {
                i->trigger_exit_callback_(other_component);
            }
        }
    }

	//------------------------------------------------------------------------------------------------------
	SGNode::SGNode()
		:parent_(nullptr), is_static_(true)
	{
		position_ = DirectX::XMVectorReplicate(0.0f);
		rotation_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		scale_ = DirectX::XMVectorReplicate(1.0f);

		world_position_ = position_;
		world_rotation_ = rotation_;
		world_scale_ = scale_;

		world_position_updated_ = true;
		world_rotation_updated_ = true;
		world_scale_updated_ = true;

		world_transform_updated_ = true;

		world_transform_ =
			DirectX::XMMatrixScalingFromVector(scale_) *
			DirectX::XMMatrixRotationQuaternion(rotation_) *
			DirectX::XMMatrixTranslationFromVector(position_);
	}

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetPositionForPhysics_(const Vector3& world_position)
    {
        Mat44 inverse_global_transform = parent_->GetWorldTransform().Inverse();
        Vector3 local_position = inverse_global_transform * Vector4(world_position, 1);
        SetLocalPositionForPhysics_(local_position);
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetLocalPositionForPhysics_(const Vector3& position)
    {
        //Cannot move a static node
        CHECK_THIS_DYNAMIC();
        if (position != position_)
        {
            InvalidateWorldPositionForPhysics_();
        }
        position_ = position;
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetRotationQuaternionForPhysics_(const Quaternion& world_rotation)
    {
        CHECK_THIS_DYNAMIC();
        Quaternion local_rotation = ~parent_->GetRotationQuaternion() * world_rotation;
        SetLocalRotationQuaternionForPhysics_(local_rotation);
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetLocalRotationQuaternionForPhysics_(const Quaternion & rotation_quaternion)
    {
        //Cannot move a static node
        CHECK_THIS_DYNAMIC();
        if (rotation_ != rotation_quaternion)
        {
            InvalidateWorldRotationForPhysics_();
        }
        rotation_ = rotation_quaternion;
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::ResetMovedByPhysics_()
    {
        moved_by_physics_ = false;
        for each (SGNode* i in children_)
        {
            i->ResetMovedByPhysics_();
        }
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::ResetMovedByUser_()
    {
        moved_by_user_ = false;
        for each (SGNode* i in children_)
        {
            i->ResetMovedByUser_();
        }
    }

    //------------------------------------------------------------------------------------------------------
    SGNode::SGNode(SGNode* parent) :
        parent_(parent),
        is_static_(false),
        associated_with_(parent->associated_with_)
    {
        position_ = DirectX::XMVectorReplicate(0.0f);
        rotation_ = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        scale_ = DirectX::XMVectorReplicate(1.0f);

        world_transform_updated_ = false;

        world_position_updated_ = false;
        world_rotation_updated_ = false;
        world_scale_updated_ = false;

        parent_->CoupleChild_(this);
    }

    //------------------------------------------------------------------------------------------------------
    SGNode::SGNode(SGNode * parent, bool is_static, const Vector3& position, const Quaternion& rotation, const Vector3& scale) :
        parent_(parent),
        associated_with_(parent->associated_with_),
        is_static_(false) //A little hack to be able to use UpdateWorldTransform()
    {
        world_transform_updated_ = false;

        world_position_updated_ = false;
        world_rotation_updated_ = false;
        world_scale_updated_ = false;

        parent_->CoupleChild_(this);

        position_ = position;
        rotation_ = rotation;
        scale_ = scale;

        //Multiplication takes order as parentWT * S * R * T
        UpdateWorldTransform_();

        is_static_ = is_static;

        //A parent of a static node can only be static (or nonexistent, in case of scene)
        ASSERT(is_static == false || parent_ == nullptr || parent->IsStatic() == true);
    }

    //------------------------------------------------------------------------------------------------------
    SGNode::~SGNode()
    {
        ASSERT(children_.size() == 0);
        ASSERT(components_.size() == 0);
    }

    //------------------------------------------------------------------------------------------------------
    Scene::Scene(size_t memory_size)
    {
        ASSERT(sg_allocator_ == nullptr);
        sg_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(memory_size);
    }

    //------------------------------------------------------------------------------------------------------
    Scene::~Scene()
    {
        DeleteChildren();
        DeleteComponents();
        Get::ComponentManager()->ClearDeletionQueue_();
        SGNode::ClearDeletionQueue_();
        Get::MemoryManager()->DeleteAllocator(sg_allocator_);
        sg_allocator_ = nullptr;
    }

	//------------------------------------------------------------------------------------------------------
	inline void SGNode::InvalidateWorldPosition_()
	{
		if (world_position_updated_)
		{
            moved_by_user_ = true;
			world_transform_updated_ = false;
			world_position_updated_ = false;
			for each (SGNode* i in children_)
			{
				i->InvalidateWorldPosition_();
			}
		}
	}

    //------------------------------------------------------------------------------------------------------
    inline void SGNode::InvalidateWorldPositionForPhysics_()
    {
        if (world_position_updated_)
        {
            moved_by_physics_ = true;
            world_transform_updated_ = false;
            world_position_updated_ = false;
            for each (SGNode* i in children_)
            {
                i->InvalidateWorldPositionForPhysics_();
            }
        }
    }

	//------------------------------------------------------------------------------------------------------
	inline void SGNode::UpdateWorldPosition_()
	{
		world_position_ = parent_->GetPosition() + (parent_->GetRotationQuaternion() * position_) * parent_->GetScale();
		world_position_updated_ = true;
	}

	//------------------------------------------------------------------------------------------------------
	inline void SGNode::InvalidateWorldRotation_()
	{
		CHECK_THIS_DYNAMIC();
		if (world_rotation_updated_)
		{
            moved_by_user_ = true;
			world_transform_updated_ = false;
			world_rotation_updated_ = false;
            world_scale_updated_ = false;
			for each (SGNode* i in children_)
			{
				i->InvalidateWorldRotation_();
                i->InvalidateWorldPosition_();
			}
		}
	}

    //------------------------------------------------------------------------------------------------------
    inline void SGNode::InvalidateWorldRotationForPhysics_()
    {
        CHECK_THIS_DYNAMIC();
        if (world_rotation_updated_)
        {
            moved_by_physics_ = true;
            world_transform_updated_ = false;
            world_rotation_updated_ = false;
            world_scale_updated_ = false;
            for each (SGNode* i in children_)
            {
                i->InvalidateWorldRotationForPhysics_();
                i->InvalidateWorldPositionForPhysics_();
            }
        }
    }

	//------------------------------------------------------------------------------------------------------
	inline void SGNode::UpdateWorldRotation_()
	{
		CHECK_THIS_DYNAMIC();
		world_rotation_ = rotation_ * parent_->GetRotationQuaternion();
		world_rotation_updated_ = true;
	}

	//------------------------------------------------------------------------------------------------------
	inline void SGNode::InvalidateWorldScale_()
	{
		CHECK_THIS_DYNAMIC();
		if (world_scale_updated_)
		{
            moved_by_user_ = true;
			world_transform_updated_ = false;
			world_scale_updated_ = false;
			for each (SGNode* i in children_)
			{
				i->InvalidateWorldScale_();
                i->InvalidateWorldPosition_();
			}
		}
	}

    //------------------------------------------------------------------------------------------------------
    inline void SGNode::InvalidateWorldScaleForPhysics_()
    {
        CHECK_THIS_DYNAMIC();
        if (world_scale_updated_)
        {
            moved_by_physics_ = true;
            world_transform_updated_ = false;
            world_scale_updated_ = false;
            for each (SGNode* i in children_)
            {
                i->InvalidateWorldScaleForPhysics_();
                i->InvalidateWorldPositionForPhysics_();
            }
        }
    }

	//------------------------------------------------------------------------------------------------------
	inline void SGNode::UpdateWorldScale_()
	{
		CHECK_THIS_DYNAMIC();
		//Vector3 scale = parent_->GetScale() * (GetLocalRotationQuaternion().Inverse() * scale_);
        //world_scale_ = Vector3(scale.GetX().Abs(), scale.GetY().Abs(), scale.GetZ().Abs());
        world_scale_ = GetParent()->GetScale() * GetLocalScale();
		world_scale_updated_ = true;
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::Delete()
	{
		DeleteChildren();
		DeleteComponents();
        deletion_queue_.push(this);
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::DeleteChildren()
	{
		size_t size = children_.size();
		for (int i = 0; i < size; i++)
		{
            children_[i]->Delete();
		}
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::SetLocalPosition(const Vector3& position)
	{
		//Cannot move a static node
		CHECK_THIS_DYNAMIC();
        if (position != position_)
        {
            InvalidateWorldPosition_();
        }
        position_ = position;
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::Move(const Vector3 & movement)
	{
		SetLocalPosition(position_ + movement);
	}

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetRotationQuaternion(const Quaternion& world_rotation)
    {
        CHECK_THIS_DYNAMIC();
        Quaternion local_rotation = ~parent_->GetRotationQuaternion() * world_rotation;
        rotation_ = local_rotation;
        InvalidateWorldRotation_();
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetRotationRadians(const Vector3& rotation_radians)
    {
        SetRotationQuaternion(Quaternion(rotation_radians.GetZ(), rotation_radians.GetX(), rotation_radians.GetY()));
    }

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetRotationDegrees(const Vector3& rotation_radians)
    {
        SetRotationQuaternion(Quaternion(rotation_radians.GetZ().ToRadians(), rotation_radians.GetX().ToRadians(), rotation_radians.GetY().ToRadians()));
    }

	//------------------------------------------------------------------------------------------------------
	void SGNode::SetLocalRotationQuaternion(const Quaternion& rotation_quaternion)
	{
		//Cannot rotate a static node
		CHECK_THIS_DYNAMIC();
		rotation_ = rotation_quaternion;
		InvalidateWorldRotation_();
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::SetLocalRotationRadians(const Vector3& rotation_radians)
	{
		SetLocalRotationQuaternion(Quaternion(rotation_radians));
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::SetLocalRotationDegrees(const Vector3& rotation_degrees)
	{
		SetLocalRotationQuaternion(Quaternion(rotation_degrees.GetZ().ToRadians(), rotation_degrees.GetX().ToRadians(), rotation_degrees.GetY().ToRadians()));
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::SetLocalScale(const Vector3& scale)
	{
		//Cannot scale a static node
		CHECK_THIS_DYNAMIC();
		scale_ = scale;
		InvalidateWorldScale_();
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateQuaternion(const Quaternion& rotation)
	{
		SetLocalRotationQuaternion(rotation_ * rotation);
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateRadians(const Vector3& rotation_radians)
	{
		RotateQuaternion(Quaternion(rotation_ * Vector3(rotation_radians.GetX(), Scalar(0.0f), rotation_radians.GetZ())) * Quaternion(Vector3(0, 1, 0), rotation_radians.GetY()));
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateDegrees(const Vector3& rotation_degrees)
	{
		RotateRadians(Vector3(rotation_degrees.GetX().ToRadians(), rotation_degrees.GetY().ToRadians(), rotation_degrees.GetZ().ToRadians()));
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateXRadians(const Scalar radians_to_rotate)
	{
		RotateQuaternion(Quaternion(rotation_ * Vector3(1, 0, 0), radians_to_rotate));
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateYRadians(const Scalar radians_to_rotate)
	{
		RotateQuaternion(Quaternion(Vector3(0, 1, 0), radians_to_rotate));
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateZRadians(const Scalar radians_to_rotate)
	{
		RotateQuaternion(Quaternion(rotation_ * Vector3(0, 0, 1), radians_to_rotate));
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateXDegrees(const Scalar degrees_to_rotate)
	{
		RotateXRadians(degrees_to_rotate.ToRadians());
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateYDegrees(const Scalar degrees_to_rotate)
	{
		RotateYRadians(degrees_to_rotate.ToRadians());
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::RotateZDegrees(const Scalar degrees_to_rotate)
	{
		RotateZRadians(degrees_to_rotate.ToRadians());
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3 SGNode::GetForwardVectorWorld()
	{
		return GetRotationQuaternion() * Vector3(0, 0, 1);
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3 SGNode::GetRightVectorWorld()
	{
		return GetRotationQuaternion() * Vector3(1, 0, 0);
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3 SGNode::GetUpVectorWorld()
	{
		return GetRotationQuaternion() * Vector3(0, 1, 0);
	}

	//------------------------------------------------------------------------------------------------------
	const Quaternion SGNode::GetRotationQuaternion()
	{
		if (!world_rotation_updated_)
		{
			UpdateWorldRotation_();
		}
		return world_rotation_;
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3 SGNode::GetRotationRadians()
	{
		return GetRotationQuaternion().ToEuler();
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3 SGNode::GetRotationDegrees()
	{
		Vector3 rotation = GetRotationRadians();
		rotation.SetX(rotation.GetX().ToDegrees());
		rotation.SetY(rotation.GetY().ToDegrees());
		rotation.SetZ(rotation.GetZ().ToDegrees());
		return rotation;
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3 SGNode::GetLocalRotationRadians()
	{
		return rotation_.ToEuler();
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3 SGNode::GetLocalRotationDegrees()
	{
		Vector3 rotation = GetLocalRotationRadians();
		rotation.SetX(rotation.GetX().ToDegrees());
		rotation.SetY(rotation.GetY().ToDegrees());
		rotation.SetZ(rotation.GetZ().ToDegrees());
		return rotation;
	}

	//------------------------------------------------------------------------------------------------------
	const Mat44& SGNode::GetWorldTransform()
	{
		if (world_transform_updated_ == false)
		{
			UpdateWorldTransform_();
		}
		return world_transform_;
	}

	//------------------------------------------------------------------------------------------------------
	const Vector3& SGNode::GetPosition()
	{
		if (world_position_updated_ == false)
		{
			UpdateWorldPosition_();
		}
		return world_position_;
	}

    //------------------------------------------------------------------------------------------------------
    void SGNode::SetPosition(const Vector3& world_position)
    {
        Mat44 inverse_global_transform = GetParent()->GetWorldTransform().Inverse();
        Vector3 local_position = inverse_global_transform * world_position;
        SetLocalPosition(local_position);
    }

	//------------------------------------------------------------------------------------------------------
	const Vector3& SGNode::GetScale()
	{
 		if (world_scale_updated_ == false)
		{
			UpdateWorldScale_();
		}
		return world_scale_;
	}

	//------------------------------------------------------------------------------------------------------
	SGNode* SGNode::AddChild(bool create_static, Vector3 position, Vector3 rotation_radians, Vector3 scale)
	{
		return sg_allocator_->New<SGNode>(this, create_static, position, Quaternion(rotation_radians), scale);
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::CoupleChild_(SGNode* new_child)
	{
		children_.push_back(new_child);
	}

    //------------------------------------------------------------------------------------------------------
    void SGNode::DecoupleComponent_(Component* component)
    {
        auto find = std::find(components_.begin(), components_.end(), component);
        ASSERT(find != components_.end());
        components_.erase(find);
    }

	//------------------------------------------------------------------------------------------------------
	void SGNode::DecoupleChild_(SGNode* child_to_remove)
	{
		std::vector<SGNode*>::iterator found_child = std::find(children_.begin(), children_.end(), child_to_remove);
		ASSERT(found_child != children_.end());
		children_.erase(found_child);
	}

	//------------------------------------------------------------------------------------------------------
	void SGNode::InvalidateWorldTransform_()
	{
		//World transform of a static node doesn't make sense to be invalidated.
		//It gets constructed one time during initialization of a static node and stays like this all time
		CHECK_THIS_DYNAMIC();
		if (world_transform_updated_ == true)
		{
            moved_by_user_ = true;
			world_transform_updated_ = false;
			for each (SGNode* i in children_)
			{
				i->InvalidateWorldTransform_();
			}
		}
	}

    //------------------------------------------------------------------------------------------------------
    inline void SGNode::InvalidateWorldTransformForPhysics_()
    {
        //World transform of a static node doesn't make sense to be invalidated.
        //It gets constructed one time during initialization of a static node and stays like this all time
        CHECK_THIS_DYNAMIC();
        if (world_transform_updated_ == true)
        {
            moved_by_physics_ = true;
            world_transform_updated_ = false;
            for each (SGNode* i in children_)
            {
                i->InvalidateWorldTransformForPhysics_();
            }
        }
    }

	//------------------------------------------------------------------------------------------------------
	void SGNode::UpdateWorldTransform_()
	{
		//World transform of a static node doesn't make sense to be updated.
		//It gets constructed one time during initialization of a static node and stays like this all time
		CHECK_THIS_DYNAMIC();

		world_transform_ =
			DirectX::XMMatrixScalingFromVector(GetScale()) *
			DirectX::XMMatrixRotationQuaternion(GetRotationQuaternion()) *
			DirectX::XMMatrixTranslationFromVector(GetPosition());

		world_transform_updated_ = true;
	}

    //------------------------------------------------------------------------------------------------------
    void SGNode::ClearDeletionQueue_()
    {
        // ASSERT(iter->children_.size() == 0);
        // ASSERT(iter->components_.size() == 0);
        while (deletion_queue_.size() > 0)
        {
            deletion_queue_.front()->parent_->DecoupleChild_((deletion_queue_.front()));
            sg_allocator_->Delete(deletion_queue_.front());
            deletion_queue_.pop();
        }
    }

	//------------------------------------------------------------------------------------------------------
	void SGNode::DeleteComponents()
	{
		size_t size = components_.size();
		for (size_t i = size - 1; i >= 0; i--)
		{
			Get::ComponentManager()->DeleteComponent(components_[i]);
		}
	}
}