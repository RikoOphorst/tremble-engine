#include <interface_font_buffers.hlsli>

GSin main(VSin input)
{
	GSin output;
    output.spriteIndex = input.spriteIndex;
	return output;
}