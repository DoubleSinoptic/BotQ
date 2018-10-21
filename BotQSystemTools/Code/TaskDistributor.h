#pragma once

#ifndef TASK_DISTIBUTOR_H
#define TASK_DISTIBUTOR_H

#include "Export.h"
#include "IDistributor.h"

#include "SyncObject.h"
#include <thread>
#include <atomic>
#include <vector>
#include <mutex>

/*
"ПИСТОН! ЭЭто просто ПИСТОН!": выкрикнул ZERGTV,
смотря как кучка мариков разносили его базу с 2 прокси бараков...
я в это время писал этот код и не мог
уйти от искушения заюзать это чудесное слово.
*/
struct KH_EXPORT Piston
{
	class ITask* task;
	int begin;
	int end;
	int GetCount()
	{
		return end - begin;
	}
};

class KH_EXPORT TaskDistributor : public IDistributor
{
	SyncObject					tasksLocks;
	std::vector<std::thread*>	threads;
	std::atomic_bool			breakAll;
	std::vector<Piston>			pistons;

	void ThreadWorker();
public:
	void Yeld(class ITask* filter) override;

	TaskDistributor();

	~TaskDistributor();

	void AddTask(class ITask* task, int begin, int end) override;

	void BreakAllWorkers();

	void AddThreads(int threadCount);
};

#endif