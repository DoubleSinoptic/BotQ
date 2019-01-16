

#include "pch.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include "../zlib_static/Code/deflate.h"

#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
namespace fs = std::experimental::filesystem;

uint64_t strhrd(const char* s) 
{
	uint64_t x;
	snprintf((char*)&x, sizeof(x), "%s", s);
	return x;
}

struct MpzHeader 
{
	uint64_t header = strhrd("INDENT");
	uint64_t s_compressed;
	uint64_t s_uncompressed;
};


struct ObjText
{
	uint64_t header = strhrd("TEXT");
	uint64_t s_path;
	uint64_t s_data;
};

struct ObjFirstHeader
{
	uint64_t header = strhrd("FOBJECT");
	uint64_t s_path;
};

struct ObjHeader 
{
	uint64_t header = strhrd("OBJECT");
	uint64_t s_name;
	uint64_t n_mesh;
	uint64_t n_parents;
	struct transform 
	{
		float pos[3];
		float rot[4];
	}  transform;
};

struct ObjMesh
{
	uint64_t header = strhrd("MESH");
	uint64_t s_indeces;
	uint64_t s_vertexes;
};

struct BmpHeader 
{
	uint64_t header = strhrd("IMAGE");
	uint64_t s_path;
	uint64_t s_data;
	uint64_t w;
	uint64_t h;
	uint64_t chanels;
	uint64_t flt;
};

struct MemStream 
{
	std::vector<char> data;
	
	template<typename T>
	void write(const T& x) 
	{	
		data.insert(data.end(), ((const char*)&x), ((const char*)&x) + sizeof(T));
	}

	template<typename T>
	void write(const T* x, size_t count)
	{
		data.insert(data.end(), ((const char*)x), ((const char*)x) + count * sizeof(T));
	}
};

struct tcoord
{
	float x;
	float y;
};

void rootBuild(MemStream& mem, const aiScene* scene, const struct aiNode* nd)
{
	Assert(scene != nullptr);
	Assert(nd != nullptr);
	
	aiVector3D scale;
	aiQuaternion rot;
	aiVector3D pos;
	aiMatrix4x4 m = nd->mTransformation;
	m.DecomposeNoScaling(rot, pos);
	
	ObjHeader object;
	object.s_name = strlen(nd->mName.C_Str()) + 1;
	object.n_parents = nd->mNumChildren;
	object.n_mesh = nd->mNumMeshes;
	memcpy(object.transform.pos, &pos, sizeof(aiVector3D));
	memcpy(object.transform.rot, &rot, sizeof(aiVector3D));
	mem.write(object);
	mem.write(nd->mName.C_Str(), strlen(nd->mName.C_Str()) + 1);

	for (unsigned int n = 0; n < nd->mNumMeshes; n++)
	{
		const aiMesh* asmesh = scene->mMeshes[nd->mMeshes[n]];
	
		std::vector<tcoord> c;
		c.reserve(asmesh->mNumVertices);
		if (asmesh->HasTextureCoords(0))
			for (int i = 0; i < asmesh->mNumVertices; i++)	
				c.push_back(tcoord{ asmesh->mTextureCoords[0][i].x, asmesh->mTextureCoords[0][i].y });
			
		std::vector<unsigned int> arr;
		arr.reserve(asmesh->mNumFaces * 6);
		for (unsigned int t = 0; t < asmesh->mNumFaces; ++t)
		{
			aiFace* face = &asmesh->mFaces[t];
			if (face->mNumIndices == 3)
			{
				unsigned int ix0 = face->mIndices[0];
				unsigned int ix1 = face->mIndices[1];
				unsigned int ix2 = face->mIndices[2];
				arr.push_back(ix0);
				arr.push_back(ix1);
				arr.push_back(ix2);
			}
			else if (face->mNumIndices == 2)
			{
				unsigned int ix0 = face->mIndices[0];
				unsigned int ix2 = face->mIndices[1];
				arr.push_back(ix0);
				arr.push_back(ix2);
				arr.push_back(ix0);

			}
			else if (face->mNumIndices == 4)
			{
				unsigned int ix0 = face->mIndices[0];
				unsigned int ix1 = face->mIndices[1];
				unsigned int ix2 = face->mIndices[2];
				unsigned int ix3 = face->mIndices[3];
				arr.push_back(ix0);
				arr.push_back(ix1);
				arr.push_back(ix2);
				arr.push_back(ix0);
				arr.push_back(ix2);
				arr.push_back(ix3);
			}
			else
				throw - 1;

		}
	
		ObjMesh mesh;
		mesh.s_indeces = arr.size();
		mesh.s_vertexes = asmesh->mNumVertices;
		mem.write(mesh);
		mem.write(asmesh->mVertices, asmesh->mNumVertices);
		mem.write(c.data(), asmesh->mNumVertices);
		mem.write(asmesh->mNormals ? asmesh->mNormals : asmesh->mVertices, asmesh->mNumVertices);
		mem.write(asmesh->mTangents ? asmesh->mTangents : asmesh->mVertices, asmesh->mNumVertices);
		mem.write(arr.data(), arr.size());
	}

	for (unsigned int n = 0; n < nd->mNumChildren; ++n)
	{
		rootBuild(mem, scene, nd->mChildren[n]);
	}
}


void import_fbx(MemStream& out, const fs::path& path, const std::string& extend)
{
	const aiScene* sc = aiImportFile(path.string().c_str(), aiProcess_Triangulate | aiProcessPreset_TargetRealtime_MaxQuality | aiProcess_FlipUVs | aiProcess_CalcTangentSpace | aiProcess_OptimizeMeshes);
	if (!sc)
		return;

	ObjFirstHeader f;
	f.s_path = extend.size() + 1;
	out.write(f);
	out.write(extend.data(), extend.size() + 1);
    rootBuild(out, sc, sc->mRootNode);
	aiReleaseImport(sc);
}

void algo(MemStream& out, const fs::path& path, const std::string& extend)
{
	for (auto i : fs::directory_iterator(path)) 
	{
		if (fs::is_directory(i))
		{
			algo(out, i.path(), extend + "/" + i.path().filename().string());
		}
		else 
		{
			std::string path = extend + "/" + i.path().filename().string();
			std::string ext = i.path().extension().string();
			for (auto& x : ext)
				x = tolower(x);
			
			printf("imported: %s\n", path.c_str());
			if (ext == ".png" || ext == ".bmp" || ext == ".jpg" || ext == ".tga")
			{
				FILE* f = fopen(i.path().string().c_str(), "rb");
				int x, y, comp;
				stbi_uc* data = stbi_load_from_file(f, &x, &y, &comp, 4);
				if(f)
					fclose(f);
				if (data)
				{
					BmpHeader header;
					header.w = x;
					header.h = y;
					header.chanels = comp;
					header.flt = false;
					header.s_path = path.size() + 1;
					header.s_data = x * y * header.chanels;
					out.write(header);
					out.write(path.c_str(), header.s_path);
					out.write(data, header.s_data);
					stbi_image_free(data);
				}	
				printf("PNG: %s\n", path.c_str());
			}
			else if (ext == ".fbx")
			{
				import_fbx(out, i.path(), path);
				printf("FBX: %s\n", path.c_str());
			}
			else if (ext == ".txt" || ext == ".xml" || ext == ".h" || ext == ".cfg")
			{
				std::ifstream s(i.path().string(), std::ios::binary);
				s.seekg(0, std::ios::end);
				size_t len = s.tellg();
				s.seekg(0, std::ios::beg);
				std::vector<char> d(len + 1, '\0');
				s.read(d.data(), d.size());
				ObjText txt;
				txt.s_data = d.size();
				txt.s_path = path.size() + 1;
				out.write(txt);
				out.write(path.data(), path.size() + 1);
				out.write(d.data(), d.size());

				printf("TEXT: %s\n", path.c_str());
			}
			else 
			{
				printf("no imported: %s\n", path.c_str());
			}
		}
	}
}
template<typename... Args>
const char* strprint(const char* cf, const Args&... a) 
{
	static char buffer[1024];
	snprintf(buffer, 1024, cf, a...);
	return buffer;
}

const size_t partSize = 1024 * 1024 * 10;

int main(int argc, char** argv)
{
	for (int i = 1; i < argc; i++) 
	{
		fs::path f = fs::absolute(argv[i]);
		fs::path idx = fs::absolute(f.filename().string() + std::string(".idx"));
		std::ofstream idxOut(idx, std::ios::binary);

		MemStream mem;
		algo(mem, f, "");
		std::vector<char> compressed(mem.data.size());
		uLongf d;
		const char* start = compressed.data();
		printf("(compressing)\n");
		compress((Bytef*)start, &d, (const Bytef*)mem.data.data(), mem.data.size());


		MpzHeader h;
		h.s_compressed = d;
		h.s_uncompressed = mem.data.size();
		idxOut.write((const char*)&h, sizeof(h));

		int index = 0;
		while (d > (partSize * 2))
		{
			fs::path of = fs::absolute(f.filename().string() + std::string(strprint("%02X", index)) + std::string(".indent"));
			std::ofstream outFile(of, std::ios::binary);
			outFile.write(start, partSize);
			start += partSize;
			d -= partSize;
			index++;
		}
		fs::path of = fs::absolute(f.filename().string() + std::string(strprint("%02X", index)) + std::string(".indent"));
		std::ofstream outFile(of, std::ios::binary);
		outFile.write(start, d);
	}
	printf("(succsessfull)\n");
	system("pause");
}

