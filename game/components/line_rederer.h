#pragma once

namespace tremble
{
    class LineRenderer : public Component
    {
    public:
        void Start();

        void Update();

    private:

        static DirectX::XMFLOAT3 PxVec3ToXMFLOAT3(physx::PxVec3 vector);

        Model* model_ = nullptr;

        size_t mesh_count_;
        std::vector<std::vector<DirectX::XMFLOAT3>> vertices_;
        std::vector<std::vector<physx::PxU32>> indices_;
        std::vector<std::vector<DebugVolume>> lines_;

        int framecount_ = 0;
    };
}
