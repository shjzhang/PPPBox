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
