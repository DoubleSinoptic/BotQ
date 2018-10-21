#ifndef GAMEOBJECTEVENTS_H
#define GAMEOBJECTEVENTS_H

#include "Common/StorageFunction.h"
#include "Common/Defines.h"

class GameObject;
class SGE_EXPORT GameObjectEvents
{
public:
    virtual ~GameObjectEvents();

    virtual void CreateObject(GameObject* obj) = 0;
    virtual void DisposeObject(GameObject* obj) = 0;
    virtual void ObjectParentChanged(GameObject* obj) = 0;
    virtual void ObjectChildRemoved(GameObject* obj, GameObject* child) = 0;
    virtual void ObjectChildAdded(GameObject* obj, GameObject* child) = 0;
    virtual void ObjectNameChanged(GameObject* obj) = 0;
};

#endif // GAMEOBJECTEVENTS_H
