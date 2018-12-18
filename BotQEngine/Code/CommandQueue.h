#pragma once

#ifndef TASK_TRANSLATOR_H
#define TASK_TRANSLATOR_H

#include "Common/Defines.h"
#include "Common/StorageFunction.h"
#include <atomic>

class SGE_EXPORT CommandDesc
{
	std::atomic_bool m_compliteFlag;
	bool m_delete;
public:
	CommandDesc() :
		m_compliteFlag(false)
	{}
	virtual ~CommandDesc() {}
	virtual void Execute() = 0;

	void Awake()
	{
		m_compliteFlag.store(true);
	}

	template<typename T>
	void Wait(const T& h)
	{
		bool b = true;
		while (!m_compliteFlag.compare_exchange_weak(b, false))
		{
			b = true;
			h();
		}
	}

	void Wait() 
	{
		bool b = true;
		while (!m_compliteFlag.compare_exchange_weak(b, false))
		{
			b = true;
		}
	}
};

class SGE_EXPORT StoragedCommand : public CommandDesc
{
	StorageFunction<void(void)> m_function;
public:
	template<typename T>
	StoragedCommand(const T& val) :
		m_function(val)
	{}

	virtual void Execute() override 
	{
		m_function();
	}
};

#define CreateCommand(x) (new StoragedCommand(x))

class TaskTranslatorImpl;
class SGE_EXPORT CommandQueue
{
public:
	void Enqueue(CommandDesc* command);
	void EnqueueAndWaitDelete(CommandDesc* command);
	void EnqueueAndWait(CommandDesc* command);

	void Excecute();
	void ClearAccamulator();

	CommandQueue();
	~CommandQueue();

	void Attach();
private:
	TaskTranslatorImpl * mImpl;
};

#endif