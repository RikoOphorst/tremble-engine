#ifndef INTERFACEBUFFERSHLSL
#define INTERFACEBUFFERSHLSL

cbuffer SceneData : register(b0) 
{
	float4x4 ProjectionMatrix;
	float4x4 ViewMatrix;
    float AspcectRatio;
    int textured;
};

struct PSin
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
    uint particleIndex : BLENDINDICES0;
};

struct GSin
{
    uint particleIndex : BLENDINDICES0;
};

struct VSin
{
    uint index : SV_VertexID;
};

struct ParticleData
{
    float4 Color;
    float3 Position;
    float Size;
};

StructuredBuffer<ParticleData> particles : register(t0);
Texture2D ParticleTexture : register(t1);
SamplerState ParticleSampler: register(s0);

#endif