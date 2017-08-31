#ifndef INTERFACEBUFFERSHLSL
#define INTERFACEBUFFERSHLSL

cbuffer ViewData : register(b0) 
{
	float4x4 ProjectionMatrix;
	float4x4 ViewMatrix;
};

cbuffer SpriteData : register(b1)
{
	float4x4 TransformMatrix;
    float4 Color;
    float2 uvMin;
    float2 uvMax;
};

struct PSin
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

struct VSin
{
	float4 position : POSITION;
	float2 uv : TEXCOORD;
};

SamplerState SpriteSampler : register(s0);

Texture2D SpriteTexture : register(t0);

#endif