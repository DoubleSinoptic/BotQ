#include "Indent.h"
#include "Core.h"
#include "Common/TinyFile.h"
#include "Resource.h"
#include "ModelConstructor.h"

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

void Indent::Load(const String& name) 
{
	DynamicArray<char> data;
	FileStream s(name + ".idx", OpenMode::Read);
	MpzHeader hd = s.ReadT<MpzHeader>();


}				