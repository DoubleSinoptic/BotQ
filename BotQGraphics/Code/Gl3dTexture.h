#pragma once
#include "Gl3dCore.h"

enum class PixelFormat
{
	RGB_8 = 0,
	RGBA_8 = 1,
	RGB_16F = 2,
	RGBA_16F = 3,
	RGB_32F = 4,
	RGBA_32F = 5,
	DEPTH_8 = 6,
	DEPTH_16 = 7,
	DEPTH_32 = 8
};

enum class Filter 
{
	NEAREST = 0,
	LINEAR = 1,
	MIPMAP_NEAREST,
	MIPMAP_LINEAR
};

class Gl3dTextureImpl;
class GL3DC_EXPORT Gl3dTexture
{
	Gl3dTextureImpl* mImpl = nullptr;
public:
	Gl3dTexture(unsigned int object, unsigned int type);
	Gl3dTexture();
	~Gl3dTexture();
	void SetDataMultisample(int width, int height, PixelFormat format);
	void SetData(int width, int height, PixelFormat format, const void* data);
	void SetDataF(int width, int height, PixelFormat format, const float* data);
	void DisableClamp();
	int GetWidth() const;
	int GetHeight() const;
	unsigned int GetInternalType() const;
	/*
	Порядок обмотки такой
	right - 0
	left - 1
	top - 2
	bottom - 3
	back - 4
	front - 5
	*/
	void SetDataCube(int id, int width, int height, PixelFormat format, const void* data);

	void SetMagMinFilters(Filter mag, Filter min);
	//void SetWrapType();
	void GenMipmaps();
	unsigned int GetObject() const;
	static void Activate(int id, Gl3dTexture* texture);
};

