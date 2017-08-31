#include "component_manager.h"
#include "scene_graph.h"

namespace tremble
{
    ComponentManager::ComponentManager(size_t size)
    {
        component_vectors_allocator_ = Get::MemoryManager()->GetNewAllocator<FreeListAllocator>(size);
    }
    ComponentManager::~ComponentManager()
    {
        for (std::map<std::type_index, BaseComponentVector*>::iterator it = vectors_.begin(); it != vectors_.end();)
        {
            component_vectors_allocator_->Delete(it->second);
            it = vectors_.erase(it);
        }
        Get::MemoryManager()->DeleteAllocator(component_vectors_allocator_);
    }

    void ComponentManager::DeleteComponent(Component* component)
    {
        deletion_queue_.insert(component);
    }

    void ComponentManager::Start()
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter;
        for (iter = vectors_.begin(); iter != vectors_.end(); iter++)
        {
            iter->second->Start();
        }
    }
    void ComponentManager::Update()
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter;
        for (iter = vectors_.begin(); iter != vectors_.end(); iter++)
        {
            iter->second->Update();
        }
    }
    void ComponentManager::UpdateBeforePhysics()
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter;
        for (iter = vectors_.begin(); iter != vectors_.end(); iter++)
        {
            iter->second->UpdateBeforePhysics();
        }
    }
    void ComponentManager::UpdateAfterPhysics()
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter;
        for (iter = vectors_.begin(); iter != vectors_.end(); iter++)
        {
            iter->second->UpdateAfterPhysics();
        }
    }

    void ComponentManager::OnPlayerConnect(const PlayerData& player_data)
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter;
        for (iter = vectors_.begin(); iter != vectors_.end(); iter++)
        {
            iter->second->OnPlayerConnect(player_data);
        }
    }

    void ComponentManager::OnPlayerDisconnect(const PlayerData& player_data)
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter;
        for (iter = vectors_.begin(); iter != vectors_.end(); iter++)
        {
            iter->second->OnPlayerDisonnect(player_data);
        }
    }

    void ComponentManager::OnHostConnect(const HostData& host_data)
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter;
        for (iter = vectors_.begin(); iter != vectors_.end(); iter++)
        {
            iter->second->OnHostConnect(host_data);
        }
    }

    void ComponentManager::DestroyComponent_(Component* component)
    {
        std::map<std::type_index, BaseComponentVector*>::iterator iter = vectors_.find(component->GetType());
        ASSERT(iter != vectors_.end() && "You are trying to delete a component, which there is 0 instances of");
        iter->second->DestroyComponent(component);
    }

    void ComponentManager::ClearDeletionQueue_()
    {
        std::set<Component*>::iterator iter = deletion_queue_.begin();
        while (deletion_queue_.size() > 0)
        {
            // ASSERT(iter->children_.size() == 0);
            // ASSERT(iter->components_.size() == 0);
            DestroyComponent_(*iter);
            (*iter)->GetNode()->DecoupleComponent_(*iter);
            iter = deletion_queue_.erase(iter);
        }
    }
}