#pragma once

#include "Control.h"

class SGE_EXPORT Box : public Control
{
	Ref<Texture> box;
public:
	Box();
	const Ref<Control>& GetItems() const;
	virtual void Draw(GuiRenderData* drawData, const Point& globlPosition, const RectangleF& renderField);
};