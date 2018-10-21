#include "Gl3dDevice.h"
#include "glad.h"
#undef GetObject
#include <stack>
void Gl3dDevice::Clear()
{
    glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Gl3dDevice::Viewport(int i, int itox)
{
	glViewport(0, 0, i, itox);
}

void Gl3dDevice::MultiSampleTest(bool value)
{
	if (value)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);
	
}

void Gl3dDevice::GetMemory(long long & aviable, long long & total)
{
#define GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX 0x9048
#define GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX 0x9049
	glGetInteger64v(GL_GPU_MEM_INFO_TOTAL_AVAILABLE_MEM_NVX,
		&total);
	glGetInteger64v(GL_GPU_MEM_INFO_CURRENT_AVAILABLE_MEM_NVX,
		&aviable);
}

void Gl3dDevice::AlphaTest(bool value)
{
	if (value)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else 
	{
		glDisable(GL_BLEND);
    }
}

void Gl3dDevice::DepthTest(bool value)
{
    if (value)
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
    }
}

void Gl3dDevice::CullTest(bool value)
{
    if (value)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    }
    else
    {
        glDisable(GL_CULL_FACE);
    }
}

void Gl3dDevice::SrTest(bool value)
{
	if (value)
	{
		/*glEnable(GL_POLYGON_SMOOTH);
		glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
		glEnable(GL_LINE_SMOOTH);
		glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
		glShadeModel(GL_SMOOTH);*/
	}
	else
	{
		/*glDisable(GL_POLYGON_SMOOTH);*/
	}
}

void Gl3dDevice::LinkAddresses(Gl3dDeviceLinker lnk)
{
	if(!gladLoadGLLoader(lnk))
		throw Gl3dCoreException("error of init gl3d instance");
}

void Gl3dDevice::Configurate()
{
	glEnable(GL_MULTISAMPLE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glEnable(GL_CULL_FACE);

}

void Gl3dDevice::CullBack()
{
	glCullFace(GL_BACK);
}

void Gl3dDevice::CullFront()
{
	glCullFace(GL_FRONT);
}

//void Gl3dDevice::DepthMask(bool val)
//{
//	glDepthMask(val ? GL_TRUE : GL_FALSE);
//}
//
//void Gl3dDevice::InsertDepthMod(bool isLess)
//{
//	if(isLess)
//		glDepthFunc(GL_LESS);
//	else
//		glDepthFunc(GL_LEQUAL);
//}

void Gl3dDevice::SkipErrors()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		
	}
}


void Gl3dDevice::ValidateContext()
{
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR)
	{
		g3dlog("G3D$: CRITICAL: %d", err);
    }
}

void Gl3dDevice::ExecCommands()
{
	glFinish();
}

int profiler[10] = {};

int * Gl3dDevice::GetProfilerData()
{
	return profiler;
}

void Gl3dDevice::ResetProfiler()
{
	memset(profiler, 0, sizeof(profiler));
}

void Gl3dDevice::SwapToRGB8BPP(void * pixelBuffer, int w, int h)
{
	glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, pixelBuffer);
}

std::stack<GLint> framebuffersStack;
void Gl3dDevice::FramebufferPush()
{
    GLint tmp;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &tmp);
    framebuffersStack.push(tmp);
    g3dlog("G3D$: pushed framebuffer: %d", tmp);
}

void Gl3dDevice::FramebufferPop()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffersStack.top());
    framebuffersStack.pop();
}

void Gl3dDevice::FramebufferRestore()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffersStack.top());
}
