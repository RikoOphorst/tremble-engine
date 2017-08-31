#include "obj_loader.h"
#include "tiny_obj_loader.h"

namespace tremble
{
	//------------------------------------------------------------------------------------------------------
	bool OBJLoader::LoadOBJ(const std::string& file_path_to_obj, Mesh::MeshData* mesh_data)
	{
		std::string input_file = file_path_to_obj;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;

		std::string error;
		bool ret = tinyobj::LoadObj(shapes, materials, error, input_file.c_str());

		if (!error.empty())
		{
			std::cerr << error << std::endl;
		}

		if (!ret)
		{
			return false;
		}

		for (size_t i = 0; i < shapes.size(); i++) {
			for (size_t f = 0; f < shapes[i].mesh.indices.size() / 3; f++) {
				mesh_data->indices.push_back(shapes[i].mesh.indices[3 * f + 0]);
				mesh_data->indices.push_back(shapes[i].mesh.indices[3 * f + 1]);
				mesh_data->indices.push_back(shapes[i].mesh.indices[3 * f + 2]);
			}

			for (size_t v = 0; v < shapes[i].mesh.positions.size() / 3; v++) {
				Vertex vert;
				vert.position = DirectX::XMFLOAT3(shapes[i].mesh.positions[3 * v + 0], shapes[i].mesh.positions[3 * v + 1], shapes[i].mesh.positions[3 * v + 2]);

				if (shapes[i].mesh.normals.size() > 3 * v + 0)
				{
					vert.normal = DirectX::XMFLOAT3(shapes[i].mesh.normals[3 * v + 0], shapes[i].mesh.normals[3 * v + 1], shapes[i].mesh.normals[3 * v + 2]);
				}

				if (shapes[i].mesh.normals.size() > 2 * v + 0)
				{
					vert.uv = DirectX::XMFLOAT2(shapes[i].mesh.texcoords[2 * v + 0], shapes[i].mesh.texcoords[2 * v + 1]);
				}

				mesh_data->vertices.push_back(vert);
			}
		}

		auto Vector2Cross = [](const DirectX::XMFLOAT2& a, const DirectX::XMFLOAT2& b)
		{
			DirectX::XMFLOAT2 result;
			result.x = a.x * b.y - a.y * b.x;
			result.y = a.x * b.y - a.y * b.x;

			return result;
		};

		int idx1, idx2, idx3;

		DirectX::XMVECTOR e1, e2, p;
		DirectX::XMVECTOR uv, t1, t2;

		for (unsigned int i = 0; i < mesh_data->indices.size(); i += 3)
		{
			idx1 = i;
			idx2 = i + 1;
			idx3 = i + 2;

			Vertex& v1 = mesh_data->vertices.at(mesh_data->indices.at(idx1));
			Vertex& v2 = mesh_data->vertices.at(mesh_data->indices.at(idx2));
			Vertex& v3 = mesh_data->vertices.at(mesh_data->indices.at(idx3));

			p = DirectX::XMLoadFloat3(&v1.position);
			e1 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&v2.position), p);
			e2 = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&v3.position), p);

			uv = DirectX::XMVectorSet(v1.uv.x, v1.uv.y, 1.0f, 0.0f);
			t1 = DirectX::XMVectorSubtract(DirectX::XMVectorSet(v2.uv.x, v2.uv.y, 1.0f, 0.0f), uv);
			t2 = DirectX::XMVectorSubtract(DirectX::XMVectorSet(v3.uv.x, v3.uv.y, 1.0f, 0.0f), uv);

			DirectX::XMFLOAT2 cross = Vector2Cross(DirectX::XMFLOAT2(DirectX::XMVectorGetX(t1), DirectX::XMVectorGetY(t1)), DirectX::XMFLOAT2(DirectX::XMVectorGetX(t2), DirectX::XMVectorGetY(t2)));
			float r = 1.0f / cross.x;

			if (r == std::numeric_limits<float>::infinity())
			{
				int i = 0;
			}

			DirectX::XMVECTOR tangent = DirectX::XMVectorScale((DirectX::XMVectorSubtract(DirectX::XMVectorScale(e1, DirectX::XMVectorGetY(t2)), DirectX::XMVectorScale(e2, DirectX::XMVectorGetY(t1)))), r);
			DirectX::XMVECTOR bitangent = DirectX::XMVectorScale((DirectX::XMVectorSubtract(DirectX::XMVectorScale(e2, DirectX::XMVectorGetY(t1)), DirectX::XMVectorScale(e1, DirectX::XMVectorGetY(t2)))), r);

			DirectX::XMFLOAT3 tangentf3;
			DirectX::XMFLOAT3 bitangentf3;

			XMStoreFloat3(&tangentf3, tangent);
			XMStoreFloat3(&bitangentf3, bitangent);

			v1.tangent = v2.tangent = v3.tangent = tangentf3;
			v1.bitangent = v2.bitangent = v3.bitangent = bitangentf3;
		}

		return true;
	}
}