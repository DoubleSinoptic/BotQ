#include "Utility.h"

#include "TaskDistributor.h"

TaskDistributor Distributor;

void AddDistributorThreads(int count)
{
	Distributor.AddThreads(count);
}

void StopAllDistributorThreads()
{
	Distributor.BreakAllWorkers();
}

IDistributor * GetCurrentDistributor()
{
	return &Distributor;
}

KH_EXPORT int GetProcessorCount()
{
	return  std::thread::hardware_concurrency();
}
