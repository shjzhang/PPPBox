#include "PPPMain.hpp"

// Constructor
PPPMain::PPPMain()
{
    m_bRunning = false;
    m_dCorrWaitTime = 5.0;
    m_pppTask = new PPPTask();
    m_pppThread = new WorkThread();
    m_pppThread->setTask(m_pppTask);
}

// Destructor
PPPMain::~PPPMain()
{
    stop();
    delete m_pppTask;
    delete m_pppThread;
}

void PPPMain::start()
{
    if(m_bRunning)
    {
        return;
    }

    try
    {
        readOptions();

        m_pppThread->onStart();

        m_bRunning = true;
    }
    catch(...)
    {
        m_bRunning = true;
        stop();
    }
}

void PPPMain::stop()
{
    m_pppThread->onStop();
}

void PPPMain::readOptions()
{
    int choice = 1;
    if(choice == 1)
    {
        m_bRealTime = true;
    }
    else if(choice == 2)
    {
        m_bRealTime = false;
    }
    else
    {
        return;
    }
}

//bool PPPMain::waitForCorr(const gpstk::CommonTime &epoTime) const
//{
//    if(!m_bRealTime || m_sCorrMount.empty())
//    {
//        return false;
//    }
//    // Verify the validity of m_lastClkCorrTime
//    else if(m_lastClkCorrTime.getDays()==0.0||
//            m_lastClkCorrTime.getSecondOfDay()==0.0)
//    {
//        return false;
//    }
//    else
//    {
//        double dt = epoTime - m_lastClkCorrTime;
//        if(dt > 1.0 && dt < m_dCorrWaitTime)
//        {
//            return true;
//        }
//        else
//        {
//            return false;
//        }
//    }
//    return false;
//}
