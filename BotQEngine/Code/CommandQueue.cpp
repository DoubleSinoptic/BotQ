#include "CommandQueue.h"
#include "Common/DynamicArray.h"

#include <thread>
#include <atomic>
#include "SyncObject.h"
#include "LockGuard.h"


class TaskTranslatorImpl 
{
public:
	SyncObject lock;
    DynamicArray<CommandDesc*> tasks[2];
	bool currentId = false;
	std::thread::id id;
};

CommandQueue::CommandQueue() :
	mImpl(new TaskTranslatorImpl())
{
	mImpl->id = std::this_thread::get_id();
}


CommandQueue::~CommandQueue()
{
	if (mImpl)
	{
		delete mImpl;
		mImpl = nullptr;
	}	
}


void CommandQueue::Enqueue(CommandDesc* command)
{
	LockGuard<SyncObject> spin(mImpl->lock);
	mImpl->tasks[mImpl->currentId ? 1 : 0].Add(command);
}

void CommandQueue::Excecute()
{
	DynamicArray<CommandDesc*>* current;
	{
		LockGuard<SyncObject> spin(mImpl->lock);
		current = &(mImpl->tasks[mImpl->currentId ? 1 : 0]);
		mImpl->currentId = !mImpl->currentId;
	}

	for (int i = 0; i < current->Length(); i++)
	{	
		CommandDesc* cmd = (*current)[i];
		cmd->Execute();
		cmd->Awake();
	}
	current->Clear();
}

void CommandQueue::ClearAccamulator()
{
	DynamicArray<CommandDesc*>* current;
	{
		LockGuard<SyncObject> spin(mImpl->lock);
		current = &mImpl->tasks[mImpl->currentId ? 1 : 0];
		mImpl->currentId = !mImpl->currentId;
	}
	current->Clear();
}

void CommandQueue::Attach()
{
	mImpl->id = std::this_thread::get_id();
}


void CommandQueue::EnqueueAndWaitDelete(CommandDesc* command)
{
	if (std::this_thread::get_id() == mImpl->id)
	{
		command->Execute();
		return;
	}
	{
		LockGuard<SyncObject> spin(mImpl->lock);
		mImpl->tasks[mImpl->currentId ? 1 : 0].Add(command);
	}
	command->Wait();
	delete command;
}

void CommandQueue::EnqueueAndWait(CommandDesc * command)
{
	if (std::this_thread::get_id() == mImpl->id)
	{
		command->Execute();
		return;
	}
	{
		LockGuard<SyncObject> spin(mImpl->lock);
		mImpl->tasks[mImpl->currentId ? 1 : 0].Add(command);
	}
	command->Wait();
}

