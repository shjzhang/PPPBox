#ifndef TASK_BASE_H_H_
#define TASK_BASE_H_H_
#include <iostream>
#include <string>
#include <thread>
#include <deque>


class TaskBase
{

public:

	 TaskBase(){};

	 ~TaskBase(){};

public:

	// 
	virtual void run() = 0 ;



};

#endif