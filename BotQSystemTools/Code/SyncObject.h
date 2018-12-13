#pragma once

#include <mutex>
#include <atomic>
#include "Export.h"
/*
	Это по сути быстрый не системный тип удовлетворяющий
	интерфейсу ISync
	Который можно юзать в качестве conditional_variabl
	и да... это тупо спинлок
*/
class KH_EXPORT SyncObject
{
	std::atomic_bool _lock;
public:
	SyncObject();
	~SyncObject();

	bool TryLock();
	void Lock();
	void Unlock();

	bool SwapSignal();
	void Wait();
	void Notify();
};

class KH_EXPORT SignalAccamulator
{
	std::atomic_int _counter;
public:
	SignalAccamulator();

	bool Take();
	void Notify();
};