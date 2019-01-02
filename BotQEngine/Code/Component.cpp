#include "Component.h"
#include "GameInstance.h"

Component::Component()/* :
	mId(nullptr)*/
{
}

Component::~Component()
{
	if(mEnabled)
		GameInstance::GetCurrent()->updatebleComponents.Remove(this);
}

void Component::SetEnabled(bool value)
{	
	if (value && !mEnabled)
		GameInstance::GetCurrent()->updatebleComponents.Add(this);
	if(!value && mEnabled)
		GameInstance::GetCurrent()->updatebleComponents.Remove(this);
	mEnabled = value;
}
