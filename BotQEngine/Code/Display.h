#pragma once

#include "Common/StorageFunction.h"
#include "Math/PointAndSize.h"
#include "Common/Defines.h"
#include <atomic>
class GameObject;

class SGE_EXPORT Display
{
	Point mousepoint = Point(0,0);
	Size size;
	GameObject* camera;
	StorageFunction<int(int)> keycallback;
	StorageFunction<int(int)> mousekeycallback;
public:
	Display();
	~Display();

	void SetCamera(GameObject* camera);
	GameObject* GetCamera() const;

	Size GetSize() const;
	void SetSize(const Size& sz);

	Point GetMausePosition();
	void SetMausePosition(const Point& point);

	bool IsKeyDown(int scancode);
	void SetKeyCallback(const StorageFunction<int(int)>& fnc);

    bool IsMouseKeyDown(int scancode);
	void SetMouseKeyCallback(const StorageFunction<int(int)>& fnc);

	static Display* GetCurrent();
};

