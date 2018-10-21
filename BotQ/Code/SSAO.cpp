#include "SSAO.h"
#include <random>

std::uniform_real_distribution<float> randomFloats(0.0, 1.0);
std::default_random_engine generator;

DynamicArray<Vector3> ssaoKernel;
DynamicArray<Vector3> ssaoNoise;

float lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

void SSAO::Regenerate()
{
	for (unsigned int i = 0; i < 32; ++i)
	{
		Vector3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
		sample.Normalize();
		sample *= randomFloats(generator);
		float scale = float(i) / 32;

		scale = lerp(0.1f, 1.0f, scale * scale);
		sample *= scale;
		ssaoKernel.Add(sample);
	}


	for (unsigned int i = 0; i < 16; i++)
	{
		Vector3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); 
		ssaoNoise.Add(noise);
	}
}

class _RegenInit 
{
public:
	_RegenInit() 
	{
		SSAO::Regenerate();
	}
	
} _RegenInitObject;


const DynamicArray<Vector3>& SSAO::SSAOKernel()
{
	return ssaoKernel;
}

const DynamicArray<Vector3>& SSAO::SSAONoise()
{
	return ssaoNoise;
}
