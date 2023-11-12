#pragma once
#include "Math/Vec3.h"

class PostProcessor
{
public:
	PostProcessor(unsigned int screenWidth, unsigned int screenHeight);

	void PostProcess(vec3* sampleBuffer, int sampleCount);
	void CopyProcessedData(unsigned int* screenBuffer);

	void Resize(unsigned int screenWidth, unsigned int screenHeight);

private:
	void ApplyGaussianFilter(int screenWidth, int screenHeight);
	void GenerateGaussianFilter();

private:
	vec3* postProcessBuffer;
	vec3* postProcessBackBuffer;
	unsigned int* processedBuffer;

	unsigned int screenWidth, screenHeight;

	bool doACESTonemapping = true;
	bool doGaussianFilter = false;

	float gamma = 2.4f;
	float gammaInverse = 1.0f;
	float exposure = 0.545f;

	float GaussianFilter[7][7];
	float gaussianSigma = 1.0f;

	friend class Editor;
};