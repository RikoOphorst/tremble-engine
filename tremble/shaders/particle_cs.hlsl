
cbuffer SceneData
{
    float DeltaTime;
};

struct Particle
{
    float3 Postition;
    float Age;
    float3 Velocity;
    float Lifetime;    
};

struct Renderable
{
    float4 Color;
    float3 Position;
    float Size;
};

RWStructuredBuffer<Particle> Particles : register(u0);
RWStructuredBuffer<Renderable> Renderables : register(u1);

[numthreads(1, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    int index = DTid.x;

    Particles[index].Age += DeltaTime;
    float life = Particles[index].Age / Particles[index].Lifetime;
}