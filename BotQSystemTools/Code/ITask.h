#pragma once

#ifndef KH_ITASK_H
#define KH_ITASK_H

#include "Export.h"
#include <atomic>

class KH_EXPORT ITask
{
	std::atomic_bool _hasWorked;
	std::atomic_int	_executedCount;
	std::atomic_int	_complitedCount;
	std::atomic_bool _deleteThis;
	class IDistributor* _distr;
	friend class TaskDistributor;
public:
	ITask(bool deleteThis);
	virtual ~ITask();
	void Wait();

	void SetDistributor(class IDistributor* distr);

	virtual void Exec(int begin, int end) = 0;	
};

#endif