#pragma once

#include "Control.h"
#include "Button.h"
#include "Box.h"

class SGE_EXPORT Pottor : public Control
{
	Ref<Texture> thump;
	Point currsrStartLoc;
	Point lastLoc;
	Control * ct;
public:
	Pottor(Control* ths);


	float GetScrollTaping() const;

	void OnProcessUpdate(const Point& cursorGLoblPosition) override;

	bool downed = false;
	virtual void OnCursorStartDown(const Point& cursorGLoblPosition);

	virtual void OnCursorEndDown(const Point& cursorGLoblPosition);

	void ScrollDelta(int ds);

	void Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField);
};


class SGE_EXPORT RightSwam : public Control
{
	Ref<Texture> thump;
public:
	RightSwam();

	void Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField);
};

class SGE_EXPORT HScrollBox : public Box
{
	Ref<Pottor> mScrollButton;
	Ref<Control> mRight;
	Ref<Control> mProxy;
	Ref<Control> mItems;
public:
	HScrollBox();

	const Ref<Control>& GetItems() const;

	void Scroll(int ds);

	float Hermite();

	float MaximusY();

	void ComuteState();

	void OnProcessUpdate(const Point& cursorGLoblPosition) override;
};
