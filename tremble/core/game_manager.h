#pragma once

#include "utilities\utilities.h"
#include "config/config_manager.h"
#include "resources/scene_loader.h"

namespace tremble
{
	class ResourceManager;
	class FreeListAllocator;
	class StackAllocator;
	class MemoryManager;
	class Window;
	class InputManager;
	class ComponentManager;
	class Renderer;
	class CommandManager;
	class CommandContextManager;
	class Timer;
	class SGNode;
	class Scene;
	class PhysicsManager;
	class AudioManager;
	class NetworkManager;
	class PacketFactory;
	class Octree;

	/** 
	* @class tremble::GameManager
	* @brief This is a main game manager class, that holds a game's instance
	* @author Anton Gavrilov
	*/
	class GameManager
	{
		friend class SGNode;
		friend class PhysicsRigidbodyComponent;
	public:
		/**
		* @brief GameManager constructor
		* @param[in] memory_manager Memory manager, that manages allocators
		* @param[in] own_allocator Allocator, that is used to hold all the subsystems of the game manager
		*/
		GameManager(MemoryManager* memory_manager, FreeListAllocator* own_allocator);

		/**
		* @brief GameManager destructor
		* @pre GameManager::ShutDown()
		*/
		~GameManager();

	    /*
		* @brief Game manager factory design delete function. Only this should be used for game manager deletion
		* @pre GameManager::ShutDown()
		* @param[in] game_manager Game manager that you want to delete
		*/
		static void Delete(GameManager* game_manager);
		/*
		* @brief Game manager factory design create function. Only this should be used for game manager instantiation
		* @param[in] memory_size Memory size that should be allocated by the overall memory manager inside of game manager
		*/
		static GameManager* Create(size_t memory_size);
		
		void Startup(const std::string& name, int width, int height);
		
		void MainLoop(); //!< A general engine update loop

		void StopRunning(); //!< shutdown the game manager after the current frame @see ShutDown()
		void ShutDown(); //!< Shutdown function of the engine. Here we properly shutdown everything and release all the resources that game manager holds @see StopRunning()

		ResourceManager* GetResourceManager() { return resource_manager_; } //!< Get the resouce manager
		MemoryManager* GetMemoryManager() { return memory_manager_; } //!< Get the game's memory manager in order to get new allocators
		Renderer* GetRenderer() { return renderer_; } //!< Get the game's renderer
		CommandManager* GetCommandManager() { return command_manager_; }
		CommandContextManager* GetCommandContextManager() { return command_context_manager_; }
		SGNode* GetScene() { return scene_; } //!< Get the root scene graph node of the scene
		PhysicsManager* GetPhysicsManager() { return physics_manager_; } //!< Get the physics manager of this game manager
		ComponentManager* GetComponentManager() { return component_manager_; } //!< Get the component manager of this game manager
		AudioManager* GetAudioManager() { return audio_manager_; } //!< Get the audio manager of this game manager
        SceneLoader* GetSceneLoader() const { return scene_loader_; } //!< Get the scene loader of this game manager

		InputManager* GetInputManager() { return input_manager_; } //!< Get input manager tied with this game manager
		Window* GetWindow() { return window_; } //!< Get window of this game manager
		double GetDeltaT(); //!< Get time, that it took to run the last frame
		double GetTimeSinceStartup(); //!< Get time passed from startup to the end of the last frame
		ConfigManager* GetConfigManager(); //!< Get the config manager
		NetworkManager* GetNetworkManager() { return network_manager_; } //!< Get the game network manager
		PacketFactory* GetPacketFactory() { return packet_factory_; } //!< Get the packet factory singleton
		Octree* GetOctree(); //!< Get the octree

	private:
		Timer* timer_; //!< Takes care of delta time
		bool running_; //!< Is the game running? @see StopRunning()
		FreeListAllocator* own_allocator_; //!< Allocator, that is used to allocate resources that are in use by the game manager itself
		StackAllocator* subsystem_allocator_; //!< Stack allocation strategy for allocating subsystems

		PacketFactory* packet_factory_; //!< A factory class for creating (and after using, deleting) packets.
		NetworkManager* network_manager_; //!< The network manager used to communicate with the host player or other players.
		ConfigManager* config_manager_; //!< Manages the game config
		MemoryManager* memory_manager_; //!< Manages all the memory, used by the game
		InputManager* input_manager_; //!< Manages the input in the game
		ComponentManager* component_manager_; //!< Runs functions on all components. Components themselves are owned by the scene
		ResourceManager* resource_manager_; //!< Owns all the resources in the game (meshes, textures)
		PhysicsManager* physics_manager_; //!< Manages physics in the game(currently physx, later may be a different library)
		SGNode* scene_; //!< Scene graph root node
        SceneLoader* scene_loader_; //!< Scene loader, loads the scenes

		Window* window_; //!< The window the game is utilizing
		Renderer* renderer_; //!< The renderer that draws everything onto the window
		CommandManager* command_manager_; //!< Manages commands send to the GPU
		CommandContextManager* command_context_manager_; //!< Manages all command contexts in the application
		AudioManager* audio_manager_; //!< Contains audio system and manages all channels and clips
		Octree* octree_; //!< Octree for per renderable nodes
	};
}
