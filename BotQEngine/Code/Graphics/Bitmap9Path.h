#pragma once

#include "Bitmap.h"

class SGE_EXPORT Bitmap9Path
{
public:
	Ref<Bitmap> ul;
	Ref<Bitmap> l;
	Ref<Bitmap> bl;

	Ref<Bitmap> u;
	Ref<Bitmap> c;
	Ref<Bitmap> b;

	Ref<Bitmap> ur;
	Ref<Bitmap> r;
	Ref<Bitmap> br;

	Bitmap9Path();
	~Bitmap9Path();

	void Build(const Ref<Bitmap>& bmpx, int l, int r, int u, int b);

};

