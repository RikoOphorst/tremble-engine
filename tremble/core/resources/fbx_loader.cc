#include "fbx_loader.h"

#include "../../components/rendering/renderable.h"
#include "../../components/physics/rigidbody_static.h"
#include "../utilities/debug.h"
#include "../scene_graph/scene_graph.h"
#include "../memory/memory_includes.h"
#include "../resources/resource_manager.h"
#include "../rendering/material.h"
#include "../get.h"
#include "../physics/physics_box_geometry.h"

namespace tremble
{

	//------------------------------------------------------------------------------------------------------
	bool FBXLoader::Setup(FbxScene** output_scene, std::string file_path_to_fbx)
	{
		////Initialize fbx objects
		//FbxManager* fbx_manager = FbxManager::Create();
		//if (!fbx_manager)
		//{
		//	DLOG("Error: Unable to create FBX Manager!\n");
		//	return false;
		//}

		//FbxIOSettings* fbx_settings = FbxIOSettings::Create(fbx_manager, IOSROOT);

		//fbx_manager->SetIOSettings(fbx_settings);

		//*output_scene = FbxScene::Create(fbx_manager, "Import Scene");
		//if (!*output_scene)
		//{
		//	DLOG("Error:Unable to create a fbx scene");
		//	return false;
		//}

		////load the scene using the importer
		//int fbx_file_major, fbx_file_minor, fbx_file_revision;
		//int fbx_sdk_major, fbx_sdk_minor, fbx_sdk_revision;

		////get version of fbx sdk
		//FbxManager::GetFileFormatVersion(fbx_sdk_major, fbx_sdk_minor, fbx_sdk_revision);

		////Create an importer
		//FbxImporter* fbx_importer = FbxImporter::Create(fbx_manager, "");

		////Initialize the importer
		//const bool import_status = fbx_importer->Initialize(file_path_to_fbx.c_str(), -1, fbx_manager->GetIOSettings());
		//fbx_importer->GetFileVersion(fbx_file_major, fbx_file_minor, fbx_file_revision);

		//if (!import_status)
		//{
		//	FbxString error = fbx_importer->GetStatus().GetErrorString();
		//	DLOG("Call to FbxImporter::Initialize() failed.\n");
		//	DLOG("Error returned: ");
		//	DLOG(error.Buffer());
		//	DLOG("\n");

		//	if (fbx_importer->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		//	{
		//		DLOG("FBX file format version for this FBX SDK is");
		//		DLOG((std::to_string(fbx_sdk_major) + std::to_string(fbx_sdk_minor) + std::to_string(fbx_sdk_revision)).c_str());
		//		FBXSDK_printf("FBX file format version for file is ");
		//		DLOG((file_path_to_fbx + std::to_string(fbx_file_major) + std::to_string(fbx_file_minor) + std::to_string(fbx_file_revision)).c_str());
		//	}

		//	return false;
		//}

		////Import the scene
		//bool status = fbx_importer->Import(*output_scene);

		//if (status == false)
		//{
		//	DLOG("ERROR ON LOAD: An error occured while importing a scene")
		//		if (fbx_importer->GetStatus().GetCode() == FbxStatus::ePasswordError)
		//		{
		//			DLOG("ERROR ON LOAD: Password protected scenes are not supported.");
		//		}

		//	fbx_importer->Destroy();
		//	return false;
		//}

		//fbx_importer->Destroy();
		//return true;
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	bool FBXLoader::LoadFBXMesh(const std::string& file_path_to_fbx, Mesh::MeshData* mesh_data)
	{
		/*FbxScene* fbx_scene;
		Setup(&fbx_scene, file_path_to_fbx);

		FbxNode* node = fbx_scene->GetRootNode();

		int i;
		if (node)
		{
			for (i = 0; i < node->GetChildCount(); i++)
			{
				if (ImportFBXNode(node->GetChild(i), mesh_data) == true)
				{
					return true;
				}
			}
		}
		return false;*/
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	bool FBXLoader::LoadFBXScene(const std::string& file_path_to_fbx, SGNode* node_to_load_into, bool load_as_static)
	{
		/*FbxScene* fbx_scene;
		Setup(&fbx_scene, file_path_to_fbx);

		FbxNode* root_node = fbx_scene->GetRootNode();

		int i;
		if (root_node)
		{
			for (i = 0; i < root_node->GetChildCount(); i++)
			{
				ImportFBXNodeIntoNode(root_node->GetChild(i), node_to_load_into, file_path_to_fbx, load_as_static);
			}
		}

		return false;*/
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	void FBXLoader::ImportFBXNodeIntoNode(FbxNode* node, SGNode* parent_node, const std::string& file_path, bool load_as_static)
	{
		//FbxDouble3 translation = node->LclTranslation.Get();
		//FbxDouble3 rotation = node->LclRotation.Get();
		//FbxDouble3 scaling = node->LclScaling.Get();

		//SGNode* current_node = parent_node->sg_allocator_->New<SGNode>(
		//	parent_node, 
		//	load_as_static, 
		//	Vector3(
		//		static_cast<float>(translation[0]),
		//		static_cast<float>(translation[1]),
		//		static_cast<float>(translation[2])
		//	),
		//	Quaternion(
		//		Scalar((rotation[2] * 0.01745329251f)), //roll - z
		//		Scalar((rotation[0] * 0.01745329251f)), //pitch - x
		//		Scalar((rotation[1] * 0.01745329251f))  //yaw - y
		//	),
		//	Vector3(
		//		static_cast<float>(scaling[0]),
		//		static_cast<float>(scaling[1]),
		//		static_cast<float>(scaling[2])
		//	)
		//);


		//char* property_names[] = {
		//	"collision_size_x",
		//	"collision_size_y",
		//	"collision_size_z",
		//	"collision_offset_x",
		//	"collision_offset_y",
		//	"collision_offset_z" };

		//FbxProperty p = node->FindProperty("collision_size_x", false);
		//if (p.IsValid())
		//{
		//	Vector3 box_dimensions;
		//	Vector3 box_offset;
		//	Vector3 box_rotation;

		//	box_dimensions.SetX(Scalar(p.Get<FbxFloat>()));
		//	if (p.IsValid())
		//	{
		//		std::cout << "size_x: " << p.Get<FbxFloat>();
		//	}

		//	p = node->FindProperty("collision_size_y", false);
		//	box_dimensions.SetY(Scalar(p.Get<FbxFloat>()));
		//	if (p.IsValid())
		//	{
		//		std::cout << " size_y: " << p.Get<FbxFloat>();
		//	}

		//	p = node->FindProperty("collision_size_z", false);
		//	box_dimensions.SetZ(Scalar(p.Get<FbxFloat>()));
		//	if (p.IsValid())
		//	{
		//		std::cout << " size_z: " << p.Get<FbxFloat>() << std::endl;
		//	}

		//	p = node->FindProperty("collision_offset_x", false);
		//	box_offset.SetX(Scalar(p.Get<FbxFloat>()));
		//	if (p.IsValid())
		//	{
		//		std::cout << " offset_x: " << p.Get<FbxFloat>();
		//	}

		//	p = node->FindProperty("collision_offset_y", false);
		//	box_offset.SetY(Scalar(p.Get<FbxFloat>()));
		//	if (p.IsValid())
		//	{
		//		std::cout << " offset_y: " << p.Get<FbxFloat>();
		//	}

		//	p = node->FindProperty("collision_offset_z", false);
		//	box_offset.SetZ(Scalar(p.Get<FbxFloat>()));
		//	if (p.IsValid())
		//	{
		//		std::cout << " offset_z: " << p.Get<FbxFloat>() << std::endl;
		//	}

		//	p = node->FindProperty("collision_rotation_x", false);
		//	box_rotation.SetX(Scalar(p.Get<FbxFloat>()).ToRadians());
		//	if (p.IsValid())
		//	{
		//		std::cout << " rotation_x: " << p.Get<FbxFloat>();
		//	}
		//	
		//	p = node->FindProperty("collision_rotation_y", false);
		//	box_rotation.SetY(Scalar(p.Get<FbxFloat>()).ToRadians());
		//	if (p.IsValid())
		//	{
		//		std::cout << " rotation_y: " << p.Get<FbxFloat>();
		//	}
		//	
		//	p = node->FindProperty("collision_rotation_z", false);
		//	box_rotation.SetZ(Scalar(p.Get<FbxFloat>()).ToRadians());
		//	if (p.IsValid())
		//	{
		//		std::cout << " rotation_z: " << p.Get<FbxFloat>() << std::endl;
		//	}

		//	current_node->AddComponent<RigidbodyStatic>(&PhysicsBoxGeometry(box_dimensions));
		//}

		////Declaring values before the for loop
		//FbxNodeAttribute::EType attribute_type;
		//FbxNodeAttribute* attribute;
		//FbxMesh* fbx_mesh;
		//FbxGeometryElementMaterial* material_element;
		//Renderable* renderable;
		//int attributes_count = node->GetNodeAttributeCount();

		//for (int i = 0; i < attributes_count; i++)
		//{
		//	attribute = node->GetNodeAttributeByIndex(i);
		//	attribute_type = attribute->GetAttributeType();

		//	if (attribute_type == FbxNodeAttribute::eMesh)
		//	{
		//		fbx_mesh = static_cast<FbxMesh*>(attribute);
		//		
		//		renderable = current_node->AddComponent<Renderable>();
		//		
		//		renderable->mesh_ = Get::ResourceManager()->GetMesh(fbx_mesh, file_path);
		//		
		//		material_element = fbx_mesh->GetElementMaterial(0);
		//		
		//		if (material_element->GetIndexArray().GetAt(0) >= 0)
		//		{
		//			renderable->SetMaterial(Get::ResourceManager()->GetMaterial(
		//				node->GetMaterial(material_element->GetIndexArray().GetAt(0)), 
		//				file_path
		//			));
		//		}
		//		
		//		if (material_element->GetMappingMode() != FbxGeometryElement::eAllSame)
		//		{
		//			DEBUG_LOG("ERROR: Mesh has multiple materials assigned to it, which currently is not supported. The mesh's first material was selected.");
		//		}
		//	}
		//}

		//for (int i = 0; i < node->GetChildCount(); i++)
		//{
		//	ImportFBXNodeIntoNode(node->GetChild(i), current_node, file_path, load_as_static);
		//}
	}


	//------------------------------------------------------------------------------------------------------
	bool FBXLoader::ImportFBXNode(FbxNode* node, Mesh::MeshData* mesh_data)
	{
		/*FbxNodeAttribute* attribute;
		int attributes_count = node->GetNodeAttributeCount();
		
		for (int i = 0; i < attributes_count; i++)
		{
			attribute = node->GetNodeAttributeByIndex(i);
			if (attribute->GetAttributeType() == FbxNodeAttribute::eMesh)
			{
				ImportFBXMesh(static_cast<FbxMesh*>(attribute), mesh_data);
				return true;
			}
		}
		
		for (int i = 0; i < node->GetChildCount(); i++)
		{
			if (ImportFBXNode(node->GetChild(i), mesh_data))
			{
				return false;
			}
		}
		
		return false;*/
		return false;
	}

	//------------------------------------------------------------------------------------------------------
	void FBXLoader::ImportFBXMesh(FbxMesh* mesh, Mesh::MeshData* mesh_data)
	{
//		mesh_data->topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
//
//		int vertex_id = 0;
//
//		int poly_count = mesh->GetPolygonCount();
//		const int poly_size = 3;
//		int control_points_count = mesh->GetControlPointsCount();
//		//Max value of unsigned short. Otherwise indices don't work
//		ASSERT(control_points_count < UINT16_MAX);
//		FbxVector4* control_points = mesh->GetControlPoints();
//		mesh->GenerateTangentsDataForAllUVSets();
//
//		FbxVector2 get_vector2;
//		FbxColor get_color;
//		FbxVector4 get_vector4;
//
//		Vertex vertex;
//
//		FbxGeometryElementVertexColor* vertex_color;
//		FbxGeometryElementUV* vertex_uv;
//		FbxGeometryElementNormal* vertex_normal;
//		FbxGeometryElementTangent* vertex_tangent;
//		FbxGeometryElementBinormal* vertex_binormal;
//
//		for (int curr_poly = 0; curr_poly < poly_count; curr_poly++)
//		{
//			for (int curr_vertex = 0; curr_vertex < poly_size; curr_vertex++, vertex_id++)
//			{
//				//Construct a vertex
//
//				////////////////POSITION/////////////////////////////////////////////////////////////////////////
//				int control_point_index = mesh->GetPolygonVertex(curr_poly, curr_vertex);
//				//vertex.position = control_points[control_point_index];
//				vertex.position.x = (float)control_points[control_point_index][0];
//				vertex.position.y = (float)control_points[control_point_index][1];
//				vertex.position.z = (float)control_points[control_point_index][2];
//
//				////////////////COLOR/////////////////////////////////////////////////////////////////////////
//
//				ASSERT(mesh->GetElementVertexColorCount() <= 1);
//				if (mesh->GetElementVertexColorCount() == 1)
//				{
//					vertex_color = mesh->GetElementVertexColor(0);
//
//					switch (vertex_color->GetMappingMode())
//					{
//					default:
//					{
//						break;
//					}
//					case FbxGeometryElement::eByControlPoint:
//					{
//						switch (vertex_color->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//							get_color = vertex_color->GetDirectArray().GetAt(control_point_index);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_color->GetIndexArray().GetAt(control_point_index);
//							get_color = vertex_color->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break;
//						}
//						break;
//					}
//
//					case FbxGeometryElement::eByPolygonVertex:
//					{
//						switch (vertex_color->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//							get_color = vertex_color->GetDirectArray().GetAt(vertex_id);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_color->GetIndexArray().GetAt(vertex_id);
//							get_color = vertex_color->GetDirectArray().GetAt(id);
//						}
//						break;
//
//						default:
//							break;
//						}
//					}
//
//					break;
//					}
//
//					vertex.color.x = (float)get_color[0];
//					vertex.color.y = (float)get_color[1];
//					vertex.color.z = (float)get_color[2];
//					vertex.color.w = (float)get_color[3];
//				}
//				////////////////UVS/////////////////////////////////////////////////////////////////////////
//				//multiple uvs per vertex not supported
//#ifdef _DEBUG
//				if (mesh->GetElementUVCount() > 1)
//				{
//					DLOG("Warning: Not supported mesh. Undefined behavior");
//				}
//#endif
//				if (mesh->GetElementUVCount() == 1)
//				{
//					vertex_uv = mesh->GetElementUV(0);
//
//					switch (vertex_uv->GetMappingMode())
//					{
//					default:
//						break;
//					case FbxGeometryElement::eByControlPoint:
//					{
//						switch (vertex_uv->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//						{
//							get_vector2 = vertex_uv->GetDirectArray().GetAt(control_point_index);
//						}
//						break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_uv->GetIndexArray().GetAt(control_point_index);
//							get_vector2 = vertex_uv->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//						{
//							break; // other reference modes not shown here!
//						}
//						}
//						break;
//					}
//					case FbxGeometryElement::eByPolygonVertex:
//					{
//						int texture_uv_index = mesh->GetTextureUVIndex(curr_poly, curr_vertex);
//						switch (vertex_uv->GetReferenceMode())
//						{
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							get_vector2 = vertex_uv->GetDirectArray().GetAt(texture_uv_index);
//						}
//						break;
//						}
//					}
//					break;
//					}
//
//					vertex.uv.x = 1 - (float)get_vector2[0];
//					vertex.uv.y = -(float)get_vector2[1];
//				}
//
//				////////////////NORMALS/////////////////////////////////////////////////////////////////////////
//				if( mesh->GetElementNormalCount() > 0)
//				{
//					vertex_normal = mesh->GetElementNormal(0);
//
//					switch (vertex_normal->GetMappingMode())
//					{
//					case FbxGeometryElement::eByControlPoint:
//					{
//						switch (vertex_normal->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//							get_vector4 = vertex_normal->GetDirectArray().GetAt(control_point_index);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_normal->GetIndexArray().GetAt(control_point_index);
//							get_vector4 = vertex_normal->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break; // other reference modes not shown here!
//						}
//						break;
//					}
//					case (FbxGeometryElement::eByPolygonVertex):
//					{
//						switch (vertex_normal->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//							get_vector4 = vertex_normal->GetDirectArray().GetAt(vertex_id);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_normal->GetIndexArray().GetAt(vertex_id);
//							get_vector4 = vertex_normal->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break; // other reference modes not shown here!
//						}
//					}
//					}
//					vertex.normal.x = (float)get_vector4[0];
//					vertex.normal.y = (float)get_vector4[1];
//					vertex.normal.z = (float)get_vector4[2];
//				}
//				////////////////TANGENTS/////////////////////////////////////////////////////////////////////////
//				ASSERT(mesh->GetElementTangentCount() <= 1);
//				if(mesh->GetElementTangentCount() > 0)
//				{
//					vertex_tangent = mesh->GetElementTangent(0);
//
//					switch (vertex_tangent->GetMappingMode())
//					{
//					case FbxGeometryElement::eByControlPoint:
//					{
//						switch (vertex_tangent->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//							get_vector4 = vertex_tangent->GetDirectArray().GetAt(control_point_index);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_tangent->GetIndexArray().GetAt(control_point_index);
//							get_vector4 = vertex_tangent->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break; // other reference modes not shown here!
//						}
//						break;
//					}
//					case (FbxGeometryElement::eByPolygonVertex):
//					{
//						switch (vertex_tangent->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//							get_vector4 = vertex_tangent->GetDirectArray().GetAt(vertex_id);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_tangent->GetIndexArray().GetAt(vertex_id);
//							get_vector4 = vertex_tangent->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break; // other reference modes not shown here!
//						}
//					}
//					}
//					vertex.tangent.x = (float)get_vector4[0];
//					vertex.tangent.y = (float)get_vector4[1];
//					vertex.tangent.z = (float)get_vector4[2];
//				}
//
//				////////////////BITANGENTS?BINORMALS/////////////////////////////////////////////////////////////////////////
//				if (mesh->GetElementBinormalCount() == 1)
//				{
//					vertex_binormal = mesh->GetElementBinormal(0);
//
//					switch (vertex_binormal->GetMappingMode())
//					{
//					case FbxGeometryElement::eByControlPoint:
//					{
//						switch (vertex_binormal->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//							get_vector4 = vertex_binormal->GetDirectArray().GetAt(control_point_index);
//							break;
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_binormal->GetIndexArray().GetAt(control_point_index);
//							get_vector4 = vertex_binormal->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//							break; // other reference modes not shown here!
//						}
//						break;
//					}
//					case (FbxGeometryElement::eByPolygonVertex):
//					{
//						switch (vertex_binormal->GetReferenceMode())
//						{
//						case FbxGeometryElement::eDirect:
//						{
//							get_vector4 = vertex_binormal->GetDirectArray().GetAt(vertex_id);
//							break;
//						}
//						case FbxGeometryElement::eIndexToDirect:
//						{
//							int id = vertex_binormal->GetIndexArray().GetAt(vertex_id);
//							get_vector4 = vertex_binormal->GetDirectArray().GetAt(id);
//						}
//						break;
//						default:
//						{
//							break; // other reference modes not shown here!
//						}
//						}
//					}
//					}
//					vertex.bitangent.x = (float)get_vector4[0];
//					vertex.bitangent.y = (float)get_vector4[1];
//					vertex.bitangent.z = (float)get_vector4[2];
//				}
//
//				//std::vector<Vertex>::iterator found_vertex = std::find((*mesh_data).vertices.begin(), (*mesh_data).vertices.end(), vertex);
//				//if (found_vertex == (*mesh_data).vertices.end())
//				//{
//				(*mesh_data).vertices.push_back(vertex);
//				(*mesh_data).indices.push_back((unsigned short)(*mesh_data).vertices.size() - 1);
//				//}
//				//else
//				//{
//				//	(*mesh_data).indices.push_back(found_vertex - (*mesh_data).vertices.begin());
//				//}
//			}
//		}
	}

	FbxFileTexture* GetFileTextureOfProperty(FbxProperty* fbx_property)
	{
		/*int nmb_textures = fbx_property->GetSrcObjectCount<FbxTexture>();
		ASSERT(nmb_textures <= 1);
		if (nmb_textures > 0)
		{
			FbxTexture* fbx_texture = fbx_property->GetSrcObject<FbxTexture>(0);
			if (fbx_texture)
			{
				return FbxCast<FbxFileTexture>(fbx_texture);
			}
		}
		return nullptr;*/
		return nullptr;
	}

	void FBXLoader::ImportFBXMaterial(FbxSurfaceMaterial* fbx_material, Material* out_material, ResourceManager* resource_manager, const std::string& file_path)
	{
		////Load colors for material
		//if (fbx_material->GetClassId().Is(FbxSurfacePhong::ClassId))
		//{
		//	FbxPropertyT<FbxDouble3> get_color;
		//	FbxPropertyT<FbxDouble> get_double;

		//	get_color = ((FbxSurfacePhong *)fbx_material)->Diffuse;
		//	out_material->diffuse = DirectX::XMFLOAT3((float)get_color.Get()[0], (float)get_color.Get()[1], (float)get_color.Get()[2]);

		//	get_color = ((FbxSurfacePhong *)fbx_material)->Specular;
		//	out_material->specular = DirectX::XMFLOAT3((float)get_color.Get()[0], (float)get_color.Get()[1], (float)get_color.Get()[2]);

		//	get_color = ((FbxSurfacePhong *)fbx_material)->Emissive;
		//	out_material->emissive = DirectX::XMFLOAT3((float)get_color.Get()[0], (float)get_color.Get()[1], (float)get_color.Get()[2]);

		//	get_double = ((FbxSurfacePhong *)fbx_material)->Shininess;
		//	out_material->shininess = (float)get_double.Get();

		//	get_color = ((FbxSurfacePhong *)fbx_material)->Ambient;
		//	out_material->ambient_reflectance = DirectX::XMFLOAT3((float)get_color.Get()[0], (float)get_color.Get()[1], (float)get_color.Get()[2]);
		//}
		//else if (fbx_material->GetClassId().Is(FbxSurfaceLambert::ClassId))
		//{
		//	FbxPropertyT<FbxDouble3> get_color;

		//	get_color = ((FbxSurfaceLambert *)fbx_material)->Diffuse;
		//	out_material->diffuse = DirectX::XMFLOAT3((float)get_color.Get()[0], (float)get_color.Get()[1], (float)get_color.Get()[2]);

		//	get_color = ((FbxSurfaceLambert *)fbx_material)->Emissive;
		//	out_material->emissive = DirectX::XMFLOAT3((float)get_color.Get()[0], (float)get_color.Get()[1], (float)get_color.Get()[2]);

		//	get_color = ((FbxSurfaceLambert *)fbx_material)->Ambient;
		//	out_material->ambient_reflectance = DirectX::XMFLOAT3((float)get_color.Get()[0], (float)get_color.Get()[1], (float)get_color.Get()[2]);

		//	//Set out_material shininess and specular color to 0, as this is lambert shading, and we don't have it, and just fake it with phong shading
		//	out_material->specular = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
		//	out_material->shininess = 0.0f;
		//}
		//else
		//{
		//	DLOG("ERROR:Shading model in loaded material unsupported");
		//}

		//FbxProperty fbx_property;
		//FbxFileTexture* texture;

		//fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sDiffuse);
		//texture = GetFileTextureOfProperty(&fbx_property);
		//if (texture)
		//{
		//	Texture* loaded_texture = resource_manager->GetTexture(texture->GetFileName());
		//	out_material->use_diffuse_map = true;
		//	out_material->diffuse_map = loaded_texture;
		//}

		//fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sEmissive);
		//texture = GetFileTextureOfProperty(&fbx_property);
		//if (texture)
		//{
		//	Texture* loaded_texture = resource_manager->GetTexture(texture->GetFileName());
		//	out_material->use_emissive_map = true;
		//	out_material->emissive_map = loaded_texture;
		//}

		//fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sAmbient);
		//texture = GetFileTextureOfProperty(&fbx_property);
		//if (texture)
		//{
		//	Texture* loaded_texture = resource_manager->GetTexture(texture->GetFileName());
		//	out_material->use_ambient_map = true;
		//	out_material->ambient_map = loaded_texture;
		//}

		//fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sNormalMap);
		//texture = GetFileTextureOfProperty(&fbx_property);
		//if (texture)
		//{
		//	Texture* loaded_texture = resource_manager->GetTexture(texture->GetFileName());
		//	out_material->use_normal_map = true;
		//	out_material->normal_map = loaded_texture;
		//}

		//fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sShininess);
		//texture = GetFileTextureOfProperty(&fbx_property);
		//if (texture)
		//{
		//	Texture* loaded_texture = resource_manager->GetTexture(texture->GetFileName());
		//	out_material->use_shininess_map = true;
		//	out_material->shininess_map = loaded_texture;
		//}

		//fbx_property = fbx_material->FindProperty(FbxSurfaceMaterial::sSpecular);
		//texture = GetFileTextureOfProperty(&fbx_property);
		//if (texture)
		//{
		//	Texture* loaded_texture = resource_manager->GetTexture(texture->GetFileName());
		//	out_material->use_specular_map = true;
		//	out_material->specular_map = loaded_texture;
		//}
	}
}