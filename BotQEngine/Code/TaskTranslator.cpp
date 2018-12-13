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
	DynamicArray<Tasker> tasks[2];
	bool currentId = false;
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

	mImpl->tasks[mImpl->currentId ? 1 : 0].Add(Tasker { Task, compliteflag });
}

void TaskTranslator::Excecute()
{
	DynamicArray<Tasker>& current = mImpl->tasks[mImpl->currentId ? 1 : 0];
	{
		LockGuard<SyncObject> spin(mImpl->lock);
		mImpl->currentId = !mImpl->currentId;
	}

	for (int i = 0; i < current.Length(); i++)
	{
		current[i].task();
		if (current[i].flag)
			*current[i].flag = true;
	}
	current.Clear();
}

void TaskTranslator::ClearAccamulator()
{
	LockGuard<SyncObject> spin(mImpl->lock);
	
	for (int i = 0; i < mImpl->tasks[0].Length(); i++)
		if (mImpl->tasks[0][i].flag)
			*mImpl->tasks[0][i].flag = true;	
	mImpl->tasks[0].Clear();

	for (int i = 0; i < mImpl->tasks[1].Length(); i++)
		if (mImpl->tasks[1][i].flag)
			*mImpl->tasks[1][i].flag = true;
	mImpl->tasks[1].Clear();
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

