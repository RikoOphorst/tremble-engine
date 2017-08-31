#pragma once

#include "mesh.h"

namespace tremble
{
	class OBJLoader
	{
	public:
 		static bool LoadOBJ(const std::string& file_path_to_obj, Mesh::MeshData* mesh_data);
	};
}