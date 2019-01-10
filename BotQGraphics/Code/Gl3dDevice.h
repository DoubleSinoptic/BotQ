#pragma once

#include "Gl3dCore.h"

enum class Gl3dSide 
{
	Back = 0,
	PosX = 1,
	NegX = 2,
	PosY = 3,
	NegY = 4,
	PosZ = 5,
	NegZ = 6,
};

enum class Gl3dFundamentalType 
{
	Float = 0,
	Integer = 1,
	UInteger = 2,
	Short = 3,
	UShort = 4,
	Byte = 5,
	UByte = 6
};

struct Gl3dVertexLatoutAttrib 
{
	class Gl3dArrayImpl*buffer;
	size_t				elementsCount;
	Gl3dFundamentalType type;
	size_t				stride;
	size_t				offset;
	bool				instanced;
};

struct Gl3dLayoutDesc 
{
	Gl3dVertexLatoutAttrib layouts[40];
	Gl3dArrayImpl*		   index;
};

enum class Gl3dPrimitive 
{
	Triangles = 0,
	Lines = 1,
	Points = 2 
};

class GL3DC_EXPORT Gl3dLayoutInstance
{
	unsigned int m_object;
public:
	Gl3dLayoutInstance(Gl3dLayoutDesc* desc);
	Gl3dLayoutInstance();
	void Create(Gl3dLayoutDesc* desc);
	Gl3dLayoutInstance(const Gl3dLayoutInstance&) = delete;
	Gl3dLayoutInstance& operator=(const Gl3dLayoutInstance&) = delete;
	~Gl3dLayoutInstance();
	void DrawIndexed(Gl3dPrimitive prim, Gl3dFundamentalType t, size_t count) const;
	void DrawIndexedInstanced(Gl3dPrimitive prim, Gl3dFundamentalType t, size_t count, size_t instCount) const;
	void Draw(Gl3dPrimitive prim, size_t vertCount) const;
};

typedef void* (*Gl3dDeviceLinker)(const char *name);

class GL3DC_EXPORT Gl3dDevice
{
public:
	static unsigned int CastFundamentalType(Gl3dFundamentalType t);
	static unsigned int CastPrimitiveType(Gl3dPrimitive t);
	static unsigned int CastSideType(Gl3dSide s);

	static void GetMemory(long long& aviable, long long& total);
	static void LinkAddresses(Gl3dDeviceLinker lnk);
	static void ThrowIfError();
};


