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
	if (mag == Gl3dFilter::MIPMAP_NEAREST || mag == Gl3dFilter::MIPMAP_LINEAR)
		throw Gl3dCoreException("invalid filter target");

	static GLenum fragers[] =
	{
		GL_NEAREST,
		GL_LINEAR,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR
	};

	glBindTexture(mImpl->mType, mImpl->mTexture);

	if (min == Gl3dFilter::MIPMAP_NEAREST || min == Gl3dFilter::MIPMAP_LINEAR)
		glGenerateMipmap(mImpl->mType);

	glTexParameteri(mImpl->mType, GL_TEXTURE_MAG_FILTER, fragers[(size_t)mag]);
	glTexParameteri(mImpl->mType, GL_TEXTURE_MIN_FILTER, fragers[(size_t)min]);
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
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_S, wrapmodes[(size_t)s]);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_T, wrapmodes[(size_t)t]);
	glTexParameteri(mImpl->mType, GL_TEXTURE_WRAP_R, wrapmodes[(size_t)r]);
}

GLenum gl3dFormatToGLtable[] =
{
	GL_R8,
	GL_R16F,
	GL_R32F,

	GL_RG8,
	GL_RG16F,
	GL_RG32F,

	GL_RGB8,
	GL_RGB16F,
	GL_RGB32F,

	GL_RGBA8,
	GL_RGBA16F,
	GL_RGBA32F,

	GL_DEPTH_COMPONENT,
	GL_DEPTH_COMPONENT16,
	GL_DEPTH_COMPONENT32
};

bool isDepth(Gl3dPixelFormat format)
{
	return	format == Gl3dPixelFormat::DEPTH_16 ||
			format == Gl3dPixelFormat::DEPTH_32 ||
			format == Gl3dPixelFormat::DEPTH_8;
}

GLenum gl3dChanelsCount[] =
{
	1,1,1,
	2,2,2,
	3,3,3,
	4,4,4,
	1,1,1
};

GLenum colorChanels[] =
{
	GL_DEPTH_COMPONENT,
	GL_R,
	GL_RG,
	GL_RGB,	
	GL_RGBA
};

const Gl3dPixelFormat formatTable[7][5] = 
{
	{ Gl3dPixelFormat::R_32F, Gl3dPixelFormat::RG_32F, Gl3dPixelFormat::RGB_32F,  Gl3dPixelFormat::RGBA_32F, Gl3dPixelFormat::DEPTH_32},
    { Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1) },
	{ Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1) },
	{ Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1) },
	{ Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1), Gl3dPixelFormat(-1) },
	{ Gl3dPixelFormat::R_8, Gl3dPixelFormat::RG_8, Gl3dPixelFormat::RGB_8,  Gl3dPixelFormat::RGBA_8, Gl3dPixelFormat::DEPTH_8},
	{ Gl3dPixelFormat::R_8, Gl3dPixelFormat::RG_8, Gl3dPixelFormat::RGB_8,  Gl3dPixelFormat::RGBA_8, Gl3dPixelFormat::DEPTH_8}
};

void Gl3dTexture::SetData(const Gl3dSubImageDesc* image, int level) 
{
	Gl3dPixelFormat fmt = formatTable[(size_t)image->type][(size_t)image->changelsCount - 1];
	SetData(fmt, image, level);
}

void Gl3dTexture::SetData(Gl3dSide side, const Gl3dSubImageDesc* image, int level)
{
	Gl3dPixelFormat fmt = formatTable[(size_t)image->type][(size_t)image->changelsCount - 1];
	SetData(side, fmt, image, level);
}

void Gl3dTexture::SetData(Gl3dPixelFormat internalFormat, const Gl3dSubImageDesc* image, int level)
{
	if (mImpl->mType != 0 && mImpl->mType != GL_TEXTURE_2D)
		throw Gl3dCoreException("target reuse not supported");

	if (image->changelsCount == 0)
		throw Gl3dCoreException("invalid chanels count");

	mImpl->mType = GL_TEXTURE_2D;
	glBindTexture(mImpl->mType, mImpl->mTexture);

	glTexImage2D(
		Gl3dDevice::CastSideType(Gl3dSide::Back),
		level,
		gl3dFormatToGLtable[(unsigned)internalFormat],
		image->width, image->height,
		0,
		colorChanels[isDepth(internalFormat) && (image->changelsCount == 1) ? 0 : image->changelsCount],
		Gl3dDevice::CastFundamentalType(image->type),
		image->data);

	SetWrapMode(Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat);
	SetMagMinFilters(Gl3dFilter::NEAREST, Gl3dFilter::NEAREST);

	Gl3dDevice::ThrowIfError();
}

void Gl3dTexture::SetData(Gl3dSide side, Gl3dPixelFormat internalFormat, const Gl3dSubImageDesc* image, int level)
{
	if (mImpl->mType != 0 && mImpl->mType != GL_TEXTURE_CUBE_MAP)
		throw Gl3dCoreException("target reuse not supported");

	if (image->changelsCount == 0)
		throw Gl3dCoreException("invalid chanels count");

	mImpl->mType = GL_TEXTURE_CUBE_MAP;
	glBindTexture(mImpl->mType, mImpl->mTexture);
	glTexImage2D(
		Gl3dDevice::CastSideType(side), 
		level,
		gl3dFormatToGLtable[(unsigned)internalFormat],
		image->width, image->height, 0,
		colorChanels[isDepth(internalFormat) && (image->changelsCount == 1) ? 0 : image->changelsCount],
		Gl3dDevice::CastFundamentalType(image->type),
		image->data);

	SetWrapMode(Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat, Gl3dWrapMode::Repeat);
	SetMagMinFilters(Gl3dFilter::NEAREST, Gl3dFilter::NEAREST);
	Gl3dDevice::ThrowIfError();
}

void Gl3dTexture::AllocateData(int w, int h, Gl3dPixelFormat internalFormat, int level)
{
	Gl3dSubImageDesc nullDesc = {};
	nullDesc.height = h;
	nullDesc.width = w;
	nullDesc.type = Gl3dFundamentalType::Float;
	nullDesc.changelsCount = gl3dChanelsCount[(size_t)internalFormat];
	nullDesc.data = nullptr;
	SetData(internalFormat, &nullDesc, level);
}

void Gl3dTexture::AllocateData(Gl3dSide side, int w, int h, Gl3dPixelFormat internalFormat, int level)
{
	Gl3dSubImageDesc nullDesc = {};
	nullDesc.height = h;
	nullDesc.width = w;
	nullDesc.type = Gl3dFundamentalType::Float;
	nullDesc.changelsCount = gl3dChanelsCount[(size_t)internalFormat];
	nullDesc.data = nullptr;
	SetData(side, internalFormat, &nullDesc, level);
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
