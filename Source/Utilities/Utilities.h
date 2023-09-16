#pragma once
// Will contain all util files at some point.
// In case you need a specific util instead of all of them.
// Include from the 'Utilities' instead of using this file.

#include "LogHelper.h"
#include <iostream>

void ClearBuffer(unsigned int* buffer, unsigned int color, unsigned int elementCount);
unsigned int AlbedoToRGB(float r, float g, float b);