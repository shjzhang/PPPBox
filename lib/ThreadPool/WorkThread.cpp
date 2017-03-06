#include "WorkThread.h"
using namespace std;

WorkThread* WorkThread::create()
{
	WorkThread* wThread = new WorkThread;

	return wThread;
}

WorkThread::WorkThread() :
	m_pThread(0),
	m_eThreadState(THREAD_MAX)
{
	m_pTask = NULL;
	m_bKeep = false;
}

WorkThread::~WorkThread(){
	this->onStop();
}

void WorkThread::setTask(TaskBase* taskBase){
	m_pTask = taskBase;
}

TaskBase* WorkThread::getTask(){
	return m_pTask;
}

void WorkThread::setState(THREAD_STATE eState){
	m_eThreadState = eState;
}

THREAD_STATE WorkThread::getState(){
	return m_eThreadState;
}

void WorkThread::onStart(){
	if (m_pThread) return;
	m_bRun = true;
	this->setState(THREAD_IDLE);
	m_pThread = new thread(WorkThread::runEntry,(void*)this);
}

void WorkThread::onStop(){
	m_bRun = false;
	if (m_pThread) {
		m_pThread->join();
		delete m_pThread;
		m_pThread = 0;
	}
}

bool WorkThread::isRun(){
	return m_bRun;
}

void WorkThread::setKeepTask(bool bKeep){
	m_bKeep = bKeep;
}

bool WorkThread::isKeepTask(){
	return m_bKeep;
}

void WorkThread::runEntry(void* parameter){
	WorkThread* thiz = static_cast<WorkThread*>(parameter);

	while ( thiz->isRun() ){

		TaskBase* pTask = thiz->getTask();

		if ( pTask ) {
			thiz->setState(THREAD_BUSY);
            if(!pTask->run())
            {
                return;
            }

			thiz->setState(THREAD_IDLE);
			if(! (thiz->isKeepTask()) ) pTask = 0;
		}

	}

}
