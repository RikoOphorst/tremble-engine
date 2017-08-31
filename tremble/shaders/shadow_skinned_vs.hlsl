#include "shadow_buffers.hlsli"
#include "vertex_data.hlsli"

StructuredBuffer<float4x4> bones : register(t0);

float4 main(VertexIn vin) : SV_POSITION
{
    uint bone_id0 = (vin.BoneID >> 0) & 0xFF;
    uint bone_id1 = (vin.BoneID >> 8) & 0xFF;
    uint bone_id2 = (vin.BoneID >> 16) & 0xFF;
    uint bone_id3 = (vin.BoneID >> 24) & 0xFF;
    
    float4x4 bone_transform =   mul(bones[bone_id0], vin.BoneWeights[0]);
    bone_transform +=           mul(bones[bone_id1], vin.BoneWeights[1]);
    bone_transform +=           mul(bones[bone_id2], vin.BoneWeights[2]);
    bone_transform +=           mul(bones[bone_id3], vin.BoneWeights[3]);

    float4 pos = mul(bone_transform, float4(vin.PosL, 1.0f));

    return mul(pos, world_view_projection);
}