#pragma once

#include <Core.h>

class SSAO
{
public:
	static void Regenerate();

	static const DynamicArray<Vector3>& SSAOKernel();
	static const DynamicArray<Vector3>& SSAONoise();
};

