#ifndef WORK_THREAD_H_H_
#define WORK_THREAD_H_H_
#include "TaskBase.h"

enum THREAD_STATE
{
	THREAD_IDLE = 0,
	THREAD_BUSY,
	THREAD_MAX
};

class WorkThread
{

public:

	static WorkThread* create();

public:

	 WorkThread();

	~WorkThread();

public:

	/// set and get task
	void setTask(TaskBase* taskBase);

	TaskBase* getTask();

	/// set and get state
	void setState(THREAD_STATE eState);

	THREAD_STATE getState();

	/// start and stop
	void onStart();

	void onStop();

	bool isRun();

	void setKeepTask(bool bKeep);

	bool isKeepTask();

public:

	/// thread function
	static void runEntry(void* parameter);

private:

	TaskBase*  m_pTask;

	bool m_bRun;

	bool m_bKeep;

	THREAD_STATE  m_eThreadState;

	std::thread*  m_pThread;
};

#endif