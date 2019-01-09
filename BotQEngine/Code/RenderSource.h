#pragma once

#include "Common/DynamicArray.h"
#include "Component.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshRenderer.h"
#include "GameInstance.h"
#include "Gl3dArray.h"
#include "Gl3dDevice.h"

struct MeshStateUpdater 
{
	virtual ~MeshStateUpdater() {}
	virtual bool Update(const Matrix4& transform) = 0;
};

struct RenderThreadTag {};
class RenderSource
{
	Mesh*						m_mesh;
	DynamicArray<MeshRenderer*> m_dynamicRenderes;
	DynamicArray<MeshRenderer*> m_instancedRenderes;
	Gl3dArray<Matrix4>*			m_instancedData;
	Gl3dLayoutInstance*			m_instancedLayout;
	Gl3dLayoutInstance*			m_layout;
public:
	void Draw(MeshStateUpdater* updater, RenderThreadTag);
	void DrawInstanced(RenderThreadTag);

	void Add(MeshRenderer* r);
	void Remove(MeshRenderer* r);

	void ChangeInstanced(unsigned int id, const Matrix4& transform);
	void AddInstanced(MeshRenderer* r, const Matrix4& transform);

	void RemoveInstanced(MeshRenderer* r);
	RenderSource(Mesh* m);

	~RenderSource();
};

