#include "PostProcessor.h"
#include <cmath>
#include "Utilities/Utilities.h"

PostProcessor::PostProcessor()
{
	gammaInverse = 1.0f / gamma;
}

unsigned int PostProcessor::PostProcess(vec3& color)
{
	color.x = pow(Clamp(color.x, 0.0f, 1.0f), gammaInverse);
	color.y = pow(Clamp(color.y, 0.0f, 1.0f), gammaInverse);
	color.z = pow(Clamp(color.z, 0.0f, 1.0f), gammaInverse);

	// ACES 
	// Other filters (Luminance), enc.

	return AlbedoToRGB(color.x, color.y, color.z);
}
