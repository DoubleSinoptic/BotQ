#include "ResourceLoader.h"
#include "GameInstance.h"
#include "Resource.h"
#include "Graphics/Texture.h"
#include "MeshImporter.h"

#include "Common/TinyFile.h"
#include "Graphics/Texture.h"
#include "Graphics/Bitmap.h"
#include "Common/PathHelper.h"
#include "../../zlib_static/Code/zlib.h"
#include "Audio/AudioSource.h"

#ifdef __EXPERENMENTAL_FS
#include <experimental/filesystem>
using namespace std::experimental;
namespace fs = std::experimental::filesystem;
#endif

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
		SamplerStateDesc s = {};
		s.addressModeU = TextureAddressMode::Repeat;
		s.addressModeV = TextureAddressMode::Repeat;
		s.addressModeW = TextureAddressMode::Repeat;
		s.mag = FilterMode::Linear;
		s.min = FilterMode::MipLinear;
		texture->SetSampler(s);
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
#ifdef __EXPERENMENTAL_FS
template<typename T>
void loadFromDir(const T& x, const fs::path& path, const String& startSeg) 
{
	for (auto i : fs::directory_iterator(path))
	{
		if (fs::is_directory(i))
		{
			loadFromDir(x, i.path(), startSeg + "/" + i.path().filename().string().c_str());
		}
		else
		{
			x(String(i.path().string().c_str()), startSeg + "/" +i.path().filename().string().c_str());
		}
	}
}
#endif
void ResourceLoader::LoadResourcesFromDir(const String & path)
{
#ifdef __EXPERENMENTAL_FS
	DynamicArray < Def > coruntune;

	loadFromDir([&](const String& file, const String& fileName)
	{
		printf("asdasd\n");
		FileStream f(file, OpenMode::Read);

		String fpath = Path::PathFix(fileName);
		String ext = Path::FileExt(fileName);

		Ref<DynamicArray<char>> digital = New<DynamicArray<char>>();
		digital->Initialize(f.LongLength());
		f.Read(digital->GetData(), digital->Length());

		InstantResource(coruntune, fpath, digital);
	}, path.c_str(), "/");

	for (auto i : coruntune)
	{
		MeshImporter::ImportRW(i.path, i.data->GetData(), i.data->Length());
	}
#else
	throw - 1;
#endif
}

void ResourceLoader::LoadResourcesPk(const String & path)
{
	LoadResourcesPk2(path);
	return;
}



void ResourceLoader::LoadStaticAssets(const String& apth)
{
#ifdef __EXPERENMENTAL_FS
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
#else
	throw - 1;
#endif
}
