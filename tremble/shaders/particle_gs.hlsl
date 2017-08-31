#include <particle_buffers.hlsli>

[maxvertexcount(6)]
void main(point GSin input[1], inout TriangleStream<PSin> output)
{
    float4 position = mul(float4(particles[input[0].particleIndex].Position, 1), mul(ViewMatrix, ProjectionMatrix));

    float size = particles[input[0].particleIndex].Size;

    PSin vertex[4];
    vertex[0].particleIndex = input[0].particleIndex;
    vertex[0].uv = float2(0, 0);
    vertex[0].position = position + float4(-.5f, -.5f * AspcectRatio, 0, 0) * size;

    vertex[1].particleIndex = input[0].particleIndex;
	vertex[1].uv = float2(1, 0);
    vertex[1].position = position + float4(.5f, -.5f * AspcectRatio, 0, 0) * size;

    vertex[2].particleIndex = input[0].particleIndex;
	vertex[2].uv = float2(1, 1);
    vertex[2].position = position + float4(.5f, .5f * AspcectRatio, 0, 0) * size;

    vertex[3].particleIndex = input[0].particleIndex;
	vertex[3].uv = float2(0, 1);
    vertex[3].position = position + float4(-.5f, .5f * AspcectRatio, 0, 0) * size;

	output.Append(vertex[0]);
	output.Append(vertex[1]);
	output.Append(vertex[2]);

	output.RestartStrip();

	output.Append(vertex[2]);
	output.Append(vertex[3]);
	output.Append(vertex[0]);

	output.RestartStrip();
}