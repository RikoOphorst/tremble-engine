#pragma once

#include "component.h"
#include "../utilities/utilities.h"
#include "../utilities/stopwatch.h"
#include "../memory/memory_includes.h"
#include "../networking/peer.h"
#include "../input/input_manager.h"
#include "../networking/player_connectivity_data.h"
#include <string>

namespace tremble
{
    class Peer;

	template <typename T, typename = std::enable_if_t<std::is_base_of<Component, T>::value>	>
	class VectorComponent;

	/**
	* @class BaseComponentVector
	* @brief Interface for ComponentVector, and provides ability to store pointers to ComponentVector 
	*/
	class BaseComponentVector
	{
	public:
		virtual void Start() = 0; //!< Calls Awake() on all components if they have a void Awake()
		virtual void Update() = 0; //!< Calls Update() on all components if they have a void Update()
		virtual void UpdateBeforePhysics() = 0; //!< Calls UpdateBeforePhysics() on all components if they have a void UpdateBeforePhysics()
		virtual void UpdateAfterPhysics() = 0; //!< Calls UpdateAfterPhysics() on all components if they have a void UpdateAfterPhysics()
        virtual void OnHostConnect(const HostData& player_data) = 0; //!< Calls UpdateAfterPhysics() on all components if they have a void OnPlayerConnect()
        virtual void OnPlayerConnect(const PlayerData& player_data) = 0; //!< Calls UpdateAfterPhysics() on all components if they have a void OnPlayerConnect()
        virtual void OnPlayerDisonnect(const PlayerData& player_data) = 0; //!< Calls OnPlayerConnect() on all components if they have a void OnPlayerConnect()
		virtual void DestroyComponent(Component* component) = 0;
        virtual void AssociateComponentWithPeer(Component* component, PeerID associated_peer) = 0;
        virtual void DeAssociateComponentFromPeer(Component* component) = 0;
        virtual ~BaseComponentVector() {};
	};

	/**
	* @class tremble::ComponentVector
	* @brief Templated class of vector of components. Provides funcions to call certain functions (Update, Awake...) on all the components in the vector
	*
	* Accessor functions are implemented in such a way that if component has, for example, a function void Update() implemented, vector calls for Update() in all components
	* In other scenarios, Update() is just an empty function
	* @author Anton Gavrilov
	*/
	template <
		typename T,
		typename = std::enable_if_t<std::is_base_of<Component, T>::value>
	>
	class ComponentVector : BaseComponentVector
	{
		HAS_MEM_FUNC(Start, has_start);
        HAS_MEM_FUNC(OnPlayerConnect, has_on_player_connect);
        HAS_MEM_FUNC(OnHostConnect, has_on_host_connect);
        HAS_MEM_FUNC(OnPlayerDisconnect, has_on_player_disconnect);
		HAS_MEM_FUNC(Update, has_update);
		HAS_MEM_FUNC(UpdateBeforePhysics, has_before_physics);
		HAS_MEM_FUNC(UpdateAfterPhysics, has_after_physics);
        HAS_MEM_FUNC(Shutdown, has_shutdown);
	public:

        //------------------------------------------------------------------------------------------------------
        ComponentVector(FreeListAllocator* allocateSpaceIn, int numberOfObjects)
            :manager_allocator_(allocateSpaceIn)
        {
            components_pool_ = allocateSpaceIn->NewAllocator<PoolAllocator>(numberOfObjects * sizeof(T) + alignof(T), sizeof(T), alignof(T));
        }

        //------------------------------------------------------------------------------------------------------
        virtual ~ComponentVector() override
        {
            manager_allocator_->DeleteAllocator(components_pool_);
        }

        //------------------------------------------------------------------------------------------------------
		/**
		* @brief Get a vector of components stored inside of this component vector
		*/
		const std::vector<T*>& GetComponents()
		{
			return all_components_;
		}

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Get a vector of components, owned by this client stored inside of this component vector
        */
        const std::vector<T*>& GetOwnComponents()
        {
            return own_components_;
        }

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Get a vector of components, not owned by this client stored inside of this component vector
        */
        const std::vector<T*>& GetSharedComponents()
        {
            return shared_components_;
        }

        //------------------------------------------------------------------------------------------------------
		/**
		* @brief Add a component to the components vector. Creates and returns a new component
		*/
        T* AddComponent()
        {
            T* component = components_pool_->New<T>();
            ASSERT(component != NULL);
            all_components_.push_back(component);
            own_components_.push_back(component);
            return component;
        }

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Add a component to the components vector. Creates and returns a new component, associated with peer
        */
        T* AddComponent(PeerID associated_peer)
        {
            T* component = components_pool_->New<T>();
            all_components_.push_back(component);
            shared_components_.push_back(std::pair<T*, PeerID>(component, associated_peer));
            return component;
        }

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Associate component with a peer. That means that peer's input will be used for component's update steps
        * @param component Component, that you wish to associate
        * @param associated_peer peer, that you want to associate it with
        */
        void AssociateComponentWithPeer(Component* component, PeerID associated_peer) override
        {
            //look for in the own_components
            for (size_t i = own_components_.size(); i-- > 0; )
            {
                if (own_components_[i] == component)
                {
                    own_components_.erase(own_components_.begin() + i);
                    shared_components_.push_back(std::pair<T*, PeerID>((T*)component, associated_peer));
                    return;
                }
            }
            //look for in the shared_components
            for (size_t i = shared_components_.size(); i-- > 0; )
            {
                if (shared_components_[i].first == component)
                {
                    shared_components_[i].second = associated_peer;
                    return;
                }
            }
            ASSERT(false && "trying to associate component, that doesn't exist, with a peer");
        }

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Deassociate component from peer
        * @param component Component that you wish to deassociate
        */
        void DeAssociateComponentFromPeer(Component* component) override
        {
            for (size_t i = shared_components_.size(); i-- > 0; )
            {
                if (shared_components_[i].first == component)
                {
                    own_components_.push_back(shared_components_[i].first);
                    shared_components_.erase(shared_components_.begin() + i);
                    return;
                }
            }
        }

        //------------------------------------------------------------------------------------------------------
		/**
		* @brief Remove component from the components vector and call the destructor
        * should be called only from ClearDeletionQueue()
		* @param component Component that you wish to remove from the vector
		*/
		void DestroyComponent(Component* component) override
		{
            RemoveComponentFromVectors(component);
		}

        //------------------------------------------------------------------------------------------------------
        void RemoveComponentFromVectors(Component* component)
        {
            component->Shutdown();
            T* find = static_cast<T*>(component);
            //if (find->GetType() == typeid(Renderable))
            //{
            //    find->Shutdown();
            //}
            std::vector<T*>::iterator found_comp = std::find(own_components_.begin(), own_components_.end(), find);
            if (found_comp != own_components_.end())
            {
                components_pool_->Delete(find);
                own_components_.erase(found_comp);
            }
            else
            {
                size_t i = shared_components_.size();
                while (i-- > 0)
                {
                    if (shared_components_[i].first == component)
                    {
                        components_pool_->Delete(find);
                        shared_components_.erase(shared_components_.begin() + i);
                        goto destroy_in_all;
                    }
                }
                ASSERT(false && "You are trying to delete a component, which there is 0 instances of");
            }
        destroy_in_all:
            found_comp = std::find(all_components_.begin(), all_components_.end(), find);
            ASSERT(found_comp != all_components_.end());
            all_components_.erase(found_comp);
        }

        //------------------------------------------------------------------------------------------------------
		/**
		* @brief Templated function, that exists only if T has void Update() implemented
		*
		* has_update<class, sign> has a static variable value = 1 if T has an Update() implemented.
		* enable_if enables this function only if has_update<>::value = 1
		*/
		template<typename T>
		std::enable_if_t<has_update<T, void(T::*)()>::value, void>
			Update()
		{
			for each (T* i in own_components_)
			{
				i->Update();
			}
            for each (std::pair<T*, PeerID> i in shared_components_)
            {
                Get::InputManager()->UseInputOfPeer(i.second);
                i.first->Update();
            }
            Get::InputManager()->UseRealInput();
		}

		/**
		* @brief empty function template, that exists only if T does not have void Update() implemented
		*
		* has_update<class, sign> has a static variable value = 0 if T does not have an Update() implemented.
		* enable_if enables this function only if has_update<>::value = 1
		*/
		template<typename T>
		std::enable_if_t<!has_update<T, void(T::*)()>::value, void>
			Update()
		{

		}

		/**
		* @brief calls Update on components if they have awake implemented
		*
		* Just calls a generated template function Update with a template argument T so that you can call Update() on a component vector without passing template arguments
		*/
		void Update()
		{
			Update<T>();
		}

        //------------------------------------------------------------------------------------------------------
		/**
		* @brief Templated Update function, that exists only if T has void UpdateBeforePhysics() implemented
		*/
		template<typename T>
		std::enable_if_t<has_before_physics<T, void(T::*)()>::value, void>
			UpdateBeforePhysics()
		{
			for each (T* i in own_components_)
			{
				i->UpdateBeforePhysics();
			}
            for each (std::pair<T*, PeerID> i in shared_components_)
            {
                Get::InputManager()->UseInputOfPeer(i.second);
                i.first->UpdateBeforePhysics();
            }
            Get::InputManager()->UseRealInput();
		}

		/**
		* @brief empty function template, that exists only if T does not have void UpdateBeforePhysics() implemented
		*/
		template<typename T>
		std::enable_if_t<!has_before_physics<T, void(T::*)()>::value, void>
			UpdateBeforePhysics()
		{

		}

		/**
		* @brief Calls UpdateBeforePhysics on all components if they have void UpdateBeforePhysics() implemented
		*/
		void UpdateBeforePhysics()
		{
			UpdateBeforePhysics<T>();
		}


        //------------------------------------------------------------------------------------------------------
		/**
		* @brief Templated Update function, that exists only if T has void UpdateBeforePhysics() implemented
		*/
		template<typename T>
		std::enable_if_t<has_after_physics<T, void(T::*)()>::value, void>
			UpdateAfterPhysics()
		{
			for each (T* i in own_components_)
			{
				i->UpdateAfterPhysics();
			}
            for each (std::pair<T*, PeerID> i in shared_components_)
            {
                Get::InputManager()->UseInputOfPeer(i.second);
                i.first->UpdateAfterPhysics();
            }
            Get::InputManager()->UseRealInput();
		}

		/**
		* @brief empty function template, that exists only if T does not have void UpdateBeforePhysics() implemented
		*/
		template<typename T>
		std::enable_if_t<!has_after_physics<T, void(T::*)()>::value, void>
			UpdateAfterPhysics()
		{

		}

		/**
		* @brief Calls UpdateBeforePhysics on all components if they have void UpdateBeforePhysics() implemented
		*/
		void UpdateAfterPhysics()
		{
			UpdateAfterPhysics<T>();
		}

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Add the components to the start queue
        */
        template<typename T>
        std::enable_if_t<has_start<T, void(T::*)()>::value, void>
            AddToStartQueue(T* component)
        {
            unstarted_components_.push(component);
        }

        //------------------------------------------------------------------------------------------------------
		/**
		* @brief Templated Update function, that exists only if T has void Start() implemented
		*/
		template<typename T>
		std::enable_if_t<has_start<T, void(T::*)()>::value, void>
			Start()
		{
            for (size_t i = unstarted_components_.size(); i > 0; i--)
            {
                unstarted_components_.front()->Start();
                unstarted_components_.pop();
            }
            ASSERT(unstarted_components_.size() == 0);
		}

		/**
		* @brief empty function template, that exists only if T does not have void Start() implemented
		*/
		template<typename T>
		std::enable_if_t<!has_start<T, void(T::*)()>::value, void>
			Start()
		{

		}

		/**
		* @brief Calls Start on all components if they have void Start() implemented
		*/
		void Start()
		{
			Start<T>();
		}

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Templated Update function, that exists only if T has void OnPlayerConnect() implemented
        */
        template<typename T>
        std::enable_if_t<has_on_player_connect<T, void(T::*)(const PlayerData&)>::value, void>
            OnPlayerConnect(const PlayerData& player_data)
        {
            for each (T* i in all_components_)
            {
                i->OnPlayerConnect(player_data);
            }
        }

        /**
        * @brief empty function template, that exists only if T does not have void OnPlayerConnect() implemented
        */
        template<typename T>
        std::enable_if_t<!has_on_player_connect<T, void(T::*)(const PlayerData&)>::value, void>
            OnPlayerConnect(const PlayerData& player_data)
        {}

        /**
        * @brief Calls OnPlayerConnect on all components if they have void OnPlayerConnect() implemented
        */
        void OnPlayerConnect(const PlayerData& player_data) override
        {
            OnPlayerConnect<T>(player_data);
        }

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Templated OnPlayerDisonnect function, that exists only if T has void OnPlayerDisonnect() implemented
        */
        template<typename T>
        std::enable_if_t<has_on_player_disconnect<T, void(T::*)(const PlayerData&)>::value, void>
            OnPlayerDisonnect(const PlayerData& player_data)
        {
            for each (T* i in all_components_)
            {
                i->OnPlayerDisonnect(player_data);
            }
        }

        /**
        * @brief empty function template, that exists only if T does not have void OnPlayerDisonnect() implemented
        */
        template<typename T>
        std::enable_if_t<!has_on_player_disconnect<T, void(T::*)(const PlayerData&)>::value, void>
            OnPlayerDisonnect(const PlayerData& player_data)
        {}

        /**
        * @brief Calls OnPlayerDisonnect on all components if they have void OnPlayerDisonnect() implemented
        */
        void OnPlayerDisonnect(const PlayerData& player_data) override
        {
            OnPlayerDisonnect<T>(player_data);
        }

        //------------------------------------------------------------------------------------------------------
        /**
        * @brief Templated Update function, that exists only if T has void OnPlayerConnect() implemented
        */
        template<typename T>
        std::enable_if_t<has_on_host_connect<T, void(T::*)(const HostData&)>::value, void>
            OnHostConnect(const HostData& player_data)
        {
            for each (T* i in all_components_)
            {
                i->OnHostConnect(player_data);
            }
        }

        /**
        * @brief empty function template, that exists only if T does not have void OnPlayerConnect() implemented
        */
        template<typename T>
        std::enable_if_t<!has_on_host_connect<T, void(T::*)(const HostData&)>::value, void>
            OnHostConnect(const HostData& player_data)
        {}

        /**
        * @brief Calls OnPlayerConnect on all components if they have void OnPlayerConnect() implemented
        */
        void OnHostConnect(const HostData& player_data) override
        {
            OnHostConnect<T>(player_data);
        }
	private:
        std::vector<T*> all_components_; //!< shared and owned components
        std::vector<std::pair<T*, PeerID>> shared_components_;  //!< Pointers to all shared components, that are stored in this ComponentVector
		std::vector<T*> own_components_; //!< Pointers of all own components, that are stored in this ComponentVector
        std::queue<T*> unstarted_components_; //!< Components, that are still to be started
        PoolAllocator* components_pool_; //!< Allocator, that stores components
        FreeListAllocator* manager_allocator_; //!< Component manager's allocator (kept here for the destructor)
	};
}