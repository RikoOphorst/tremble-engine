#include "line_rederer.h"

namespace tremble
{

    void LineRenderer::Start()
    {
        Renderable* renderable = GetNode()->FindComponentInChildren<Renderable>();
        if (renderable != nullptr)
        {
            renderable->SetActive(false);
            model_ = renderable->GetModel();

            mesh_count_ = model_->GetMeshes().size();
            vertices_.resize(mesh_count_);
            indices_.resize(mesh_count_);
            lines_.resize(mesh_count_);

            for (int i = 0; i < mesh_count_; ++i)
            {
                auto vertices = PhysicsTriangleMeshGeometry::GetVertices(model_->GetMeshes()[i]);
                vertices_[i].resize(vertices.size());
                for (int j = 0; j < vertices.size(); ++j)
                {
                    vertices_[i][j] = DirectX::XMFLOAT3(
                        vertices[j].x,
                        vertices[j].y,
                        vertices[j].z
                    );
                }
                indices_[i] = PhysicsTriangleMeshGeometry::GetIndices(model_->GetMeshes()[i]);
            }

            for (int i = 0; i < mesh_count_; ++i)
            {
                lines_[i].resize(indices_[i].size());
                for (int j = 0; j < indices_[i].size(); j += 3)
                {
                    DirectX::XMFLOAT3 pointA = vertices_[i][indices_[i][j]];
                    DirectX::XMFLOAT3 pointB = vertices_[i][indices_[i][j + 1]];
                    DirectX::XMFLOAT3 pointC = vertices_[i][indices_[i][j + 2]];
                    lines_[i][j] = DebugVolume::CreateLine(pointA, pointB);
                    lines_[i][j + 1] = DebugVolume::CreateLine(pointB, pointC);
                    lines_[i][j + 2] = DebugVolume::CreateLine(pointC, pointA);
                }
            }
        }
    }

    void LineRenderer::Update()
    {
        if (lines_.size() == 0)
            return;

        for (int i = 0; i < lines_.size(); ++i)
        {
            for (int j = 0; j < lines_[i].size(); ++j)
            {
                Get::Renderer()->RenderDebugVolume(lines_[i][j]);
            }
        }
        framecount_++;
    }

    DirectX::XMFLOAT3 LineRenderer::PxVec3ToXMFLOAT3(physx::PxVec3 vector)
    {
        return DirectX::XMFLOAT3(
            vector.x,
            vector.y,
            vector.z
        );
    }
}
