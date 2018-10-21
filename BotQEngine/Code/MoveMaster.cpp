#include "MoveMaster.h"

class MoveMasterImpl 
{
public:
	MoveMaster* _this;
	MoveMasterImpl(MoveMaster* __this)
		: _this(__this)
	{
		
	}
	~MoveMasterImpl()
	{
		
	}
};

void MoveMaster::Awake()
{
	mImpl = new MoveMasterImpl(this);
	//Do code ...
}

void MoveMaster::Destroy()
{
	//Do code ...
	if (mImpl) 
		delete mImpl; 
}	

MoveMaster::MoveMaster()
{}

MoveMaster::~MoveMaster()
{}
