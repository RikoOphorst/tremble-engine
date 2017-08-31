#pragma once

#include "../game_manager.h"
#include "component_manager.h"
#include "../../components/rendering/renderable.h"
#include "../rendering/renderer.h"
#include "../memory/memory_includes.h"
#include "../math/math.h"
#include "../get.h"
#include "../utilities/utilities.h"
#include <set>

#define CHECK_THIS_DYNAMIC()	\
{	\
	if (is_static_ == true)	\
	{	\
		_CrtDbgBreak();	\
	}	\
}	\

namespace tremble
{
	HAS_MEM_FUNC(Awake, has_awake);

	class Component;
	class GameManager;
	class FreeListAllocator;

	/**
	* @class SGNode
	* @brief A scene graph node. Has distinction between a static and dynamic node.
	*
	* A static node is not meant to be moved, rotated, or scaled. 
	* @author Anton Gavrilov
	*/

	class SGNode
	{
        friend class GameManager;
		friend class FBXLoader;
        friend class RigidbodyDynamic; 
        friend class ComponentManager;
	public:
		SGNode(SGNode* parent); //!< Creates a dynamic scene graph node
		/**
		* @brief Construct a scene graph node with with an offset. The only way to construct a static node
		*/
		SGNode(SGNode* parent, bool is_static, const Vector3& position = Vector3(0.0f, 0.0f, 0.0f), const Quaternion& rotation = Quaternion(), const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f));
        ~SGNode(); //!< Destructor, which calls destroy function

		void Delete(); //!< Destroy this node, everything beneath it (components and nodes) and remove a pointer to this from the parent
        void DeleteChildren(); //!< Destroy children of this node.

		bool IsStatic() { return is_static_; } //!< Is this node static?
		bool IsDynamic() { return !is_static_; } //!< Is this node dynamic? (opposite of IsStatic)
        bool WasMoved() { return moved_by_physics_ || moved_by_user_; }  //!< Was the object transformed since the last frame? This value is reset before every Update()
        bool WasMovedByUser() { return moved_by_user_; } //!< Was the object moved by user since the last frame?
        bool WasMovedByPhysics() { return moved_by_physics_; } //!< Was the object moved by physics since last frame?

		//////////////POSITION

		const Vector3& GetPosition(); //!< Get world position of this node
	    void SetPosition(const Vector3& position); //!< Set world position of this node

		const Vector3& GetLocalPosition() { return position_; } //!< Get local position of this node. Updates world transform matrix
		void SetLocalPosition(const Vector3& position); //!< Set local position of this node. Can be used only with dynamic nodes. Marks world transform dirty. 

		void Move(const Vector3& movement); //!< Move the position of this node. Can be used only with dynamic nodes. Marks world transform dirty. 
		
		/////////////ROTATION

        void SetRotationQuaternion(const Quaternion& world_rotation); //!< Set world rotation of this node with a quaternion. Can be used only with dynamic nodes. Marks world transform dirty. 
        void SetRotationRadians(const Vector3& rotation_radians); //!< Set world rotation of this node with a vector, containing rotations in radians(x,y,z). Can be used only with dynamic nodes. Marks world transform dirty. 
        void SetRotationDegrees(const Vector3& rotation_degrees); //!< Set world rotation of this node with a vector, containing rotations in degrees(x,y,z). Can be used only with dynamic nodes. Marks world transform dirty. 

		void SetLocalRotationQuaternion(const Quaternion& rotation_quaternion); //!< Set local rotation of this node with a quaternion. Can be used only with dynamic nodes. Marks world transform dirty. 
		void SetLocalRotationRadians(const Vector3& rotation_radians); //!< Set local rotation of this node with a vector, containing rotations in radians(x,y,z). Can be used only with dynamic nodes. Marks world transform dirty. 
		void SetLocalRotationDegrees(const Vector3& rotation_degrees); //!< Set local rotation of this node with a vector, containing rotations in degrees(x,y,z). Can be used only with dynamic nodes. Marks world transform dirty. 

		void RotateQuaternion(const Quaternion& rotation); //!< Rotate this object using a quaternion
		void RotateDegrees(const Vector3& rotation_degrees); //!< Rotate this object using degees 
		void RotateRadians(const Vector3& rotation_radians); //!< Rotate this object uing radians

		void RotateXRadians(Scalar radians_to_rotate); //!< Rotate this object around X axis by a certain angle around the x axis in radians) @param[in] local_axis Should the axis be the local axis of this node? 
		void RotateYRadians(Scalar radians_to_rotate); //!< Rotate this object around Y axis by a certain angle around the y axis in radians) @param[in] local_axis Should the axis be the local axis of this node? 
		void RotateZRadians(Scalar radians_to_rotate); //!< Rotate this object around Z axis by a certain angle around the z axis in radians) @param[in] local_axis Should the axis be the local axis of this node? 

		void RotateXDegrees(Scalar degrees_to_rotate); //!< Rotate this object around X axis by a certain angle around the world x axis in degrees
		void RotateYDegrees(Scalar degrees_to_rotate); //!< Rotate this object around Y axis by a certain angle around the world y axis in degrees
		void RotateZDegrees(Scalar degrees_to_rotate); //!< Rotate this object around Z axis by a certain angle around the world z axis in degrees

		const Quaternion& GetLocalRotationQuaternion() { return rotation_; } //!< Get local quaternion rotation of this node 
		const Vector3 GetLocalRotationRadians(); //!< Get local rotation of this node represented by a vector, containing angles in radians(x, y, z)
		const Vector3 GetLocalRotationDegrees(); //!< Get local rotation of this node represented by a vector, containing angles in degrees(x, y, z)

		const Quaternion GetRotationQuaternion(); //!< Get world quaternion rotation of this node 
		const Vector3 GetRotationRadians(); //!< Get world rotation of this node represented by a vector, containing angles in radians(x, y, z)
		const Vector3 GetRotationDegrees(); //!< Get world rotation of this node represented by a vector, containing angles in degrees(x, y, z)

		///////////////SCALE

		void SetLocalScale(const Vector3& scale); //!< Setlocal scale of this node.  Can be used only with dynamic nodes. Marks world transform dirty.
		const Vector3& GetScale(); //!< Get world scale of this node
		const Vector3& GetLocalScale() { return scale_; } //!< Get local scale of this node

		///////////////TRANSFORMATION MATRIX

		const Mat44& GetWorldTransform(); //!< Get world transform matrix of this node

		///////////////DIRECTIONS

		const Vector3 GetForwardVectorWorld(); //!< Get a vector pointing forward of this node in world space (0, 0, 1 in case of no rotation)
		const Vector3 GetRightVectorWorld(); //!< Get a vector pointing right of this node in world space (1, 0, 0 in case of no rotation)
		const Vector3 GetUpVectorWorld(); //!< Get a vector pointing right of this node in world space (1, 0, 0 in case of no rotation)

		///////////////PARENT CHILD COMPONENT RELASHIONSHIPS 

		SGNode* GetParent() { return parent_; } //!< Get parent of this node
		SGNode* AddChild(bool create_static = false, Vector3 position = Vector3(), Vector3 Rotation = Vector3(), Vector3 scale = Vector3(1, 1, 1)); //!< Add a child to this node
        const std::vector<SGNode*>& GetChildren() { return children_; }

		template<typename T, typename... Args>
		T* AddComponent(Args... args); //!< Creates a component and adds it to the components vector of pointers + to a component manager

        template<typename T>
        std::enable_if_t<!has_awake<T, void(T::*)()>::value, T*>
        AddComponent(); //!< Creates a component and adds it to the components vector of pointers + to a component manager

        template<typename T>
        std::enable_if_t<has_awake<T, void(T::*)()>::value, T*>
        AddComponent(); //!< Creates a component and adds it to the components vector of pointers + to a component manager

		template <class T>
		void DeleteComponent(T* component); //!< Removes a component from the component manager and deletes it
		void DeleteComponents(); //!< Deletes all components of this node and removes them from the components vector.

		//////////////FIND COMPONENTS

		template <class T>
		T* FindComponent(); //!< Finds the first component of type T, that is attached to this object

		template <class T>
		std::vector<T*> FindComponents(); //!< Find all the components of type T, that are attached to this object

		template <class T>
		T* FindComponentInChildren(); //!< Find the first component of type T in this entity or any of it's children

		template <class T>
		std::vector<T*> FindComponentsInChildren(); //!< Find all components of type T, attached to this object, or any of it's children

		template <class T>
		void FindComponentsInChildren(std::vector<T*>*);

        template <class T>
        void FindComponentsInChildren(std::vector<Component*>*);

		template <class T>
		static const std::vector<T*>& FindAllComponents(); //!< Find all the components of type t 

		/////////////////OTHER

        void DoOnCollisionCallbacks(const CollisionData& collision_data);
        void DoTriggerEnterCallbacks(const Component& other_component);
        void DoTriggerExitCallbacks(const Component& other_component);

        void AssociateWithPeer(PeerID associate_with) { associated_with_ = associate_with; };

	protected:
        PeerID associated_with_ = -69;

		SGNode(); //!< Constuctor, only called by scene. To construct a node by yourself, call one of the other two
		bool is_static_; //!< Is this node static?

        void DecoupleChild_(SGNode* new_child); //!< Add a new child pointer to children's vector. Called only from SGNode's constructor
        void CoupleChild_(SGNode* child_to_remove); //!< Remove a chid pointer from childrens_ vector
        void DecoupleComponent_(Component* component); //!< Removes the component from the components vector. Doesn't actually destroy it

        void SetPositionForPhysics_(const Vector3& world_position); //!< Exact same as set position, but it doesn't set the moved by user flag to true. For physics 
        void SetLocalPositionForPhysics_(const Vector3& local_position); //!< Exact same as set local position, but it doesn't set the moved by user flag to true. For physics 
        void SetRotationQuaternionForPhysics_(const Quaternion& world_rotation); //!< Exact same as set rotation, but it doesn't set the moved by user flag to true. For physics 
        void SetLocalRotationQuaternionForPhysics_(const Quaternion& local_rotation_quaternion);//!< Exact same as set rotation quaternion, but it doesn't set the moved by user flag to true. For physics 
        
        bool moved_by_user_; //!< Was the node moved since the last frame? This value is reset before every physics update
        bool moved_by_physics_; //!< Was the node moved since the last frame? This value is reset before every physics update
        void ResetMovedByUser_(); //!< Reset the value that tells if the object was moved or not since the last frame
        void ResetMovedByPhysics_(); //!< Reset the value that tells if the object was moved or not since the last frame

		SGNode* parent_; //!< This node's parent
		std::vector<SGNode*> children_; //!< A vector of children, that this node owns

		//local transform values
		Vector3 position_; //!< Local position
		Quaternion rotation_; //!< Local rotation
		Vector3 scale_; //!< Local scale

		bool world_position_updated_;
		Vector3 world_position_;
		inline void InvalidateWorldPosition_();
        inline void InvalidateWorldPositionForPhysics_();
		inline void UpdateWorldPosition_();

		bool world_rotation_updated_;
		Quaternion world_rotation_;
		inline void InvalidateWorldRotation_();
        inline void InvalidateWorldRotationForPhysics_();
		inline void UpdateWorldRotation_();

		bool world_scale_updated_;
		Vector3 world_scale_;
		inline void InvalidateWorldScale_();
        inline void InvalidateWorldScaleForPhysics_();
		inline void UpdateWorldScale_();

		Mat44 world_transform_; //!< World transformation matrix
		bool world_transform_updated_; //!< Is the world transform matrix updated at this moment
		inline void InvalidateWorldTransform_(); //!< Mark world transform matrix of this node and it's children as dirty. Also mark the moved_by_user_ flag as true
        inline void InvalidateWorldTransformForPhysics_(); //!< Mark world transform matrix of this node and it's children as dirty. Also mark the moved_by_physics_ flag as true
		inline void UpdateWorldTransform_(); //!< Update this node's world transform 

		std::vector<Component*> components_; //!< Components of this node


        //////////////////Static members
        static FreeListAllocator* sg_allocator_; //!< Pointer to the allocator, used by the scene graph
        static std::queue<SGNode*> deletion_queue_; //!< Nodes, queued for deletion
        static void ClearDeletionQueue_();
	};


	//------------------------------------------------------------------------------------------------------
	class Scene : public SGNode
	{
	public:
		Scene(size_t memory_size);
		~Scene();
	};

	//------------------------------------------------------------------------------------------------------
	template<typename T, typename... Args>
	T* SGNode::AddComponent(Args... args)
	{
        T* created;
        if (associated_with_ != -69)
        {
            created = Get::ComponentManager()->AddComponent<T>(associated_with_);
        }
        else
        {
            created = Get::ComponentManager()->AddComponent<T>();
        }
		components_.push_back(created);
        created->Init(this, typeid(T));
		created->Awake(args...);
        Get::ComponentManager()->AddToStartQueue(created);
		return created;
	}

    //------------------------------------------------------------------------------------------------------
    template<typename T>
    std::enable_if_t<!has_awake<T, void(T::*)()>::value, T*>
        SGNode::AddComponent()
    {
        T* created;
        if (associated_with_ != -69)
        {
            created = Get::ComponentManager()->AddComponent<T>(associated_with_);
        }
        else
        {
            created = Get::ComponentManager()->AddComponent<T>();
        }
        components_.push_back(created);
        created->Init(this, typeid(T));
        Get::ComponentManager()->AddToStartQueue(created);
        return created;
    }

    //------------------------------------------------------------------------------------------------------
    template<typename T>
    std::enable_if_t<has_awake<T, void(T::*)()>::value, T*>
    SGNode::AddComponent()
    {
        T* created;
        if (associated_with_ != -69)
        {
            created = Get::ComponentManager()->AddComponent<T>(associated_with_);
        }
        else
        {
            created = Get::ComponentManager()->AddComponent<T>();
        }
        components_.push_back(created);
        created->Init(this, typeid(T));
        created->Awake();
        Get::ComponentManager()->AddToStartQueue(created);
        return created;
    }

	//------------------------------------------------------------------------------------------------------
	template<class T>
	inline void SGNode::DeleteComponent(T* component)
	{
        Get::ComponentManager()->DeleteComponent(component);
	}

	//------------------------------------------------------------------------------------------------------
	template<class T>
	inline T* SGNode::FindComponent()
	{
		for each(Component* i in components_)
		{
			if (i->GetType() == typeid(T))
			{
				return static_cast<T*>(i);
			}
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	template<class T>
	inline std::vector<T*> SGNode::FindComponents()
	{
		std::vector<T*> componentsFound;
		for each(Component* i in components_)
		{
			if (i->GetType() == typeid(T))
			{
				componentsFound.push_back(static_cast<T*>(i));
			}
		}
		return componentsFound;
	}

	//------------------------------------------------------------------------------------------------------
	template<class T>
	T* SGNode::FindComponentInChildren()
	{
		for each(Component* i in components_)
		{
			if (i->GetType() == typeid(T))
			{
				return static_cast<T*>(i);
			}
		}

		for each (SGNode* child in children_)
		{
			T* FoundInChild = child->FindComponentInChildren<T>();
			if (FoundInChild != nullptr)
			{
				return FoundInChild;
			}
		}
		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------
	template<class T>
	std::vector<T*> SGNode::FindComponentsInChildren()
	{
		std::vector<T*> foundComponents;
		for each(Component* i in components_)
		{
			if (i->GetType() == typeid(T))
			{
				foundComponents.push_back(static_cast<T*>(i));
			}
		}

		for each (SGNode* child in children_)
		{
			child->FindComponentsInChildren<T>(&foundComponents);
		}
		return foundComponents;
	}


	//------------------------------------------------------------------------------------------------------
	template<class T>
	void SGNode::FindComponentsInChildren(std::vector<T*>* container)
	{
		for each(Component* i in components_)
		{
			if (i->GetType() == typeid(T))
			{
				container->push_back(static_cast<T*>(i));
			}
		}

		for each (SGNode* child in children_)
		{
			child->FindComponentsInChildren<T>(container);
		}
	}

    //------------------------------------------------------------------------------------------------------
    template<class T>
    void SGNode::FindComponentsInChildren(std::vector<Component*>* container)
    {
        for each(Component* i in components_)
        {
            if (i->GetType() == typeid(T))
            {
                container->push_back(i);
            }
        }

        for each (SGNode* child in children_)
        {
            child->FindComponentsInChildren<T>(container);
        }
    }

	//------------------------------------------------------------------------------------------------------
	template<class T>
	static  const std::vector<T*>& SGNode::FindAllComponents()
	{
		return Get::ComponentManager()->GetComponents<T>();
	}
}