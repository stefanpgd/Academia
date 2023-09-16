#include "Utilities.h"

void ClearBuffer(unsigned int* buffer, unsigned int color, unsigned int elementCount)
{
	memset(buffer, color, elementCount * sizeof(unsigned int));
}

unsigned int AlbedoToRGB(float r, float g, float b)
{
	return (int(b * 255.0f) << 16) + (int(g * 255.0f) << 8) + (int(r * 255.0f));
}