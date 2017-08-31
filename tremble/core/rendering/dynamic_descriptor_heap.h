#pragma once

namespace tremble
{
	/**
	* @class tremble::DynamicDescriptorHeap
	* @author Riko Ophorst
	* @brief Wrapper around the DX12 DescriptorHeap, allowing for unbounded descriptor allocation
	*/
	class DynamicDescriptorHeap
	{
	public:
		/**
		* @brief DynamicDescriptorHeap constructor
		* @param[in] device The device used to create the heaps with
		* @param[in] heap_type The type of heap you wish this to be
		* @param[in] flags Any flags you want to pass onto DirectX 12
		* @param[in] descriptor_count_per_heap The amount of descriptors per heap
		*/
		DynamicDescriptorHeap(ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heap_type, D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, UINT descriptor_count_per_heap = 100U);
		
		/**
		* @brief DynamicDescriptorHeap destructor
		*/
		~DynamicDescriptorHeap();

		/**
		* @brief Creates a DepthStencilView (descriptor)
		* @param[in] device The device used to create the descriptor & heaps with
		* @param[in] dsv_buffer The buffer this descriptor should describe
		* @param[in] desc The description of this descriptor used to describe the resource with
		*/
		UINT CreateDepthStencilView(ID3D12Device* device, ID3D12Resource* dsv_buffer, D3D12_DEPTH_STENCIL_VIEW_DESC* desc);

		/**
		* @brief Creates a RenderTargetView (descriptor)
		* @param[in] device The device used to create the descriptor & heaps with
		* @param[in] rtv_buffer The buffer this descriptor should describe
		* @param[in] desc The description of this descriptor used to describe the resource with
		*/
		UINT CreateRenderTargetView(ID3D12Device* device, ID3D12Resource* rtv_buffer, D3D12_RENDER_TARGET_VIEW_DESC* desc);

		/**
		* @brief Creates a ConstantBufferView (descriptor)
		* @param[in] device The device used to create the descriptor & heaps with
		* @param[in] desc The description of this descriptor used to describe the resource with
		*/
		UINT CreateConstantBufferView(ID3D12Device* device, D3D12_CONSTANT_BUFFER_VIEW_DESC* desc);

		/**
		* @brief Creates a ShaderResourceView (descriptor)
		* @param[in] device The device used to create the descriptor & heaps with
		* @param[in] desc The description of this descriptor used to describe the resource with
		*/
		UINT CreateShaderResourceView(ID3D12Device* device, ID3D12Resource* srv_buffer, D3D12_SHADER_RESOURCE_VIEW_DESC* desc);

		/**
		* @brief Creates a ShaderResourceView (descriptor)
		* @param[in] device The device used to create the descriptor & heaps with
		* @param[in] buffer The buffer this descriptor should describe
		* @param[in] counter_buffer The counter buffer this descriptor should describe
		* @param[in] desc The description of this descriptor used to describe the resource with
		*/
		UINT CreateUnorderedAccessView(ID3D12Device* device, ID3D12Resource* buffer, ID3D12Resource* counter_buffer, D3D12_UNORDERED_ACCESS_VIEW_DESC* desc);

		/**
		* @brief Creates a sampler state (descriptor)
		* @param[in] device The device used to create the descriptor & heaps with
		* @param[in] desc The description of this descriptor used to describe the resource with
		*/
		UINT CreateSampler(ID3D12Device* device, D3D12_SAMPLER_DESC* desc);

		/**
		* @brief Get a cpu handle to a descriptor by its id
		* @param[in] id The id of the desired descriptor
		*/
		D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorById(UINT id);

		/**
		* @brief Get a gpu handle to a descriptor by its id
		* @param[in] id The id of the desired descriptor
		*/
		D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorById(UINT id);

		const std::vector<ID3D12DescriptorHeap*>& GetHeaps() const { return heaps_; }

	private:
		/**
		* @brief Finds a new index for heap & descriptors
		* @param[in] idx The index you want to find the heap&descriptor for
		* @param[out] heap_index The index of the heap
		* @param[out] descriptor_heap The index of the descriptor
		*/
		void FindHeapIndexAndDescriptorIndex(int idx, int* heap_index, int* descriptor_index);

		/**
		* @brief Creates a new inline heap used to store our descriptors with
		*/
		void CreateNewHeap(ID3D12Device* device);

		/**
		* @brief Get a handle to a descriptor by heap & descriptor index
		* @param[in] heap_index The heap in which the descriptor resides, identified by its index in the array of heaps
		* @param[in] descriptor_index The index of the descriptor in the specified heap. This will be multiplied by this heap's descriptor size to find the desired descriptor.
		*/
		D3D12_CPU_DESCRIPTOR_HANDLE GetDescriptorHandle(int heap_index, int descriptor_index);
	private:
		std::vector<ID3D12DescriptorHeap*> heaps_; //!< All heaps we use to dynamically store descriptors are in here
		UINT allocated_descriptors_; //!< The number of descriptors that have been allocated in ALL of the heaps we have running right now
		D3D12_DESCRIPTOR_HEAP_FLAGS flags_; //!< The flags used to setup a new descriptor heap with
		D3D12_DESCRIPTOR_HEAP_TYPE heap_type_; //!< The type of heap we are mimicking
		UINT descriptor_count_per_heap_; //!< The maximum amount of descriptors in a single descriptor heap
		UINT descriptor_size_; //!< The size of a single descriptor of this heap's type in bytes
	};
}