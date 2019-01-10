#pragma once

#include "Bitmap.h"
#include "../DigitalContainer.h"

enum class TextureAddressMode 
{
	Repeat = 0,
	MirroredRepeat = 1,
	ClampToEdge = 2,
	ClampToBorder = 3
};

enum class FilterMode
{
	Nearest = 0,
	Linear = 1,
	MipLinear = 2,
	MipNearest = 3
};

struct SamplerStateDesc 
{
	FilterMode		   mag;
	FilterMode		   min;
	TextureAddressMode addressModeU;
	TextureAddressMode addressModeV;
	TextureAddressMode addressModeW;
};

class SGE_EXPORT Texture
{
	DigitalContainer handle;
	bool isCubeMap;
	int w, h;
public:
	~Texture();
	Texture();
	
	void* GetTextureObject() const;

	void SetSampler(const SamplerStateDesc& state);
	void SetFromBitmap(const Ref<Bitmap>& bitmap);
	void SetFromCubeBitmap(const Ref<Bitmap>& left, const Ref<Bitmap>& right, const Ref<Bitmap>& top, const Ref<Bitmap>& bottom, const Ref<Bitmap>& back, const Ref<Bitmap>& front);

	int GetWidth() const;
	int GetHeight() const;

	void GenMipmaps();
};