#include <interface_font_buffers.hlsli>

float4 main(PSin input) : SV_TARGET
{
    float2 uv = sprites[input.spriteIndex].uvMin + (input.uv * (sprites[input.spriteIndex].uvMax - sprites[input.spriteIndex].uvMin));
    return SpriteTexture.Sample(SpriteSampler, uv) * sprites[input.spriteIndex].Color;
}