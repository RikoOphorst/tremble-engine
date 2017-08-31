#pragma once

#define MAX_COMPONENTS 4096
#include "component_vector.h"
#include "../memory/memory_includes.h"
#include <set>
#include "../networking/player_connectivity_data.h"

namespace tremble
{
	class PhysicsRigidbodyComponent;

    HAS_MEM_FUNC(Start, has_start);

	/**
	* @class tremble::ComponentManager
	* @brief Component manager class, that holds component vectors, and runs update functions through all of them
	* 
	* @author Anton Gavrilov
	*/
	class ComponentManager
	{
	public:
        friend class GameManager;
        friend class Scene;

        ComponentManager(size_t size);
        ~ComponentManager();

		template<typename T>
		const std::vector<T*>& GetComponents()
		{
			std::map<std::type_index, BaseComponentVector*>::iterator iter = vectors_.find(typeid(T));
			if (iter != vectors_.end())
			{
				return ((ComponentVector<T>*)iter->second)->GetComponents();
			}
			else
			{
				ComponentVector<T>* new_vec = component_vectors_allocator_->New<ComponentVector<T>>(component_vectors_allocator_, MAX_COMPONENTS);
				vectors_[typeid(T)] = (BaseComponentVector*)new_vec;
				return new_vec->GetComponents();
			}
		}

		/**
		* @brief Adds a pointer to a component to an appropriate component vector. If a vector doesn't exist - creates one
		* @param component Component that you wish to add to the updated components list
		*/
		template <class T>
		T* AddComponent()
		{
			std::map<std::type_index, BaseComponentVector*>::iterator iter = vectors_.find(typeid(T));
			if (iter != vectors_.end())
			{
				return ((ComponentVector<T>*)(iter->second))->AddComponent();
			}
			else
			{
				ComponentVector<T>* new_vec = component_vectors_allocator_->New<ComponentVector<T>>(component_vectors_allocator_, MAX_COMPONENTS);
				vectors_[typeid(T)] = (BaseComponentVector*)new_vec;
				return new_vec->AddComponent();
			}
		}

        /**
        * @brief Adds a pointer to a component to an appropriate component vector, associated with Peer. If a vector doesn't exist - creates one
        * @param component Component that you wish to add to the updated components list
        */
        template <class T>
        T* AddComponent(PeerID peer_id)
        {
            std::map<std::type_index, BaseComponentVector*>::iterator iter = vectors_.find(typeid(T));
            if (iter != vectors_.end())
            {
                return ((ComponentVector<T>*)(iter->second))->AddComponent(peer_id);
            }
            else
            {
                ComponentVector<T>* new_vec = component_vectors_allocator_->New<ComponentVector<T>>(component_vectors_allocator_, MAX_COMPONENTS);
                vectors_[typeid(T)] = (BaseComponentVector*)new_vec;
                return new_vec->AddComponent(peer_id);
            }
        }

        /**
        * @brief Deletes a component from the component manager at the end of each game loop
        */
        void DeleteComponent(Component* component);

        /**
        * @brief Associate a component with a peer
        * @param component Component, that you wish to associate
        * @param peer_id Peer, that you wish to associate the component with
        */
        void AssociateComponentWithPeer(Component* component, PeerID peer_id)
        {
            std::map<std::type_index, BaseComponentVector*>::iterator iter = vectors_.find(component->GetType());
            ASSERT(iter != vectors_.end());
            return ((iter->second))->AssociateComponentWithPeer(component, peer_id);
        }

        /**
        * @brief Deassociate a component with a peer
        * @param component Component, that you wish to deassociate
        */
        void DeAssociateComponentFromPeer(Component* component)
        {
            std::map<std::type_index, BaseComponentVector*>::iterator iter = vectors_.find(component->GetType());
            ASSERT(iter != vectors_.end());
            return ((iter->second))->DeAssociateComponentFromPeer(component);
        }

		/**
		* @brief Calls void Start() on all the component, scheduled for start (start queue)
		*/
        void Start();

		/**
		* @brief Calls void Update() on all the component vectors
		*/
        void Update();

		/**
		* @brief Calls void UpdateBeforePhysics() on all the component vectors
		*/
        void UpdateBeforePhysics();

		/**
		* @brief Calls void UpdateAfterPhysics() on all the component vectors
		*/
        void UpdateAfterPhysics();

        /**
        * @brief Calls OnPlayerConnect() on all the component vectors
        */
        void OnPlayerConnect(const PlayerData& player_data);

        /**
        * @brief Calls OnPlayerDisconnect() on all the component vectors
        */
        void OnPlayerDisconnect(const PlayerData& player_data);

        /**
        * @brief Calls OnPlayerConnect() on all the component vectors
        */
        void OnHostConnect(const HostData& host_data);

        template<typename T>
        std::enable_if_t<has_start<T, void(T::*)()>::value, void>
            AddToStartQueue(T* component_to_start) //!< Adds the component to the start queue of the component_manager
        {
            std::map<std::type_index, BaseComponentVector*>::iterator iter = vectors_.find(typeid(T));
            if (iter != vectors_.end())
            {
                ((ComponentVector<T>*)(iter->second))->AddToStartQueue(component_to_start);
                return;
            }
            ASSERT(false); // This function should only be called when a component of such type is already added
        }

        template<typename T>
        std::enable_if_t<!has_start<T, void(T::*)()>::value, void>
            AddToStartQueue(T* component_to_start) //!< Does nothing if a component doesn't have start
        {

        }

	private:
        /**
        * @brief Removes a component pointer from an appropriate vector
        * @param component Component that you want to remove
        * note: not to be used as public because it should be called when the deletion queue is being cleared
        */
        void DestroyComponent_(Component* component);
        void ClearDeletionQueue_(); //!< should be called at the end of each loop

		std::map<std::type_index, BaseComponentVector*> vectors_; //!< Map of all the component vectors, contained in this component manager, associated with their type_index
        FreeListAllocator* component_vectors_allocator_;
        std::set<Component*> deletion_queue_; //!< A queue of components marked for deletion to be deleted at the end of each loop at ClearDeletionQueue_()
	};
}