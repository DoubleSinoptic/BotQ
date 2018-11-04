#include "ResourceLoader.h"
#include "GameInstance.h"
#include "Resource.h"
#include "Graphics/Texture.h"
#include "MeshImporter.h"
#include <experimental/filesystem>
#include "Common/TinyFile.h"
#include "Graphics/Texture.h"
#include "Graphics/Bitmap.h"
#include "Common/PathHelper.h"
#include "../../zlib_static/Code/zlib.h"
#include "Audio/AudioSource.h"
//#include "Common/ArrayStreamIterator.h"
using namespace std::experimental;

#pragma pack(push, 1)
struct PK_Header
{
	std::uint32_t signature;
	std::uint16_t version;
	std::uint16_t __padding0;
	std::uint16_t compression_method;

	std::uint16_t __padding1;
	std::uint16_t __padding2;

	std::uint32_t crc32;

	std::uint32_t compresses_size;
	std::uint32_t uncompressed_size;

	std::uint16_t filename_size;
	std::uint16_t extra_size;

	bool IsValid() const 
	{
		return signature == 0x04034b50;
	}
};
#pragma pack(pop)


void UnpackData(Ref<char[]> datain, size_t inlen, Ref<char[]> dataout, size_t outlen, int level) 
{
	
}

struct Def { String path; Ref<DynamicArray<char>> data; };

void InstantResource(DynamicArray <Def>& coruntune, const String& fpath, Ref<DynamicArray<char>> dt)
{
	String ext = Path::FileExt(fpath);

	if (ext == "txt" || ext == "TXT" ||
		ext == "mtl" || ext == "MTL")
	{
		String text(dt->Length(), ' ');
		memcpy(&text[0], dt->GetData(), dt->Length());
		new Resource(text, fpath);
	}
	else if (ext == "wav" || ext == "WAV")
	{
		new Resource(New<AudioClip>(*dt), fpath);
	}
	else if (ext == "ttf" || ext == "ttf")
	{
		new Resource(dt, fpath);
	}
	else if (ext == "hdr" || ext == "HDR")
	{
		new Resource(dt, fpath);
	}
	else if (ext == "fbx" || ext == "FBX")
	{
		coruntune.Add({ fpath, dt });
	}
	else if (ext == "obj" || ext == "OBJ")
	{
		coruntune.Add({ fpath, dt });
	}
	else if (ext == "btga" || ext == "BTGA" ||
		ext == "bpng" || ext == "BPNG" ||
		ext == "bjpg" || ext == "BJPG" ||
		ext == "bbmp" || ext == "BBMP" ||
		ext == "bdds" || ext == "BDDS")
	{
		new Resource(Ref<Bitmap>(new Bitmap(dt->GetData(), dt->Length())), fpath);
	}
	else if (ext == "tga" || ext == "TGA" ||
		ext == "png" || ext == "PNG" ||
		ext == "jpg" || ext == "JPG" ||
		ext == "bmp" || ext == "BMP" ||
		ext == "dds" || ext == "DDS")
	{
		Ref<Texture> texture = new Texture();
		texture->SetFromBitmap(new Bitmap(dt->GetData(), dt->Length()));
		new Resource(texture, fpath);
	}

}



void ResourceLoader::LoadResourcesPk2(const String & path)
{
	FileStream s(path, OpenMode::Read);

	
	DynamicArray < Def > coruntune;

	char extra[1024];
	char filename[1024];
	PK_Header header;
	while (s.Read((char*)&header, sizeof(header)) == sizeof(PK_Header))
	{
		if (!header.IsValid())
		{
			break;
		}

		if (header.filename_size >= sizeof(filename) ||
			header.extra_size >= sizeof(extra))
		{

		}

		s.Read(filename, header.filename_size);
		filename[header.filename_size] = '\0';

		s.Read(extra, header.extra_size);

		String fpath = Path::PathFix(filename);
		String ext = Path::FileExt(fpath);

	
		Ref<DynamicArray<char>> digital = New<DynamicArray<char>>();
		if (header.compression_method == 0)
		{
			digital->Initialize(header.uncompressed_size);
			s.Read(digital->GetData(), digital->Length());
		}
		else if (header.compression_method == 8)
		{
			Ref<DynamicArray<char>> bytes = New<DynamicArray<char>>();
			bytes->Initialize(header.compresses_size);
			s.Read(bytes->GetData(), bytes->Length());

			digital->Initialize(header.uncompressed_size);

			Bytef* dest = (Bytef*)digital->GetData();
			uLongf destLen = digital->Length();

			Bytef* src = (Bytef*)bytes->GetData();
			uLongf srcLen = bytes->Length();
			uncompress(dest, &destLen, src, srcLen);
		}
		else
			Error("ivalid compression type");
	
		InstantResource(coruntune, fpath, digital);

	}

	for (auto i : coruntune)
	{
		MeshImporter::ImportRW(i.path, i.data->GetData(), i.data->Length());
	}

}


void ResourceLoader::LoadResourcesPk(const String & path)
{
	LoadResourcesPk2(path);
	return;

	FileStream s(path, OpenMode::Read);

	struct Def { String path;  Ref<char[]> data; size_t len; };
	DynamicArray < Def > coruntune;

	char extra[1024];
	char filename[1024];
	PK_Header header;
	while (s.Read((char*)&header, sizeof(header)) == sizeof(PK_Header))
	{
		if (!header.IsValid())
		{
			break;
		}

		if (header.filename_size >= sizeof(filename) ||
			header.extra_size >= sizeof(extra))
		{

		}

		s.Read(filename, header.filename_size);

		filename[header.filename_size] = '\0';
		s.Read(extra, header.extra_size);

		String fpath = Path::PathFix(filename);
		String ext = Path::FileExt(fpath);

		if (header.compression_method != 0)
		{
			Log("error of import file: %s: compression not supported", *fpath);
			continue;
		}

		if (ext == "txt" || ext == "TXT" ||
			ext == "mtl" || ext == "MTL")
		{
			String text(header.uncompressed_size, ' ');
			s.Read(&text[0], header.uncompressed_size);
			new Resource(text, fpath);
		}
		else if (ext == "ttf" || ext == "ttf")
		{
			Ref<DynamicArray<char>> dt(new DynamicArray<char>());
			dt->Initialize(header.uncompressed_size);
			s.Read(dt->GetData(), header.uncompressed_size);
			new Resource(dt, fpath);
		}
		else if (ext == "hdr" || ext == "HDR")
		{
			Ref<DynamicArray<char>> dt(new DynamicArray<char>());
			dt->Initialize(header.uncompressed_size);
			s.Read(dt->GetData(), header.uncompressed_size);
			new Resource(dt, fpath);
		}
		else if (ext == "fbx" || ext == "FBX")
		{
			Ref<char[]> data = new char[header.uncompressed_size];
			s.Read(data.GetPtr(), header.uncompressed_size);
			coruntune.Add({ fpath, data, header.uncompressed_size });
		}
		else if (ext == "obj" || ext == "obj")
		{
			Ref<char[]> data = new char[header.uncompressed_size];
			s.Read(data.GetPtr(), header.uncompressed_size);
			coruntune.Add({ fpath, data, header.uncompressed_size });
		}
		else if (ext == "btga" || ext == "BTGA" ||
			ext == "bpng" || ext == "BPNG" ||
			ext == "bjpg" || ext == "BJPG" ||
			ext == "bbmp" || ext == "BBMP" ||
			ext == "bdds" || ext == "BDDS")
		{
			Ref<char[]> data = new char[header.uncompressed_size];
			s.Read(data.GetPtr(), header.uncompressed_size);
			new Resource(Ref<Bitmap>(new Bitmap(data.GetPtr(), header.uncompressed_size)), fpath);
		}
		else if (ext == "tga" || ext == "TGA" ||
			ext == "png" || ext == "PNG" ||
			ext == "jpg" || ext == "JPG" ||
			ext == "bmp" || ext == "BMP" ||
			ext == "dds" || ext == "DDS")
		{
			Ref<char[]> data = new char[header.uncompressed_size];
			s.Read(data.GetPtr(), header.uncompressed_size);
			Ref<Texture> texture = new Texture();
			texture->SetFromBitmap(new Bitmap(data.GetPtr(), header.uncompressed_size));
			new Resource(texture, fpath);
		}
		else
		{
			s.Seek(header.uncompressed_size, SeekDir::Current);
		}
	}

	for (auto i : coruntune)
	{
		MeshImporter::ImportRW(i.path, i.data.GetPtr(), i.len);
		//MeshImporter::ImportRW(i.path, i.data.GetPtr(), i.len);
	}

}

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
void ResourceLoader::LoadStaticAssets(const String& apth)
{
	for (auto i : fs::directory_iterator(apth.c_str()))
	{
		const fs::path& pf = i.path();
		if (fs::is_regular_file(i))
		{
			if (Path::FileExt(pf.u8string().c_str()) == "indent")
			{
				ResourceLoader::LoadResourcesPk2(pf.u8string().c_str());
				Log("loaded resource file: %s", *Path::FileName(Path::PathFix(pf.u8string().c_str())));
			}
		}
	}

}
