#ifndef INTERFACEBUFFERSHLSL
#define INTERFACEBUFFERSHLSL

cbuffer ViewData : register(b0) 
{
	float4x4 ProjectionMatrix;
	float4x4 ViewMatrix;
};

struct PSin
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
    uint spriteIndex : BLENDINDICES0;
};

struct GSin
{
    uint spriteIndex : BLENDINDICES0;
};

struct VSin
{
    uint spriteIndex : BLENDINDICES0;
};

struct SpriteData
{
    float4x4 TransformMatrix;
    float4 Color;
    float2 uvMin;
    float2 uvMax;
};

StructuredBuffer<SpriteData> sprites : register(t0);
Texture2D SpriteTexture : register(t1);
SamplerState SpriteSampler : register(s0);

#endif