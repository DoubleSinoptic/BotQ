#include "pch.h"
#include <GameInstance.h>
#include <Display.h>
#include <Core.h>

//#if _WIN32
//
//#include <Windows.h>
//typedef BOOL (WINAPI * PFNWGLSWAPINTERVALEXTPROC)(int interval);
//PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = NULL;
//
//#endif
static void error_callback(int error, const char* description)
{
	Error("window manager error: %s", description);
}


static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	
	reinterpret_cast<GameInstance*>(glfwGetWindowUserPointer(window))->display->SetMausePosition({ int(xpos), int(ypos) });
}

class GlfwProxy
{
public:
	GlfwProxy()
	{
		glfwSetErrorCallback(error_callback);

		if (!glfwInit())
			Error("window manager make error:");


		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);  // yes, 3 and 2!!!
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_SAMPLES, 4);
	}

	~GlfwProxy()
	{
		glfwTerminate();
	}
} proxy;

Window::Window()
{
	

#ifdef FULLSCREEN
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);

	glfwWindowHint(GLFW_RED_BITS, mode->redBits);
	glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
	glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
	glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

	window = glfwCreateWindow(mode->width, mode->height, "[BotQ.exe Survival (build 10025) (-D3D12,-BTPHYS,ACODEK)]", monitor, NULL);
#else
	window = glfwCreateWindow(1280, 768, "[BotQ.exe Survival (build 10025) (-W_GL41,-BTPHYS,ACODEK,FMPEG)]", NULL, NULL);
#endif



	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);
////
//#ifdef _WIN32
//// Turn on vertical screen sync under Windows.
//// (I.e. it uses the WGL_EXT_swap_control extension)
//
//	wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
//	if (wglSwapIntervalEXT)
//		wglSwapIntervalEXT(120);
//#else	
//	glfwSwapInterval(0);
//#endif
}

void Window::AttachKeyCallbacks(class GameInstance* instance)
{
	glfwSetWindowUserPointer(window, instance);
	glfwSetCursorPosCallback(window, cursor_position_callback);

	instance->display->SetKeyCallback([=](int code) -> int
	{
		return glfwGetKey(window, code);
	});

	instance->display->SetMouseKeyCallback([=](int code) -> int
	{
		return glfwGetMouseButton(window, code);
	});
}

bool Window::Update()
{
	glfwPollEvents();
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	if (GameInstance::GetCurrent())
		GameInstance::GetCurrent()->display->SetSize({ width , height });
	return !glfwWindowShouldClose(window);
}

Window::~Window()
{
	if (window)
		glfwDestroyWindow(window);
}
void Window::SwapBuffers()
{
	glfwSwapBuffers(window);
}

void Window::MakeCurrent()
{
	glfwMakeContextCurrent(window);
}

