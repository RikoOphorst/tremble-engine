#pragma once

#include "../../core/scene_graph/component.h"
#include "../../core/networking/serializable.h"

namespace tremble
{
    const int prev_data_size_ = 10;

	class TransformSerializationComponent : public Component, public Serializable
	{
	public:
		void Awake(); //!< Creates the serialization component
		void Update();

		void Shutdown() override;


		void Serialize(RakNet::BitStream& packet) override;
		void Deserialize(RakNet::BitStream& packet) override;
        void SetIsMe(bool is_me) { is_me_ = is_me; }
	private:
        float avg_distance = 0.5f;
        float avg_x_rot = 0.0f;
        float avg_y_rot = 0.0f;
        int last_previous_data_ = 0;
        Vector3 previous_positions_[prev_data_size_]; //Stored previous positions
        float previous_rot_y[prev_data_size_];
        float previous_rot_x[prev_data_size_];
        SGNode* camera_node_;
        bool is_me_ = false; //!< Quick hack to indicate that it's component for a peer on this machine
	};
}