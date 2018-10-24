#include "Gl3dRenderPas.h"
#include "Gl3dShader.h"
#include "Gl3dTexture.h"
#include "Gl3dFrameBuffer.h"
#include "Gl3dDevice.h"
#include "glad.h"
#undef GetObject
Gl3dRenderPas::Gl3dRenderPas(Gl3dShader * shader, Gl3dFrameBufferBase * framebuffer)
	: mShader(shader), mFramebuffer(framebuffer)
{
	if (!framebuffer)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glUseProgram(shader->GetObject());
		return;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->GetObject());
	glUseProgram(shader->GetObject());
}

Gl3dRenderPas::~Gl3dRenderPas()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
	//glFinish();
}

void Gl3dRenderPas::Uniform(const char* uniform, Gl3dTexture* texture) const
{
	if (!texture)
		return;
	int cap = mShader->MapLocation(uniform, -1);
	g3dlogD("G3D$:: uniform val texture(%d): %s\n", cap, uniform);
	Gl3dTexture::Activate(cap, texture);
	glUniform1i(mShader->UniformLocation(uniform), cap);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::Uniform(const char* uniform, int s) const
{
	g3dlogD("G3D$:: uniform val int: %s\n", uniform);
	glUniform1i(mShader->UniformLocation(uniform), s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::Uniform(const char * uniform, bool s)const
{
	g3dlogD("G3D$:: uniform val bool: %s\n", uniform);
	glUniform1i(mShader->UniformLocation(uniform), s ? 1 : 0);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::Uniform(const char* uniform, float s) const
{
	g3dlogD("G3D$:: uniform val 1f: %s\n", uniform);
	glUniform1f(mShader->UniformLocation(uniform), s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::Uniform16f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 16f: %s\n", uniform);
	glUniformMatrix4fv(mShader->UniformLocation(uniform), 1, GL_FALSE, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}
void Gl3dRenderPas::Uniform4f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 4f: %s\n", uniform);
	glUniform4fv(mShader->UniformLocation(uniform), 1, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}
void Gl3dRenderPas::Uniform3f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform3fv(mShader->UniformLocation(uniform), 1, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}
void Gl3dRenderPas::Uniform2f(const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform2fv(mShader->UniformLocation(uniform), 1, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}




void Gl3dRenderPas::OUniform(int ca, const char* uniform, Gl3dTexture* texture) const
{
	int cap = mShader->MapLocation(uniform, -1);
	g3dlogD("G3D$:: uniform val texture(%d): %s\n", cap, uniform);
	Gl3dTexture::Activate(cap, texture);
	glUniform1i(mShader->UniformLocationExt(uniform, ca), cap);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::OUniform(int cap, const char* uniform, int s) const
{
	g3dlogD("G3D$:: uniform val int: %s\n", uniform);
	glUniform1i(mShader->UniformLocationExt(uniform, cap), s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::OUniform(int cap, const char * uniform, bool s)const
{
	g3dlogD("G3D$:: uniform val bool: %s\n", uniform);
	glUniform1i(mShader->UniformLocationExt(uniform, cap), s ? 1 : 0);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::OUniform(int cap, const char* uniform, float s) const
{
	g3dlogD("G3D$:: uniform val 1f: %s\n", uniform);
	glUniform1f(mShader->UniformLocationExt(uniform, cap), s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

void Gl3dRenderPas::OUniform16f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 16f: %s\n", uniform);
	glUniformMatrix4fv(mShader->UniformLocationExt(uniform, cap), 1, GL_FALSE, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}
void Gl3dRenderPas::OUniform4f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 4f: %s\n", uniform);
	glUniform4fv(mShader->UniformLocationExt(uniform, cap), 1, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}
void Gl3dRenderPas::OUniform3f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform3fv(mShader->UniformLocationExt(uniform, cap), 1, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}
void Gl3dRenderPas::OUniform2f(int cap, const char* uniform, const float* s) const
{
	g3dlogD("G3D$:: uniform val 3f: %s\n", uniform);
	glUniform2fv(mShader->UniformLocationExt(uniform, cap), 1, s);
	GL3D_COUNTER_INCREMENT(GL3D_PROFILE_UNIFORM_CALLS);
}

