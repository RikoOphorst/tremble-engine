#include "cbuffers.hlsli"
#include "vertex_data.hlsli"

struct VertexOut
{
	float4 PosH : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut VS(VertexIn vin)
{
	VertexOut vout;

	vout.PosH = mul(float4(vin.PosL, 1.0f), view_projection); // debug mesh local positions are already in world space, so L*V*P instead of L*W*V*P
	vout.Color = vin.Color;

	return vout;
}

float4 main(VertexOut pin) : SV_TARGET
{
	return pin.Color;
}