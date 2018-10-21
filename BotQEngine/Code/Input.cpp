#include "Input.h"
#include "GameInstance.h"
#include "Display.h"
#include <string.h>


Point Input::GetMausePosition()
{
	return GameInstance::GetCurrent()->display->GetMausePosition();
}


bool Input::IsKeyDown(int scancode)
{
	return GameInstance::GetCurrent()->display->IsKeyDown(scancode);
}



bool Input::IsMouseKeyDown(int scancode)
{
	return GameInstance::GetCurrent()->display->IsMouseKeyDown(scancode);
}
