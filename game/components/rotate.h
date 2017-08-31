#pragma once

namespace tremble
{
    class Rotate : public Component
    {
    public:
        void Awake(float x, float y, float z)
        {
            rotation_x = x;
            rotation_y = y;
            rotation_z = z;
        }

        float rotation_x = 0.0f;
        float rotation_y = 0.0f;
        float rotation_z = 0.0f;
        void Update()
        {
            GetNode()->RotateXRadians(Scalar(rotation_x * Get::DeltaT()));
            GetNode()->RotateYRadians(Scalar(rotation_y * Get::DeltaT()));
            GetNode()->RotateZRadians(Scalar(rotation_z * Get::DeltaT()));
        }
    };

    class Rotate2 : public Component
    {
    public:
        void Awake(float x, float y, float z)
        {
            rotation_x = x;
            rotation_y = y;
            rotation_z = z;
        }

        float rotation_x = 0.0f;
        float rotation_y = 0.0f;
        float rotation_z = 0.0f;
        void Update()
        {
            GetNode()->RotateRadians(Vector3(rotation_x * Get::DeltaT(), rotation_y * Get::DeltaT(), rotation_z * Get::DeltaT()));
        }
    };
}