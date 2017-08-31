#include <particle_buffers.hlsli>

GSin main(VSin input)
{
	GSin output;
    output.particleIndex = input.index;
	return output;
}