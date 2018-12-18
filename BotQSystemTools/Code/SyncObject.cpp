#include "SyncObject.h"
#include <thread>

SyncObject::SyncObject() 
	: _lock(false)
{
}

SyncObject::~SyncObject()
{
}

bool SyncObject::TryLock() 
{
	bool false_bool = false;
	return _lock.compare_exchange_strong(false_bool, true);
}

void SyncObject::Lock() 
{
	int spinCount = 1000;
	while (!TryLock())
		if (spinCount != 0)
			spinCount--;
		else
			std::this_thread::yield();
}


void SyncObject::Unlock() 
{
	_lock.store(false);
}


bool SyncObject::SwapSignal()
{
	bool true_bool = true;
	return _lock.compare_exchange_strong(true_bool, false);
}

void SyncObject::Wait()
{
	int spinCount = 1000;
	while (!SwapSignal())
		if (spinCount != 0)
			spinCount--;
		else
		std::this_thread::yield();	
}

void SyncObject::Notify()
{
	_lock.store(true);
}

SignalAccamulator::SignalAccamulator() :
	_counter(0)
{
}

bool SignalAccamulator::Take()
{
	if (_counter.load() != 0) 
	{
		_counter.fetch_add(-1);
		return true;
	}
	return false;
}

void SignalAccamulator::Notify()
{
	_counter.fetch_add(1);
}
