#pragma once

#include <utility>
#include "Gl3dCore.h"

class Gl3dVertexArray_3x1Impl;
class GL3DC_EXPORT Gl3dVertexArray_3x1
{
	Gl3dVertexArray_3x1Impl* mImpl = nullptr;
public:
	Gl3dVertexArray_3x1();
	~Gl3dVertexArray_3x1();
	
	void DrawLines();
	void DrawTriangles();
	void DrawTrianglesStrip();

	//void DrawAllLines();
	void DrawAllTriangles(int vertecesCount);
	//void DrawAllTrianglesStrip();
	void EvaluteToStream();
	void SetVertexesNative(const char* data, size_t len);
	void SetNormalsNative(const char* data, size_t len);
	void SetTexcoordsNative(const char* data, size_t len);
	void SetIndexesNative(const char* data, size_t len);
	void SetTangetsNative(const char* data, size_t len);
	
	int GetIndexesCount() const;
	template<typename VectorType>
	void SetVertexes(const VectorType* vertexes, size_t count) 
	{ SetVertexesNative(reinterpret_cast<const char*>(vertexes), count * sizeof(VectorType)); }
	template<typename VectorType>
	void SetNormals(const VectorType* vertexes, size_t count)
	{ SetNormalsNative(reinterpret_cast<const char*>(vertexes), count * sizeof(VectorType)); }
	template<typename VectorType>
	void SetTexcoords(const VectorType* vertexes, size_t count)
	{ SetTexcoordsNative(reinterpret_cast<const char*>(vertexes), count * sizeof(VectorType)); }
	template<typename IndexType>
	void SetIndexes(const IndexType* vertexes, size_t count)
	{
		static_assert(
			std::is_same< std::remove_cv<std::remove_extent<IndexType>>, unsigned int>::value,
			"invalid index type");
		SetIndexesNative(reinterpret_cast<const char*>(vertexes), count * sizeof(IndexType));
	}
};

