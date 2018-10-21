#include "Gl3dTexture.h"
#include "Gl3dDevice.h"
#include "glad.h"
#undef GetObject
#include <chrono>
class Gl3dTextureImpl
{
public:
	GLenum mType = 0;
	GLuint mTexture = 0;
	Gl3dTextureImpl(GLuint tex, GLenum typ) :
		mTexture(tex),
		mType(typ)
	{
		g3dlog("G3D$: load texture(%u)", mTexture);
	}
	Gl3dTextureImpl()
	{
		glGenTextures(1, &mTexture);
		g3dlog("G3D$: struct texture(%u)", mTexture);
	}
	~Gl3dTextureImpl() 
	{
		if (mTexture) 
		{
			g3dlog("G3D$: destruct texture(%u)", mTexture);
			glDeleteTextures(1, &mTexture);
		}
			
	}

};

Gl3dTexture::Gl3dTexture(unsigned int object, unsigned int type)
	: mImpl(new Gl3dTextureImpl(object, type))
{

}

Gl3dTexture::Gl3dTexture()
	: mImpl(new Gl3dTextureImpl())
{}


Gl3dTexture::~Gl3dTexture()
{
	if (mImpl)
		delete mImpl;
}

GLenum fragers[] =
{
	GL_LINEAR,
	GL_NEAREST,
	GL_NEAREST_MIPMAP_NEAREST,
	GL_LINEAR_MIPMAP_LINEAR
};

void Gl3dTexture::SetMagMinFilters(Filter mag, Filter min)
{
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexParameteri(mImpl->mType, GL_TEXTURE_MAG_FILTER, fragers[(unsigned int)mag]);
	glTexParameteri(mImpl->mType, GL_TEXTURE_MIN_FILTER, fragers[(unsigned int)min]);
}

GLenum currentChangels[] =
{
	GL_RGB,
	GL_RGBA,
	GL_RGB16F,
	GL_RGBA16F,
	GL_RGB32F,
	GL_RGBA32F,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT32
};

GLenum normalasedChangels[] =
{
	GL_RGB,
	GL_RGBA,
	GL_RGB,
	GL_RGBA,
	GL_RGB,
	GL_RGBA,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT
};

void Gl3dTexture::SetDataMultisample(int width, int height, PixelFormat format)
{
	mImpl->mType = GL_TEXTURE_2D_MULTISAMPLE;
	glBindTexture(mImpl->mType, mImpl->mTexture);

	glTexImage2DMultisample(mImpl->mType, 4, currentChangels[(unsigned)format], width, height, GL_TRUE);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_T, GL_REPEAT);

}

void Gl3dTexture::SetData(int width, int height, PixelFormat format, const void * data)
{
	mImpl->mType = GL_TEXTURE_2D;
	glBindTexture(mImpl->mType, mImpl->mTexture);
	
    glTexImage2D(mImpl->mType, 0, currentChangels[(unsigned)format], width, height, 0, normalasedChangels[(unsigned)format], GL_UNSIGNED_BYTE, data);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	SetMagMinFilters(Filter::NEAREST, Filter::LINEAR);
	
	
	/* glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);*/
}

void Gl3dTexture::SetDataF(int width, int height, PixelFormat format, const float * data)
{
	mImpl->mType = GL_TEXTURE_2D;
	glBindTexture(mImpl->mType, mImpl->mTexture);

	glTexImage2D(mImpl->mType, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	SetMagMinFilters(Filter::NEAREST, Filter::LINEAR);

}

int Gl3dTexture::GetWidth() const
{
	int r;
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &r);
	return r;
}
int Gl3dTexture::GetHeight() const
{
	int r;
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &r);
	return r;
}

unsigned int Gl3dTexture::GetInternalType() const
{
	return mImpl->mType;
}

void Gl3dTexture::DisableClamp()
{
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
}

void Gl3dTexture::SetDataCube(int id, int width, int height, PixelFormat format, const void * data)
{
	mImpl->mType = GL_TEXTURE_CUBE_MAP;
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + id, 0, currentChangels[(unsigned)format], width, height, 0, normalasedChangels[(unsigned)format], GL_UNSIGNED_BYTE, data);

	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


	SetMagMinFilters(Filter::NEAREST, Filter::LINEAR);
	glBindTexture(mImpl->mType, 0);
}

void Gl3dTexture::GenMipmaps()
{
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glGenerateMipmap(mImpl->mType);
}

unsigned int Gl3dTexture::GetObject() const
{
	return mImpl->mTexture;
}

std::chrono::high_resolution_clock::time_point clockX = std::chrono::high_resolution_clock::now();

double GetTotalTime()
{
	std::chrono::high_resolution_clock::duration delta = std::chrono::high_resolution_clock::now() - clockX;
	return double(std::chrono::duration_cast<std::chrono::microseconds>(delta).count()) * 0.000001;
}

void Gl3dTexture::Activate(int id, Gl3dTexture * texture)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(texture->mImpl->mType, texture->mImpl->mTexture);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_ACTIVETE_CALLS);
}
