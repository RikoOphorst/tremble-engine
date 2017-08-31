#include "get.h"
#include "game_manager.h"

#include "rendering/renderer.h"
#include "utilities/octree.h"
#include "networking/packet_factory.h"

namespace tremble
{
	GameManager* Get::game_manager_ = nullptr;

	//------------------------------------------------------------------------------------------------------
	bool Get::Create(tremble::GameManager* game_manager)
	{
		if (game_manager_ == nullptr)
		{
			game_manager_ = game_manager;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	GameManager* Get::GameManager()
	{
		return game_manager_;
	}

	//------------------------------------------------------------------------------------------------------
	MemoryManager* Get::MemoryManager()
	{
		return game_manager_->GetMemoryManager();
	}

	//------------------------------------------------------------------------------------------------------
	ResourceManager* Get::ResourceManager()
	{
		return game_manager_->GetResourceManager();
	}

	//------------------------------------------------------------------------------------------------------
	InputManager* Get::InputManager()
	{
		return game_manager_->GetInputManager();
	}

	//------------------------------------------------------------------------------------------------------
	Renderer* Get::Renderer()
	{
		return game_manager_->GetRenderer();
	}

	//------------------------------------------------------------------------------------------------------
	CommandManager* Get::CommandManager()
	{
		return game_manager_->GetCommandManager();
	}

	//------------------------------------------------------------------------------------------------------
	CommandContextManager* Get::CommandContextManager()
	{
		return game_manager_->GetCommandContextManager();
	}

	//------------------------------------------------------------------------------------------------------
	SGNode* Get::Scene()
	{
		return game_manager_->GetScene();
	}

	//------------------------------------------------------------------------------------------------------
	PhysicsManager* Get::PhysicsManager()
	{
		return game_manager_->GetPhysicsManager();
	}

	//------------------------------------------------------------------------------------------------------
	ComponentManager* Get::ComponentManager()
	{
		return game_manager_->GetComponentManager();
	}

	//------------------------------------------------------------------------------------------------------
	Window* Get::Window()
	{
		return game_manager_->GetWindow();
	}

	//------------------------------------------------------------------------------------------------------
	float Get::DeltaT()
	{
		return static_cast<float>(game_manager_->GetDeltaT());
	}

	//------------------------------------------------------------------------------------------------------
	float Get::TimeSinceStartup()
	{
		return static_cast<float>(game_manager_->GetTimeSinceStartup());
	}

	//------------------------------------------------------------------------------------------------------
	const Config& Get::Config()
	{
		return game_manager_->GetConfigManager()->GetConfig();
	}

	//------------------------------------------------------------------------------------------------------
	ConfigManager* Get::ConfigManager()
	{
		return game_manager_->GetConfigManager();
	}

    //------------------------------------------------------------------------------------------------------
	PacketFactory * Get::PacketFactory()
	{
		return game_manager_->GetPacketFactory();
	}

	//------------------------------------------------------------------------------------------------------
	NetworkManager* Get::NetworkManager()
	{
		return game_manager_->GetNetworkManager();
	}
	
	//------------------------------------------------------------------------------------------------------
	SwapChain& Get::SwapChain()
	{
		return Renderer()->GetSwapChain();
	}

	//------------------------------------------------------------------------------------------------------
	Device& Get::Device()
	{
		return Renderer()->GetDevice();
	}

	//------------------------------------------------------------------------------------------------------
	DescriptorHeap& Get::CbvSrvUavHeap()
	{
		return Renderer()->GetCbvSrvUavHeap();
	}

	//------------------------------------------------------------------------------------------------------
	DescriptorHeap& Get::RtvHeap()
	{
		return Renderer()->GetRtvHeap();
	}

	//------------------------------------------------------------------------------------------------------
	DescriptorHeap& Get::DsvHeap()
	{
		return Renderer()->GetDsvHeap();
	}

	//------------------------------------------------------------------------------------------------------
	DescriptorHeap& Get::SamplerHeap()
	{
		return Renderer()->GetSamplerHeap();
	}
	//------------------------------------------------------------------------------------------------------
	BufferManager& Get::BufferManager()
	{
		return Renderer()->GetBufferManager();
	}

	//------------------------------------------------------------------------------------------------------
	AudioManager* Get::AudioManager()
	{
		return game_manager_->GetAudioManager();
	}

	//------------------------------------------------------------------------------------------------------
	SceneLoader* Get::SceneLoader()
    {
        return game_manager_->GetSceneLoader();
    }
	
	//------------------------------------------------------------------------------------------------------
	Octree* Get::Octree()
	{
		return game_manager_->GetOctree();
	}
}
