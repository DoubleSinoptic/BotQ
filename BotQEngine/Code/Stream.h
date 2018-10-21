#pragma once

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

typedef long long StreamOffset;
typedef unsigned char StreamByte;
typedef signed long long StreamSize;

class Stream
{ 
public:
	virtual ~Stream() {}

	virtual StreamOffset Write(const StreamByte* data, StreamSize length) = 0;
	virtual StreamOffset Read(StreamByte* data, StreamSize length) = 0;

	virtual void Seek(StreamOffset offset, SeekDir dir) = 0;
	virtual StreamSize Tell() = 0;
};