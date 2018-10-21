#pragma once

#ifndef KH_LOCKGUARD_H
#define KH_LOCKGUARD_H

template<typename T>
class LockGuard
{
public:
	inline LockGuard(T& obj) :
		mObject(obj)
	{
		obj.Lock();
	}

	inline ~LockGuard()
	{
		mObject.Unlock();
	}
private:
	T& mObject;
};

#endif