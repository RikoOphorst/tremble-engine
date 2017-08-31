#include "component.h"

#include "scene_graph.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	Component::Component() :
		type_(typeid(nullptr))
	{

	}

	//------------------------------------------------------------------------------------------------------
	void Component::Init(SGNode* node, std::type_index component_type)
	{
		node_ = node;
		type_ = component_type;
	}

    //------------------------------------------------------------------------------------------------------
    void Component::AssociateWithPeer(PeerID peer_id)
    {
        Get::ComponentManager()->AssociateComponentWithPeer(this, peer_id);
    }

    //------------------------------------------------------------------------------------------------------
    void Component::DeAssociateFromPeer()
    {
        Get::ComponentManager()->DeAssociateComponentFromPeer(this);
    }
}