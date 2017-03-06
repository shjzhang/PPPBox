#include "ThreadPool.h"

ThreadPool* ThreadPool::create(int iMaxThreadNum)
{
	ThreadPool* pool = new ThreadPool(iMaxThreadNum);
	return pool;
}
   
ThreadPool::ThreadPool(int iMaxThreadNum){
	this->m_iMaxThreadNum = iMaxThreadNum;
	
	for (int i = 0; i < iMaxThreadNum; i++){
		WorkThread* wThread = WorkThread::create();
		wThread->onStart();
		m_threadQueue.push_back(wThread);
	}

}

ThreadPool::~ThreadPool(){
	this->onStop();
}

void ThreadPool::pushTask(TaskBase* taskBase){
    //m_taskLocker.lock();
	m_taskQueue.push_back(taskBase);
    //m_taskLocker.unlock();
}

TaskBase* ThreadPool::popTask(){
	m_taskLocker.lock();
	if (m_taskQueue.empty()) return 0;
	TaskBase* pTask = m_taskQueue.front();
	m_taskQueue.pop_front();
	m_taskLocker.unlock();
	return pTask;
}

bool ThreadPool::run(){
	m_threadLocker.lock();
	WorkThread* wThread = *(m_threadQueue.begin());
	TaskBase* pTask = this->popTask();
	if (wThread->getState() == THREAD_IDLE && pTask != 0){
		m_threadQueue.pop_front();
		m_threadQueue.push_back(wThread);
		wThread->setTask(pTask);
	}
	m_threadLocker.unlock();
    return true;
}

void ThreadPool::onStart(){
	m_poolThread.setTask(this);
	m_poolThread.setKeepTask(true);
	m_poolThread.onStart();
}

void ThreadPool::onStop(){
	m_poolThread.onStop();
	m_poolThread.setTask(0);

	deque<WorkThread*>::iterator it = m_threadQueue.begin();
	for (; it != m_threadQueue.end(); it++){
		(*it)->onStop();
	}

	m_threadQueue.clear();
	m_taskQueue.clear();
}

bool ThreadPool::isRun(){
	return m_poolThread.isRun();
}
