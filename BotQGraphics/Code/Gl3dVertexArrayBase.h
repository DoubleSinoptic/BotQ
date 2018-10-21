#pragma once

#include "Gl3dCore.h"
#include "Gl3dArray.h"

enum class Gl3dDrawPrimitive
{
	Triangles,
	Quads,
	Lines
};

enum class Gl3dDataSplitType
{
	Float = 0,
	Float2 = 1,
	Float3 = 2,
	Float4 = 3,
	Int = 4,
	Short = 5,
	Pointer = 6
};

const size_t Gl3dDataSplitTypeSize[] = 
{
	sizeof(float),
	sizeof(float) * 2,
	sizeof(float) * 3,
	sizeof(float) * 4,
	sizeof(int),
	sizeof(short),
	sizeof(void*)
};

class Gl3dVertexArrayBaseImpl;
class GL3DC_EXPORT Gl3dVertexArrayBase
{
	Gl3dVertexArrayBaseImpl* mImpl = nullptr;
public:
	Gl3dVertexArrayBase();
	~Gl3dVertexArrayBase();

	template<typename T>
	void Attach(int startIndex, const std::initializer_list<Gl3dDataSplitType>& splits, Gl3dArray<T>* arr)
	{
		AttachImplSplit(startIndex, splits, arr, sizeof(T));
	}

	/*
	Only float divisor supported
	*/
	void AttachInstnacedImpl(int index, Gl3dArrayImpl* impl, size_t elemSize, int elemCount);

	template<typename T>
	void Attach(int index, Gl3dArray<T>* arr) 
	{
		AttachImpl(index, arr, sizeof(T));
	}

	void AttachImpl(int index, Gl3dArrayImpl* impl, size_t elemSize);
	void AttachImplSplit(int index, const std::initializer_list<Gl3dDataSplitType>& splits, Gl3dArrayImpl* impl, size_t elemSize);

	void Draw(Gl3dDrawPrimitive primitive, int n);
	void DrawIndexed(Gl3dDrawPrimitive primitive, int n);
	void DrawInstnaced(Gl3dDrawPrimitive privmtive, int n, int drawCount);
};

