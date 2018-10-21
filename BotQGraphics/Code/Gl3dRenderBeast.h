#pragma once
#ifndef _Gl3dRenderBeast_h_
#define _Gl3dRenderBeast_h_

class Gl3dRenderBeastImpl;
class Gl3dRenderBeast
{
public:
	Gl3dRenderBeast();
	~Gl3dRenderBeast();
private:
	Gl3dRenderBeastImpl* mImpl = nullptr;
};

#endif