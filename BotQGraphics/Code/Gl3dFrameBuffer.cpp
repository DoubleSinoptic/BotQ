#include "Gl3dFrameBuffer.h"
#include "glad.h"
#undef GetObject
#include <vector>
#include <memory>
#include "Gl3dTexture.h"

Gl3dFrameBufferInstance::Gl3dFrameBufferInstance() :
	m_object(0)
{}

Gl3dFrameBufferInstance::~Gl3dFrameBufferInstance()
{
	if (m_object)
	{
		glDeleteFramebuffers(1, &m_object);
		m_object = 0;
	}

}

void Gl3dFrameBufferInstance::Create(Gl3dFrameBufferDesc* desc)
{
	if (m_object)
	{
		glDeleteFramebuffers(1, &m_object);
		m_object = 0;
	}

	glGenFramebuffers(1, &m_object);
	if (!m_object)
		throw Gl3dCoreException("error of generate layout instance");
	glBindFramebuffer(GL_FRAMEBUFFER, m_object);
	if (desc->depthAttachment)
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, Gl3dDevice::CastSideType(desc->depthSide), desc->depthAttachment->GetObject(), desc->depthLevel);
	
	size_t bc = 0;
	GLenum buffers[20];
	for (size_t i = 0; i < 24; i++) 
	{
		if (desc->colorAttachments[i])
		{
			bc++;
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, Gl3dDevice::CastSideType(desc->colorSide[i]), desc->colorAttachments[i]->GetObject(), desc->colorLevels[i]);
			buffers[i] = GL_COLOR_ATTACHMENT0 + i;
		}
	}
	glDrawBuffers(bc, buffers);
	
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

unsigned int Gl3dFrameBufferInstance::GetObject() const
{
	return m_object;
}

