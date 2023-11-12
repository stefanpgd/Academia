#include "PostProcessor.h"
#include <cmath>
#include "Utilities/Utilities.h"
#include "Math/MathCommon.h"

PostProcessor::PostProcessor(unsigned int screenWidth, unsigned int screenHeight) :
	screenWidth(screenWidth), screenHeight(screenHeight)
{
	postProcessBuffer = new vec3[screenWidth * screenHeight];
	postProcessBackBuffer = new vec3[screenWidth * screenHeight];
	processedBuffer = new unsigned int[screenWidth * screenHeight];

	gammaInverse = 1.0f / gamma;

	GenerateGaussianFilter();
}

void PostProcessor::PostProcess(vec3* sampleBuffer, int sampleCount)
{
	float sampleINV = (1.0f / (float)sampleCount);
	for(int x = 0; x < screenWidth; x++)
	{
		for(int y = 0; y < screenHeight; y++)
		{
			int i = x + y * screenWidth;
			vec3 color = sampleBuffer[i] * sampleINV; // average out all samples taken

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

			postProcessBuffer[i] = color;
		}
	}

	if(doGaussianFilter)
	{
		ApplyGaussianFilter(screenWidth, screenHeight);
	}
}

void PostProcessor::CopyProcessedData(unsigned int* screenBuffer)
{
	for(int i = 0; i < screenWidth * screenHeight; i++)
	{
		screenBuffer[i] = AlbedoToRGB(postProcessBuffer[i].x, postProcessBuffer[i].y, postProcessBuffer[i].z);
	}
}

void PostProcessor::Resize(unsigned int screenWidth, unsigned int screenHeight)
{
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	postProcessBuffer = new vec3[screenWidth * screenHeight];
	postProcessBackBuffer = new vec3[screenWidth * screenHeight];
	processedBuffer = new unsigned int[screenWidth * screenHeight];
}

void PostProcessor::ApplyGaussianFilter(int screenWidth, int screenHeight)
{
	if(!doGaussianFilter)
	{
		return;
	}

	for(int x = 0; x < screenWidth; x++)
	{
		for(int y = 0; y < screenHeight; y++)
		{
			vec3 sum;

			for(int i = -3; i <= 3; i++)
			{
				for(int j = -3; j <= 3; j++)
				{
					int xIndex = Clamp(x + i, 0, screenWidth - 1);
					int yIndex = Clamp(y + j, 0, screenHeight - 1);
					int index = xIndex + yIndex * screenWidth;

					sum += postProcessBuffer[index] * GaussianFilter[i + 3][j + 3];
				}
			}

			postProcessBackBuffer[x + y * screenWidth] = sum;
		}
	}

	memcpy(postProcessBuffer, postProcessBackBuffer, sizeof(vec3) * screenWidth * screenHeight);
}

void PostProcessor::GenerateGaussianFilter()
{
	float r, s = 2.0f * gaussianSigma * gaussianSigma;
	float sum = 0.0f;

	for(int x = -3; x <= 3; x++)
	{
		for(int y = -3; y <= 3; y++)
		{
			r = sqrtf(x * x + y * y);
			GaussianFilter[x + 3][y + 3] = (expf(-(r * r) / s)) / (PI * s);
			sum += GaussianFilter[x + 3][y + 3];
		}
	}

	float ss = 0.0f;
	for(int i = 0; i < 7; i++)
	{
		for(int j = 0; j < 7; j++)
		{
			GaussianFilter[i][j] /= sum;
			ss += GaussianFilter[i][j];
		}
	}
}
