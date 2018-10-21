#include "ITask.h"
#include "IDistributor.h"
#include <thread>

ITask::ITask(bool deleteThis) :
	_hasWorked(true),
	_deleteThis(deleteThis),
	_executedCount(0),
	_complitedCount(0)
{}

ITask::~ITask() {}

void ITask::Wait()
{
	while (_hasWorked.load())
		_distr->Yeld(this);
}

void ITask::SetDistributor(IDistributor * distr)
{
	_distr = distr;
}
