#pragma once
#ifndef _TrianglesCollider_h_
#define _TrianglesCollider_h_

#include "Collider.h"
class Mesh;
class TrianglesColliderImpl;
class SGE_EXPORT TrianglesCollider : public Collider
{
	TYPED_OBJECT
public:
	TrianglesCollider();
	~TrianglesCollider();
	
	void AddTriangles(const Vector3* vertexes, const unsigned int* indeces, int indecesCount);

	void AttachMeshData(Mesh* mesh);

	void AttachMeshesData(Mesh** meshes, int count);

	void CollectMeshesFromRenderer(const Vector3& scale = Vector3(1.0, 1.0, 1.0));

	void Awake() override;

	void Destroy() override;	
private:

	TrianglesColliderImpl* mImpl = nullptr;
};

#endif