#include "Bitmap9Path.h"



Bitmap9Path::Bitmap9Path()
{
}


Bitmap9Path::~Bitmap9Path()
{
}

void Bitmap9Path::Build(const Ref<Bitmap>& bmpx, int ls, int rs, int us, int bs)
{

	int w = bmpx->GetWidth();
	int h = bmpx->GetHeight();

	ul = bmpx->CopyS(Point(0,		0),						Point(ls - 1,		us - 1));
	u =  bmpx->CopyS(Point(ls,		0),						Point(w - rs - 1,	us - 1));
	ur = bmpx->CopyS(Point(w - rs, 0),						Point(w - 1,		us - 1));

	l = bmpx->CopyS(Point(0,		us),					Point(ls - 1,		h - bs - 1));
	c = bmpx->CopyS(Point(ls,		us),					Point(w - rs - 1,	h - bs - 1));
	b = bmpx->CopyS(Point(ls, h - bs), Point(w - rs - 1, h - 1));

	br = bmpx->CopyS(Point(w - rs, h - bs), Point(w - 1, h - 1));
	r = bmpx->CopyS(Point(w - rs, us),					Point(w - 1,		h - bs - 1));
	bl = bmpx->CopyS(Point(0,		h - bs),				Point(ls - 1,		h - 1));



}
