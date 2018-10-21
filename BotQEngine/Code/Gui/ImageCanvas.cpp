#include "ImageCanvas.h"

#define inline

inline ImageCanvas::ImageCanvas()
{}

inline void ImageCanvas::SetImage(const Ref<Texture>& tex)
{
	image = tex;
}

inline void ImageCanvas::Draw(GuiRenderData * drawData, const Point& globlPosition, const RectangleF & renderField)
{
	drawData->BasisDrawQuad(renderField, globlPosition, RectangleF(Point(0, 0), GetSize()), Color(255, 255, 255, 255), image.GetPtr());
}
