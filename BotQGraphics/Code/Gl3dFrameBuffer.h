#pragma once

#include "Gl3dCore.h"
#include "Gl3dTexture.h"

class GL3DC_EXPORT Gl3dFrameBufferBase 
{
public:
	virtual ~Gl3dFrameBufferBase() {}
	virtual unsigned int GetObject() const = 0;
};

struct Gl3dFrameBufferDesc 
{
	int				depthLevel;
	Gl3dSide		depthSide;
	Gl3dTexture*	depthAttachment;

	Gl3dTexture*	colorAttachments[24];
	int				colorLevels[24];
	Gl3dSide		colorSide[24];
};

class GL3DC_EXPORT Gl3dFrameBufferInstance : public Gl3dFrameBufferBase
{
	unsigned int m_object;
public:
	Gl3dFrameBufferInstance();
	~Gl3dFrameBufferInstance();
	void Create(Gl3dFrameBufferDesc* desc);
	virtual unsigned int GetObject() const override;
};

