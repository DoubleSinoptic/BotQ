#pragma once

#include "../Math/Color.h"
#include "../Math/PointAndSize.h"
#include "../Core.h"


class SGE_EXPORT Bitmap
{
	Ref<Color[]> mPixels;
	int mWidth, mHeight;
public:
	Bitmap(int w, int h, const Color& transparnt = Color(255, 255, 255, 255));
	Bitmap(const Size& size, const Color& transparnt = Color(255, 255, 255, 255));
	Bitmap(const char* scanData, size_t dataLen);
	Bitmap(const String& fileName);

	/*
	вообще бред... я когда писал Bitmap вообще не думал
	что придётся таскать нативные цвета
	*/
	static float* LoadHdr(int& w, int& h, const String& fileName);

	void Validate() const;

	void* GetBits();

	void Fill(const Color& color);

	void FromGray(const UnsignedColorChanel* reds);

	void SetPixel(const Point& point, const Color& color);
	Color GetPixel(const Point& point);
	Color TexturePixel(float x, float y) const;
	Ref<Bitmap> CopyS(Point upleft, Point bottright);

	enum class TargetFormat
	{
		Png,Tga,Bmp
	};

	void Save(const String& path, TargetFormat format = TargetFormat::Png) const;
	void Save(DynamicArray<uint8_t>& memory, TargetFormat format = TargetFormat::Png) const;

	void Replace(const Color& from, const Color& to);

	class Gl3dSubImageDesc* FormatDesc() const;

	int GetWidth() const;
	int GetHeight() const;

	void RotateHorizontal();
	void RotateVertical();

	Bitmap(const Bitmap& bitmap);
	Bitmap& operator=(const Bitmap& bitmap);
};