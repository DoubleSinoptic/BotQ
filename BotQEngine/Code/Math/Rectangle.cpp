#include "Rectangle.h"
#include <algorithm>
RectangleF::RectangleF(const Point& location, const Size& size)
	: left(location.x),
	top(location.y),
	right(location.x + size.width),
	bottom(location.y + size.height)
{}

RectangleF::RectangleF(const Point & min, const Point & max, bool normalize)
	: left(min.x),
	top(min.y),
	right(max.x),
	bottom(max.y)
{
	if (!normalize)
		return;
	if (left > right)
		std::swap(left, right);
	if (top > bottom)
		std::swap(top, bottom);
}

RectangleF::RectangleF(int _left, int _top, int _right, int _bottom)
	: left(_left),
	top(_top),
	right(_right),
	bottom(_bottom)
{}

RectangleF::RectangleF()
	: left(0),
	top(0),
	right(0),
	bottom(0)
{}

Point RectangleF::GetPosition() const
{
	return Point(left, top);
}

Size RectangleF::GetSize() const 
{
	return Size(right - left, bottom - top);
}

void RectangleF::SetEmpty()
{
	left = right = top = bottom = 0;
}

bool RectangleF::IsEmpty() const
{
	return ((left >= right) || (top >= bottom));
}

RectangleF operator* (const RectangleF& a, const RectangleF& b)
{
	RectangleF dst;
	RectangleF* lprcDst = &dst;
	const RectangleF* lprcSrc1 = &a;
	const RectangleF* lprcSrc2 = &b;
	if (lprcSrc1->IsEmpty() || lprcSrc2->IsEmpty() ||
		lprcSrc1->left >= lprcSrc2->right ||
		lprcSrc2->left >= lprcSrc1->right ||
		lprcSrc1->top >= lprcSrc2->bottom ||
		lprcSrc2->top >= lprcSrc1->bottom)
	{
		lprcDst->SetEmpty();
		return dst;
	}
#define __rect_min(a,b) ((a>b)? b:a)
#define __rect_max(a,b) ((a>b)? a:b)
	lprcDst->left = __rect_max(lprcSrc1->left, lprcSrc2->left);
	lprcDst->right = __rect_min(lprcSrc1->right, lprcSrc2->right);
	lprcDst->top = __rect_max(lprcSrc1->top, lprcSrc2->top);
	lprcDst->bottom = __rect_min(lprcSrc1->bottom, lprcSrc2->bottom);
#undef __rect_min
#undef __rect_max
	return dst;
}