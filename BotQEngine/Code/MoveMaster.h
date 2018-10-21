#pragma once
#ifndef _MoveMaster_h_
#define _MoveMaster_h_

#include "Component.h"

class MoveMasterImpl;
class SGE_EXPORT MoveMaster : public Component
{
	TYPED_OBJECT
public:
	MoveMaster();
	~MoveMaster();
	
	void Awake() override;
	void Destroy() override;	
private:

	MoveMasterImpl* mImpl = nullptr;
};

#endif