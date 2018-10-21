#pragma once

#include "Control.h"

class SGE_EXPORT ImageCanvas : public Control
{
	Ref<Texture> image;
public:
	ImageCanvas();

	void SetImage(const Ref<Texture>& tex);

	virtual void Draw(GuiRenderData* drawData, const Point& globlPosition, const RectangleF& renderField);
};