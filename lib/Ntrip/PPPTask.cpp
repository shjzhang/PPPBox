#include "PPPTask.hpp"


// Constructor
PPPTask::PPPTask()
{
}

// Destructor
PPPTask::~PPPTask()
{
}

bool PPPTask::run()
{
    try
    {
        if(m_bRealTime)
        {

        }
        else
        {
            processFiles();
        }
    }
    catch(...)
    {

    }
    return false;
}

void PPPTask::processFiles()
{

}
