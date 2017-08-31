#pragma once
#include <set>

namespace tremble
{
	class FreeListAllocator;
	class Vector3;
	class Component;
	class PhysicsMaterial;
	class PhysicsGeometry;

	class PhysicsManager
	{
	public:
		friend class Allocator;
		friend class GameManager;
		friend class Rigidbody;
		friend class RigidbodyDynamic;
		friend class RigidbodyStatic;
        friend class TriggerCollider;
		friend class PhysicsStaticPlane;
		friend class CharacterController;
        friend class PhysicsGeometryHolder; //to use the allocator

        PhysicsManager(size_t memory_size);
        ~PhysicsManager();

		void Update(); //!< Physics manager update function. Later to be changed to be called at fixed update
		Vector3 GetGravity(); //!< Get current gravity of the world
		bool Raycast(physx::PxRaycastBuffer* buffer, Vector3 origin, Vector3 direction, float max_distance, std::vector<Component*>* ignored_components = nullptr); //!< Cast a ray in the physics scene
		Component* GetComponentFromRaycast(physx::PxRaycastBuffer* buffer); //!< Get a component, that hit a ray, from the raycast buffer

		PhysicsMaterial* CreateMaterial(const std::string& name,float restitution = 0.6f, float dynamic_friction = 0.5f, float static_friction = 0.5f);
		PhysicsMaterial* GetMaterial(std::string name); //!< Get material by name 
		physx::PxShape* CreatePxShape(PhysicsGeometry* geometry, PhysicsMaterial* material, bool is_exclusive = true);
		physx::PxRigidStatic* CreatePxPlaneRigidStatic(const Vector3& angle, float offset, PhysicsMaterial* material);
        physx::PxCooking* GetCooking() const { return px_cooking_; }
        physx::PxPhysics* GetPxPhysics() const { return px_physics_; }

		void AddPxActor(physx::PxActor* px_actor, Component* component);
		void RemovePxActor(physx::PxActor*);
		Component* GetComponentFromPxActor(physx::PxActor* px_actor);

		/**
		* @brief Allocation callback class, used by physx
		*/
		class AllocatorCallback : public physx::PxAllocatorCallback
		{
		public:
			AllocatorCallback(FreeListAllocator* physx_allocator);
			virtual void* allocate(size_t size, const char* typeName, const char* filename, int line);
			virtual void deallocate(void* ptr);
		private:
			FreeListAllocator* physx_allocator_;
		};

		class ContactCallbackProcessing : public physx::PxSimulationEventCallback
		{
			void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count) { PX_UNUSED(constraints); PX_UNUSED(count); }
			void onWake(physx::PxActor** actors, physx::PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
			void onSleep(physx::PxActor** actors, physx::PxU32 count) { PX_UNUSED(actors); PX_UNUSED(count); }
            void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
			void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		};

        class QueryFilterCallback : public physx::PxQueryFilterCallback
        {
        public:

            /**
            * @brief This filter callback is executed before the exact intersection test if PxQueryFlag::ePREFILTER flag was set.
            * Currently just filters out ignored objects (provided with raycast function)
            * @param[in] filterData custom filter data specified as the query's filterData.data parameter.
            * @param[in] shape A shape that has not yet passed the exact intersection test.
            * @param[in] actor The shape's actor.
            * @param[in,out] queryFlags scene query flags from the query's function call (only flags from PxHitFlag::eMODIFIABLE_FLAGS bitmask can be modified)
            * @return the updated type for this hit  (see #PxQueryHitType)
            */
            virtual physx::PxQueryHitType::Enum preFilter(
                const physx::PxFilterData& filterData, const physx::PxShape* shape, const physx::PxRigidActor* actor, physx::PxHitFlags& queryFlags);

            /**
            * @brief This filter callback is executed if the exact intersection test returned true and PxQueryFlag::ePOSTFILTER flag was set.
            * 
            * @param[in] filterData custom filter data of the query
            * @param[in] hit Scene query hit information. faceIndex member is not valid for overlap queries. For sweep and raycast queries the hit information can be cast to #PxSweepHit and #PxRaycastHit respectively.
            * @return the updated hit type for this hit  (see #PxQueryHitType)
            */
            virtual physx::PxQueryHitType::Enum postFilter(const physx::PxFilterData& filterData, const physx::PxQueryHit& hit) { return physx::PxQueryHitType::Enum::eBLOCK; }

            std::vector<Component*>* ignored_components;
        };

		static physx::PxFilterFlags PhysicsFilterShader(
			physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
			physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
			physx::PxPairFlags& pairFlags, const void* constantBlock, physx::PxU32 constantBlockSize);

		/**
		* @brief Error callback class, used by physx
		*/
		class ErrorCallback : public physx::PxErrorCallback
		{
		public:
			virtual void reportError(physx::PxErrorCode::Enum code, const char* message, const char* file, int line);
		};



	private:

		void SetTolerances(physx::PxReal length, physx::PxReal speed, physx::PxReal mass); //!< Set tolerances, used by physx. (Dependent on average measures in the world)
		bool Startup(); //!< Startup physics
		void Shutdown(); //!< Shutdown physics

		ContactCallbackProcessing contact_callback_;
		AllocatorCallback* allocator_callback_;
        QueryFilterCallback query_filter_callback_;
		ErrorCallback error_callback_;

		physx::PxFoundation* px_foundation_;
		physx::PxTolerancesScale px_tolerances_scale_;
		physx::PxProfileZoneManager* px_profile_zone_manager_;
		physx::PxPhysics* px_physics_;
		physx::PxCooking* px_cooking_;
		physx::PxDefaultCpuDispatcher* px_cpu_dispatcher_;
		physx::PxScene* px_scene_;
		physx::PxVisualDebuggerConnection* px_vd_connection_;
		physx::PxControllerManager* px_controller_manager_;

		std::unordered_map<std::string, PhysicsMaterial> physics_materials_;
        FreeListAllocator* physics_allocator_; //!< Personal allocator for not PhysX object, but physics objects
        FreeListAllocator* px_allocator_; //!< Allocator, used by PhysX
	};



}