#pragma once

#include "Control.h"



class SGE_EXPORT CheckBox : public Control
{
	Ref<Texture> toggle;
	Ref<Texture> toggleActive;
	Ref<Texture> toggleHover;

	Ref<Texture> toggleOn;
	Ref<Texture> toggleOnActive;
	Ref<Texture> toggleOnHover;

	bool cheked = false;
	bool downded = false;
	bool IsMouseHovered = false;
public:
	CheckBox();
	virtual void Draw(GuiRenderData* drawData, const Point& globlPosition, const RectangleF& renderField) override;

	virtual void OnCursorEnter(const Point& cursorGLoblPosition);

	virtual void OnCursorLeave(const Point& cursorGLoblPosition);

	virtual void OnCursorStartDown(const Point& cursorGLoblPosition);

	virtual void OnCursorEndDown(const Point& cursorGLoblPosition);
	void SetCheked(bool val)
	{
		cheked = val;
	}
	StorageFunction<void(bool)> OnCheckChangedEvent;


};