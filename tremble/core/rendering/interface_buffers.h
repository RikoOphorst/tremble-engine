#pragma once

namespace tremble
{
	/// Pass constant buffer for user interface sprite rendering
	struct InterfaceSpritePassConstants
	{
		DirectX::XMMATRIX view_projection = DirectX::XMMatrixIdentity();
		DirectX::XMMATRIX view_view = DirectX::XMMatrixIdentity();
	};

	/// Object constant buffer for user interface sprite rendering
	struct InterfaceSpriteObjectConstants
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
		DirectX::XMVECTOR color = { 1, 1, 1, 1 };
		DirectX::XMFLOAT2 uv_min;
		DirectX::XMFLOAT2 uv_max;
	};

	/// Input layout for user interface sprite rendering
	const static D3D12_INPUT_ELEMENT_DESC interface_sprite_input[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	/// Vertex layout for user interface sprite rendering
	struct SpriteVertex
	{
		DirectX::XMFLOAT4 position;
		DirectX::XMFLOAT2 uv;
	};


	//------------------------------------------------------------------------------------------------------


	/// Input layout for user interface font rendering
	const static D3D12_INPUT_ELEMENT_DESC interface_font_input[] = {
		{ "BLENDINDICES", 0, DXGI_FORMAT_R32_UINT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	/// Object constant buffer for user interface sprite rendering
	struct InterfaceFontData
	{
		DirectX::XMMATRIX transform = DirectX::XMMatrixIdentity();
		DirectX::XMVECTOR color = { 1, 1, 1, 1 };
		DirectX::XMFLOAT2 uv_min;
		DirectX::XMFLOAT2 uv_max;
	};

	struct FontVertex
	{
		unsigned int sprite_index;
	};


}