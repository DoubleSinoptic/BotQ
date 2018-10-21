#pragma once

#include "String.h"
#include "DynamicArray.h"
#include <stdio.h>

enum class OpenMode
{
	Read,
	Write
};

enum class SeekDir
{
	Beg,
	End,
	Current
};

class SGE_EXPORT FileStream
{
	FILE* mFile;
public:
	FileStream(const String& path, OpenMode mode);
	~FileStream();

	int Write(const char* data, size_t length);
	int Read(char* data, size_t length);

	void WriteLine(const String& utf8str);
	String ReadLine();

	int GetByte();
	unsigned int GetUtf8Char();

	int Seek(int offset, SeekDir dir);
	size_t Tell();

	String AllContent();
	DynamicArray<char> AllBytes();

	unsigned long long LongLength();
};