#include "shadow_buffers.hlsli"

float4 main(float3 input : POSITION) : SV_POSITION
{
    return mul(float4(input, 1), world_view_projection);
}