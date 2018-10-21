
#include "../Resource.h"
#include "Bitmap.h"
#include "../Bits/stb_image.h"
#include "../Bits/stb_image_write.h"

#include "../Common/TinyFile.h"
int Bitmap::GetWidth() const 
{
	Validate();
	return mWidth;
}

int Bitmap::GetHeight() const 
{
	Validate();
	return mHeight;
}

float* Bitmap::LoadHdr(int& w , int& h, const String& fileName)
{
	Resource* rf = Resource::Find(fileName, false);
	int nr;
	stbi_set_flip_vertically_on_load(true);
	float* r = nullptr;
	if (!rf)
		r = stbi_loadf(fileName.c_str(), &w, &h, &nr, 0);
	else
	{
		Ref<DynamicArray<char>> t = rf->GetObject<Ref<DynamicArray<char>>>();
		r = stbi_loadf_from_memory((const stbi_uc*)t->GetData(), t->Length(), &w, &h, &nr, 0);
	}
		
	stbi_set_flip_vertically_on_load(false);
	return r;
}

Bitmap::Bitmap(int w, int h, const Color& transparnt)
{
	if (w < 1 || h < 1)
		Error("invalid argument");
	mPixels = new Color[w * h];
	mWidth = w;
	mHeight = h;
	Fill(transparnt);
}

Bitmap::Bitmap(const Size& size, const Color& transparnt) 
	: Bitmap(size.width, size.height, transparnt)
{}

Bitmap::Bitmap(const char* scanData, size_t dataLen)
{
	if (!scanData || !dataLen)
		Error("invalid argument");
	int cha;
	Color* c = (Color*)stbi_load_from_memory((const stbi_uc*)scanData, static_cast<int>(dataLen), &mWidth, &mHeight, &cha, 4);
	Guard g([&]() { stbi_image_free(c); });
	if (!c)
		Error("ivalid image ext");
	mPixels = new Color[mWidth * mHeight];
	memcpy(mPixels.GetPtr(), c, mWidth * mHeight * sizeof(Color));	
}

Bitmap::Bitmap(const String& fileName) 
{
	FileStream stream(fileName, OpenMode::Read);
	DynamicArray<char> s = stream.AllBytes();
	int cha;
	Color* c = (Color*)stbi_load_from_memory((const stbi_uc*)s.GetData(), static_cast<int>(s.Length()), &mWidth, &mHeight, &cha, 4);
	Guard g([&]() { stbi_image_free(c); });
	mPixels = new Color[mWidth * mHeight];
	memcpy(mPixels.GetPtr(), c, mWidth * mHeight * sizeof(Color));
}

void Bitmap::Validate() const
{
	if (mPixels.IsNull() || mWidth < 1 || mHeight < 1)
		Error("bitmap validating problem");
}

void* Bitmap::GetBits() 
{
	Validate();
	return static_cast<void*>(mPixels.GetPtr());
}

void Bitmap::Fill(const Color & color)
{
	Validate();
	for (int i = 0; i < mWidth; i++)
		for (int j = 0; j < mHeight; j++)
			SetPixel(Point(i, j), color);
}

void Bitmap::FromGray(const UnsignedColorChanel * reds)
{
	Validate();
	Assert(reds != nullptr);
	Color* cx = mPixels.GetPtr();
	for (int i = 0; i < mWidth * mHeight; i++)
		cx[i] = Color(reds[i], reds[i], reds[i], reds[i]);
}

void Bitmap::SetPixel(const Point& point, const Color& color)
{
	Validate();
	mPixels[point.y * mWidth + point.x] = color;
}

Color Bitmap::GetPixel(const Point& point)
{
	Validate();
	return mPixels[point.y * mWidth + point.x];
}

Color Bitmap::TexturePixel(float x, float y) const
{
	Validate();
	if (x > 1.0 || x < -1.0 || y > 1.0 || y < -1.0)
		Error("invalid texture coord");
	
	int xt = int(x * float(mWidth - 1));
	int yt = int(y * float(mHeight - 1));
	if (xt < 0)
		xt = mWidth - 1 - xt;
	if (yt < 0)
		yt = mHeight - 1 - yt;
	return mPixels[yt * mWidth + xt];
}





Ref<Bitmap> Bitmap::CopyS(Point upleft, Point bottright)
{
	if (upleft.x > bottright.x)
		std::swap(upleft.x, bottright.x);
	if (upleft.y > bottright.y)
		std::swap(upleft.y, bottright.y);
	Point sz = (bottright - upleft) + Point(1, 1);
	Ref<Bitmap> bt = new Bitmap(sz.x, sz.y, Color(0, 0, 0, 0));

	for (int i = 0; i < sz.x; i++) 
		for (int j = 0; j < sz.y; j++)	
			bt->SetPixel(Point(i, j), GetPixel(upleft + Point(i, j)));
	return bt;
}

void Bitmap::Save(const String & path, TargetFormat f) const
{
	switch (f)
	{
	case Bitmap::TargetFormat::Png:
		stbi_write_png(path.c_str(), mWidth, mHeight, 4, (void*)mPixels.GetPtr(), mWidth * 4);
		break;
	case Bitmap::TargetFormat::Tga:
		stbi_write_tga(path.c_str(), mWidth, mHeight, 4, (void*)mPixels.GetPtr());
		break;
	case Bitmap::TargetFormat::Bmp:
		stbi_write_bmp(path.c_str(), mWidth, mHeight, 4, (void*)mPixels.GetPtr());
		break;
	default:
		break;
	}
	
}
void _write_function(void *context, void *data, int size)
{
	DynamicArray<uint8_t>* x = reinterpret_cast<DynamicArray<uint8_t>*>(context);

}
void Bitmap::Save(DynamicArray<uint8_t>& memory, TargetFormat format) const
{
	stbi_write_func* svs = _write_function;
	switch (format)
	{
	case Bitmap::TargetFormat::Png:
		stbi_write_png_to_func(_write_function, &memory, mWidth, mHeight, 4, (void*)mPixels.GetPtr(), mWidth * 4);
		break;
	case Bitmap::TargetFormat::Tga:
		stbi_write_tga_to_func(_write_function, &memory, mWidth, mHeight, 4, (void*)mPixels.GetPtr());
		break;
	case Bitmap::TargetFormat::Bmp:
		stbi_write_bmp_to_func(_write_function, &memory, mWidth, mHeight, 4, (void*)mPixels.GetPtr());
		break;
	default:
		break;
	}
}

void Bitmap::Replace(const Color& from, const Color& to) 
{
	Validate();
	for (int i = 0; i < mHeight; i++)
		for (int j = 0; j < (mWidth / 2); j++)
			if(GetPixel(Point(j, i)) == from)
				SetPixel(Point(j, i), to);
}

void Bitmap::RotateHorizontal()
{
	Validate();
	for (int i = 0; i < mHeight; i++)
		for (int j = 0; j < (mWidth / 2); j++)	
			SetPixel(Point(j, i), GetPixel(Point((mWidth - 1) - j, i)));	
}

void Bitmap::RotateVertical() 
{
	Validate();
	for (int i = 0; i < (mHeight / 2); i++)
		for (int j = 0; j < mWidth; j++)	
			SetPixel(Point(j, i), GetPixel(Point(j, (mHeight - 1) - i)));
		
}

Bitmap::Bitmap(const Bitmap& bitmap) 
	: mWidth(bitmap.mWidth), mHeight(bitmap.mHeight)
{
	mPixels = new Color[mWidth * mHeight];
	memcpy(mPixels.GetPtr(), bitmap.mPixels.GetPtr(), mWidth * mHeight * sizeof(Color));
}

Bitmap& Bitmap::operator=(const Bitmap& bitmap) 
{
	if (&bitmap == this)
		return *this;
	mWidth = bitmap.mWidth;
	mHeight = bitmap.mHeight;
	mPixels = new Color[mWidth * mHeight];
	memcpy(mPixels.GetPtr(), bitmap.mPixels.GetPtr(), mWidth * mHeight * sizeof(Color));
	return *this;
}
