#pragma once
// Will contain all util files at some point.
// In case you need a specific util instead of all of them.
// Include from the 'Utilities' instead of using this file.

#include "LogHelper.h"
#include <iostream>

void ClearBuffer(unsigned int* buffer, unsigned int color, unsigned int elementCount);
unsigned int AlbedoToRGB(float r, float g, float b);

int Clamp(int v, int min, int max);
float Clamp(float v, float min, float max);

static unsigned int state = time(NULL);
inline unsigned int xorshift32()
{
	state ^= state << 13;
	state ^= state >> 17;
	state ^= state << 5;
	return state;
}

inline float Random01()
{
	return xorshift32() * 2.3283064365387e-10f;
}

inline float RandomInRange(float min, float max)
{
	return min + (max - min) * Random01();
}