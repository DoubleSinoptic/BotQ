#pragma once
#ifndef _Font_H_
#define _Font_H_

#include "../Core.h"
#include "../Math/Rectangle.h"
#include "Bitmap.h"

enum class FontStyle 
{
	Base,
	Bold,
	Italic
};

class FontImpl;
class SGE_EXPORT Font
{
public:
	Font(const char* scanData, size_t dataLen, FontStyle style, float fontSize, const String& name);
	~Font();
	Ref<Bitmap> GetSplatMap();
	
	float GetFontSize() const;
	String GetFontName() const;

	void RenderText(class GuiRenderData* renderData, const char* utf8, float scale, const Vector2& screenPosition, const RectangleF& clipRect);
	void MakeSizeExtents(const char * utf8, float scale, Vector2 & max, Vector2& min);
private:

	FontImpl* mImpl = nullptr;
};

#endif