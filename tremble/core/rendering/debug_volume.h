#pragma once

namespace tremble
{
	class Mesh;

	struct Point
	{
		Point(const DirectX::XMFLOAT3& point, const DirectX::XMFLOAT4& color) : point(point), color(color) {}
		DirectX::XMFLOAT3 point;
		DirectX::XMFLOAT4 color;
	};

	struct DebugVolume
	{
		std::vector<Point> points; //<! Points of this debug volume
		Mesh* mesh;

		static DebugVolume CreateLine(const DirectX::XMFLOAT3& from, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		static DebugVolume CreateRay(const DirectX::XMFLOAT3& from, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		static DebugVolume CreateSphere(const DirectX::XMFLOAT3& center, const float& radius, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		static DebugVolume CreateCube(const DirectX::XMFLOAT3& center, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		static DebugVolume CreateFrustum(const DirectX::BoundingFrustum& frustum, const DirectX::XMFLOAT4& color = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
	};
}