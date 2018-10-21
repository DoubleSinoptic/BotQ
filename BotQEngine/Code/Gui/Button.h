#pragma once

#include "Label.h"

class SGE_EXPORT Button : public Label
{
	Ref<Texture>	button;
	Ref<Texture>	buttonActive;
	Ref<Texture>	buttonHover;
	bool			down = false;
	bool			IsMouseHovered = false;
public:
	Button();

	virtual void OnCursorEnter(const Point& cursorGLoblPosition);

	virtual void OnCursorLeave(const Point& cursorGLoblPosition);

	virtual void OnCursorStartDown(const Point& cursorGLoblPosition);

	virtual void OnCursorEndDown(const Point& cursorGLoblPosition);

	StorageFunction<void(void)> OnClikedEvent;

	virtual void Draw(GuiRenderData* drawData, const Point& globlPosition, const RectangleF& renderField);
};
