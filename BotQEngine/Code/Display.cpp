#include "Display.h"
#include "GameInstance.h"

void Display::SetCamera(GameObject* caera) 
{
	camera = caera;
}
GameObject* Display::GetCamera() const
{
	return camera;
}
Display::Display()
{
}


Display::~Display()
{
}

Size Display::GetSize() const
{
	return size;
}

void Display::SetSize(const Size & sz)
{
	size = sz;
}


Point Display::GetMausePosition()
{
	return mousepoint;
}

void Display::SetMausePosition(const Point & point)
{
	mousepoint = point;
}

bool Display::IsKeyDown(int scancode)
{
	if (keycallback)
		return keycallback(scancode);
	else
		return 0;
}

void Display::SetKeyCallback(const StorageFunction<int(int)>& fnc)
{
	keycallback = fnc;
}

bool Display::IsMouseKeyDown(int scancode)
{
	if (mousekeycallback)
		return mousekeycallback(scancode);
	else
		return 0;
}

void Display::SetMouseKeyCallback(const StorageFunction<int(int)>& fnc)
{
	mousekeycallback = fnc;
}

Display* Display::GetCurrent()
{
	return GameInstance::GetCurrent()->display;
}
