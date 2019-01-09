#include "MeshImporter.h"
#include "./Common/TinyFile.h"
#include "Resource.h"
#include "./Math/Vector4.h"
#include "./Graphics/Texture.h"
#include "Common/PathHelper.h"
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Sge2Common.h"

Ref<DynamicArray<Ref<Material>>> MakesMaterial(const aiScene* scene, const String& txtureAdditionalPath)
{
	Ref<DynamicArray<Ref<Material>>> mats = new DynamicArray<Ref<Material>>();
	mats->Initialize(scene->mNumMaterials);
	for (int i = 0; i < scene->mNumMaterials; i++) 
	{
		Ref<Material> xmaterial = new ObjectMaterial();
		ObjectMaterial* material = static_cast<ObjectMaterial*>(xmaterial.GetPtr());
		aiMaterial* mt = scene->mMaterials[i];
		mats->operator[](i) = xmaterial;
		String realName = String::Format("Material_%d", i);
		aiString name;
		if (AI_SUCCESS == mt->Get(AI_MATKEY_NAME, name))
			realName = name.C_Str();
		new Resource(xmaterial, String(txtureAdditionalPath + "/" + realName));
		{
			aiString texturePath;
			if (mt->GetTextureCount(aiTextureType_DIFFUSE) > 0 && mt->GetTexture(aiTextureType_DIFFUSE, 0, &texturePath) == AI_SUCCESS)
			{
				Resource* t = Resource::Find(txtureAdditionalPath + "/" + Path::FileName(Path::PathFix(texturePath.C_Str())), false);
				if (t)
				{
					material->albedo = (t->GetObject<Ref<Texture>>().GetPtr());
					Log("texture finded: %s", texturePath.C_Str());
				}
				else
				{
					FileStream f(txtureAdditionalPath + "/" + Path::FileName(Path::PathFix(texturePath.C_Str())), OpenMode::Read);
					DynamicArray<char> e = f.AllBytes();
					Ref<Texture> texture = new Texture();
					texture->SetFromBitmap(new Bitmap(e.GetData(), e.Length()));
					new Resource(texture, txtureAdditionalPath + "/" + Path::FileName(Path::PathFix(texturePath.C_Str())));
					material->albedo = (texture.GetPtr());
					Log("texture imported: %s", *Path::FileName(Path::PathFix(texturePath.C_Str())));
				}

			}
		}
		{
			aiString texturePath;
			if (mt->GetTextureCount(aiTextureType_NORMALS) > 0 && mt->GetTexture(aiTextureType_NORMALS, 0, &texturePath) == AI_SUCCESS)
			{
				Resource* t = Resource::Find(txtureAdditionalPath + "/" + Path::FileName(Path::PathFix(texturePath.C_Str())), false);
				if (t)
				{
					material->normals = (t->GetObject<Ref<Texture>>().GetPtr());
					Log("texture finded: %s", texturePath.C_Str());
				}
				else
				{
					FileStream f(txtureAdditionalPath + "/" + Path::FileName(Path::PathFix(texturePath.C_Str())), OpenMode::Read);
					DynamicArray<char> e = f.AllBytes();
					Ref<Texture> texture = new Texture();
					texture->SetFromBitmap(new Bitmap(e.GetData(), e.Length()));
					new Resource(texture, txtureAdditionalPath + "/" + Path::FileName(Path::PathFix(texturePath.C_Str())));
					material->normals = (texture.GetPtr());
					Log("texture imported: %s", *Path::FileName(Path::PathFix(texturePath.C_Str())));
				}

			}
		}

	}

	return mats;
}



Ref<construct_info> rootBuild(Ref<DynamicArray<Ref<Material>>> mt, const aiScene* scene, const struct aiNode* nd, const String& txtureAdditionalPath)
{
	Assert(scene != nullptr);
	Assert(nd != nullptr);
	Ref<construct_info> mconstructor = new construct_info();
    mconstructor->m_name = (nd->mName.C_Str());
	
	aiVector3D scale;
	aiQuaternion rot;
	aiVector3D pos;
	aiMatrix4x4 m = nd->mTransformation;

	//m.Decompose(scale, rot, pos);
	m.DecomposeNoScaling(rot, pos);
	mconstructor->m_local_position = Vector3(pos.x, pos.y, pos.z);
	mconstructor->m_local_rotation = Quaternion(rot.x, rot.y, rot.z, rot.w).Normalize();
	for (unsigned int n = 0; n < nd->mNumChildren; ++n)
	{
		Ref<construct_info> onceChild = rootBuild(mt, scene, nd->mChildren[n], txtureAdditionalPath);
		mconstructor->m_childs.Add(onceChild);
	}

	for (unsigned int n = 0; n < nd->mNumMeshes; n++)
	{
		const aiMesh* asmesh = scene->mMeshes[nd->mMeshes[n]];
		Ref<Mesh> game_mesh = new Mesh();
		mesh_pair pair;
		pair.m_mat_id = asmesh->mMaterialIndex;
		pair.m_mesh = game_mesh;

		mconstructor->m_meshs.Add(pair);
		mconstructor->m_materials = mt;

		game_mesh->SetVertexes((const Vector3*)asmesh->mVertices, asmesh->mNumVertices);
		
		if (asmesh->HasNormals())
			game_mesh->SetNormals((const Vector3*)asmesh->mNormals, asmesh->mNumVertices);
		if (asmesh->HasTangentsAndBitangents())
			game_mesh->SetTangents((const Vector3*)asmesh->mTangents, asmesh->mNumVertices);
		
		if (asmesh->HasTextureCoords(0))
		{
			DynamicArray<Vector2> v;
			v.EnsureCapacity(asmesh->mNumVertices);
			for (int i = 0; i < asmesh->mNumVertices; i++)
			{
				v.Add(Vector2(asmesh->mTextureCoords[0][i].x, asmesh->mTextureCoords[0][i].y));
			}
			game_mesh->SetTexcoords(v.GetData(), asmesh->mNumVertices);

		}
			

		DynamicArray<unsigned int> arr;
		arr.EnsureCapacity(asmesh->mNumFaces);
		for (unsigned int t = 0; t < asmesh->mNumFaces; ++t)
		{
			aiFace* face = &asmesh->mFaces[t];
			if (face->mNumIndices == 3)
			{
				unsigned int ix0 = face->mIndices[0];
				unsigned int ix1 = face->mIndices[1];
				unsigned int ix2 = face->mIndices[2];
				arr.Add(ix0);
				arr.Add(ix1);
				arr.Add(ix2);
			}
			else if (face->mNumIndices == 2)
			{
				unsigned int ix0 = face->mIndices[0];
				unsigned int ix2 = face->mIndices[1];
				arr.Add(ix0);
				arr.Add(ix2);
				arr.Add(ix0);
	
			}
			else if (face->mNumIndices == 4)
			{
				unsigned int ix0 = face->mIndices[0];
				unsigned int ix1 = face->mIndices[1];
				unsigned int ix2 = face->mIndices[2];
				unsigned int ix3 = face->mIndices[3];
				arr.Add(ix0);
				arr.Add(ix1);
				arr.Add(ix2);
				arr.Add(ix0);
				arr.Add(ix2);
				arr.Add(ix3);
			}
			else
				Error("index error");
		
		}
		game_mesh->SetIndeces(arr.GetData(), arr.Length());
	}

	
	return mconstructor;
}


Ref<construct_info> MeshImporter::Import(String file)
{
	file = Path::PathFix(file);
	Resource* tt = Resource::Find(file, false);
	if (tt)
		return tt->GetObject<Ref<construct_info>>();
	
	FileStream f(file, OpenMode::Read);
	DynamicArray<char> dt = f.AllBytes();
	const aiScene* sc = aiImportFileFromMemory(dt.GetData(), dt.Length(), aiProcess_Triangulate | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes, "");
	Assert(sc != nullptr);
	Ref<DynamicArray<Ref<Material>>> mt = MakesMaterial(sc, Path::FileDir(file));
	Ref<construct_info> o = rootBuild(mt, sc, sc->mRootNode, Path::FileDir(file));
	aiReleaseImport(sc);
	new Resource(o, file);
	return o;
}

Ref<construct_info> MeshImporter::ImportRW(String file, const char * data, size_t len)
{
	file = Path::PathFix(file);
	Resource* tt = Resource::Find(file, false);
	if (tt)
		return tt->GetObject<Ref<construct_info>>();

	const aiScene* sc = aiImportFileFromMemory(data, len, aiProcess_Triangulate | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes, "");
	Assert(sc != nullptr);
	Ref<DynamicArray<Ref<Material>>> mt = MakesMaterial(sc, Path::FileDir(file));
	Ref<construct_info> o = rootBuild(mt, sc, sc->mRootNode, Path::FileDir(file));
	aiReleaseImport(sc);
	new Resource(o, file);
	return o;
}


