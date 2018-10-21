#pragma once

#include "Bitmap.h"
#include "../DigitalContainer.h"

class SGE_EXPORT Texture
{
	DigitalContainer handle;
	bool isCubeMap;
	int w, h;
public:
	~Texture();
	Texture();

	
	void* GetTextureObject() const;
	
	void SetFromBitmap(const Ref<Bitmap>& bitmap);
	void SetFromCubeBitmap(const Ref<Bitmap>& left, const Ref<Bitmap>& right, const Ref<Bitmap>& top, const Ref<Bitmap>& bottom, const Ref<Bitmap>& back, const Ref<Bitmap>& front);

	int GetWidth() const;
	int GetHeight() const;

	/*
	По дефолту всегда линеная фильтрация
	*/
	void EnableFiltration(bool value = true);

	void GenMipmaps();
};