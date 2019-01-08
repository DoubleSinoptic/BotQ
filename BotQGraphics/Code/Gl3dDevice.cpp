#include "Gl3dDevice.h"
#include "glad.h"
#undef GetObject
#include <stack>
#include "Gl3dArray.h"

//enum class Gl3dFundamentalType
//{
//	Float = 0,
//	Integer = 1,
//	UInteger = 2,
//	Short = 3,
//	UShort = 4,
//	Byte = 5,
//	UByte = 6
//};

unsigned int Gl3dDevice::CastFundamentalType(Gl3dFundamentalType t)
{	
	static const GLenum table[] =
	{
		GL_FLOAT,
		GL_INT,
		GL_UNSIGNED_INT,
		GL_SHORT,
		GL_UNSIGNED_SHORT,
		GL_BYTE,
		GL_UNSIGNED_BYTE
	};
	return table[(size_t)t];
}


unsigned int Gl3dDevice::CastPrimitiveType(Gl3dPrimitive t)
{
	static const GLenum prims[] =
	{
		GL_TRIANGLES,
		GL_LINES,
		GL_POINTS
	};
	return prims[(size_t)t];
}

unsigned int Gl3dDevice::CastSideType(Gl3dSide s)
{
	if (s == Gl3dSide::Back)
		return GL_TEXTURE_2D;
	return GL_TEXTURE_CUBE_MAP_POSITIVE_X + ((int)s) - 1;
}

void Gl3dDevice::GetMemory(long long & aviable, long long & total)
{
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
	glGetInteger64v(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
		&total);
	glGetInteger64v(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
		&aviable);
}


void Gl3dDevice::LinkAddresses(Gl3dDeviceLinker lnk)
{
	if(!gladLoadGLLoader(lnk))
		throw Gl3dCoreException("error of init gl3d instance");
}


void Gl3dDevice::ThrowIfError()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		throw Gl3dCoreException("error");
	}
}

Gl3dLayoutInstance::Gl3dLayoutInstance(Gl3dLayoutDesc* desc) :
	m_object(0)
{
	Create(desc);
}

Gl3dLayoutInstance::Gl3dLayoutInstance() :
	m_object(0)
{}

void Gl3dLayoutInstance::Create(Gl3dLayoutDesc* desc)
{
	if (m_object)
	{
		glDeleteVertexArrays(1, &m_object);
		m_object = 0;
	}

	glGenVertexArrays(1, &m_object);
	if (!m_object)
		throw Gl3dCoreException("error of generate layout instance");

	if (!desc)
		throw Gl3dCoreException("invalid argument: desc = nullptr");

	glBindVertexArray(m_object);
	
	if (desc->index)
		glBindBuffer(desc->index->GetTarget(), desc->index->GetObject());
	for (size_t i = 0; i < 40; i++)
		if (desc->layouts[i].buffer)
		{

			glBindBuffer(desc->layouts[i].buffer->GetTarget(), desc->layouts[i].buffer->GetObject());
			glEnableVertexAttribArray(i);
			glVertexAttribPointer(i,
				desc->layouts[i].elementsCount,
				Gl3dDevice::CastFundamentalType(desc->layouts[i].type),
				GL_FALSE,
				desc->layouts[i].stride,
				(void*)desc->layouts[i].offset);
			if (desc->layouts[i].instanced)
				glVertexAttribDivisor(i, 1);
		}

	Gl3dDevice::ThrowIfError();
}

Gl3dLayoutInstance::~Gl3dLayoutInstance()
{
	if (m_object)
	{
		glDeleteVertexArrays(1, &m_object);
		m_object = 0;
	}	
}

void Gl3dLayoutInstance::DrawIndexed(Gl3dPrimitive prim, Gl3dFundamentalType t, size_t count)
{
	glBindVertexArray(m_object);
	glDrawElements(Gl3dDevice::CastPrimitiveType(prim), count, Gl3dDevice::CastFundamentalType(t), nullptr);
}

void Gl3dLayoutInstance::DrawIndexedInstanced(Gl3dPrimitive prim, Gl3dFundamentalType t, size_t count, size_t instCount)
{
	glBindVertexArray(m_object);
	glDrawElementsInstanced(Gl3dDevice::CastPrimitiveType(prim), count, Gl3dDevice::CastFundamentalType(t), nullptr, instCount);
}

void Gl3dLayoutInstance::Draw(Gl3dPrimitive prim, size_t vertCount)
{
	glBindVertexArray(m_object);
	glDrawArrays(Gl3dDevice::CastPrimitiveType(prim), 0, vertCount);
}