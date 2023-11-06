#include "PostProcessor.h"
#include <cmath>
#include "Utilities/Utilities.h"

PostProcessor::PostProcessor()
{
	gammaInverse = 1.0f / gamma;
}

unsigned int PostProcessor::PostProcess(vec3& color)
{
	if(exposure != 1.0f)
	{
		color = color * exposure;
	}

	if(doACESTonemapping)
	{
		float a = 2.51f;
		float b = 0.03f;
		float c = 2.43f;
		float d = 0.59f;
		float e = 0.14f;
		color.x = (color.x * (a * color.x + b)) / (color.x * (c * color.x + d) + e);
		color.y = (color.y * (a * color.y + b)) / (color.y * (c * color.y + d) + e);
		color.z = (color.z * (a * color.z + b)) / (color.z * (c * color.z + d) + e);
	}

	color.x = pow(Clamp(color.x, 0.0f, 1.0f), gammaInverse);
	color.y = pow(Clamp(color.y, 0.0f, 1.0f), gammaInverse);
	color.z = pow(Clamp(color.z, 0.0f, 1.0f), gammaInverse);

	// ACES 
	// Other filters (Luminance), enc.

	return AlbedoToRGB(color.x, color.y, color.z);
}
