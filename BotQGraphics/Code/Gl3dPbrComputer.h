#pragma once
#ifndef _Gl3dPbrComputer_h_
#define _Gl3dPbrComputer_h_

#include "Gl3dCore.h"

class Gl3dPbrComputerImpl;
class GL3DC_EXPORT Gl3dPbrComputer 
{
	Gl3dPbrComputerImpl* mImpl = nullptr;
public:
	Gl3dPbrComputer();

	class Gl3dTexture* IrradianceMap = nullptr;
	class Gl3dTexture* PrefilterMap = nullptr;
	class Gl3dTexture* BrdfLut = nullptr;
	class Gl3dTexture* EnvaromentSkybox = nullptr;

	void RenderMaps(class Gl3dTexture* hdrSphereMap);
	void RenderBrdfLut();

	static void CalculateSunLuma(float* vec);

	~Gl3dPbrComputer();
};

#endif