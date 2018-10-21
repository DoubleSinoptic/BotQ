#pragma once

#include "Math/PointAndSize.h"

class SGE_EXPORT IWindow
{
public:
	virtual ~IWindow() {}

	void RunLoops(int tickrate, int framerate);

	virtual void OnDraw(){}
	virtual void OnUpdate(float delta) {}
	virtual bool OnProcess() { return false; }
	
	float pxTime;
	float rdTime;
	float perSecondPx;
	float perSecondRd;
};