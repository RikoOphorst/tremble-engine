#pragma once

#include "../math/math.h"
#include "../networking/peer.h"
#include "../../core/get.h"
#include "../../core/physics/physics_collision_data.h"
#include "../utilities/utilities.h"

namespace tremble
{
	class SGNode;

	/**
	* @class tremble::Component
	* @brief Class, that all components have to inherit. 
	*/
	class Component
	{
		friend class SGNode;
	public:
		/**
		* @brief Component default constructor
		*/
		Component();
		/**
		* @brief Component intialization function
		* @param node Node, that this component is attached to
		* @param type_info Type of this component
		*/
		void Init(SGNode* node, std::type_index component_type);
		virtual void Shutdown() {} //!< Component shutdown function. Automatically gets called before the destruction

		std::type_index GetType() const { return type_; } //!< Get type info if this component
		SGNode* GetNode() const { return node_; } //!< Get a scene graph node of this component

        void AssociateWithPeer(PeerID peer_id);
        void DeAssociateFromPeer();

		template <class T>
		void SetCollisionCallback(void (T::*collision_callback)(const CollisionData&), T* this_pointer) //!< Set a collision callback function of this component
		{
			collision_callback_ = std::bind(collision_callback, this_pointer, std::placeholders::_1);
		}

        template <class T>
        void SetOnTriggerEnter(void (T::*collision_callback)(const Component&), T* this_pointer) //!< Set a collision callback function of this component
        {
            trigger_enter_callback_ = std::bind(collision_callback, this_pointer, std::placeholders::_1);
        }

        template <class T>
        void SetOnTriggerExit(void (T::*collision_callback)(const Component&), T* this_pointer) //!< Set a collision callback function of this component
        {
            trigger_exit_callback_ = std::bind(collision_callback, this_pointer, std::placeholders::_1);
        }
	private:
		std::type_index type_; //!< type_info of the component. Used to store pointers to the base component class, and cast to a derived component class
		SGNode* node_; //!< Scene graph node of this component.
		std::function<void(const CollisionData&)> collision_callback_; //!< Collision callback function
        std::function<void(const Component&)> trigger_enter_callback_; //!< Collision callback function
        std::function<void(const Component&)> trigger_exit_callback_; //!< Collision callback function
	};
}