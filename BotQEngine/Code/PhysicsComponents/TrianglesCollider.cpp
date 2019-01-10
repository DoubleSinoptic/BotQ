#include "TrianglesCollider.h"
#include "btBulletCollisionCommon.h"
#include "../GameObject.h"
#include "../MeshRenderer.h"

class TrianglesColliderImpl 
{
public:
	TrianglesCollider* _this;
	TrianglesColliderImpl(TrianglesCollider* __this)
		: _this(__this)
	{
		
	}

	~TrianglesColliderImpl()
	{
	}

	Ref<btBvhTriangleMeshShape> shape;

};

void TrianglesCollider::Awake()
{
	mImpl = new TrianglesColliderImpl(this);
	SetEnabled(false);
	
}

void TrianglesCollider::Destroy()
{
	//Do code ...
	if (mImpl) 
		delete mImpl; 
}	

TrianglesCollider::TrianglesCollider()
{}

TrianglesCollider::~TrianglesCollider()
{}

btVector3 of__(Vector3 v)
{
	return btVector3(v.x, v.y, v.z);
}

void TrianglesCollider::AddTriangles(const Vector3 * vertexes, const unsigned int * indeces, int indecesCount)
{
	Assert((indecesCount % 3) == 0);
	Assert(indeces != nullptr);
	Assert(vertexes != nullptr);
	
	btTriangleMesh* u = new btTriangleMesh();
	for (int i = 0; i < (indecesCount / 3); i++)
	{
			u->addTriangle(
			of__(vertexes[indeces[0 + i * 3]]),
			of__(vertexes[indeces[1 + i * 3]]), 
			of__(vertexes[indeces[2 + i * 3]]));
	}
	mImpl->shape = new btBvhTriangleMeshShape(u, true);
	InstallShape(mImpl->shape.GetPtr());
}


void TrianglesCollider::AttachMeshData(Mesh * mesh)
{
	Assert(mesh != nullptr);
	AddTriangles(mesh->GetVertexes(), mesh->GetIncdeces(), mesh->GetIncdecesSize());
}

void TrianglesCollider::AttachMeshesData(Mesh ** meshes, int count)
{
	Assert(meshes != nullptr);
	Assert(count > 0);
	btTriangleMesh* u = new btTriangleMesh();
	for (int i = 0; i < count; i++) 
	{
		const Vector3 * vertexes = meshes[i]->GetVertexes();
		const unsigned int * indeces = meshes[i]->GetIncdeces();
		int indecesCount = meshes[i]->GetIncdecesSize();
		Assert(meshes[i] != nullptr);
		for (int i = 0; i < (indecesCount / 3); i++)
		{
			u->addTriangle(
				of__(vertexes[indeces[0 + i * 3]]),
				of__(vertexes[indeces[1 + i * 3]]),
				of__(vertexes[indeces[2 + i * 3]]));
		}

	}	
	mImpl->shape = new btBvhTriangleMeshShape(u, true);
	InstallShape(mImpl->shape.GetPtr());
}

Vector3 scaleOf(const Vector3& v, const Vector3& scale)
{
	return Vector3(v.x * scale.x, v.y * scale.y, v.z * scale.z);
}

void RecurseveAddMeshes(btTriangleMesh* messh, GameObject* obj, const Vector3& scale)
{
	Vector3 pos = obj->GetPosition();
	Quaternion rot = obj->GetRotation();
	for (GameObject* i : *obj)
	{
		DynamicArray<MeshRenderer*> c = i->GetComponents<MeshRenderer>();
		for (auto iv : c)
		{
			Assert(iv->GetMesh() != nullptr);
			const Vector3 * vertexes = iv->GetMesh()->GetVertexes();
			const unsigned int * indeces = iv->GetMesh()->GetIncdeces();
			int indecesCount = iv->GetMesh()->GetIncdecesSize();

			for (int i = 0; i < (indecesCount / 3); i++)
			{
				messh->addTriangle(
					of__(rot * scaleOf(vertexes[indeces[0 + i * 3]], scale) + pos),
					of__(rot * scaleOf(vertexes[indeces[1 + i * 3]], scale) + pos),
					of__(rot * scaleOf(vertexes[indeces[2 + i * 3]], scale) + pos));
			}
		}
		RecurseveAddMeshes(messh, i, scale);
	}
}

void TrianglesCollider::CollectMeshesFromRenderer(const Vector3& scale)
{
	GameObject* i = GetGameObject();
	while (i->GetParent())
	{		
		i = i->GetParent();
	}
	
	btTriangleMesh* u = new btTriangleMesh();
	RecurseveAddMeshes(u, i, scale);
	Assert(u->getNumTriangles() > 0);
	mImpl->shape = new btBvhTriangleMeshShape(u, true);
	InstallShape(mImpl->shape.GetPtr());
}
