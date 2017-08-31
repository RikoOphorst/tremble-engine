#include "cbuffers.hlsli"
#include "samplers.hlsli"
#include "vertex_data.hlsli"
#include "lighting.hlsli"
#include "shadow_mapping.hlsli"

Texture2D mat_emissive_map : register(t0);
Texture2D mat_ambient_map : register(t1);
Texture2D mat_diffuse_map : register(t2);
Texture2D mat_specular_map : register(t3);
Texture2D mat_shininess_map : register(t4);
Texture2D mat_normal_map : register(t5);

struct VertexOut
{
    float4 PosL : POS_LOCAL;
    float4 PosW : POS_WORLD;
    float4 PosH : SV_POSITION;
    float4 Color : COLOR;
    float3 Normal : NORMAL;
    float3 Bitangent : BITANGENT;
    float3 Tangent : TANGENT;
    float2 UV : UV;
};

VertexOut main(VertexIn vin)
{
    VertexOut vout;

    float4 pos = float4(vin.PosL, 1.0f);

    vout.PosH = mul(pos, gWorldViewProj);
    vout.PosW = mul(pos, gWorld);
    vout.PosL = pos;
    vout.Color = vin.Color;
    vout.UV = vin.UV;
    vout.Normal = mul(vin.Normal, (float3x3) gWorld);
    vout.Bitangent = mul(vin.Bitangent, (float3x3) gWorld);
    vout.Tangent = mul(vin.Tangent, (float3x3) gWorld);

    return vout;
}