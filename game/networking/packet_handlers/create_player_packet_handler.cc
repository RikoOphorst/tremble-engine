#include "create_player_packet_handler.h"
#include "game/networking/game_packet_defs.h"
#include "game/components/cube_movement.h"
#include "networking_demo.h"

using namespace tremble;

CreatePlayerPacketHandler::CreatePlayerPacketHandler()
{
}

CreatePlayerPacketHandler::~CreatePlayerPacketHandler()
{
}

void CreatePlayerPacketHandler::Handle(int packet_id, RakNet::Packet * packet, Peer * sender)
{
    //RakNet::BitStream* bs = IPacketHandler::GetPacketData(packet); // Extract packet data as RakNet::BitStream.
    //PlayerCreationPacketData packet_data = IPacketHandler::ReadPacketStruct<PlayerCreationPacketData>(bs);

    //NetworkingDemo::GetNetworkingDemo().CreatePlayerAsClient(packet_data);

    //delete bs;
    /*

    SGNode* player = Get::Scene()->AddChild(false, Vector3(packet_struct.spawn_x, packet_struct.spawn_y, packet_struct.spawn_z));

    Renderable* renderable = player->AddComponent<Renderable>();
    Model* model = Get::ResourceManager()->GetModel("heavy_gun/heavy_gun.obj");
    renderable->SetModel(model);

    TransformSerializationComponent* serializer = player->AddComponent<TransformSerializationComponent>();
    CubeMovement* cubeMovement = player->AddComponent<CubeMovement>();

    // Add light.
    SGNode* light_node = player->AddChild(true, Vector3(0, 0, -1.0f));
    Light* point_light = light_node->AddComponent<Light>();
    point_light->SetDirection(Vector3(0, 0, 1));
    point_light->SetLightType(LightTypePoint);
    point_light->SetColor(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    point_light->SetFalloffEnd(Scalar(50.0f));*/
}
