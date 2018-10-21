#pragma once

#include "PointAndSize.h"
#include "Vector2.h"

class SGE_EXPORT RectangleF
{
public:
	RectangleF(const Point& location, const Size& size);

	explicit RectangleF(const Point& min, const Point& max, bool normalize = false);

	RectangleF(int _left, int _top, int _right, int _bottom);

	RectangleF();

	Point GetPosition() const;
	Size GetSize() const;

	int left;
	int top;
	int right;
	int bottom;

	void SetEmpty();

	bool IsEmpty() const;
};

SGE_EXPORT RectangleF operator* (const RectangleF& a, const RectangleF& b);