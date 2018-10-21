#pragma once

#ifndef KH_DISTRIBUTOR_H
#define KH_DISTRIBUTOR_H

#include "Export.h"

class KH_EXPORT IDistributor
{
public:
	virtual ~IDistributor() {}

	virtual void Yeld(class ITask* filter) = 0;
	virtual void AddTask(class ITask* task, int begin, int end) = 0;
};

#endif