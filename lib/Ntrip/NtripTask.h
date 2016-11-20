#ifndef NTRIP_TASK_H_H_
#define NTRIP_TASK_H_H_

#include <mutex>

#include "TaskBase.h"
#include "MountPoint.hpp"


using namespace gpstk;
using namespace std;

class NtripTask : public TaskBase{

public:

	 NtripTask();
	~NtripTask();

public:

	virtual void run();
	void setMountPoint(MountPoint pt ){m_MP = pt;}
    void setRawOutOpt(bool output) {m_bOutputRaw = output;}
    void setRawOutFile(string& file){m_sRawOutFile = file;}

private:

    mutex m_mutex;
    MountPoint m_MP;
    bool m_bOutputRaw;
    string m_sRawOutFile;
};

#endif
