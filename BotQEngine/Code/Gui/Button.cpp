#include "Button.h"

#define inline

inline Button::Button()
{
	button = Resource::Find("/Skin/button.png")->GetObject<Ref<Texture>>();
	buttonActive = Resource::Find("/Skin/button active.png")->GetObject<Ref<Texture>>();
	buttonHover = Resource::Find("/Skin/button hover.png")->GetObject<Ref<Texture>>();
}

inline void Button::OnCursorEnter(const Point & cursorGLoblPosition)
{
	IsMouseHovered = true;

}

inline void Button::OnCursorLeave(const Point & cursorGLoblPosition)
{
	IsMouseHovered = false;

}

inline void Button::OnCursorStartDown(const Point & cursorGLoblPosition)
{
	down = true;
}

inline void Button::OnCursorEndDown(const Point & cursorGLoblPosition)
{
	down = false;
	if (OnClikedEvent)
		OnClikedEvent();
}

inline void Button::Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField)
{
	if (IsMouseHovered)
	{
		if (down)
			drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 6, 6, 4, 6, Color(255, 255, 255, 255), buttonActive.GetPtr());
		else
			drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 6, 6, 4, 6, Color(255, 255, 255, 255), buttonHover.GetPtr());
	}
	else
	{
		drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 6, 6, 4, 6, Color(255, 255, 255, 255), button.GetPtr());
	}
	RectangleF charNet = RectangleF(globlPosition + Point(4, 2), GetSize() - Size(8, 4));
	if (GetFont())
	{

		float scale = charNet.GetSize().height / GetFont()->GetFontSize();

		GetFont()->RenderText(drawData, *GetText(), scale, charNet.GetPosition().ToVector2() + Vector2(1, charNet.GetSize().height * (3.0f / 4.0f)), renderField);

	}

}
