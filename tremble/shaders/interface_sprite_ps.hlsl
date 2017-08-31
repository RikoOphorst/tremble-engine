#include <interface_sprite_buffers.hlsli>

float4 main(PSin input) : SV_TARGET
{
    float2 uv = uvMin + (input.uv * (uvMax - uvMin));
	return SpriteTexture.Sample(SpriteSampler, uv) * Color;
}