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



void Gl3dTexture::SetMagMinFilters(Gl3dFilter mag, Gl3dFilter min)
{
	static GLenum fragers[] =
	{
		GL_LINEAR,
		GL_NEAREST,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_LINEAR
	};

	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexParameteri(mImpl->mType, GL_TEXTURE_MAG_FILTER, fragers[(unsigned int)mag]);
	glTexParameteri(mImpl->mType, GL_TEXTURE_MIN_FILTER, fragers[(unsigned int)min]);
}



void Gl3dTexture::SetWrapMode(Gl3dWrapMode s, Gl3dWrapMode t, Gl3dWrapMode r)
{
	static GLenum wrapmodes[] =
	{
		GL_CLAMP_TO_EDGE,
		GL_REPEAT,
		GL_MIRRORED_REPEAT,
		GL_CLAMP_TO_BORDER
	};

	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_S, wrapmodes[(unsigned int)s]);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_T, wrapmodes[(unsigned int)t]);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_R, wrapmodes[(unsigned int)r]);
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

GLenum changelsCount[] =
{
	GL_R,
	GL_RG,
	GL_RGB,	
	GL_RGBA
};

void Gl3dTexture::SetData(Gl3dPixelFormat internalFormat, const Gl3dSubImageDesc* image)
{
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexImage2D(Gl3dDevice::CastSideType(Gl3dSide::Back), 0, currentChangels[(unsigned)internalFormat],
		image->width, image->height, 0, changelsCount[(unsigned)image->changelsCount], 
		Gl3dDevice::CastFundamentalType(image->type), image->data);
	SetWrapMode(Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat);
	SetMagMinFilters(Gl3dFilter::NEAREST, Gl3dFilter::NEAREST);

	Gl3dDevice::ThrowIfError();
}

void Gl3dTexture::SetData(Gl3dSide side, Gl3dPixelFormat internalFormat, const Gl3dSubImageDesc* image)
{
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexImage2D(Gl3dDevice::CastSideType(side), 0, currentChangels[(unsigned)internalFormat],
		image->width, image->height, 0, changelsCount[(unsigned)image->changelsCount],
		Gl3dDevice::CastFundamentalType(image->type), image->data);

	SetWrapMode(Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat);
	SetMagMinFilters(Gl3dFilter::NEAREST, Gl3dFilter::NEAREST);
	Gl3dDevice::ThrowIfError();
}

void Gl3dTexture::AllocateData(int w, int h, Gl3dPixelFormat internalFormat)
{
	Gl3dSubImageDesc nullDesc = {};
	nullDesc.height = h;
	nullDesc.width = w;
	nullDesc.type = Gl3dFundamentalType::UByte;
	SetData(internalFormat, &nullDesc);
}

void Gl3dTexture::AllocateData(Gl3dSide side, int w, int h, Gl3dPixelFormat internalFormat)
{
	Gl3dSubImageDesc nullDesc = {};
	nullDesc.height = h;
	nullDesc.width = w;
	nullDesc.type = Gl3dFundamentalType::UByte;
	SetData(side, internalFormat, &nullDesc);
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


void Gl3dTexture::GenMipmaps()
{
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glGenerateMipmap(mImpl->mType);
}

unsigned int Gl3dTexture::GetObject() const
{
	return mImpl->mTexture;
}

void Gl3dTexture::Activate(int id, Gl3dTexture * texture)
{
	glActiveTexture(GL_TEXTURE0 + id);
	glBindTexture(texture->mImpl->mType, texture->mImpl->mTexture);
}
