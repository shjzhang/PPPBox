#ifndef MUTEX_LOCK_H_H_
#define MUTEX_LOCK_H_H_

#include <iostream>
#include <mutex>

using namespace std;

class MutexLock
{

public:

	MutexLock(){}

	~MutexLock(){}

public:

	void lock(){ m_Mutex.lock(); }

	void unlock(){ m_Mutex.unlock(); }

private:

	std::recursive_mutex m_Mutex;
};

#endif