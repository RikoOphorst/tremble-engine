#include "game_manager.h"

#include "win32/window.h"
#include "input/input_manager.h"
#include "rendering/renderer.h"
#include "rendering/command_manager.h"
#include "rendering/command_context_manager.h"
#include "utilities/timer.h"
#include "memory/memory_manager.h"
#include "scene_graph/component_manager.h"
#include "scene_graph/scene_graph.h"
#include "resources/resource_manager.h"
#include "resources/fbx_loader.h"
#include "physics/physics_manager.h"
#include "audio/audio_manager.h"
#include "networking/network_manager.h"
#include "networking/serialization_manager.h"
#include "networking/packet_factory.h"
#include "utilities/octree.h"
#include "get.h"

#include "utilities\stopwatch.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	GameManager* GameManager::Create(size_t memory_size)
	{
        const float linear_frame_memory_proportion = 1.0f / 20.0f;
		MemoryManager* memory_manager = new MemoryManager(memory_size, memory_size * linear_frame_memory_proportion);
		FreeListAllocator* own_allocator = memory_manager->GetNewAllocator<FreeListAllocator>(100000000);
		GameManager* game_manager = own_allocator->New<GameManager>(memory_manager, own_allocator);
		return game_manager;
	}

	//------------------------------------------------------------------------------------------------------
	void GameManager::Delete(GameManager* game_manager)
	{
		FreeListAllocator* own_allocator = game_manager->own_allocator_;
		MemoryManager* memory_manager = game_manager->memory_manager_;
		own_allocator->Delete(game_manager);
		memory_manager->DeleteAllocator(own_allocator);
		delete memory_manager;
	}

	//------------------------------------------------------------------------------------------------------
	GameManager::GameManager(MemoryManager* memory_manager, FreeListAllocator* own_allocator) :
		running_(true), 
		memory_manager_(memory_manager), 
		own_allocator_(own_allocator),
		subsystem_allocator_(nullptr)
	{
		Get::Create(this);
	}

	//------------------------------------------------------------------------------------------------------
	GameManager::~GameManager()
	{

	}

	//------------------------------------------------------------------------------------------------------
	void GameManager::Startup(const std::string& name, int width, int height)
	{
		subsystem_allocator_		= memory_manager_->GetNewAllocator<StackAllocator>(40000);

		packet_factory_				= subsystem_allocator_->New<PacketFactory>(1000);
		network_manager_			= subsystem_allocator_->New<NetworkManager>(1000000);
		config_manager_				= subsystem_allocator_->New<ConfigManager>();
		window_						= subsystem_allocator_->New<Window>(name, config_manager_->GetWindowResolutions()[config_manager_->GetConfig().window_resolution].width, config_manager_->GetWindowResolutions()[config_manager_->GetConfig().window_resolution].height, this);
		input_manager_				= subsystem_allocator_->New<InputManager>(window_, this);
		audio_manager_				= subsystem_allocator_->New<AudioManager>();
		resource_manager_			= subsystem_allocator_->New<ResourceManager>();
		
		renderer_					= subsystem_allocator_->New<Renderer>();
		command_manager_			= subsystem_allocator_->New<CommandManager>();
		command_context_manager_	= subsystem_allocator_->New<CommandContextManager>();
		renderer_->Startup();

		timer_						= subsystem_allocator_->New<Timer>();
		component_manager_			= subsystem_allocator_->New<ComponentManager>(100000000);
        octree_                     = subsystem_allocator_->New<Octree>(DirectX::BoundingBox(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(17500.0f, 17500.0f, 17500.0f)));
		scene_						= subsystem_allocator_->New<Scene>(1000000);
		physics_manager_			= subsystem_allocator_->New<PhysicsManager>(10000000); // @TODO tell mr gavrilov to make the PhysicsManager subsystem allocation compliant
        scene_loader_               = subsystem_allocator_->New<SceneLoader>();
	}

	//------------------------------------------------------------------------------------------------------
	void GameManager::MainLoop()
	{
		timer_->Reset();

		while (running_)
		{
			timer_->UpdateTimer();
			window_->ProcessMessages();
            component_manager_->Start();
			component_manager_->UpdateBeforePhysics();
            scene_->ResetMovedByPhysics_(); //Reset the object moved since last frame by physics tag
			physics_manager_->Update();
			component_manager_->UpdateAfterPhysics();
			input_manager_->Update();
            input_manager_->SendInput();
            scene_->ResetMovedByUser_(); //Reset the object moved since last frame by user tag
			component_manager_->Update();
            audio_manager_->UpdateAudioSystem();
			network_manager_->GetSerializationManager().Serialize();
            network_manager_->Listen(); //Network manager's listen is here, because 
            //1 - it can add new components, therefore it cannot be between start and update, because the components will start to
            //update before they start (can add an add queue to circumvent that)
            //2 - it canchange positions of objects, therefore it has to be between the resetting of the moved by user flag and octree
            //update, because otherwise the frustrum culling fucks up
			octree_->Update();
			//octree_->Draw();
			renderer_->Draw(timer_);
            component_manager_->ClearDeletionQueue_();
            SGNode::ClearDeletionQueue_();
            memory_manager_->ClearTemp_();
		}
	}

	//------------------------------------------------------------------------------------------------------
	void GameManager::StopRunning()
	{
		running_ = false;
	}

	//------------------------------------------------------------------------------------------------------
	void GameManager::ShutDown()
	{
        subsystem_allocator_->Delete(scene_loader_);
        subsystem_allocator_->Delete(physics_manager_);
		subsystem_allocator_->Delete((Scene*)scene_);
        subsystem_allocator_->Delete(octree_);
		subsystem_allocator_->Delete(component_manager_);
		subsystem_allocator_->Delete(timer_);
		subsystem_allocator_->Delete(command_context_manager_);
		subsystem_allocator_->Delete(command_manager_);
		subsystem_allocator_->Delete(renderer_);
		subsystem_allocator_->Delete(resource_manager_);
		subsystem_allocator_->Delete(audio_manager_);
		subsystem_allocator_->Delete(input_manager_);
		subsystem_allocator_->Delete(window_);
		subsystem_allocator_->Delete(config_manager_);
		subsystem_allocator_->Delete(network_manager_);
		subsystem_allocator_->Delete(packet_factory_);

		memory_manager_->DeleteAllocator(subsystem_allocator_);
	}

	//------------------------------------------------------------------------------------------------------
	double GameManager::GetDeltaT()
	{
		return timer_->GetDeltaT();
	}

	//------------------------------------------------------------------------------------------------------
	double GameManager::GetTimeSinceStartup()
	{
		return timer_->GetTimeSinceStartup();
	}
	
	//------------------------------------------------------------------------------------------------------
	ConfigManager* GameManager::GetConfigManager()
	{
		return config_manager_;
	}

	//------------------------------------------------------------------------------------------------------
	Octree* GameManager::GetOctree()
	{
		return octree_;
	}
}