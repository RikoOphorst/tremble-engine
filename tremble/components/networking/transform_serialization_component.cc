#include "transform_serialization_component.h"
#include "../core/get.h"
#include "../core/scene_graph/scene_graph.h"
#include "../core/networking/network_manager.h"
//#include "../core/networking/packet_factory.h"
#include "../components/rendering/camera.h"
#include "../core/networking/packet_identifiers.h"
#include "../core/networking/packet_handler.h"

using namespace tremble;

void TransformSerializationComponent::Awake()
{
    camera_node_ = GetNode()->FindComponentInChildren<Camera>()->GetNode();
}

void TransformSerializationComponent::Update()
{
    //save previous positions in a circular fashion, and calculate average distance between positions
    previous_positions_[last_previous_data_] = GetNode()->GetLocalPosition();
    previous_rot_x[last_previous_data_] = camera_node_->GetRotationRadians().GetX();
    previous_rot_y[last_previous_data_] = GetNode()->GetRotationRadians().GetY();

    int previous_pos = last_previous_data_ - 1;
    if (previous_pos < 0)
    {
        previous_pos = prev_data_size_ - 1;
    }
    avg_distance = avg_distance * 0.90f + (previous_positions_[last_previous_data_] - previous_positions_[previous_pos]).LengthEst() * Scalar(0.1f);
    avg_x_rot = avg_x_rot * 0.90f + Scalar(previous_rot_x[last_previous_data_] - previous_rot_x[previous_pos]).Abs() * Scalar(0.1f);
    avg_y_rot = avg_y_rot * 0.90f + Scalar(previous_rot_y[last_previous_data_] - previous_rot_y[previous_pos]).Abs() * Scalar(0.1f);

    last_previous_data_++;
    if (last_previous_data_ >= prev_data_size_)
    {
        last_previous_data_ = 0;
    }

}

void TransformSerializationComponent::Serialize(RakNet::BitStream& packet)
{
    if (Get::NetworkManager()->IsHost())
    {
        const float rotation_y = GetNode()->GetLocalRotationRadians().GetY();
        float rotation_x = camera_node_->GetLocalRotationRadians().GetX();
        const Vector3& position = GetNode()->GetLocalPosition();
        packet.Write(
            TransformSerializationPacket
        {
            position.GetX().Get(),
            position.GetY().Get(),
            position.GetZ().Get(),
            rotation_y,
            rotation_x
        }
        );
        if (Get::NetworkManager()->IsHost())
        {
            //std::cout << "host ";
        }
        //else
        //{
        //    std::cout<< "client ";
        //}
        //std::cout << "sending position: " << position.ToString() << " r_y: "<< rotation_y << " r_x: " << rotation_x << std::endl;
    }
}

void TransformSerializationComponent::Deserialize(RakNet::BitStream& packet)
{
    TransformSerializationPacket serialization_data = IPacketHandler::ReadPacketStruct<TransformSerializationPacket>(&packet);

    Vector3 previous_pos = GetNode()->GetLocalPosition();
    Vector3 previous_camera_rotation = camera_node_->GetLocalRotationRadians();

    float previous_y_rot = GetNode()->GetLocalRotationRadians().GetY();
    float previous_x_rot = previous_camera_rotation.GetX();

    Vector3 received_pos(serialization_data.x_pos, serialization_data.y_pos, serialization_data.z_pos);

    //std::cout << "recieved position: " << serialization_data.x_pos << " " << serialization_data.y_pos << " " << serialization_data.z_pos << " " << serialization_data.y_rot << " " << serialization_data.camera_x_rot;

    if (is_me_)
    {
        for each (Vector3 prev_pos in previous_positions_)
        {
            if ((prev_pos - received_pos).LengthEst() < avg_distance)
            {
                goto skip_repositioning;
            }
        }
    }

    GetNode()->SetLocalPosition(Vector3(serialization_data.x_pos, serialization_data.y_pos, serialization_data.z_pos));

skip_repositioning:

    if (is_me_)
    {
        for each (float prev_rot_x in previous_rot_x)
        {
            if (Scalar(prev_rot_x - serialization_data.camera_x_rot).Abs() < avg_distance)
            {
                goto skip_rot_x;
            }
        }
    }

    camera_node_->SetLocalRotationRadians(Vector3(serialization_data.camera_x_rot, 0.0f, 0.0f));

skip_rot_x:

    if (is_me_)
    {
        for each (float prev_rot_y in previous_rot_y)
        {
            if (Scalar(prev_rot_y - serialization_data.y_rot).Abs() < avg_distance)
            {
                return;
            }
        }
    }

    GetNode()->SetLocalRotationRadians(Vector3(0, serialization_data.y_rot, 0));
}

void TransformSerializationComponent::Shutdown()
{

}
