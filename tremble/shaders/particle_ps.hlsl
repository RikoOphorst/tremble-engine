#include <particle_buffers.hlsli>

float4 main(PSin input) : SV_TARGET
{
    float4 color = particles[input.particleIndex].Color;
    if (textured) color *= ParticleTexture.Sample(ParticleSampler, input.uv);
    clip(color.a - 0.0001f);
    return color;
}