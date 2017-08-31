#ifndef SHADOWBUFFERSHLSL
#define SHADOWBUFFERSHLSL

struct VSin
{
	float3 Pos : POSITION;
};

struct PSin
{
    float4 Pos : SV_POSITION;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 world;
    float4x4 world_view;
    float4x4 world_view_projection;
}

#endif