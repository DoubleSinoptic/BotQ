#pragma once

#ifndef SGE_2_COMMON_H
#define SGE_2_COMMON_H

#include "Gl3dVertexArrayBase.h"
#include "Gl3dCore.h"
#include "Gl3dArray.h"


#include "Component.h"
#include "GameObject.h"
#include "DigitalContainer.h"
#include "Common/DynamicArray.h"
#include "Common/Assert.h"
#include "Math/Matrix4.h"
#include "Math/Vector3.h"
#include "Math/Vector2.h"
#include "CommandQueue.h"
#include "Common/DataVector.h"


class Texture;
class Mesh;
class MeshRenderer;

class SGE_EXPORT MeshVariant
{
	bool _deletedFlag;
	DynamicArray<MeshRenderer*> m_renderers;
#ifdef USE_INSTANCING
	Gl3dArray<Matrix4>* m_transofrms_nva;
#else
	DynamicArray<Matrix4>* m_transofrms_nva;
#endif
	Mesh * m_mesh;
	Gl3dVertexArrayBase* m_vertex_array;

	friend class Material;
	friend class Mesh;
	friend class MeshRenderer;
	friend class RenderBeast;
public:

	MeshVariant(Mesh* varic);

	void QueryAdd(MeshRenderer* renderer);

	void QueryRemove(MeshRenderer* renderer);

	void QueryChange(MeshRenderer* renderer);

	~MeshVariant();
};

class SGE_EXPORT Material
{
	bool _deletedFlag;
	DynamicArray<Ref<MeshVariant>> render_variant;
	friend class Mesh;
	friend class MeshRenderer;
	friend class RenderBeast;
public:
	Material();
	virtual ~Material();

	void TryDeleteVariant(const Ref<MeshVariant>& var);

	Ref<MeshVariant> GetOrAddVariant(Mesh* msh);
};

class ObjectMaterial : public Material
{
public:
	Texture * metalic = nullptr;
	Texture*  ao = nullptr;
	Texture*  roughness = nullptr;
	Texture*  normals = nullptr;
	Texture*  albedo = nullptr;

	Vector4	  albedo_static = {1.0, 1.0, 1.0, 1.0};
	Vector3   metalic_roughness_ao_static = {0.0, 1.0, 1.0};
};

class SGE_EXPORT TerrainMaterial : public Material
{
public:
	Vector3   metalic_roughness_ao_static;

	Texture*  albedo0 = nullptr;
	Texture*  albedo1 = nullptr;
	Texture*  albedo2 = nullptr;
	Texture*  albedo3 = nullptr;
	Texture*  albedo4 = nullptr;
	Texture*  albedo5 = nullptr;
	Texture*  albedo6 = nullptr;
	Texture*  albedo7 = nullptr;

	Texture*  splat03;
	Texture*  splat47;
};

//[component]
class SGE_EXPORT MeshRenderer : public Component
{
	Mesh* m_mesh = nullptr;
	Material* m_material = nullptr;
	Ref<MeshVariant> m_variant;
	Ref<EventHandler<>> mTransformChanged;

	int id = -1;
	Vector3 scale;
	friend class MeshVariant;

	struct SGE_EXPORT UpdateCommand : public CommandBase
	{
		Matrix4       m_transform;
		MeshRenderer* m_renderer;
		virtual void Execute() override;
	} updateCommand;

	struct SGE_EXPORT AddCommand : public CommandBase
	{
		MeshRenderer* m_renderer;
		virtual void Execute() override;
	} addCommand;

	struct SGE_EXPORT RemoveCommand : public CommandBase
	{
		MeshRenderer* m_renderer;
		virtual void Execute() override;
	} removeCommand;
public:
	
	Mesh * GetMesh() const;

	void SetMesh(Mesh* msh);


	virtual void FrameUpdate() override {
		if (m_variant)
			m_variant->QueryChange(this);
	}

	virtual void Awake() override
	{
		updateCommand.m_renderer = this;
		addCommand.m_renderer =  this;
		removeCommand.m_renderer = this;

		SetEnabled(false);
		scale = Vector3(1.0, 1.0, 1.0);
	/*	mTransformChanged = new EventHandler<>([=]()
		{
			if(m_variant)
				m_variant->QueryChange(this);
		});
		GetGameObject()->GetHandlers()->OnTransformChanged += mTransformChanged.GetPtr();*/
	}

	virtual void CloneTo(TypedObject* o) const override
	{
		MeshRenderer* obj = dynamic_cast<MeshRenderer*>(o);
		if (!obj)
			Error("mesh_renderer* obj = dynamic_cast<mesh_renderer*>(o);\n\tif (!obj)");
		obj->SetMesh(m_mesh);
		obj->SetMaterial(m_material);
		obj->SetScale(scale);
	
	}

	virtual void Destroy() override
	{
		SetMaterial(nullptr);
		SetMesh(nullptr);
		GetGameObject()->GetHandlers()->OnTransformChanged -= mTransformChanged.GetPtr();
	}

	Material* GetMaterial()const 
	{
		return m_material;
	}
	void SetMaterial(Material* mat)
	{
		if (m_material)
		{
			m_variant->QueryRemove(this);
			m_material->TryDeleteVariant(m_variant);
			m_material = nullptr;
			m_variant = nullptr;
		}
		if (mat)
		{
			m_material = mat;
			m_variant = m_material->GetOrAddVariant(m_mesh);
			m_variant->QueryAdd(this);
		}
		else
			m_material = nullptr;
	}

	Vector3 GetScale() const 
	{ 
		return scale; 
	}

	void SetScale(const Vector3& scle)
	{
		scale = scle;
		mTransformChanged->GetFunctor();
	}

};


struct SGE_EXPORT Mesh 
{
	bool _deletedFlag;
	DynamicArray<Vector3> vertices;
	DynamicArray<Vector3> normals;
	DynamicArray<Vector2> texcoords;
	DynamicArray<Vector3> tangets;
	DynamicArray<unsigned int> indeces;


	DigitalContainer vertices_buffer;
	DigitalContainer normals_buffer;
	DigitalContainer texcoords_buffer;
	DigitalContainer indeces_buffer;
	DigitalContainer tangets_buffer;

	void calculate_uvs() 
	{}

	void calculate_tangets() 
	{}

	void calculate_normals() 
	{}

	Mesh();
	~Mesh();
	void applay_data();
};

#endif



