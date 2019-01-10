#pragma once

#include "Common/DynamicArray.h"
#include "Component.h"
#include "Material.h"
#include "Mesh.h"
#include "GameInstance.h"
#include "Gl3dArray.h"
#include "Gl3dDevice.h"

class MeshRenderer;
struct MeshStateUpdater 
{
	virtual ~MeshStateUpdater() {}
	virtual bool Update(const Matrix4& transform) = 0;
};

struct RenderThreadTag {};
class SGE_EXPORT RenderSource
{
	ONLY_RENDER_THREAD_ACCESS Mesh*							m_mesh;
	ONLY_RENDER_THREAD_ACCESS DynamicArray<MeshRenderer*>	m_dynamicRenderes;
	ONLY_RENDER_THREAD_ACCESS DynamicArray<MeshRenderer*>	m_instancedRenderes;
	ONLY_RENDER_THREAD_ACCESS Gl3dArray<Matrix4>*			m_instancedData;
	ONLY_RENDER_THREAD_ACCESS Gl3dLayoutInstance*			m_instancedLayout;
	ONLY_RENDER_THREAD_ACCESS Gl3dLayoutInstance*			m_layout;
public:
	ONLY_RENDER_THREAD_ACCESS void Draw(MeshStateUpdater* updater, RenderThreadTag) const;
	ONLY_RENDER_THREAD_ACCESS void DrawInstanced(RenderThreadTag) const;

	Mesh* GetMesh() const;
	bool IsEmpty() const;

	ONLY_CORE_THREAD_ACCESS void Add(MeshRenderer* r);
	ONLY_CORE_THREAD_ACCESS void Remove(MeshRenderer* r);

	ONLY_CORE_THREAD_ACCESS void ChangeInstanced(unsigned int id, const Matrix4& transform);
	ONLY_CORE_THREAD_ACCESS void AddInstanced(MeshRenderer* r, const Matrix4& transform);

	ONLY_CORE_THREAD_ACCESS void RemoveInstanced(MeshRenderer* r);
	ONLY_CORE_THREAD_ACCESS RenderSource(Mesh* m);

	ONLY_CORE_THREAD_ACCESS ~RenderSource();
};

