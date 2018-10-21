#include "Gl3dFrameBuffer.h"
#include "glad.h"
#undef GetObject
#include <vector>
#include <memory>
#include "Gl3dTexture.h"

class Gl3dFrameBufferImpl 
{
public:
	GLuint mFraembuffer;
	std::vector<std::shared_ptr<Gl3dTexture>> mColors;
	std::vector<std::shared_ptr<Gl3dTexture>> mDepths;
	Gl3dFrameBufferImpl()
	{
	
		glGenFramebuffers(1, &mFraembuffer);
		g3dlog("G3D$: struct framebuffer(%u)", mFraembuffer);
	}

	~Gl3dFrameBufferImpl() 
	{
		if (mFraembuffer) 
		{
			g3dlog("G3D$: destruct framebuffer(%u)", mFraembuffer);
			glDeleteFramebuffers(1, &mFraembuffer);
		}
	}
};

unsigned int Gl3dFrameBuffer::GetObject() const
{
	return mImpl->mFraembuffer;
}

Gl3dFrameBuffer::Gl3dFrameBuffer(
	int width, 
	int height,
	std::initializer_list<PixelFormat> colorFormats,
	std::initializer_list<PixelFormat> depthFormats,
	bool useMultisampling
) : mImpl(new Gl3dFrameBufferImpl())
{
	if (colorFormats.size() + depthFormats.size() > 20 || depthFormats.size() > 1)
		throw Gl3dCoreException("invalid frame buffer settings set");
	if (width < 1)
		width = 1;
	if (height < 1)
		height = 1;
	glBindFramebuffer(GL_FRAMEBUFFER, mImpl->mFraembuffer);
	GLenum buffers[20];
	for (int i = 0; i < colorFormats.size(); i++)
	{
		auto texture = std::make_shared<Gl3dTexture>();
		if(useMultisampling)
			texture->SetDataMultisample(width, height, *(colorFormats.begin() + i));
		else
		{
			texture->SetData(width, height, *(colorFormats.begin() + i), nullptr);
			texture->SetMagMinFilters(Filter::NEAREST, Filter::NEAREST);
		}
		texture->DisableClamp();
		
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture->GetInternalType(), texture->GetObject(), 0);
		
		mImpl->mColors.push_back(texture);
		buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	for (int i = colorFormats.size(); i < colorFormats.size() + depthFormats.size(); i++)
	{
		auto texture = std::make_shared<Gl3dTexture>();
		if (useMultisampling)
			texture->SetDataMultisample(width, height, *(depthFormats.begin() + (i - colorFormats.size())));
		else
		{
			texture->SetData(width, height, *(depthFormats.begin() + (i - colorFormats.size())), nullptr);
			texture->SetMagMinFilters(Filter::NEAREST, Filter::NEAREST);
		}
		texture->DisableClamp();
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->GetInternalType(), texture->GetObject(), 0);
		
		mImpl->mDepths.push_back(texture);
		buffers[i] = GL_NONE;
	}
	
	glDrawBuffers(colorFormats.size() + depthFormats.size(), buffers);



	GLuint result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{

		switch (result)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			throw Gl3dCoreException("attachment point incomplete");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			throw Gl3dCoreException("missing attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			throw Gl3dCoreException("draw buffer has missing color attachments");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			throw Gl3dCoreException("read buffer has missing color attachments");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			throw Gl3dCoreException("unsupported attachment format");
			break;
		}
	}

}

Gl3dFrameBuffer::Gl3dFrameBuffer(int width, int height, const FraembufferSettings& settings)
	: mImpl(new Gl3dFrameBufferImpl())
{
	throw Gl3dCoreException("deprecated exception");
	if (settings.colors + settings.depths > 20 || settings.depths > 1)
		throw Gl3dCoreException("invalid frame buffer settings set");

	glBindFramebuffer(GL_FRAMEBUFFER, mImpl->mFraembuffer);
	GLenum buffers[20];
	for (int i = 0; i < settings.colors; i++)
	{
			auto texture = std::make_shared<Gl3dTexture>();
			texture->SetData(width, height, settings.ColorFormat, nullptr);
			texture->DisableClamp();
			texture->SetMagMinFilters(Filter::LINEAR, Filter::LINEAR);
			
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, texture->GetInternalType(), texture->GetObject(), 0);
			mImpl->mColors.push_back(texture);
			buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	for (int i = settings.colors; i < settings.colors + settings.depths; i++)
	{
			auto texture = std::make_shared<Gl3dTexture>();
			texture->SetData(width, height, settings.DepthFormat, nullptr);
			texture->SetMagMinFilters(Filter::LINEAR, Filter::LINEAR);
			texture->DisableClamp();
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, texture->GetInternalType(), texture->GetObject(), 0);
			mImpl->mDepths.push_back(texture);
			buffers[i] = GL_NONE;
	}

	glDrawBuffers(settings.colors + settings.depths, buffers);
	


	GLuint result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (result != GL_FRAMEBUFFER_COMPLETE)
	{

    	switch (result)
		{
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			throw Gl3dCoreException("attachment point incomplete");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			throw Gl3dCoreException("missing attachment");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			throw Gl3dCoreException("draw buffer has missing color attachments");
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			throw Gl3dCoreException("read buffer has missing color attachments");
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			throw Gl3dCoreException("unsupported attachment format");
			break;
		}
	}
}

Gl3dTexture* Gl3dFrameBuffer::GetColorTexture(int level) const 
{
	return mImpl->mColors[level].get();
}

Gl3dTexture* Gl3dFrameBuffer::GetDepthTexture(int level) const
{
	return mImpl->mDepths[level].get();
}

void Gl3dFrameBuffer::Blit(Gl3dFrameBuffer * frameBuffer, int x0, int y0, int x, int y, const FraembufferSettings& settings) const
{

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, mImpl->mFraembuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer->mImpl->mFraembuffer);
	glDrawBuffers(0, nullptr);

	
	for (unsigned int i = 0; i < settings.colors; i++)
	{
		glReadBuffer(GL_COLOR_ATTACHMENT0 + i);
		glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
		glBlitFramebuffer(x0, y0, x, y, x0, y0, x, y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}

	if (settings.depths)
	{
		glReadBuffer(GL_NONE);
		glDrawBuffer(GL_NONE);

		glBlitFramebuffer(x0, y0, x, y, x0, y0, x, y, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

Gl3dFrameBuffer::~Gl3dFrameBuffer()
{
	if (mImpl)
		delete mImpl;
}
//
//class Gl3dPrepareFrameBufferImpl
//{
//public:
//	GLuint mFraembuffer;
//	
//	Gl3dPrepareFrameBufferImpl()
//	{
//
//		glGenFramebuffers(1, &mFraembuffer);
//		g3dlog("G3D$: struct framebuffer(%u)", mFraembuffer);
//	}
//
//	~Gl3dPrepareFrameBufferImpl()
//	{
//		if (mFraembuffer)
//		{
//			g3dlog("G3D$: destruct framebuffer(%u)", mFraembuffer);
//			glDeleteFramebuffers(1, &mFraembuffer);
//		}
//	}
//};
//
//Gl3dPrepareFrameBuffer::Gl3dPrepareFrameBuffer() :
//	mImpl(new Gl3dPrepareFrameBufferImpl())
//{
//}
//
//Gl3dPrepareFrameBuffer::~Gl3dPrepareFrameBuffer()
//{
//	if (mImpl)
//		delete mImpl;
//}
//
//void Gl3dPrepareFrameBuffer::Attach(Gl3dAttachmentColor attacment, Gl3dTexture * texture)
//{
//	glBindFramebuffer(GL_FRAMEBUFFER, mImpl->mFraembuffer);
//
//}
//
//unsigned int Gl3dPrepareFrameBuffer::GetObject() const
//{
//	return mImpl->mFraembuffer;
//}
