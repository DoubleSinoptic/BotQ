#include "Gl3dVertexArray_3x1.h"
#include "glad.h"
#undef GetObject

class Gl3dVertexArray_3x1Impl 
{
public:
	GLenum drawtype;
	GLuint vao = 0;
	GLuint buffs[5] = { 0 };
	int indecesCount = 0;
	Gl3dVertexArray_3x1Impl() 
	{
		drawtype = GL_STATIC_DRAW;
		glGenVertexArrays(1, &vao);
		glGenBuffers(5, buffs);	
		g3dlog("G3D$: struct vao(%u) vbos(%d, %d, %d, %d, %d)", vao, buffs[0], buffs[1], buffs[2], buffs[3], buffs[4]);
		if (vao < 1)
			throw Gl3dCoreException("error of create vao");
		if (buffs[0] < 1)
			throw Gl3dCoreException("error of create vbo 1");
		if (buffs[1] < 1)
			throw Gl3dCoreException("error of create vbo 2");
		if (buffs[2] < 1)
			throw Gl3dCoreException("error of create vbo 3");
		if (buffs[3] < 1)
			throw Gl3dCoreException("error of create vbo 4");
		if (buffs[4] < 1)
			throw Gl3dCoreException("error of create vbo 5");
	}

	~Gl3dVertexArray_3x1Impl() 
	{
		g3dlog("G3D$: destruct vao(%u) vbos(%d, %d, %d, %d, %d)", vao, buffs[0], buffs[1], buffs[2], buffs[3], buffs[4]);
		glDeleteBuffers(5, buffs);
		glDeleteVertexArrays(1, &vao);
	}
};

Gl3dVertexArray_3x1::Gl3dVertexArray_3x1()
	: mImpl(new Gl3dVertexArray_3x1Impl())
{}

Gl3dVertexArray_3x1::~Gl3dVertexArray_3x1()
{
	if (mImpl)
		delete mImpl;
}

void Gl3dVertexArray_3x1::DrawLines()
{
	glBindVertexArray(mImpl->vao);
	glDrawElements(GL_LINES, mImpl->indecesCount, GL_UNSIGNED_INT, NULL);
}

void Gl3dVertexArray_3x1::DrawTriangles()
{
	glBindVertexArray(mImpl->vao);
	glDrawElements(GL_TRIANGLES, mImpl->indecesCount, GL_UNSIGNED_INT, NULL);
}

void Gl3dVertexArray_3x1::DrawTrianglesStrip()
{
	glBindVertexArray(mImpl->vao);
	glDrawElements(GL_TRIANGLE_STRIP, mImpl->indecesCount, GL_UNSIGNED_INT, NULL);
}

void Gl3dVertexArray_3x1::DrawAllTriangles(int vertecesCount)
{
	glBindVertexArray(mImpl->vao);
	glDrawArrays(GL_TRIANGLES, 0, vertecesCount);
}

void Gl3dVertexArray_3x1::EvaluteToStream()
{
	mImpl->drawtype = GL_DYNAMIC_DRAW;
}

void Gl3dVertexArray_3x1::SetVertexesNative(const char * data, size_t len)
{
	glBindVertexArray(mImpl->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mImpl->buffs[0]);
	glBufferData(GL_ARRAY_BUFFER, len, data, mImpl->drawtype);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

}

void Gl3dVertexArray_3x1::SetNormalsNative(const char * data, size_t len)
{
	
	glBindVertexArray(mImpl->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mImpl->buffs[1]);
	glBufferData(GL_ARRAY_BUFFER, len, data, mImpl->drawtype);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,0, (void*)0);
}

void Gl3dVertexArray_3x1::SetTexcoordsNative(const char * data, size_t len)
{
	glBindVertexArray(mImpl->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mImpl->buffs[2]);
	glBufferData(GL_ARRAY_BUFFER, len, data, mImpl->drawtype);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void Gl3dVertexArray_3x1::SetIndexesNative(const char * data, size_t len)
{
	mImpl->indecesCount = static_cast<int>(len / sizeof(unsigned int));
	glBindVertexArray(mImpl->vao);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mImpl->buffs[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, len, data, mImpl->drawtype);
}

void Gl3dVertexArray_3x1::SetTangetsNative(const char * data, size_t len)
{
	glBindVertexArray(mImpl->vao);
	glBindBuffer(GL_ARRAY_BUFFER, mImpl->buffs[4]);
	glBufferData(GL_ARRAY_BUFFER, len, data, mImpl->drawtype);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

int Gl3dVertexArray_3x1::GetIndexesCount() const
{
	return mImpl->indecesCount;
}
