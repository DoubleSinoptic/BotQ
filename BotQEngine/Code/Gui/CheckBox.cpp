#include "CheckBox.h"


CheckBox::CheckBox()
{
	{
		toggle = Resource::Find("/Skin/toggle.png")->GetObject<Ref<Texture>>();
	}
	{
		toggleActive = Resource::Find("/Skin/toggle active.png")->GetObject<Ref<Texture>>();
	}
	{
		toggleHover = Resource::Find("/Skin/toggle hover.png")->GetObject<Ref<Texture>>();
	}
	
	{
		toggleOn = Resource::Find("/Skin/toggle on.png")->GetObject<Ref<Texture>>();
	}
	{
		toggleOnActive = Resource::Find("/Skin/toggle on active.png")->GetObject<Ref<Texture>>();
	}
	{
		toggleOnHover = Resource::Find("/Skin/toggle on hover.png")->GetObject<Ref<Texture>>();
	}
}


void CheckBox::Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & clip) 
{

	if (IsMouseHovered)
		if (cheked)
			if (downded)
				drawData->BasisDrawQuad8Path(clip, globlPosition, RectangleF(Point(0, 0), GetSize()), 5, 6, 5, 5, Color(255, 255, 255, 255), toggleOnActive.GetPtr());
			else
				drawData->BasisDrawQuad8Path(clip, globlPosition, RectangleF(Point(0, 0), GetSize()), 5, 6, 5, 5, Color(255, 255, 255, 255), toggleOnHover.GetPtr());
		else
			if (downded)
				drawData->BasisDrawQuad8Path(clip, globlPosition, RectangleF(Point(0, 0), GetSize()), 5, 6, 5, 5, Color(255, 255, 255, 255), toggleActive.GetPtr());
			else
				drawData->BasisDrawQuad8Path(clip, globlPosition, RectangleF(Point(0, 0), GetSize()), 5, 6, 5, 5, Color(255, 255, 255, 255), toggleHover.GetPtr());
	else
		if (cheked)	
			drawData->BasisDrawQuad8Path(clip, globlPosition, RectangleF(Point(0, 0), GetSize()), 5, 6, 5, 5, Color(255, 255, 255, 255), toggleOn.GetPtr());
		else	
			drawData->BasisDrawQuad8Path(clip, globlPosition, RectangleF(Point(0, 0), GetSize()), 5, 6, 5, 5, Color(255, 255, 255, 255), toggle.GetPtr());
		
	

}

void CheckBox::OnCursorEnter(const Point & cursorGLoblPosition)
{
	IsMouseHovered = true;

}

void CheckBox::OnCursorLeave(const Point & cursorGLoblPosition)
{
	IsMouseHovered = false;

}

void CheckBox::OnCursorStartDown(const Point & cursorGLoblPosition)
{
	downded = true;
}

void CheckBox::OnCursorEndDown(const Point & cursorGLoblPosition)
{
	downded = false;
	cheked = !cheked;
	if (OnCheckChangedEvent)
		OnCheckChangedEvent(cheked);
}
