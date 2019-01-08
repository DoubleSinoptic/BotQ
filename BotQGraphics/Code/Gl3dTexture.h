#pragma once
#include "Gl3dCore.h"
#include "Gl3dDevice.h"

enum class Gl3dPixelFormat
{
	R_8,
	R_16F,
	R_32F,

	RG_8,
	RG_16F,
	RG_32F,

	RGB_8,
	RGB_16F,
	RGB_32F,

	RGBA_8,
	RGBA_16F,
	RGBA_32F,

	DEPTH_8,
	DEPTH_16,
	DEPTH_32
};

enum class Gl3dFilter 
{
	NEAREST = 0,
	LINEAR = 1,
	MIPMAP_NEAREST,
	MIPMAP_LINEAR
};

enum class Gl3dWrapMode 
{
	ClampToEdge,
	Repeat,
	MirroredRepeat,
	ClampToBorder
};

struct Gl3dSubImageDesc
{
	int					width;
	int					height;
	Gl3dFundamentalType type;
	int					changelsCount;
	void*				data;
};

class Gl3dTextureImpl;
class GL3DC_EXPORT Gl3dTexture
{
	Gl3dTextureImpl* mImpl = nullptr;
public:
	Gl3dTexture(unsigned int object, unsigned int type);
	Gl3dTexture();
	~Gl3dTexture();
	
	void SetData(const Gl3dSubImageDesc* image, int level = 0);
	void SetData(Gl3dSide side, const Gl3dSubImageDesc* image, int level = 0);

	void SetData(Gl3dPixelFormat internalFormat, const Gl3dSubImageDesc* image, int level = 0);
	void SetData(Gl3dSide side, Gl3dPixelFormat internalFormat, const Gl3dSubImageDesc* image, int level = 0);

	void AllocateData(int w, int h, Gl3dPixelFormat internalFormat, int level = 0);
	void AllocateData(Gl3dSide side, int w, int h, Gl3dPixelFormat internalFormat, int level = 0);

	int GetWidth() const;
	int GetHeight() const;
	unsigned int GetInternalType() const;
	
	void GenMipmaps();
	void SetMagMinFilters(Gl3dFilter mag, Gl3dFilter min);
	void SetWrapMode(Gl3dWrapMode s = Gl3dWrapMode::Repeat, Gl3dWrapMode t = Gl3dWrapMode::Repeat, Gl3dWrapMode r = Gl3dWrapMode::Repeat);
	
	unsigned int GetObject() const;
	static void Activate(int id, Gl3dTexture* texture);
};

