#pragma once

#ifndef KH_UTILITY_H
#define KH_UTILITY_H

#include "IDistributor.h"
#include "ITask.h"

KH_EXPORT void AddDistributorThreads(int count);
KH_EXPORT void StopAllDistributorThreads();

KH_EXPORT IDistributor* GetCurrentDistributor();

KH_EXPORT int GetProcessorCount();

template<typename T>
void ParallelFor(int beg, int end, const T& predicate)
{
	class ParallelForBody : public ITask
	{
		const T& _predicate;
	public:
		ParallelForBody(const T& _apredicate)
			: ITask(false), _predicate(_apredicate)
		{}

		virtual void Exec(int begin, int end) override
		{
			_predicate(begin, end);
		}
	} ParallelForBodyObject(predicate);
	GetCurrentDistributor()->AddTask(&ParallelForBodyObject, beg, end);
	ParallelForBodyObject.Wait();
}

template<typename T>
void RunForget(const T& predicate)
{
	class RunForgetBody : public ITask
	{
		const T& _predicate;
	public:
		RunForgetBody(const T& _apredicate)
			: ITask(true), _predicate(_apredicate)
		{}

		virtual void Exec(int begin, int end)
		{
			_predicate();
		}
	};
	RunForgetBody* task = new RunForgetBody(predicate);
	GetCurrentDistributor()->AddTask(task, 0, 1);
}

template<typename T>
ITask* RunTask(const T& predicate)
{
	class RunTaskBody : public ITask
	{
		const T& _predicate;
	public:
		RunTaskBody(const T& _apredicate)
			: ITask(false), _predicate(_apredicate)
		{}

		virtual void Exec(int begin, int end)
		{
			_predicate();
		}
	};
	RunTaskBody* task = new RunTaskBody(predicate);
	GetCurrentDistributor()->AddTask(task, 0, 1);
	return task;
}

#endif