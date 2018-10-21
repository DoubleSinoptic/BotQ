#include "Box.h"

#define inline

inline Box::Box()
{
	box = Resource::Find("/Skin/box.png")->GetObject<Ref<Texture>>();
}

const Ref<Control>& Box::GetItems() const
{
	Error("hi hi hi Ref<>");
	return nullptr;
}

inline void Box::Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & clip)
{
	drawData->BasisDrawQuad8Path(clip, globlPosition, RectangleF(Point(0, 0), GetSize()), 6, 6, 6, 6, Color(255, 255, 255, 255), box.GetPtr());
}
