#include "Gl3dRenderPas.h"
#include "Gl3dShader.h"
#include "Gl3dTexture.h"
#include "Gl3dFrameBuffer.h"
#include "Gl3dDevice.h"
#include "glad.h"
#include <stdio.h>
#undef GetObject

typedef void (*BlendSetFunction)();

Gl3dRenderPas::Gl3dRenderPas(Gl3dShader * shader, Gl3dFrameBufferBase * framebuffer, Gl3dRenderPassDesc* desc)
	: mShader(shader), mFramebuffer(framebuffer)
{

	Gl3dDevice::ThrowIfError();


	if (!framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(shader->GetObject());		
	}
	else 
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetObject());
		glUseProgram(shader->GetObject());
	}

	Gl3dDevice::ThrowIfError();



	if (!desc)
		return;

	if (desc->clear != Gl3dClear::None)
	{	
		glClearDepth(desc->clearDepth);
		glClearColor(desc->clearColor.r, desc->clearColor.g, desc->clearColor.b, desc->clearColor.a);
		switch (desc->clear)
		{
		case Gl3dClear::Color:
			glClear(GL_COLOR_BUFFER_BIT);
			break;
		case Gl3dClear::DepthColor:
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			break;
		case Gl3dClear::Depth:
			glClear(GL_DEPTH_BUFFER_BIT);
			break;
		}
	}


	Gl3dDevice::ThrowIfError();


	glViewport(desc->viewport.x, desc->viewport.y, desc->viewport.w, desc->viewport.h);

	if (desc->depth == Gl3dDepth::Disable)
		glDisable(GL_DEPTH_TEST);
	else
	{
		glEnable(GL_DEPTH_TEST);
		switch (desc->depth)
		{
		case Gl3dDepth::Greater:
			glDepthFunc(GL_GREATER);
			break;
		case Gl3dDepth::Less:
			glDepthFunc(GL_LESS);
			break;
		case Gl3dDepth::GreaterEqual:
			glDepthFunc(GL_GEQUAL);
			break;
		case Gl3dDepth::LessEqual:
			glDepthFunc(GL_LEQUAL);
			break;
		}
	}
		
	Gl3dDevice::ThrowIfError();


	if (desc->cullFace == Gl3dCullFace::Disable)
		glDisable(GL_CULL_FACE);
	else
	{
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
		glCullFace(desc->cullFace == Gl3dCullFace::Front ? GL_FRONT : GL_BACK);
	}

	if (desc->blending == Gl3dBlending::Disable)
		glDisable(GL_BLEND);
	else 
	{
		glEnable(GL_BLEND);
	
		static BlendSetFunction blendTable[] =
		{
			[]() { glBlendFunc(GL_NONE, GL_NONE); },
			[]() { glBlendFunc(GL_ZERO, GL_ONE); },
			[]() { glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); },
			[]() { glBlendFunc(GL_ONE, GL_ONE); },
		};
		blendTable[(size_t)desc->blending]();	
	}

	Gl3dDevice::ThrowIfError();
}

Gl3dRenderPas::~Gl3dRenderPas()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}



void Gl3dRenderPas::Uniform(const char* uniform, Gl3dTexture* texture) const
{
	if (!texture)
		return;
	int cap = mShader->MapLocation(uniform, -1);
	g3dlogD("G3D$:: uniform val texture(%d): %s\n", cap, uniform);
	Gl3dTexture::Activate(cap, texture);
	glUniform1i(mShader->UniformLocation(uniform), cap);
}

void Gl3dRenderPas::Uniform(const char* uniform, int s) const
{
	g3dlogD("G3D$:: uniform val int: %s\n", uniform);
	glUniform1i(mShader->UniformLocation(uniform), s);
}

void Gl3dRenderPas::Uniform(const char * uniform, bool s)const
{
	g3dlogD("G3D$:: uniform val bool: %s\n", uniform);
	glUniform1i(mShader->UniformLocation(uniform), s ? 1 : 0);
}

void Gl3dRenderPas::Uniform(const char* uniform, float s) const
{
	g3dlogD("G3D$:: uniform val 1f: %s\n", uniform);
	glUniform1f(mShader->UniformLocation(uniform), s);
}

void Gl3dRenderPas::Uniform16f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 16f: %s\n", uniform);
	glUniformMatrix4fv(mShader->UniformLocation(uniform), 1, GL_FALSE, s);
}
void Gl3dRenderPas::Uniform4f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 4f: %s\n", uniform);
	glUniform4fv(mShader->UniformLocation(uniform), 1, s);
}
void Gl3dRenderPas::Uniform3f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform3fv(mShader->UniformLocation(uniform), 1, s);
}
void Gl3dRenderPas::Uniform2f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform2fv(mShader->UniformLocation(uniform), 1, s);
}




void Gl3dRenderPas::OUniform(int ca, const char* uniform, Gl3dTexture* texture) const
{
	int cap = mShader->MapLocation(uniform, -1);
	g3dlogD("G3D$:: uniform val texture(%d): %s\n", cap, uniform);
	Gl3dTexture::Activate(cap, texture);
	glUniform1i(mShader->UniformLocationExt(uniform, ca), cap);
}

void Gl3dRenderPas::OUniform(int cap, const char* uniform, int s) const
{
	g3dlogD("G3D$:: uniform val int: %s\n", uniform);
	glUniform1i(mShader->UniformLocationExt(uniform, cap), s);
}

void Gl3dRenderPas::OUniform(int cap, const char * uniform, bool s)const
{
	g3dlogD("G3D$:: uniform val bool: %s\n", uniform);
	glUniform1i(mShader->UniformLocationExt(uniform, cap), s ? 1 : 0);
}

void Gl3dRenderPas::OUniform(int cap, const char* uniform, float s) const
{
	g3dlogD("G3D$:: uniform val 1f: %s\n", uniform);
	glUniform1f(mShader->UniformLocationExt(uniform, cap), s);
}

void Gl3dRenderPas::OUniform16f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 16f: %s\n", uniform);
	glUniformMatrix4fv(mShader->UniformLocationExt(uniform, cap), 1, GL_FALSE, s);
}
void Gl3dRenderPas::OUniform4f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 4f: %s\n", uniform);
	glUniform4fv(mShader->UniformLocationExt(uniform, cap), 1, s);
}
void Gl3dRenderPas::OUniform3f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform3fv(mShader->UniformLocationExt(uniform, cap), 1, s);
}
void Gl3dRenderPas::OUniform2f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform2fv(mShader->UniformLocationExt(uniform, cap), 1, s);
}

