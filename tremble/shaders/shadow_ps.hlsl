#include "shadow_buffers.hlsli"

float4 main(PSin input) : SV_TARGET
{
    return float4(input.Pos.z, 0, 0, 1);
}