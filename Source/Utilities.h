#pragma once
// Will contain all util files at some point.
// In case you need a specific util instead of all of them.
// Include from the 'Utilities' instead of using this file.

#include "Utilities\LogHelper.h"
#include <iostream>

void ClearBuffer(unsigned int* buffer, unsigned int color, unsigned int elementCount)
{
	memset(buffer, color, elementCount * sizeof(unsigned int));
}

void GLFWErrorCallback(int, const char* err_str)
{
	LOG(Log::MessageType::Error, err_str);
}