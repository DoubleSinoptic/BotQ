#include "TaskDistributor.h"
#include "ITask.h"

#include "LockGuard.h"
#include <algorithm>

void TaskDistributor::ThreadWorker()
{
	while (!breakAll.load())
	{
		Yeld(nullptr);
		std::this_thread::yield();
	}
}

void TaskDistributor::Yeld(ITask* filter)
{
	ITask* prdicictor = nullptr;
	int begin = 0;
	int end = 0;
	int total = 0;
	{
		if (pistons.size() == 0)
			return;
		LockGuard<SyncObject> _(tasksLocks);
		for (int i = 0; i < pistons.size(); i++)
		{
			Piston& piston = pistons[i];
			if (filter != nullptr)
				if (piston.task != filter)
					continue;
			int complitedCount = piston.task->_executedCount.load();
			int needCount = piston.GetCount() - complitedCount;
			if (needCount)
			{
				prdicictor = piston.task;
				begin = complitedCount + piston.begin;
				int threadPart = piston.GetCount() / threads.size();
				if (threadPart == 0)
					end = complitedCount + 1 + piston.begin;
				else if (threadPart < needCount)
					end = complitedCount + threadPart + piston.begin;
				else
					end = complitedCount + needCount + piston.begin;


				piston.task->_executedCount.fetch_add(end - begin);
				total = piston.GetCount();
				break;
			}
		}
	}
	if (prdicictor)
	{
		prdicictor->Exec(begin, end);
		{
			LockGuard<SyncObject> _(tasksLocks);
			prdicictor->_complitedCount.fetch_add(end - begin);
			if (prdicictor->_complitedCount.load() == total)
			{
				pistons.erase(std::remove_if(pistons.begin(), pistons.end(), [=](const Piston& p) -> bool
				{
					return p.task == prdicictor;
				}), pistons.end());
				if (prdicictor->_deleteThis.load())
					delete prdicictor;
				prdicictor->_hasWorked.store(false);
			}

		}
	}
}

TaskDistributor::TaskDistributor()
	: breakAll(false)
{
}

TaskDistributor::~TaskDistributor()
{
	BreakAllWorkers();
}

void TaskDistributor::AddTask(ITask * task, int begin, int end)
{
	if (begin == end)
	{
		task->_hasWorked.store(false);
		return;
	}
	Piston piston;
	piston.begin = begin;
	piston.end = end;
	piston.task = task;
	task->SetDistributor(this);
	LockGuard<SyncObject> _(tasksLocks);
	pistons.push_back(piston);
}

void TaskDistributor::BreakAllWorkers()
{

	breakAll.store(true);
	int errorCount = 0;
	for (int i = 0; i < threads.size(); i++)
	{
		try 
		{
			threads[i]->join();
		}
		catch (std::exception&) 
		{
			errorCount++;
		}
	}
	threads.clear();
	printf("all jobs workers deleted, error count: %d\n", errorCount);
}

void TaskDistributor::AddThreads(int threadCount)
{
	printf("added jobs workers, count: %d\n", threadCount);
	for (int i = 0; i < threadCount; i++)
	{
		std::thread* th = new std::thread([=]()
		{
			ThreadWorker();
		});

		threads.push_back(th);
	}
}
