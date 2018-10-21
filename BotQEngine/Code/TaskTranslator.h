#pragma once

#ifndef TASK_TRANSLATOR_H
#define TASK_TRANSLATOR_H

#include "Common/Defines.h"
#include "Common/StorageFunction.h"
#include <atomic>

typedef volatile bool CompliteFlag;

class TaskTranslatorImpl;
class SGE_EXPORT TaskTranslator
{
public:
	void Translate(CompliteFlag * volatile compliteflag, const StorageFunction<void(void)>& Task);
	void Excecute();
	void ClearAccamulator();
	void SigTranslate(const StorageFunction<void(void)>& Task);

	static void WaitWeak(CompliteFlag * volatile flag);
	static void WaitStrong( CompliteFlag * volatile flag);

	TaskTranslator();
	~TaskTranslator();

	void Attach();
private:
	TaskTranslatorImpl * mImpl;
};

#endif