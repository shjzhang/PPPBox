#include "PPPTask.hpp"


// Constructor
PPPTask::PPPTask()
{
}

// Destructor
PPPTask::~PPPTask()
{
}

void PPPTask::run()
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
}

void PPPTask::processFiles()
{

}
