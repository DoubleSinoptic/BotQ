#pragma once

#include "Common/Log.h"
#include "Common/StorageFunction.h"
#include "Common/DynamicArray.h"
#include "Common/Defines.h"

template<typename... Args>
class EventHandler
{
	StorageFunction<void(Args...)> mFunction;
	bool							  mDelete;
public:
	EventHandler() :
		mFunction(),
		mDelete(true)
	{}

	template<typename Functor>
	explicit EventHandler(const Functor& functor, bool delteThis = true) :
		mFunction(functor),
		mDelete(delteThis)
	{
		if (!mFunction)
			Error("taking a null function pointer");
	}

	bool HasDeleteThis() const 
	{
		return mDelete;
	}

	FORCEINLINE StorageFunction<void(Args...)>& GetFunctor() 
	{
		return mFunction;
	}
};

template<typename... Args>
class Event
{
public:
	typedef EventHandler<Args...>* EventHandlerPointer;
	Event() 
	{}

	~Event()
	{
		for (auto i : mHandlers)
			if (i->HasDeleteThis())
				delete i;
	}

	FORCEINLINE void Emit(const Args&... args) 
	{
		for (int i = 0; i < mHandlers.Length(); i++) 
			mHandlers[i]->GetFunctor()(args...);
	}

	void operator +=(EventHandlerPointer eventHandler)
	{
		if (mHandlers.IndexOf(eventHandler) != -1)
			Error("event exsits this handler");
		mHandlers.Add(eventHandler);
	}
	void operator -=(EventHandlerPointer eventHandler)
	{
		mHandlers.Remove(eventHandler);
	}

private:
	DynamicArray<EventHandlerPointer> mHandlers;
};

