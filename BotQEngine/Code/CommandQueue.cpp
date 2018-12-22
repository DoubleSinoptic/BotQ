#include "CommandQueue.h"
#include "Common/DynamicArray.h"

#include <thread>
#include <atomic>
#include "SyncObject.h"
#include "LockGuard.h"

struct CommandDesc 
{
	CommandBase*  command;
	CompliteFlag* compliteFlag;
};

class TaskTranslatorImpl 
{
public:
	SyncObject lock;
    DynamicArray<CommandDesc> tasks[2];
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


void CommandQueue::Queue(CommandBase* command, CompliteFlag* compliteFlag)
{
	if (std::this_thread::get_id() == mImpl->id)
	{
		command->Execute();
		if (compliteFlag)
			compliteFlag->store(true);
		return;
	}
	LockGuard<SyncObject> spin(mImpl->lock);
	mImpl->tasks[mImpl->currentId ? 1 : 0].Add({ command, compliteFlag });
}

void CommandQueue::Playback()
{
	DynamicArray<CommandDesc>* current;
	{
		LockGuard<SyncObject> spin(mImpl->lock);
		current = &(mImpl->tasks[mImpl->currentId ? 1 : 0]);
		mImpl->currentId = !mImpl->currentId;
	}

	for (int i = 0; i < current->Length(); i++)
	{	
		CommandDesc& cmd = (*current)[i];
		cmd.command->Execute();
		if(cmd.compliteFlag)
			cmd.compliteFlag->store(true);
	}
	current->Clear();
}

void CommandQueue::ClearAccamulator()
{
	DynamicArray<CommandDesc>* current;
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

void CommandQueue::QueueWait(CommandBase* command)
{
	CompliteFlag flag(false);
	Queue(command, &flag);
	Wait(&flag);
}
