#pragma once

#include "Gl3dCore.h"
#include "Gl3dTexture.h"

struct FraembufferSettings
{
	FraembufferSettings() {}
	FraembufferSettings(int c)
		: colors(c) {}
	FraembufferSettings(int c, int d)
		: colors(c), depths(d) {}
	FraembufferSettings(int c, int d, PixelFormat cf)
		: colors(c), depths(d), ColorFormat(cf) {}
	FraembufferSettings(int c, int d, PixelFormat cf, PixelFormat df)
		: colors(c), depths(d), ColorFormat(cf), DepthFormat(df) {}

	int colors = 0;
	int depths = 0;
	PixelFormat ColorFormat = PixelFormat::RGBA_16F;	
	PixelFormat DepthFormat = PixelFormat::DEPTH_16;
};

class GL3DC_EXPORT Gl3dFrameBufferBase 
{
public:
	virtual ~Gl3dFrameBufferBase() {}
	virtual unsigned int GetObject() const = 0;
};

enum class Gl3dSide : unsigned int
{ 
	Back = 0,
	PosX = 1,
	NegX = 2,
	PosY = 3,
	NegY = 4,
	PosZ = 5,
	NegZ = 6,
};

struct Gl3dFrameBufferDesc 
{
	int				depthLevel;
	int				depthSide;
	Gl3dTexture*	depthAttachment;

	Gl3dTexture*	colorAttachment[24];
	int				colorLevels[24];
	Gl3dSide		colorSide[24];
};

class GL3DC_EXPORT Gl3dFrameBufferExt : public Gl3dFrameBufferBase
{
	unsigned int m_object;
public:
	Gl3dFrameBufferExt();
	~Gl3dFrameBufferExt();
	void Create(Gl3dFrameBufferDesc* desc);
	virtual unsigned int GetObject() const override;
};

class Gl3dTexture;
class Gl3dFrameBufferImpl;
class GL3DC_EXPORT Gl3dFrameBuffer : public Gl3dFrameBufferBase
{
	Gl3dFrameBufferImpl* mImpl = nullptr;
public:
	Gl3dFrameBuffer(int width, int height,
		std::initializer_list<PixelFormat> colorFormats,
		std::initializer_list<PixelFormat> depthFormats = {},
		bool useMultisampling = false
	);

	Gl3dFrameBuffer(int width, int height, const FraembufferSettings& settings = FraembufferSettings());
	~Gl3dFrameBuffer();
	Gl3dTexture* GetColorTexture(int level) const;
	Gl3dTexture* GetDepthTexture(int level) const;
	void Blit(Gl3dFrameBuffer* frameBuffer, int x0, int y0, int x, int y, const FraembufferSettings& settings) const;
	unsigned int GetObject() const override;
};

enum class Gl3dAttachmentColor 
{
	Depth = -1,
	Color0 = 0,
	Color1 = 1,
	Color2 = 2,
	Color3 = 3,
	Color4 = 4,
	Color5 = 5,
	Color6 = 6,
	Color7 = 7,
	Color8 = 8,
	Color9 = 9
};

//class Gl3dPrepareFrameBufferImpl;
//class GL3DC_EXPORT Gl3dPrepareFrameBuffer 
//{ 
//	Gl3dPrepareFrameBufferImpl* mImpl = nullptr;
//public:
//	Gl3dPrepareFrameBuffer();
//	~Gl3dPrepareFrameBuffer();
//	void Attach(Gl3dAttachmentColor attacment, Gl3dTexture* texture);
//	unsigned int GetObject() const override { return 0; }
//};