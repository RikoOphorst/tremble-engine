#include <interface_sprite_buffers.hlsli>

PSin main(VSin input)
{
	PSin output;
    output.position = mul(input.position, mul(TransformMatrix, mul(ProjectionMatrix, ViewMatrix)));
	output.uv = input.uv;
	return output;
}