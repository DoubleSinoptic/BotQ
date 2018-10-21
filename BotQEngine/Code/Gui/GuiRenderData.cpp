#include "GuiRenderData.h"



void GuiRenderData::BasisDrawRect(const RectangleF& basis, const RectangleF& rec, const Color& color, Texture* texture)
{
	int clip = clips.Add(basis);
}

void GuiRenderData::BasisDrawQuad(const RectangleF& basis, const RectangleF& rec, const Color& color, Texture* texture)
{

	Quad q;
	q.cullid = clips.Add(basis);
	q.texture = texture;
	q.c0 = color;
	q.c1 = color;
	q.c2 = color;
	q.c3 = color;

	RectangleF realrect = RectangleF(rec.GetPosition() + basis.GetPosition(), rec.GetSize());
	q.p0 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y);
	q.p1 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y);
	q.p2 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y + realrect.GetSize().height);
	q.p3 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y + realrect.GetSize().height);

	q.tc0 = Vector2(0, 0);
	q.tc1 = Vector2(1, 0);
	q.tc2 = Vector2(1, 1);
	q.tc3 = Vector2(0, 1);

	quads.Add(q);
}

void GuiRenderData::BasisDrawQuad8Path(const RectangleF& basis, const RectangleF& rectInBasis, int ls, int rs, int bs, int us, const Color& color, Texture* texture)
{
	int w = texture->GetWidth();
	int h = texture->GetHeight();

	int wk = rectInBasis.GetSize().width;
	int hk = rectInBasis.GetSize().height;

	BasisDrawQuadTexel(basis, RectangleF(Point(0, 0) + rectInBasis.GetPosition(), Point(ls, us) + rectInBasis.GetPosition(), true),
		RectangleF(Point(0, 0), Point(ls, us), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(basis, RectangleF(Point(ls, 0) + rectInBasis.GetPosition(), Point(wk - rs, us) + rectInBasis.GetPosition(), true),
		RectangleF(Point(ls, 0), Point(w - rs, us), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(basis, RectangleF(Point(wk - rs, 0) + rectInBasis.GetPosition(), Point(wk, us) + rectInBasis.GetPosition(), true),
		RectangleF(Point(w - rs, 0), Point(w, us), true),
		Color(255, 255, 255, 255), texture);


	BasisDrawQuadTexel(basis, RectangleF(Point(0, us) + rectInBasis.GetPosition(), Point(ls, hk - bs) + rectInBasis.GetPosition(), true),
		RectangleF(Point(0, us), Point(ls, h - bs), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(basis, RectangleF(Point(ls, us) + rectInBasis.GetPosition(), Point(wk - rs, hk - bs) + rectInBasis.GetPosition(), true),
		RectangleF(Point(ls, us), Point(w - rs, h - bs), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(basis, RectangleF(Point(ls, hk - bs) + rectInBasis.GetPosition(), Point(wk - rs, hk) + rectInBasis.GetPosition(), true),
		RectangleF(Point(ls, h - bs), Point(w - rs, h), true),
		Color(255, 255, 255, 255), texture);


	BasisDrawQuadTexel(basis, RectangleF(Point(wk - rs, hk - bs) + rectInBasis.GetPosition(), Point(wk, hk) + rectInBasis.GetPosition(), true),
		RectangleF(Point(w - rs, h - bs), Point(w, h), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(basis, RectangleF(Point(wk - rs, us) + rectInBasis.GetPosition(), Point(wk, hk - bs) + rectInBasis.GetPosition(), true),
		RectangleF(Point(w - rs, us), Point(w, h - bs), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(basis, RectangleF(Point(0, hk - bs) + rectInBasis.GetPosition(), Point(ls, hk) + rectInBasis.GetPosition(), true),
		RectangleF(Point(0, h - bs), Point(ls, h), true),
		Color(255, 255, 255, 255), texture);
}


void GuiRenderData::BasisDrawQuadTexel(const RectangleF& basis, const RectangleF& rec, const RectangleF& textureExel, const Color& color, Texture* texture)
{

	Quad q;
	q.cullid = clips.Add(basis);
	q.texture = texture;
	q.c0 = color;
	q.c1 = color;
	q.c2 = color;
	q.c3 = color;

	RectangleF realrect = RectangleF(rec.GetPosition() + basis.GetPosition(), rec.GetSize());
	q.p0 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y);
	q.p1 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y);
	q.p2 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y + realrect.GetSize().height);
	q.p3 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y + realrect.GetSize().height);

	q.tc0 = Vector2(textureExel.left, textureExel.top) / Vector2(texture->GetWidth(), texture->GetHeight());
	q.tc1 = Vector2(textureExel.right, textureExel.top) / Vector2(texture->GetWidth(), texture->GetHeight());
	q.tc2 = Vector2(textureExel.right, textureExel.bottom) / Vector2(texture->GetWidth(), texture->GetHeight());
	q.tc3 = Vector2(textureExel.left, textureExel.bottom) / Vector2(texture->GetWidth(), texture->GetHeight());

	quads.Add(q);
}






















void GuiRenderData::BasisDrawRect(const RectangleF& clip, const Point& pos, const RectangleF& rec, const Color& color, Texture* texture)
{
	int clipvss = clips.Add(clip);
}

void GuiRenderData::BasisDrawQuad(const RectangleF& clip, const Point& pos, const RectangleF& rec, const Color& color, Texture* texture)
{

	Quad q;
	q.cullid = clips.Add(clip);
	q.texture = texture;
	q.c0 = color;
	q.c1 = color;
	q.c2 = color;
	q.c3 = color;

	RectangleF realrect = RectangleF(rec.GetPosition() + pos, rec.GetSize());
	q.p0 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y);
	q.p1 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y);
	q.p2 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y + realrect.GetSize().height);
	q.p3 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y + realrect.GetSize().height);

	q.tc0 = Vector2(0, 0);
	q.tc1 = Vector2(1, 0);
	q.tc2 = Vector2(1, 1);
	q.tc3 = Vector2(0, 1);

	quads.Add(q);
}

void GuiRenderData::BasisDrawQuad8Path(const RectangleF& clip, const Point& pos, const RectangleF& rectInBasis, int ls, int rs, int bs, int us, const Color& color, Texture* texture)
{
	int w = texture->GetWidth();
	int h = texture->GetHeight();

	int wk = rectInBasis.GetSize().width;
	int hk = rectInBasis.GetSize().height;

	BasisDrawQuadTexel(clip,pos, RectangleF(Point(0, 0) + rectInBasis.GetPosition(), Point(ls, us) + rectInBasis.GetPosition(), true),
		RectangleF(Point(0, 0), Point(ls, us), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(clip, pos, RectangleF(Point(ls, 0) + rectInBasis.GetPosition(), Point(wk - rs, us) + rectInBasis.GetPosition(), true),
		RectangleF(Point(ls, 0), Point(w - rs, us), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(clip, pos, RectangleF(Point(wk - rs, 0) + rectInBasis.GetPosition(), Point(wk, us) + rectInBasis.GetPosition(), true),
		RectangleF(Point(w - rs, 0), Point(w, us), true),
		Color(255, 255, 255, 255), texture);


	BasisDrawQuadTexel(clip, pos, RectangleF(Point(0, us) + rectInBasis.GetPosition(), Point(ls, hk - bs) + rectInBasis.GetPosition(), true),
		RectangleF(Point(0, us), Point(ls, h - bs), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(clip, pos, RectangleF(Point(ls, us) + rectInBasis.GetPosition(), Point(wk - rs, hk - bs) + rectInBasis.GetPosition(), true),
		RectangleF(Point(ls, us), Point(w - rs, h - bs), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(clip, pos, RectangleF(Point(ls, hk - bs) + rectInBasis.GetPosition(), Point(wk - rs, hk) + rectInBasis.GetPosition(), true),
		RectangleF(Point(ls, h - bs), Point(w - rs, h), true),
		Color(255, 255, 255, 255), texture);


	BasisDrawQuadTexel(clip, pos, RectangleF(Point(wk - rs, hk - bs) + rectInBasis.GetPosition(), Point(wk, hk) + rectInBasis.GetPosition(), true),
		RectangleF(Point(w - rs, h - bs), Point(w, h), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(clip, pos, RectangleF(Point(wk - rs, us) + rectInBasis.GetPosition(), Point(wk, hk - bs) + rectInBasis.GetPosition(), true),
		RectangleF(Point(w - rs, us), Point(w, h - bs), true),
		Color(255, 255, 255, 255), texture);
	BasisDrawQuadTexel(clip, pos, RectangleF(Point(0, hk - bs) + rectInBasis.GetPosition(), Point(ls, hk) + rectInBasis.GetPosition(), true),
		RectangleF(Point(0, h - bs), Point(ls, h), true),
		Color(255, 255, 255, 255), texture);
}


void GuiRenderData::BasisDrawQuadTexel(const RectangleF& clip, const Point& pos, const RectangleF& rec, const RectangleF& textureExel, const Color& color, Texture* texture)
{

	Quad q;
	q.cullid = clips.Add(clip);
	q.texture = texture;
	q.c0 = color;
	q.c1 = color;
	q.c2 = color;
	q.c3 = color;

	RectangleF realrect = RectangleF(rec.GetPosition() + pos, rec.GetSize());
	q.p0 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y);
	q.p1 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y);
	q.p2 = Vector2(realrect.GetPosition().x + realrect.GetSize().width, realrect.GetPosition().y + realrect.GetSize().height);
	q.p3 = Vector2(realrect.GetPosition().x, realrect.GetPosition().y + realrect.GetSize().height);

	q.tc0 = Vector2(textureExel.left, textureExel.top) / Vector2(texture->GetWidth(), texture->GetHeight());
	q.tc1 = Vector2(textureExel.right, textureExel.top) / Vector2(texture->GetWidth(), texture->GetHeight());
	q.tc2 = Vector2(textureExel.right, textureExel.bottom) / Vector2(texture->GetWidth(), texture->GetHeight());
	q.tc3 = Vector2(textureExel.left, textureExel.bottom) / Vector2(texture->GetWidth(), texture->GetHeight());

	quads.Add(q);
}