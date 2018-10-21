#pragma once

#include "Control.h"
#include "../Graphics/Font.h"

class SGE_EXPORT Label : public Control
{
	bool mAutoSize = true;
	Ref<Font> mFont;
	float mLowOffset;
public:
	Label();
	void SetFont(const Ref<Font>& font);

	const Ref<Font>& GetFont() const;

	float GetLowOffset() const;

	void EnableAutoSize(bool value = true);

	virtual void SetText(const String& s);

	virtual void Draw(GuiRenderData* drawData, const Point& globlPosition, const RectangleF& renderField);
};