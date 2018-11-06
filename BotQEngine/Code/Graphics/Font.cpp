#include "Font.h"
#include "../Bits/stb_truetype.h"
#include "Texture.h"
#include "../Gui/Control.h"
#include "../Common/Utf8.h"

class FontImpl 
{
public:

	stbtt_bakedchar cdata[10000];

	Font* _this;
	FontImpl(Font* __this, const char * scanData, float fontSize)
		: _this(__this), mFontSize(fontSize)
	{
		mSplat = new Bitmap(1024, 1024, Color(255, 0, 0, 255));
		static char buffer[1024 * 1024];
		
		if (!stbtt_BakeFontBitmap((const unsigned char *)scanData, 0, fontSize,(unsigned char*) buffer, 1024, 1024, 0x0, 0x500, cdata))
			Error("Create font fail");
		mSplat->FromGray((const UnsignedColorChanel*)buffer);
		/*mSplat->Save("./file.png", Bitmap::TargetFormat::Png);*/
		mSplatTexture = new Texture();
		mSplatTexture->SetFromBitmap(mSplat);
		mSplatTexture->EnableFiltration(false);
	}

	~FontImpl()
	{
		
	}
	float mFontSize;
	Ref<Bitmap> mSplat;
	Ref<Texture> mSplatTexture;
	String mName;
	FontStyle mStyle;
};

Font::Font(const char * scanData, size_t dataLen, FontStyle style, float fontSize, const String & name)
	: mImpl(new FontImpl(this, scanData, fontSize))
{
	mImpl->mName = name;
	mImpl->mStyle = style;

	

}

Font::~Font()
{
	if (mImpl)
		delete mImpl;
}

Ref<Bitmap> Font::GetSplatMap()
{
	return mImpl->mSplat;
}

float Font::GetFontSize() const
{
	return mImpl->mFontSize;
}

String Font::GetFontName() const
{
	return mImpl->mName;
}

void Font::RenderText(class GuiRenderData* drawData, const char * utf8, float scale, const Vector2 & screenPosition, const RectangleF & clipRect)
{	
	Vector2 xypos = Vector2(0, 0);
	GuiRenderData::Quad q;
	q.cullid = drawData->clips.Add(clipRect);
	q.texture = mImpl->mSplatTexture.GetPtr();



	int ch;
	while (ch = utf8::utf8_char_iterator((const uint8_t*&)utf8)) 
	{
		stbtt_aligned_quad qt;
		stbtt_GetBakedQuad(mImpl->cdata, 1024, 1024, ch, &xypos.x, &xypos.y, &qt, 1);//1=opengl & d3d10+,0=d3d9
		stbtt_bakedchar& qtp = mImpl->cdata[ch];
		q.c0 = Color(255, 255, 255, 255);
		q.c1 = Color(255, 255, 255, 255);
		q.c2 = Color(255, 255, 255, 255);
		q.c3 = Color(255, 255, 255, 255);

		q.tc3 = Vector2(qt.s0, qt.t1);
		q.tc2 = Vector2(qt.s1, qt.t1);
		q.tc1 = Vector2(qt.s1, qt.t0);
		q.tc0 = Vector2(qt.s0, qt.t0);
	
		q.p0 = Vector2(qt.x0 * scale , qt.y0 * scale ) + screenPosition + Vector2(0, 0);
		q.p1 = Vector2(qt.x1 * scale , qt.y0 * scale ) + screenPosition + Vector2(0, 0);
		q.p2 = Vector2(qt.x1 * scale, qt.y1 * scale ) + screenPosition + Vector2(0, 0);
		q.p3 = Vector2(qt.x0 * scale, qt.y1 * scale ) + screenPosition + Vector2(0, 0);

		drawData->quads.Add(q);
	}

}

void Font::MakeSizeExtents(const char * utf8, float scale, Vector2 & max, Vector2& min)
{
	float xt = 0;
	float yt = 0;
	int ch;

	max = Vector2(-100000, -100000);
	min = Vector2( 100000,  100000);
	while (ch = utf8::utf8_char_iterator((const uint8_t*&)utf8))
	{
		stbtt_aligned_quad qt;
		stbtt_GetBakedQuad(mImpl->cdata, 1024, 1024, ch, &xt, &yt, &qt, 1);
		stbtt_bakedchar& qtp = mImpl->cdata[ch];

#define __rect_min(a,b) (((a)>(b))? (b):(a))
#define __rect_max(a,b) (((a)>(b))? (a):(b))

		 max.x = __rect_max(qt.x1 * scale, max.x);
		 max.x = __rect_max(qt.x0 * scale, max.x);
		 max.y = __rect_max(qt.y0 * scale, max.y);
		 max.y = __rect_max(qt.y1 * scale, max.y);

		 min.x = __rect_min(qt.x1 * scale, min.x);
		 min.x = __rect_min(qt.x0 * scale, min.x);
		 min.y = __rect_min(qt.y0 * scale, min.y);
		 min.y = __rect_min(qt.y1 * scale, min.y);

#undef __rect_min
#undef __rect_max
		
	}

}

