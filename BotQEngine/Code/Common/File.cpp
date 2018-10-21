#include "File.h"
#include <stdio.h>

class SafeCloseFile 
{
	FILE* f;
public:
	SafeCloseFile(FILE* _f) : 
		f(_f)
	{}

	operator FILE*() const 
	{
		return f;
	}

	~SafeCloseFile() 
	{
		if (f)
			fclose(f);
	}
};

DynamicArray<char> File::ReadAllBytes(const String & fileName)
{
	SafeCloseFile f = fopen(fileName.c_str(), "rb");
	if (!f)
		throw std::runtime_error("ivalid file name or access");
	fseek(f, 0, SEEK_END);
	size_t s = ftell(f);
	fseek(f, 0, SEEK_CUR);
	DynamicArray<char> dynamit;
	dynamit.Initialize(s);
	fread(dynamit.GetData(), 1, s, f);
	return std::move(dynamit);
}

String File::ReadAllText(const String & fileName)
{
	SafeCloseFile f = fopen(fileName.c_str(), "r");
	if (!f)
		throw std::runtime_error("ivalid file name or access");
	fseek(f, 0, SEEK_END);
	size_t s = ftell(f);
	fseek(f, 0, SEEK_CUR);
	String dynamit(s,'\0');
	fread((char*)dynamit.c_str(), 1, s, f);
	return std::move(dynamit);
}
