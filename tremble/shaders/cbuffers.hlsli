#include "lights.hlsli"

#ifndef CBUFFERSHLSL
#define CBUFFERSHLSL

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gWorldView;
    float4x4 gWorldViewProj;
}

cbuffer cbPerPass : register(b1)
{
    float total_time;
    float delta_time;
    float near_z;
    float far_z;
    float4x4 view;
    float4x4 inv_view;
    float4x4 projection;
    float4x4 inv_projection;
    float4x4 view_projection;
    float4x4 inv_view_projection;
    float3 eye_pos_w;
    float pass_padding;
    float2 render_target_size;
    float2 inv_render_target_size;
}

struct Light
{
    float4 PositionWS; //-- 16 bytes
    float4 DirectionWS; //-- 16 bytes
    float4 PositionVS; //-- 16 bytes
    float4 DirectionVS; //-- 16 bytes
    float4 Color; //-- 16 bytes
    float SpotlightAngle;
    float Range;
    float Intensity;
    bool Enabled; //-- 16 bytes
    bool Selected;
    uint Type;
    int ShadowIndex;
    int ShadowRange; //-- 16 bytes
    //-- 16 * 8 = 112 bytes
};

StructuredBuffer<Light> lights : register(t8);
StructuredBuffer<float4x4> bones : register(t11);

struct Material
{
    float4 GlobalAmbient; //-- 16 bytes
    float4 AmbientColor; //-- 16 bytes
    float4 EmissiveColor; //-- 16 bytes
    float4 DiffuseColor; //-- 16 bytes
    float4 SpecularColor; //-- 16 bytes
    float4 Reflectance; //-- 16 bytes
    float Opacity;
    float SpecularPower;
    float IndexOfRefraction;
    bool HasAmbientTexture; //-- 16 bytes
    bool HasEmissiveTexture;
    bool HasDiffuseTexture;
    bool HasSpecularTexture;
    bool HasSpecularPowerTexture; //-- 16 bytes
    bool HasNormalTexture;
    bool HasBumpTexture;
    bool HasOpacityTexture;
    float BumpIntensity; //-- 16 bytes
    float SpecularScale;
    float AlphaThreshold;
    float2 Padding; //-- 16 bytes
    //-- 16 * 10 = 160 bytes
};

cbuffer cbPerMaterial : register(b2)
{
    Material material;
};

#endif