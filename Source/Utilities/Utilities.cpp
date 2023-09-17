#include "Utilities.h"

void ClearBuffer(unsigned int* buffer, unsigned int color, unsigned int elementCount)
{
	memset(buffer, color, elementCount * sizeof(unsigned int));
}

unsigned int AlbedoToRGB(float r, float g, float b)
{
#if _DEBUG
	if (r > 1.0f || r < 0.0f)
	{
		LOG(Log::MessageType::Error, "Red channel exceeds [0-1] range!")
	}

	if (g > 1.0f || g < 0.0f)
	{
		LOG(Log::MessageType::Error, "Green channel exceeds [0-1] range!")
	}

	if (b > 1.0f || b < 0.0f)
	{
		LOG(Log::MessageType::Error, "Blue channel exceeds [0-1] range!")
	}
#endif

	return (int(b * 255.0f) << 16) + (int(g * 255.0f) << 8) + (int(r * 255.0f));;
}