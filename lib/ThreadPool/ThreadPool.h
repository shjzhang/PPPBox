#ifndef THREAD_POOL_H_H_
#define THREAD_POOL_H_H_

#include "WorkThread.h"

#include "MutexLock.h"

class ThreadPool : public TaskBase
{

public:

	// by default, create 5 threads
	static ThreadPool* create(int iMaxThreadNum = 5);

protected:

	ThreadPool(int iMaxThreadNum = 5);

	~ThreadPool();

public:

	void pushTask(TaskBase* taskBase);
	TaskBase* popTask();

public:

	virtual void run();

public:

	void onStart();
	void onStop();
	bool isRun();

private:

	int m_iMaxThreadNum;

	WorkThread m_poolThread;

	MutexLock  m_taskLocker;

	MutexLock  m_threadLocker;

	deque<TaskBase*> m_taskQueue;

	deque<WorkThread*> m_threadQueue;
};

#endif