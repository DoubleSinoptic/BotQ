#include "Gl3dArray.h"

#include "glad.h"
#undef GetObject

Gl3dArrayImpl::~Gl3dArrayImpl()
{
	if (mId)
		glDeleteBuffers(1, &mId);
}

const GLenum bufferTargets[] = 
{
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER,
	GL_ARRAY_BUFFER,
	GL_ELEMENT_ARRAY_BUFFER
};

const GLenum bufferDataTypes[] =
{
	GL_STATIC_DRAW,
	GL_STATIC_DRAW,
	GL_DYNAMIC_DRAW,
	GL_DYNAMIC_DRAW
};

Gl3dArrayImpl::Gl3dArrayImpl(Gl3dArrayTarget target_va)
{
	glGenBuffers(1, &mId);	
	g3dlog("G3D$: struct vbo(%d)", mId);
	mTarget = (unsigned int)target_va;
}

unsigned int Gl3dArrayImpl::GetTarget() const
{
	return bufferTargets[mTarget];
}

unsigned int Gl3dArrayImpl::GetObject() const
{
	return mId;
}

void Gl3dArrayImpl::__reserve_data(const void * data, size_t length)
{
	glBindBuffer(bufferTargets[mTarget], mId);
	glBufferData(bufferTargets[mTarget], length, data, bufferDataTypes[mTarget]);
}

void Gl3dArrayImpl::__write_data(const void * data, int offset, size_t length)
{
	glBindBuffer(bufferTargets[mTarget], mId);
	glBufferSubData(bufferTargets[mTarget], offset, length, data);
}
