#include "serializable.h"
#include "../get.h"
#include "network_manager.h"
#include "serialization_manager.h"

using namespace tremble;

Serializable::Serializable()
{
	SetNetworkIDManager(&Get::NetworkManager()->GetNetworkIdManager());
	Get::NetworkManager()->GetSerializationManager().Register(*this);
}

Serializable::~Serializable()
{
	Get::NetworkManager()->GetSerializationManager().Unregister(*this);
}