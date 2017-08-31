#pragma once

#include "debug_volume.h"
#include "constant_buffers.h"

#include "device.h"
#include "swap_chain.h"
#include "depth_buffer.h"
#include "descriptor_heap.h"
#include "buffer_manager.h"
#include "upload_buffer.h"
#include "interface_sprite_renderer.h"
#include "interface_font_renderer.h"
#include "shadow_renderer.h"
#include "particle_renderer.h"

namespace tremble
{
	class Window;
	class RootSignature;
	class Shader;
	class Camera;
	class Renderable;
	class Light;
	struct Material;
	class Timer;
	class GameManager;
	class DepthStencil;
	class Octree;
	class CommandContext;
	class GraphicsContext;
	class FreeListAllocator;
	
	/**
	* @class tremble::Renderer
	* @author Riko Ophorst
	* @brief Forward DirectX 12 renderer
	*/
	class Renderer
	{
	public:
		/**
		* @brief Renderer constructor
		*/
		Renderer();

		/**
		* @brief Renderer destructor
		*/
		~Renderer();

		/**
		* @brief Starts up the renderer, initializes everything, etc.
		*/
		void Startup();

		/**
		* @brief Makes the renderer draw to screen
		* @param[in] gt The timer the game is running with
		*/
		void Draw(Timer* gt);

		void SetCamera(Camera* camera);
        Camera* GetCamera() const { return camera_; }

		void RenderDebugVolume(const DebugVolume& volume);

		Device& GetDevice() { return device_; } //!< Get the underlying device
		const Device& GetDevice() const { return device_; } //!< Get the underlying device
		
		DescriptorHeap& GetRtvHeap() { return rtv_heap_; }
		DescriptorHeap& GetDsvHeap() { return dsv_heap_; }
		DescriptorHeap& GetCbvSrvUavHeap() { return cbv_srv_uav_heap_; }
		DescriptorHeap& GetSamplerHeap() { return sampler_heap_; }

		SwapChain& GetSwapChain() { return swap_chain_; }

		inline BufferManager& GetBufferManager() { return buffer_manager_; }

		InterfaceFontRenderer* GetFontRenderer() { return &font_renderer_; }

		void SetVirtualSize(int x, int y) { virtual_size_x_ = x; virtual_size_y_ = y; }
		int GetVirtualSizeX() { return virtual_size_x_; }
		int GetVirtualSizeY() { return virtual_size_y_; }

		UploadBuffer& GetPassConstantsBuffer() { return pass_constants_; }
		UploadBuffer& GetDebugConstantsBuffer() { return debug_constants_; }

		UploadBuffer& GetMaterialConstantsBuffer() { return material_constants_; }
		UINT GetNumMaterialCbs() { return num_material_cbs_; }
		void SetNumMaterialCbs(UINT num_material_cbs) { num_material_cbs_ = num_material_cbs; }
		UINT GetNewMaterialConstantBufferID() { return num_material_cbs_++; }

		UploadBuffer& GetObjectConstantsBuffer() { return object_constants_; }
		UINT GetNumObjectCbs() { return num_object_cbs_; }
		void SetNumObjectCbs(UINT num_object_cbs) { num_object_cbs_ = num_object_cbs; }
		UINT GetNewObjectConstantBufferID() { return num_object_cbs_++; }

	protected:
		void CreateDevice(); //!< Creates the Direct3D device
		void CreateCommandObjects(); //!< Creates the command objects (queue, allocator, list)
		void CreateSwapChain(); //!< Creates the swap chain
		void CreateDescriptorHeaps(); //!< Creates the resource heaps for the render target views & depth/stencil views
		void CreatePSOs(); //!< Creates the default pipeline state object
		void CreateBaseResources(); //!< Creates all the buffered frame resources
		void FlushCommandQueue(); //!< Informs the renderer to flush the entire command queue

		void DrawDebugVolumes(GraphicsContext& context, std::vector<DebugVolume>& debug_volumes);
		void CheckDeviceRemoved(); //!< Check whether the ID3D12Device was removed and for what reason (outputs to console)

	private:
		Device device_;
		SwapChain swap_chain_;
		DepthBuffer depth_buffer_;

		DescriptorHeap rtv_heap_; //!< Heap to store descriptors of the render target views
		DescriptorHeap dsv_heap_; //!< Heap to store descriptors of the depth/stencil views
		DescriptorHeap cbv_srv_uav_heap_; //!< Heap to store descriptors of shader resource views
		DescriptorHeap sampler_heap_; //!< Heap to store descriptors of sampler states

		UploadBuffer pass_constants_;
		UploadBuffer debug_constants_;

		UploadBuffer material_constants_;
		UINT num_material_cbs_;
		UploadBuffer object_constants_;
		UINT num_object_cbs_;

		BufferManager buffer_manager_;
		UploadBuffer light_upload_buffer_;
		StructuredBuffer light_buffer_;

		std::vector<DebugVolume> debug_volumes_; //!< The queue of debug volumes that will be rendered each frame

		Camera* camera_; //!< The camera that is used to render the scene with

		InterfaceSpriteRenderer sprite_renderer_;
		InterfaceFontRenderer font_renderer_; 
		ShadowRenderer shadow_renderer_;
		ParticleRenderer particle_renderer_;
		int virtual_size_x_ = 1280;
		int virtual_size_y_ = 720;
		bool depthpass_ = true;
	};
}