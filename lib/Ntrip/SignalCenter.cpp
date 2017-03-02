#include <iostream>
#include "SignalCenter.hpp"


SignalCenter* SignalCenter::instance()
{
    static SignalCenter sigCenter;
    return &sigCenter;
}

SignalCenter::SignalCenter()
{
    m_navStream = 0;
    m_sp3Stream = 0;
    m_sCorrPath = ".";
    m_navStream = new NtripNavStream();
    m_sp3Stream = new NtripSP3Stream();
    m_pppMain = new PPPMain();
}

SignalCenter::~SignalCenter()
{
    delete m_navStream;
    delete m_sp3Stream;
    delete m_pppMain;
}

void SignalCenter::newGPSEph(GPSEphemeris2& eph)
{
    //std::cout << "New GPS ephmeris! " << std::endl;
    //std::lock_guard<std::mutex> guard(m_gpsEphMutex);
    //m_navStream->putNewEph(&eph);
    writeGPSEph(&eph);
}

void SignalCenter::newOrbCorr(list<t_orbCorr> orbCorr)
{
    std::cout << "New OrbCorr!" << std::endl;
    std::lock_guard<std::mutex> guard(m_orbCorrMutex);
    if(orbCorr.size() == 0)
    {
        return;
    }

    list<t_orbCorr>::iterator it = orbCorr.begin();
    if(m_bRealTime)
    {
        if(m_sCorrMount.empty() || m_sCorrMount != it->_staID)
        {
            return;
        }
    }
    for(;it!=orbCorr.end();++it)
    {
        string prn = asString(it->_prn);
        OrbitEph2* ephLast = m_navStream->ephLast(prn);
        OrbitEph2* ephPrev = m_navStream->ephPrev(prn);
        if(ephLast && ephLast->IOD() == it->_iod)
        {
          ephLast->setOrbCorr(&(*it));
        }
        else if (ephPrev && ephPrev->IOD() == it->_iod)
        {
          ephPrev->setOrbCorr(&(*it));
        }
    }
    m_sp3Stream->updateEphmerisStore(m_navStream);
    return;
}

void SignalCenter::newClkCorr(list<t_clkCorr> clkCorr)
{

    std::lock_guard<std::mutex> guard(m_clkCorrMutex);
    if(clkCorr.size() == 0)
    {
        return;
    }

    list<t_clkCorr>::iterator it = clkCorr.begin();
    if(m_bRealTime)
    {
        if(m_sCorrMount.empty() || m_sCorrMount != it->_staID)
        {
            return;
        }
    }

    for(;it!=clkCorr.end();++it)
    {
        string prn = asString(it->_prn);
        OrbitEph2* ephLast = m_navStream->ephLast(prn);
        OrbitEph2* ephPrev = m_navStream->ephPrev(prn);
        if(ephLast && ephLast->IOD() == it->_iod)
        {
          ephLast->setClkCorr(&(*it));
        }
        else if (ephPrev && ephPrev->IOD() == it->_iod)
        {
          ephPrev->setClkCorr(&(*it));
        }
        m_lastClkCorrTime = it->_time;
    }
    std::cout << "New ClkCorr at time -> " << m_lastClkCorrTime.asString() << std::endl;
    m_sp3Stream->setLastClkCorrTime(m_lastClkCorrTime);
    m_sp3Stream->updateEphmerisStore(m_navStream);
    writeSP3File();
    return;
}


void SignalCenter::writeGPSEph(GPSEphemeris2* eph)
{
    m_navStream->checkPrintEph(eph);
}

void SignalCenter::writeSP3File()
{
    //std::lock_guard<std::mutex> guard(m_gpsEphMutex);
    m_sp3Stream->printSP3Ephmeris();
}

void SignalCenter::startPPP()
{
    m_pppMain->start();
}

void SignalCenter::stopPPP()
{
    m_pppMain->stop();
}
