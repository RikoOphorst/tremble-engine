#include "physics_manager.h"

#include "../memory/memory_includes.h"
#include "../scene_graph/component_manager.h"
#include "../scene_graph/scene_graph.h"
#include "../get.h"
#include "../math/math.h"
#include "physics_material.h"
#include "physics_geometry.h"

namespace tremble
{
	using namespace physx;

	//------------------------------------------------------------------------------------------------------
	PhysicsManager::AllocatorCallback::AllocatorCallback(FreeListAllocator* physx_allocator)
		:physx_allocator_(physx_allocator)
	{

	}

	//------------------------------------------------------------------------------------------------------
	void* PhysicsManager::AllocatorCallback::allocate(size_t size, const char* typeName, const char* filename, int line)
	{
		void* allocated = physx_allocator_->Allocate(size, 16);
        //void* allocated = _aligned_malloc(size, 16);
        ASSERT(allocated != nullptr);
        ASSERT(((unsigned long)allocated & 15) == 0);
        return allocated;
	}

	//------------------------------------------------------------------------------------------------------
	void PhysicsManager::AllocatorCallback::deallocate(void* ptr)
	{
		if (ptr != nullptr)
		{
			return physx_allocator_->Deallocate(ptr);
            //_aligned_free(ptr);
		}
	}

	//------------------------------------------------------------------------------------------------------
	void PhysicsManager::ErrorCallback::reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line)
	{
		std::cout << "PhysX Error: " << message << std::endl;
	}

	//------------------------------------------------------------------------------------------------------
	void PhysicsManager::Update()
	{
		px_scene_->simulate(Get::DeltaT());
		px_scene_->fetchResults(true);
	}

	//------------------------------------------------------------------------------------------------------
	Vector3 PhysicsManager::GetGravity()
	{
		return Vector3(px_scene_->getGravity());
	}

	//------------------------------------------------------------------------------------------------------
	bool PhysicsManager::Raycast(PxRaycastBuffer* buffer, Vector3 origin, Vector3 direction, float max_distance, std::vector<Component*>* ignored_components)
	{
        if (ignored_components == nullptr)
        {
            px_scene_->raycast(origin.ToPxVec3(), direction.ToPxVec3(), max_distance, *buffer);
        }
        else
        {
            query_filter_callback_.ignored_components = ignored_components;
            PxQueryFilterData filter_data;
            filter_data.flags |= PxQueryFlag::ePREFILTER;
            px_scene_->raycast(origin.ToPxVec3(), direction.ToPxVec3(), max_distance, *buffer, PxHitFlag::eDEFAULT, filter_data, &query_filter_callback_);
        }
		return buffer->hasBlock;
	}

	//------------------------------------------------------------------------------------------------------
	Component* PhysicsManager::GetComponentFromRaycast(physx::PxRaycastBuffer* buffer)
	{
		PxActor* actor = buffer->block.actor;
		return GetComponentFromPxActor(actor);
	}

	//------------------------------------------------------------------------------------------------------
	PhysicsMaterial* PhysicsManager::CreateMaterial(const std::string& name, float restitution, float dynamic_friction, float static_friction)
	{
		physx::PxMaterial* px_material = px_physics_->createMaterial(static_friction, dynamic_friction, restitution);
		//if no material with a name like this exists, create one
		std::unordered_map<std::string, PhysicsMaterial>::iterator found = physics_materials_.find(name);
		if (found == physics_materials_.end())
		{
			PhysicsMaterial* physics_material = &(physics_materials_.emplace(name, PhysicsMaterial(px_material)).first._Ptr->_Myval.second);
			px_material->userData = physics_material;
			return physics_material;
		}
		else
		{
			DLOG("trying to create material, when a material of the same name already exists")
				return nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------
	PhysicsMaterial* PhysicsManager::GetMaterial(std::string name)
	{
		std::unordered_map<std::string, PhysicsMaterial>::iterator found = physics_materials_.find(name);
		if (found != physics_materials_.end())
		{
			return &found->second;
		}
		else
		{
			return nullptr;
		}
	}

	//------------------------------------------------------------------------------------------------------
	physx::PxShape* PhysicsManager::CreatePxShape(PhysicsGeometry* geometry, PhysicsMaterial* material, bool is_exclusive)
	{
		return px_physics_->createShape(*geometry->GetPxGeometry(), *material->GetPxMaterial(), is_exclusive);
	}

	//------------------------------------------------------------------------------------------------------
	physx::PxRigidStatic* PhysicsManager::CreatePxPlaneRigidStatic(const Vector3& angle, float offset, PhysicsMaterial* material)
	{
		return PxCreatePlane(*px_physics_, PxPlane(angle.ToPxVec3(), offset), *material->GetPxMaterial());
	}

	//------------------------------------------------------------------------------------------------------
	void PhysicsManager::AddPxActor(PxActor* px_actor, Component* component)
	{
        ASSERT(component != nullptr);
		px_actor->userData = component;
		px_scene_->addActor(*px_actor);
	}

	//------------------------------------------------------------------------------------------------------
	void PhysicsManager::RemovePxActor(PxActor* px_actor)
	{
		px_scene_->removeActor(*px_actor);
	}

	//------------------------------------------------------------------------------------------------------
	Component* PhysicsManager::GetComponentFromPxActor(physx::PxActor* px_actor)
	{
		return static_cast<Component*>(px_actor->userData);
	}

    //------------------------------------------------------------------------------------------------------
    PhysicsManager::PhysicsManager(size_t memory_size)
    {
        physics_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(memory_size);
        px_allocator_ = physics_allocator_->NewAllocator<FreeListAllocator>(memory_size * 0.9f);
        allocator_callback_ = physics_allocator_->New<AllocatorCallback>(px_allocator_);
        Startup();
    }

    //------------------------------------------------------------------------------------------------------
    PhysicsManager::~PhysicsManager()
    {
        Shutdown();
        physics_allocator_->Delete(allocator_callback_);
        physics_allocator_->DeleteAllocator(px_allocator_);
        Get::MemoryManager()->DeleteAllocator(physics_allocator_);
    }

	//------------------------------------------------------------------------------------------------------
	void PhysicsManager::SetTolerances(PxReal length, PxReal speed, PxReal mass)
	{
		px_tolerances_scale_.length = length;
		px_tolerances_scale_.speed = speed;
		px_tolerances_scale_.mass = mass;
	}

	//------------------------------------------------------------------------------------------------------
	bool PhysicsManager::Startup()
	{
		px_foundation_ = PxCreateFoundation(PX_PHYSICS_VERSION, *allocator_callback_, error_callback_);
		if (!px_foundation_)
		{
			DLOG("PhysX startup error: PxCreateFoundation failed")
				return false;
		}

		px_profile_zone_manager_ = &PxProfileZoneManager::createProfileZoneManager(px_foundation_);
		if (!px_profile_zone_manager_)
		{
			DLOG("PhysX startup error: createProfileZoneManager failed");
			return false;
		}

		bool record_memory_allocations = true;
		px_physics_ = PxCreatePhysics(PX_PHYSICS_VERSION, *px_foundation_, px_tolerances_scale_, record_memory_allocations, px_profile_zone_manager_);
		if (!px_physics_)
		{
			DLOG("PhysX startup error: PxCreatePhysics failed");
			return false;
		}

		px_cooking_ = PxCreateCooking(PX_PHYSICS_VERSION, *px_foundation_, PxCookingParams(px_tolerances_scale_));
		if (!px_cooking_)
		{
			DLOG("PhysX startup error: PxCreateCooking failed");
			return false;
		}

		if (!PxInitExtensions(*px_physics_))
		{
			DLOG("PhysX startup error: PxInitExtensions failed");
			return false;
		}


		if (px_physics_->getPvdConnectionManager())
		{
			px_physics_->getVisualDebugger()->setVisualizeConstraints(true);
			px_physics_->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_CONTACTS, true);
			px_physics_->getVisualDebugger()->setVisualDebuggerFlag(PxVisualDebuggerFlag::eTRANSMIT_SCENEQUERIES, true);
			px_vd_connection_ = PxVisualDebuggerExt::createConnection(px_physics_->getPvdConnectionManager(), "127.0.0.1", 5425, 10);
		}

		px_cpu_dispatcher_ = PxDefaultCpuDispatcherCreate(4);
		PxSceneDesc px_scene_desc(px_tolerances_scale_);
		px_scene_desc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		px_scene_desc.cpuDispatcher = px_cpu_dispatcher_;
		px_scene_desc.filterShader = PhysicsFilterShader;
		px_scene_ = px_physics_->createScene(px_scene_desc);

		px_controller_manager_ = PxCreateControllerManager(*px_scene_);

		px_scene_->setSimulationEventCallback(&contact_callback_);

		CreateMaterial("default", 0.5f, 0.5f, 0.6f);

		return true;
	}

	//------------------------------------------------------------------------------------------------------
	void PhysicsManager::Shutdown()
	{
		//TODO: Release all materials and shapes

		px_controller_manager_->release();

		if (px_vd_connection_ != nullptr)
		{
			px_vd_connection_->release();
		}

		px_scene_->release();
		delete px_cpu_dispatcher_;
		PxCloseExtensions();
		px_cooking_->release();
		px_physics_->release();
		px_profile_zone_manager_->release();
		px_foundation_->release();
	}

	//------------------------------------------------------------------------------------------------------
	PxFilterFlags PhysicsManager::PhysicsFilterShader(PxFilterObjectAttributes attributes0, PxFilterData filterData0, PxFilterObjectAttributes attributes1, PxFilterData filterData1, PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		{
			// let triggers through
			if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
			{
				pairFlags = PxPairFlag::eTRIGGER_DEFAULT; //eNOTIFY_TOUCH_FOUND | eNOTIFY_TOUCH_LOST | eDETECT_DISCRETE_CONTACT
				return PxFilterFlag::eDEFAULT;
			}
			// generate contacts for all that were not filtered above
			pairFlags = PxPairFlag::eCONTACT_DEFAULT; //eSOLVE_CONTACT | eDETECT_DISCRETE_CONTACT,

			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND | PxPairFlag::eNOTIFY_CONTACT_POINTS; //trigger contact callback with the contacts information

			return PxFilterFlag::eDEFAULT;
		}
	}

    //------------------------------------------------------------------------------------------------------
    void PhysicsManager::ContactCallbackProcessing::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
    {
        for (physx::PxU32 i = 0; i < count; i++)
        {
            Component* trigger_component = static_cast<Component*>(pairs[i].triggerActor->userData);
            Component* other_component = static_cast<Component*>(pairs[i].otherActor->userData);
            if (pairs[i].otherActor->userData == 0)
            {
                DLOG("actor without a component entered a trigger collider");
            }

            if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                trigger_component->GetNode()->DoTriggerEnterCallbacks(*other_component);
            }
            else if (pairs[i].status == PxPairFlag::eNOTIFY_TOUCH_LOST)
            {
                trigger_component->GetNode()->DoTriggerExitCallbacks(*other_component);
            }
        }
    }

    //------------------------------------------------------------------------------------------------------
	void PhysicsManager::ContactCallbackProcessing::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		//std::cout << "OnContact" << std::endl;
		PX_UNUSED((pairHeader));

		Component* component0 = static_cast<Component*>(pairHeader.actors[0]->userData);
		Component* component1 = static_cast<Component*>(pairHeader.actors[1]->userData);

		Vector3 impulse = Vector3(0, 0, 0); //overall impulse

		std::vector<PxContactPairPoint> contactPoints;
		std::vector<ContactData> contacts_data;
		for (PxU32 i = 0; i<nbPairs; i++)
		{
			PxU32 contactCount = pairs[i].contactCount;
			if (contactCount)
			{
				contactPoints.resize(contactCount);
				contacts_data.resize(contactCount);
				pairs[i].extractContacts(&contactPoints[0], contactCount);

				for (PxU32 j = 0; j<contactCount; j++)
				{
					contacts_data[j].position = contactPoints[j].position;
					contacts_data[j].impulse = contactPoints[j].impulse;
					contacts_data[j].normal = contactPoints[j].normal;
					contacts_data[j].separation = contactPoints[j].separation;

					impulse += contacts_data[j].impulse;
				}
			}
		}

		CollisionData collision_data;
        collision_data.contacts = &contacts_data;

        if (component0 != nullptr)
        {
            collision_data.other_component = component1;
            collision_data.impulse = impulse;
            component0->GetNode()->DoOnCollisionCallbacks(collision_data);
        }

        if (component1 != nullptr)
        {
            collision_data.other_component = component0;
            for each(ContactData i in contacts_data)
            {
                i.impulse = -i.impulse;
            }
            impulse = impulse;
            component1->GetNode()->DoOnCollisionCallbacks(collision_data);
        }
	}

    PxQueryHitType::Enum PhysicsManager::QueryFilterCallback::preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
    {
        if (std::find(ignored_components->begin(), ignored_components->end(), static_cast<Component*>(actor->userData)) == ignored_components->end())
        {
            return PxQueryHitType::Enum::eBLOCK;
        }
        return PxQueryHitType::Enum::eNONE;
    }
}