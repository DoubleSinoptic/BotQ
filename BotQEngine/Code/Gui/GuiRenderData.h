#pragma once

#include "../Math/PointAndSize.h"
#include "../Math/Color.h"
#include "../Math/Rectangle.h"
#include "../Graphics/Texture.h"

class SGE_EXPORT GuiRenderData
{
public:
	struct Line
	{
		Vector2 p0;
		Vector2 p1;
		Color c0;
		Color c1;
		int cullid = -1;
	};

	void BasisDrawRect(const RectangleF& basis, const RectangleF& rec, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);
	void BasisDrawQuad(const RectangleF& basis, const RectangleF& rec, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);
	void BasisDrawQuad8Path(const RectangleF& basis, const RectangleF& rectInBasis, int ls, int rs, int bs, int us, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);
	void BasisDrawQuadTexel(const RectangleF& basis, const RectangleF& rec, const RectangleF& textureExel, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);

	void BasisDrawRect(const RectangleF& clip, const Point& pos, const RectangleF& rec, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);
	void BasisDrawQuad(const RectangleF& clip, const Point& pos, const RectangleF& rec, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);
	void BasisDrawQuad8Path(const RectangleF& clip, const Point& pos, const RectangleF& rectInBasis, int ls, int rs, int bs, int us, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);
	void BasisDrawQuadTexel(const RectangleF& clip, const Point& pos, const RectangleF& rec, const RectangleF& textureExel, const Color& color = Color(255, 255, 255, 255), Texture* texture = nullptr);



	struct Face
	{
		Vector2 p0;
		Vector2 p1;
		Vector2 p2;
		Color c0;
		Color c1;
		Color c2;
		Vector2 tc0;
		Vector2 tc1;
		Vector2 tc2;
		class Texture* texture = nullptr;
		int cullid = -1;
	};

	struct Quad
	{
		Vector2 p0;
		Vector2 p1;
		Vector2 p2;
		Vector2 p3;
		Color c0;
		Color c1;
		Color c2;
		Color c3;
		Vector2 tc0;
		Vector2 tc1;
		Vector2 tc2;
		Vector2 tc3;
		class Texture* texture = nullptr;
		int cullid = -1;
	};

	DynamicArray<RectangleF> clips;
	DynamicArray<Face> faces;
	DynamicArray<Line> lines;
	DynamicArray<Quad> quads;
};

