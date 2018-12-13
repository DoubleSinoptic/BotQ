#pragma once

#include <mutex>
#include <atomic>
#include "Export.h"
/*
	��� �� ���� ������� �� ��������� ��� ���������������
	���������� ISync
	������� ����� ����� � �������� conditional_variabl
	� ��... ��� ���� �������
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