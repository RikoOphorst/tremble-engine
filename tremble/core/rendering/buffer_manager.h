#pragma once

#include "color_buffer.h"
#include "depth_buffer.h"
#include "byte_address_buffer.h"
#include "typed_buffer.h"
#include "structured_buffer.h"

#include "../get.h"

namespace tremble
{
	enum ColorBufferType
	{
		ColorBufferUnknown,
		ColorBufferScene
	};

	enum DepthBufferType
	{
		DepthBufferUnknown,
		DepthBufferScene
	};

	enum ByteBufferType
	{
		ByteBufferUnknown
	};

	enum TypedBufferType
	{
		TypedBufferUnknown
	};

	enum StructuredBufferType
	{
		StructuredBufferUnknown,
		StructuredBufferLights
	};

	class BufferManager
	{
	public:
		BufferManager();
		~BufferManager();

		void Create(UINT width, UINT height);
		void Destroy();

		static ColorBuffer& GetBuffer(const ColorBufferType& type);
		static DepthBuffer& GetBuffer(const DepthBufferType& type);
		static ByteAddressBuffer& GetBuffer(const ByteBufferType& type);
		static TypedBuffer& GetBuffer(const TypedBufferType& type);
		static StructuredBuffer& GetBuffer(const StructuredBufferType& type);
	private:
		std::unordered_map<ColorBufferType, ColorBuffer> buffers_color_;
		std::unordered_map<DepthBufferType, DepthBuffer> buffers_depth_;
		std::unordered_map<ByteBufferType, ByteAddressBuffer> buffers_byte_address_;
		std::unordered_map<TypedBufferType, TypedBuffer> buffers_typed_;
		std::unordered_map<StructuredBufferType, StructuredBuffer> buffers_structured_;
	};
}