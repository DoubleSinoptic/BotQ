#include "Label.h"
#include "../Resource.h"
#define inline

Label::Label()
{
	mFont = Resource::Find("/DefaultFont_24")->GetObject<Ref<Font>>();
}

inline void Label::SetFont(const Ref<Font>& font)
{
	mFont = font;
}

inline const Ref<Font>& Label::GetFont() const
{
	return mFont;
}

inline float Label::GetLowOffset() const
{
	return mLowOffset;
}

inline void Label::EnableAutoSize(bool value)
{
	mAutoSize = value;
}

inline void Label::SetText(const String & s)
{
	Control::SetText(s);
	if (mAutoSize)
	{
		Control::SetSize(Size(GetSize().width, GetFont()->GetFontSize() + 4));
		float scale = GetSize().height / GetFont()->GetFontSize();
		Vector2 min, max;
		GetFont()->MakeSizeExtents(*s, scale, max, min);
		SetSize(Size(Mathf::Abs(max.x - min.x) + 8, GetSize().height));
	}
}

inline void Label::Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField)
{
	RectangleF charNet = RectangleF(globlPosition + Point(4, 2), GetSize() - Size(8, 4));
	if (GetFont())
	{

		float scale = charNet.GetSize().height / GetFont()->GetFontSize();

		GetFont()->RenderText(drawData, *GetText(), scale, charNet.GetPosition().ToVector2() + Vector2(1, charNet.GetSize().height * (3.0f / 4.0f)), renderField);

	}

}
