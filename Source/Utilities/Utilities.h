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

inline float Random01()
{
	return float(rand() / (RAND_MAX + 1.0));
}

inline float RandomInRange(float min, float max)
{
	return min + (max - min) * Random01();
}