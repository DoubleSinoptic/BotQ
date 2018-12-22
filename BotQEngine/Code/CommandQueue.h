#pragma once

#ifndef TASK_TRANSLATOR_H
#define TASK_TRANSLATOR_H

#include "Common/Defines.h"
#include "Common/StorageFunction.h"
#include "CacheAlloc.h"
#include <atomic>

typedef std::atomic_bool CompliteFlag;

class SGE_EXPORT CommandBase
{
public:
	inline CommandBase() {}
	virtual ~CommandBase() {}
	virtual void Execute() = 0;
};

class TaskTranslatorImpl;
class SGE_EXPORT CommandQueue
{
	struct StoragedCommand : public CommandBase
	{
		StorageFunction<void(void)> m_function;
		template<typename  T>
		StoragedCommand(const T& function) :
			m_function(function)
		{}
		virtual void Execute() override
		{
			m_function();
			CacheDestroy(this);
		}
	};

public:
	void Queue(CommandBase* command, CompliteFlag* compliteFlag = nullptr);
	void QueueWait(CommandBase* command);


	template<typename T>
	void QueueFunction(const T& x) 
	{	
		StoragedCommand* c = CacheAlloc<StoragedCommand>(x);
		Queue(c);
	}

	template<typename T>
	void QueueFunctionWait(const T& x)
	{
		StoragedCommand* c = CacheAlloc<StoragedCommand>(x);
		QueueWait(c);
	}

	template<typename T>
	static inline void Wait(CompliteFlag* flag, const T& h)
	{
		bool b = true;
		while (!flag->compare_exchange_weak(b, false))
		{
			b = true;
			h();
		}
	}

	static inline void Wait(CompliteFlag* flag)
	{
		bool b = true;
		while (!flag->compare_exchange_weak(b, false))
		{
			b = true;
		}
	}

	void Playback();
	void ClearAccamulator();

	CommandQueue();
	~CommandQueue();

	void Attach();
private:
	TaskTranslatorImpl * mImpl;
};

#endif