#include "debug_volume.h"

#include "../resources/mesh.h"
#include "../math/math.h"
#include "core/memory/memory_manager.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	DebugVolume DebugVolume::CreateLine(const DirectX::XMFLOAT3& from, const DirectX::XMFLOAT3& to, const DirectX::XMFLOAT4& color)
	{
		DebugVolume volume;

		volume.points.push_back(Point(from, color));
		volume.points.push_back(Point(to, color));

		Mesh::MeshData data;
		
		for (int i = 0; i < volume.points.size(); i++)
		{
			Vertex v;

			v.position = volume.points[i].point;
			v.color = volume.points[i].color;

			data.vertices.push_back(v);
		}

		data.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

        volume.mesh = Get::MemoryManager()->NewTemp<Mesh>(data);
//		volume.mesh = new Mesh(data);

		return volume;
	}
	
	//------------------------------------------------------------------------------------------------------
	DebugVolume DebugVolume::CreateRay(const DirectX::XMFLOAT3& from, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT4& color)
	{
		DebugVolume volume;

		volume.points.push_back(Point(from, color));

		DirectX::XMVECTOR fromv = DirectX::XMVectorSet(from.x, from.y, from.z, 1.0f);
		DirectX::XMVECTOR dirv = DirectX::XMVectorSet(direction.x, direction.y, direction.z, 0.0f);
		DirectX::XMVECTOR tov = DirectX::XMVectorAdd(fromv, dirv);

		volume.points.push_back(Point(
			DirectX::XMFLOAT3(DirectX::XMVectorGetX(tov), DirectX::XMVectorGetY(tov), DirectX::XMVectorGetZ(tov)),
			color
		));

		Mesh::MeshData data;

		for (int i = 0; i < volume.points.size(); i++)
		{
			Vertex v;

			v.position = volume.points[i].point;
			v.color = volume.points[i].color;

			data.vertices.push_back(v);
		}

		data.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

        volume.mesh = Get::MemoryManager()->NewTemp<Mesh>(data);

		return volume;
	}
	
	//------------------------------------------------------------------------------------------------------
	DebugVolume DebugVolume::CreateSphere(const DirectX::XMFLOAT3& center, const float& radius, const DirectX::XMFLOAT4& color)
	{
		DebugVolume volume;

		float circle_preciseness = 20.0f;
		float angle_per_it = 360.0f / circle_preciseness;

		DirectX::XMVECTOR centerv = DirectX::XMVectorSet(center.x, center.y, center.z, 1.0f);

		for (float angle = 0.0f; angle <= 360.0f; angle += angle_per_it)
		{
			DirectX::XMFLOAT3 point;
			point = DirectX::XMFLOAT3(center.x, center.y + (std::sinf(angle * 0.0174533f) * radius), center.z + (std::cosf(angle * 0.0174533f) * radius));

			if (angle > angle_per_it)
			{
				volume.points.push_back(volume.points.back());
			}

			volume.points.push_back(Point(point, color));
		}

		for (float angle = 0.0f; angle <= 360.0f; angle += angle_per_it)
		{
			DirectX::XMFLOAT3 point;
			point = DirectX::XMFLOAT3(center.x + (std::cosf(angle * 0.0174533f) * radius), center.y, center.z + (std::sinf(angle * 0.0174533f) * radius));

			if (angle > angle_per_it)
			{
				volume.points.push_back(volume.points.back());
			}

			volume.points.push_back(Point(point, color));
		}

		for (float angle = 0.0f; angle <= 360.0f; angle += angle_per_it)
		{
			DirectX::XMFLOAT3 point;
			point = DirectX::XMFLOAT3(center.x + (std::cosf(angle * 0.0174533f) * radius), center.y + (std::sinf(angle * 0.0174533f) * radius), center.z);

			if (angle > angle_per_it)
			{
				volume.points.push_back(volume.points.back());
			}

			volume.points.push_back(Point(point, color));
		}

		Mesh::MeshData data;

		for (int i = 0; i < volume.points.size(); i++)
		{
			Vertex v;

			v.position = volume.points[i].point;
			v.color = volume.points[i].color;

			data.vertices.push_back(v);
		}

		data.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

        volume.mesh = Get::MemoryManager()->NewTemp<Mesh>(data);

		return volume;
	}
	
	//------------------------------------------------------------------------------------------------------
	DebugVolume DebugVolume::CreateCube(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& size, const DirectX::XMFLOAT4& color)
	{
		DebugVolume volume;

		DirectX::XMFLOAT3 hs(size.x / 2.0f, size.y / 2.0f, size.z / 2.0f);

		Point p1 = Point(DirectX::XMFLOAT3(position.x - hs.x, position.y - hs.y, position.z + hs.z), color);
		Point p2 = Point(DirectX::XMFLOAT3(position.x - hs.x, position.y + hs.y, position.z + hs.z), color);
		Point p3 = Point(DirectX::XMFLOAT3(position.x + hs.x, position.y + hs.y, position.z + hs.z), color);
		Point p4 = Point(DirectX::XMFLOAT3(position.x + hs.x, position.y - hs.y, position.z + hs.z), color);
		Point p5 = Point(DirectX::XMFLOAT3(position.x - hs.x, position.y - hs.y, position.z - hs.z), color);
		Point p6 = Point(DirectX::XMFLOAT3(position.x - hs.x, position.y + hs.y, position.z - hs.z), color);
		Point p7 = Point(DirectX::XMFLOAT3(position.x + hs.x, position.y + hs.y, position.z - hs.z), color);
		Point p8 = Point(DirectX::XMFLOAT3(position.x + hs.x, position.y - hs.y, position.z - hs.z), color);

		volume.points.push_back(p1);
		volume.points.push_back(p2);
		volume.points.push_back(p3);
		volume.points.push_back(p4);
		volume.points.push_back(p1);
		volume.points.push_back(p5);
		volume.points.push_back(p6);
		volume.points.push_back(p2);
		volume.points.push_back(p1);
		volume.points.push_back(p5);
		volume.points.push_back(p6);
		volume.points.push_back(p7);
		volume.points.push_back(p8);
		volume.points.push_back(p5);
		volume.points.push_back(p8);
		volume.points.push_back(p7);
		volume.points.push_back(p3);
		volume.points.push_back(p4);
		volume.points.push_back(p8);

		Mesh::MeshData data;

		for (int i = 0; i < volume.points.size(); i++)
		{
			Vertex v;

			v.position = volume.points[i].point;
			v.color = volume.points[i].color;

			data.vertices.push_back(v);
		}

		data.topology = D3D_PRIMITIVE_TOPOLOGY_LINESTRIP; // linestrip because line list would require 24 physical points

        volume.mesh = Get::MemoryManager()->NewTemp<Mesh>(data);
		volume.mesh->BuildBuffers();

		return volume;
	}
	
	//------------------------------------------------------------------------------------------------------
	DebugVolume DebugVolume::CreateFrustum(const DirectX::BoundingFrustum& frustum, const DirectX::XMFLOAT4& color)
	{
		DirectX::XMFLOAT3 corners[8];
		frustum.GetCorners(corners);

		Point p0 = Point(corners[0], color);
		Point p1 = Point(corners[1], color);
		Point p2 = Point(corners[2], color);
		Point p3 = Point(corners[3], color);
		Point p4 = Point(corners[4], color);
		Point p5 = Point(corners[5], color);
		Point p6 = Point(corners[6], color);
		Point p7 = Point(corners[7], color);

		DebugVolume volume;
		volume.points.push_back(p0);
		volume.points.push_back(p1);
		volume.points.push_back(p1);
		volume.points.push_back(p2);
		volume.points.push_back(p2);
		volume.points.push_back(p3);
		volume.points.push_back(p3);
		volume.points.push_back(p0);
		volume.points.push_back(p4);
		volume.points.push_back(p5);
		volume.points.push_back(p5);
		volume.points.push_back(p6);
		volume.points.push_back(p6);
		volume.points.push_back(p7);
		volume.points.push_back(p7);
		volume.points.push_back(p4);
		volume.points.push_back(p0);
		volume.points.push_back(p4);
		volume.points.push_back(p1);
		volume.points.push_back(p5);
		volume.points.push_back(p2);
		volume.points.push_back(p6);
		volume.points.push_back(p3);
		volume.points.push_back(p7); 
		
		Mesh::MeshData data;

		for (int i = 0; i < volume.points.size(); i++)
		{
			Vertex v;

			v.position = volume.points[i].point;
			v.color = volume.points[i].color;

			data.vertices.push_back(v);
		}

		data.topology = D3D_PRIMITIVE_TOPOLOGY_LINELIST;

        volume.mesh = Get::MemoryManager()->NewTemp<Mesh>(data);
		volume.mesh->BuildBuffers();

		return volume;
	}
}
