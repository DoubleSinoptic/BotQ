#pragma once
#include <GLFW/glfw3.h>

class Window
{
	GLFWwindow * window;
public:
	Window();
	bool Update();
	void SwapBuffers();
	void MakeCurrent();
	void AttachKeyCallbacks(class GameInstance* instance);
	~Window();
};
