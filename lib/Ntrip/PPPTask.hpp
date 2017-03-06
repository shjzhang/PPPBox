#ifndef PPPTASK_HPP
#define PPPTASK_HPP
#include "TaskBase.h"

class PPPTask:public TaskBase
{
public:

    /// Constructor
    PPPTask();

    /// Destructor
    ~PPPTask();

    /// Run the task
    virtual bool run();

    /// Set real-time flag
    void setRealTimeFlag(bool flag){m_bRealTime = flag;}

    /// Post-process use saved files
    void processFiles();
private:
    bool m_bRealTime;    ///< Real-time process flag
};


#endif // PPPTASK_HPP
