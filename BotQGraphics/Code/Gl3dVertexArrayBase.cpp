#include "Gl3dVertexArrayBase.h"
#include "glad.h"
#undef GetObject

class Gl3dVertexArrayBaseImpl 
{
public:
	Gl3dVertexArrayBase * _this;
	Gl3dVertexArrayBaseImpl(Gl3dVertexArrayBase* __this)
		: _this(__this) 
	{
		glGenVertexArrays(1, &vao);
		g3dlog("G3D$: struct vao(%d)", vao);
		glBindVertexArray(vao);
	}
	
	~Gl3dVertexArrayBaseImpl() 
	{
		if (vao)
			glDeleteVertexArrays(1, &vao);
	}
	GLuint vao = 0;
};

Gl3dVertexArrayBase::Gl3dVertexArrayBase()
	: mImpl(new Gl3dVertexArrayBaseImpl(this))
{
}

GLenum primitiveOf(Gl3dDrawPrimitive p) 
{
	GLenum prim;
	switch (p)
	{
	case Gl3dDrawPrimitive::Triangles:
		prim = GL_TRIANGLES;
		break;
	case Gl3dDrawPrimitive::Quads:
		prim = GL_QUADS;
		break;
	case Gl3dDrawPrimitive::Lines:
		prim = GL_LINES;
		break;	
	default:
		throw Gl3dCoreException("Invalid primitive type");
		break;
	}
	return prim;
}



Gl3dVertexArrayBase::~Gl3dVertexArrayBase()
{
	if (mImpl)
		delete mImpl;
}

void Gl3dVertexArrayBase::AttachImpl(int index, Gl3dArrayImpl * impl, size_t elemSize)
{
	glBindVertexArray(mImpl->vao);
	glBindBuffer(impl->GetTarget(), impl->GetObject());
	if (impl->GetTarget() == GL_ARRAY_BUFFER) 
	{
	
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, elemSize / sizeof(float), GL_FLOAT, GL_FALSE, elemSize, 0);
	}
	
}

const GLenum Gl3dDataSplitTypeGlFormated[] =
{
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_FLOAT,
	GL_UNSIGNED_INT,
	GL_UNSIGNED_SHORT,
	GL_UNSIGNED_INT
};

const GLenum Gl3dDataSplitTypeGlFormatedCount[] =
{
	1,
	2,
	3,
	4,
	1,
	1,
	sizeof(void*) / 4
};

void Gl3dVertexArrayBase::AttachImplSplit(int index, const std::initializer_list<Gl3dDataSplitType>& splits, Gl3dArrayImpl * impl, size_t elemSize)
{
	glBindVertexArray(mImpl->vao);
	glBindBuffer(impl->GetTarget(), impl->GetObject());
	if (impl->GetTarget() == GL_ARRAY_BUFFER)
	{
		size_t offsetStart = 0;
		size_t stride = 0;
		for (int i = 0; i < splits.size(); i++)
			stride += Gl3dDataSplitTypeSize[(int)splits.begin()[i]];
		for (int i = 0; i < splits.size(); i++)
		{
			Gl3dDataSplitType currentType = splits.begin()[i];
			size_t elemSize = Gl3dDataSplitTypeSize[(int)currentType];
			GLenum elemType = Gl3dDataSplitTypeGlFormated[(int)currentType];
			int elemCount = Gl3dDataSplitTypeGlFormatedCount[(int)currentType];

			glEnableVertexAttribArray(index + i);
			glVertexAttribPointer(index + i, elemCount, elemType, GL_FALSE, stride, (void*)offsetStart);
			offsetStart += elemSize;
		}
		
	}
}

void Gl3dVertexArrayBase::AttachInstnacedImpl(int index, Gl3dArrayImpl* impl, size_t elemSize, int elemCount)
{
	glBindVertexArray(mImpl->vao);
	glBindBuffer(impl->GetTarget(), impl->GetObject());
	if (impl->GetTarget() == GL_ARRAY_BUFFER)
	{
		for (int i = 0; i < elemCount; i++)
		{
			glEnableVertexAttribArray(index + i);
			glVertexAttribPointer(index + i, elemCount, GL_FLOAT, GL_FALSE, elemSize * elemCount, (void*)(elemSize * i));
			glVertexAttribDivisor(index + i, 1);
		}

	}
}

void Gl3dVertexArrayBase::Draw(Gl3dDrawPrimitive primitive, int n)
{
	glBindVertexArray(mImpl->vao);
	glDrawArrays(primitiveOf(primitive), 0, n);
}

void Gl3dVertexArrayBase::DrawIndexed(Gl3dDrawPrimitive primitive, int n)
{
	glBindVertexArray(mImpl->vao);
	glDrawElements(primitiveOf(primitive), n, GL_UNSIGNED_INT, nullptr);
}

void Gl3dVertexArrayBase::DrawInstnaced(Gl3dDrawPrimitive primitive, int n, int drawCount)
{
	glBindVertexArray(mImpl->vao);
	glDrawElementsInstanced(primitiveOf(primitive), n, GL_UNSIGNED_INT, nullptr, drawCount);
}
