#include "Gl3dRenderBeast.h"
#include "glad.h"
class Gl3dRenderBeastImpl 
{
public:
	Gl3dRenderBeast* _this;
	Gl3dRenderBeastImpl(Gl3dRenderBeast* __this)
		: _this(__this)
	{
	
	
	}
	~Gl3dRenderBeastImpl()
	{


	}
};

Gl3dRenderBeast::Gl3dRenderBeast()
	: mImpl(new Gl3dRenderBeastImpl(this))
{
}


Gl3dRenderBeast::~Gl3dRenderBeast()
{
	if (mImpl)
		delete mImpl;
}
