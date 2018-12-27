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
	TaskTranslatorImpl() :
		current(new  DynamicArray<CommandDesc>())
	{}

	SyncObject addlock;
	SyncObject feedlock;
    DynamicArray<CommandDesc>* current;
	DynamicArray<DynamicArray<CommandDesc>*> empty;
	bool currentId = false;
	std::thread::id id;

	~TaskTranslatorImpl() 
	{
		LockGuard<SyncObject> spin1(feedlock);
		LockGuard<SyncObject> spin2(addlock);
		delete current;
		for (auto x : empty)
			delete x;
	}
};

CommandQueue::CommandQueue() :
	mImpl(new TaskTranslatorImpl())
{
	mImpl->id = std::this_thread::get_id();
}


CommandQueue::~CommandQueue()
{
	if (mImpl)
		delete mImpl;	
}


void CommandQueue::Queue(CommandBase* command, CompliteFlag* compliteFlag)
{
	/*if (std::this_thread::get_id() == mImpl->id)
	{
		Log("[WARN]: Queue this thread called");
		command->Execute();
		if (compliteFlag)
			compliteFlag->store(true);
		return;
	}*/
	LockGuard<SyncObject> spin(mImpl->addlock);
	mImpl->current->Add({ command, compliteFlag });
}

void CommandQueue::Playback()
{
	DynamicArray<CommandDesc>* current;
	{
		DynamicArray<CommandDesc>* nextArray;
		{
			LockGuard<SyncObject> spin(mImpl->feedlock);
			if (mImpl->empty.Length() != 0)
			{
				nextArray = mImpl->empty.Back();
				mImpl->empty.PopBack();
			}
			else
				nextArray = new DynamicArray<CommandDesc>();
		}
		
		LockGuard<SyncObject> spin(mImpl->addlock);
		current = mImpl->current;
		mImpl->current = nextArray;
	}

	for (int i = 0; i < current->Length(); i++)
	{	
		CommandDesc& cmd = (*current)[i];
		cmd.command->Execute();
		if(cmd.compliteFlag)
			cmd.compliteFlag->store(true);
	}
	current->Clear();
	LockGuard<SyncObject> spin(mImpl->feedlock);
	mImpl->empty.Add(current);
}

void CommandQueue::Clear()
{
	DynamicArray<CommandDesc>* current;
	{
		DynamicArray<CommandDesc>* nextArray;
		{
			LockGuard<SyncObject> spin(mImpl->feedlock);
			if (mImpl->empty.Length() != 0)
			{
				nextArray = mImpl->empty.Back();
				mImpl->empty.PopBack();
			}
			else
				nextArray = new DynamicArray<CommandDesc>();
		}

		LockGuard<SyncObject> spin(mImpl->addlock);
		current = mImpl->current;
		mImpl->current = nextArray;
	}

	current->Clear();
	LockGuard<SyncObject> spin(mImpl->feedlock);
	mImpl->empty.Add(current);
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

int __GrainClamp(int val, int min, int max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}
int __Abs(int val)
{
	if (val < 0)
		return -val;
	return val;
}

template<typename T>
int GrainParting(const T& fn, int beg, int end, int grainSize = 10)
{
	int rezult = 0;
	int length = __Abs(end - beg);
	while (length)
	{
		rezult++;
		int grain = __GrainClamp(grainSize, 0, length);
		fn(end - length + 0, end - length + grain);
		length -= grain;
	}
	return rezult;
}


void CommandQueue::ParallelFor(const StorageFunction<void(int, int)>& m_function, int beg, int end, int grainSize)
{
	std::atomic_int total(0);
	int sz = GrainParting([&](int a, int b) 
	{
		total += 1;
		QueueFunction([a, b, &m_function]()
		{
			m_function(a, b);
		});
	}, beg, end, grainSize);

	while (total.load() != sz)
		std::this_thread::yield();
}

void CommandQueue::StoragedCommand::Execute()
{
	m_function();
	CacheDestroy(this);
}
