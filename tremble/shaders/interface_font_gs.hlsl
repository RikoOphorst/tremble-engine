#include <interface_font_buffers.hlsli>

[maxvertexcount(6)]
void main(point GSin input[1], inout TriangleStream<PSin> output)
{
    PSin vertex[4];
    vertex[0].spriteIndex = input[0].spriteIndex;
    vertex[0].uv = float2(0, 0);
    vertex[0].position = mul(float4(0, 0, 0, 1), mul(sprites[input[0].spriteIndex].TransformMatrix, mul(ProjectionMatrix, ViewMatrix)));

	vertex[1].spriteIndex = input[0].spriteIndex;
	vertex[1].uv = float2(1, 0);
	vertex[1].position = mul(float4(1, 0, 0, 1), mul(sprites[input[0].spriteIndex].TransformMatrix, mul(ProjectionMatrix, ViewMatrix)));

	vertex[2].spriteIndex = input[0].spriteIndex;
	vertex[2].uv = float2(1, 1);
	vertex[2].position = mul(float4(1, 1, 0, 1), mul(sprites[input[0].spriteIndex].TransformMatrix, mul(ProjectionMatrix, ViewMatrix)));

	vertex[3].spriteIndex = input[0].spriteIndex;
	vertex[3].uv = float2(0, 1);
	vertex[3].position = mul(float4(0, 1, 0, 1), mul(sprites[input[0].spriteIndex].TransformMatrix, mul(ProjectionMatrix, ViewMatrix)));

	output.Append(vertex[0]);
	output.Append(vertex[1]);
	output.Append(vertex[2]);

	output.RestartStrip();

	output.Append(vertex[2]);
	output.Append(vertex[3]);
	output.Append(vertex[0]);

	output.RestartStrip();
}