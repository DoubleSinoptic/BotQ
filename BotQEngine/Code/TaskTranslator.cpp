#include "TaskTranslator.h"
#include "Common/DynamicArray.h"

#include <thread>
#include <atomic>
#include "SyncObject.h"
#include "LockGuard.h"

struct Tasker 
{
	StorageFunction<void(void)> task;
	CompliteFlag* flag;
};

class TaskTranslatorImpl 
{
public:
	SyncObject lock;
	DynamicArray<Tasker> tasks;
	std::thread::id id;
};

TaskTranslator::TaskTranslator() :
	mImpl(nullptr)
{
	mImpl = new TaskTranslatorImpl();
	mImpl->id = std::this_thread::get_id();
}


TaskTranslator::~TaskTranslator()
{
	if (mImpl)
	{
		delete mImpl;
		mImpl = nullptr;
	}	
}


void TaskTranslator::Translate(CompliteFlag * volatile compliteflag, const StorageFunction<void(void)>& Task)
{
	LockGuard<SyncObject> spin(mImpl->lock);
	if (compliteflag)
		*compliteflag = false;

	mImpl->tasks.Add(Tasker { Task, compliteflag });
}

void TaskTranslator::Excecute()
{
	if (mImpl->tasks.Length() == 0)
		return;
	LockGuard<SyncObject> spin(mImpl->lock);
	if (mImpl->tasks.Length() == 0)
		return;
	for (int i = 0; i < mImpl->tasks.Length(); i++) 
	{
		mImpl->tasks[i].task();
		if (mImpl->tasks[i].flag)
			*mImpl->tasks[i].flag = true;
	}
	mImpl->tasks.Clear();
}

void TaskTranslator::ClearAccamulator()
{
	if (mImpl->tasks.Length() == 0)
		return;
	LockGuard<SyncObject> spin(mImpl->lock);
	if (mImpl->tasks.Length() == 0)
		return;
	for (int i = 0; i < mImpl->tasks.Length(); i++)
		if (mImpl->tasks[i].flag)
			*mImpl->tasks[i].flag = true;	
	mImpl->tasks.Clear();
}

void TaskTranslator::Attach() 
{
	mImpl->id = std::this_thread::get_id();
}


void TaskTranslator::SigTranslate(const StorageFunction<void(void)>& Task)
{
	if (std::this_thread::get_id() == mImpl->id)
	{
		Task();
		return;
	}
	CompliteFlag flag;
	Translate(&flag, Task);
	WaitWeak(&flag);
}

void TaskTranslator::WaitWeak( CompliteFlag * volatile flag)
{
	while (*flag == false)
	{}
}

void TaskTranslator::WaitStrong(volatile CompliteFlag * volatile flag)
{
	while (*flag == false)
	{
		std::this_thread::yield();
	}
}

