#include "TinyFile.h"
#include "../Common/Log.h"
#include <stdio.h>

FileStream::FileStream(const String & path1, OpenMode mode)
	: mFile(nullptr)
{
	String path = path1;
	if (path.IsEmpty())
		Error("error of open file: path is empty");
	if (path[0] == '/')
		path = "." + path;
	char modes[] = "rb";
	if (mode == OpenMode::Read)
		modes[0] = 'r';
	else
		modes[0] = 'w';
	mFile = fopen(path.c_str(), modes);
	if (!mFile) 
		Error("error of open file: %s", *path);	
}

FileStream::~FileStream()
{
	if (mFile)
		fclose(mFile);
}

int FileStream::Write(const char* data, size_t length) 
{
	return fwrite(data, 1, length, mFile);
}

int FileStream::Read(char* data, size_t length) 
{
	return fread(data, 1, length, mFile);
}

void FileStream::WriteLine(const String& utf8str) 
{
	fwrite(utf8str.c_str(), 1, utf8str.Length(), mFile);
	putc('\n', mFile);
}

String FileStream::ReadLine() 
{
	String rez;
	while (true)
	{
		int c = getc(mFile);
		if (c == EOF || c == '\n')
			break;
		rez += c;
	}
	return rez;
}

int FileStream::GetByte() 
{
	return getc(mFile);
}

unsigned int FileStream::GetUtf8Char() 
{
	return getc(mFile);
}

int FileStream::Seek(int offset, SeekDir dir)
{
	int nativemod = 0;
	switch (dir)
	{
	case SeekDir::Beg:
		nativemod = SEEK_SET;
		break;
	case SeekDir::End:
		nativemod = SEEK_END;
		break;
	case SeekDir::Current:
		nativemod = SEEK_CUR;
		break;
	}
	return fseek(mFile, offset, nativemod);
}

size_t FileStream::Tell() 
{
	return ftell(mFile);
}

String FileStream::AllContent()
{
	Seek(0, SeekDir::End);
	size_t sz = Tell();
	Seek(0, SeekDir::Beg);
	String rd(sz, 0);
	Read(&rd[0], sz);
	return rd;
}

DynamicArray<char> FileStream::AllBytes()
{
	Seek(0, SeekDir::End);
	size_t sz = Tell();
	Seek(0, SeekDir::Beg);
	DynamicArray<char> rd(sz, char(0));
	Read(&rd[0], sz);
	return rd;
}

unsigned long long FileStream::LongLength()
{
	Seek(0, SeekDir::End);
	unsigned long long d = Tell();
	Seek(0, SeekDir::Beg);
	return d;
}
