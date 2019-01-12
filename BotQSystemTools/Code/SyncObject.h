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
	SyncObject() noexcept;
	~SyncObject() noexcept;

	bool TryLock() noexcept;
	void Lock() noexcept;
	void Unlock() noexcept;

	bool SwapSignal() noexcept;
	void Wait() noexcept;
	void Notify() noexcept;
};

class KH_EXPORT SignalAccamulator
{
	std::atomic_int _counter;
public:
	SignalAccamulator();

	bool Take();
	void Notify();
};