#pragma once

#include "Control.h"
#include "Button.h"
#include "Box.h"

class SGE_EXPORT SliderThumb : public Control
{
	Point currsrStartLoc;
	Point lastLoc;
	Ref<Texture>	button;
	Ref<Texture>	buttonActive;
	Ref<Texture>	buttonHover;
	bool			down = false;
	bool			IsMouseHovered = false;
public:
	SliderThumb() 
	{
		button = Resource::Find("/Skin/slider thumb.png")->GetObject<Ref<Texture>>();

		buttonActive = Resource::Find("/Skin/slider thumb active.png")->GetObject<Ref<Texture>>();

		buttonHover = Resource::Find("/Skin/slidert humb hover.png")->GetObject<Ref<Texture>>();
	}

	void OnCursorEnter(const Point & cursorGLoblPosition)
	{
		IsMouseHovered = true;

	}

	void OnCursorLeave(const Point & cursorGLoblPosition)
	{
		IsMouseHovered = false;

	}

	void OnCursorStartDown(const Point & cursorGLoblPosition)
	{
		down = true;
		currsrStartLoc = cursorGLoblPosition;
		lastLoc = GetPosition();
	}

	void OnCursorEndDown(const Point & cursorGLoblPosition)
	{
		down = false;
		currsrStartLoc = { 0, 0 };
	}


	void Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField) 
	{
		if (IsMouseHovered)
		{
			if (down)
				drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 6, 6, 6, 6, Color(255, 255, 255, 255), buttonActive.GetPtr());
			else
				drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 6, 6, 6, 6, Color(255, 255, 255, 255), buttonHover.GetPtr());
		}
		else
		{
			drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), 4, 4, 5, 5, Color(255, 255, 255, 255), button.GetPtr());
		}
	}
	StorageFunction<void(float)> changed;
	float aspect;
	void OnProcessUpdate(const Point& cursorGLoblPosition) override 
	{
		if (down)
		{
			float newY = lastLoc.x + (cursorGLoblPosition - currsrStartLoc).x;
			int pos = GetPosition().x;
			this->SetPosition(Point(Mathf::Clamp(newY, 0, GetParent()->GetSize().width - (GetSize().width)), 0));
			aspect = float(GetPosition().x) / (GetParent()->GetSize().width - (GetSize().width));
			if (pos != GetPosition().x)
				if(changed)
					changed(aspect);		
		}
	}
};

class SGE_EXPORT HSlider : public Box
{
	Ref<SliderThumb> mThumb;
	Ref<Control> mSlider;
	Ref<Texture> mTextu;
public:
	HSlider() 
	{
		this->SetSize({100, 13});

		mThumb = New<SliderThumb>();
		mThumb->SetSize({8, 13});
		AddControl(mThumb);
		mTextu = Resource::Find("/Skin/horizontalslider.png")->GetObject<Ref<Texture>>();
	
	}

	void Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField)
	{
		drawData->BasisDrawQuad8Path(renderField, globlPosition, RectangleF(Point(0, 3), Size(GetSize().width,7)), 4, 4, 3, 3, Color(255, 255, 255, 255), mTextu.GetPtr());		
	}


};
