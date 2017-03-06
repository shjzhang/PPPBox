#include <iostream>
#include <iomanip>
#include "SignalCenter.hpp"
#include "FileUtils.hpp"

SignalCenter* SignalCenter::instance()
{
    static SignalCenter sigCenter;
    return &sigCenter;
}

SignalCenter::SignalCenter()
{
    m_obsStream = 0;
    m_navStream = 0;
    m_sp3Stream = 0;
    m_obsOutStream = 0;
    m_sCorrPath = ".";
    m_navStream = new NtripNavStream();
    m_sp3Stream = new NtripSP3Stream();
    m_ephStore = new RealTimeEphStore();
    m_pppMain = new PPPMain();
    m_dOutWait = 5.0;
    m_bWriteAllSta = true;
    reopenObsOutFile();
}

SignalCenter::~SignalCenter()
{
    delete m_obsStream;
    delete m_navStream;
    delete m_sp3Stream;
    delete m_ephStore;
    delete m_pppMain;
    m_obsOutStream->close();
    delete m_obsOutStream;
}

void SignalCenter::newObs(const string &staID, list<t_satObs> obsList)
{
    lock_guard<mutex> guard(m_obsMutex);

    cout << "New Obs for station --> " << staID << endl;

    list<t_satObs>::iterator it;
    for(it = obsList.begin();it != obsList.end();++it)
    {
        t_satObs& obs = *it;

        // Rename the station
        obs._staID = staID;

        // First time: set the _lastDumpTime
        if(!(m_lastObsDumpTime.getDays() != 0.0 ||
             m_lastObsDumpTime.getSecondOfDay() !=0.0))
        {
            m_lastObsDumpTime = obs._time - 1.0;
        }

        // An old observation - throw it away
        if (obs._time <= m_lastObsDumpTime)
        {
            continue;
        }

        // Save the observation
        m_epoObsMap[obs._time].push_back(obs);

        // Dump Epochs
        if((obs._time - m_dOutWait)>m_lastObsDumpTime)
        {
            dumpObsEpoch(obs._time - m_dOutWait);
            m_lastObsDumpTime = obs._time - m_dOutWait;
        }
    }
}

void SignalCenter::newGPSEph(GPSEphemeris2& eph)
{
    std::cout << ", New GPS ephmeris! " << std::endl;
    //std::lock_guard<std::mutex> guard(m_gpsEphMutex);
    m_ephStore->putNewEph(&eph, true);
    m_navStream->addNewEph(&eph, true);
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
        OrbitEph2* ephLast = (OrbitEph2*)m_ephStore->ephLast(it->_prn);
        OrbitEph2* ephPrev = (OrbitEph2*)m_ephStore->ephPrev(it->_prn);
        if(ephLast && ephLast->IOD() == it->_iod)
        {
          ephLast->setOrbCorr(&(*it));
        }
        else if (ephPrev && ephPrev->IOD() == it->_iod)
        {
          ephPrev->setOrbCorr(&(*it));
        }
    }
    m_sp3Stream->updateEphmerisStore(m_ephStore);
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
        OrbitEph2* ephLast = (OrbitEph2*)m_ephStore->ephLast(it->_prn);
        OrbitEph2* ephPrev = (OrbitEph2*)m_ephStore->ephPrev(it->_prn);
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
    m_sp3Stream->updateEphmerisStore(m_ephStore);
    writeSP3File();
    return;
}


void SignalCenter::writeGPSEph(GPSEphemeris2* eph)
{

}

void SignalCenter::writeSP3File()
{
    m_sp3Stream->printSP3Ephmeris();
}

void SignalCenter::reopenObsOutFile()
{
    SystemTime dateTime;
    CommonTime comTime(dateTime);
    string timeStr = CivilTime(comTime).printf("%Y%02m%02d%02H");
    string obsFileName = "Obs" + timeStr +".out";

    if(!m_obsOutStream)
    {
        delete m_obsOutStream;
        m_obsOutStream = new ofstream;
        if(FileUtils::fileAccessCheck(obsFileName))
        {
            m_obsOutStream->open(obsFileName.c_str(), ios::app);
        }
        else
        {
            m_obsOutStream->open(obsFileName.c_str(), ios::out);
        }
    }
}

void SignalCenter::dumpObsEpoch(const CommonTime &maxTime)
{
    map<CommonTime,list<t_satObs> >::iterator it = m_epoObsMap.begin();
    while(it!=m_epoObsMap.end())
    {
        const CommonTime& epoTime = it->first;
        if(epoTime <= maxTime)
        {
            list<t_satObs>& allObs = it->second;
            list<t_satObs>::iterator itObs;
            bool firstObs = true;
            EpochObsMap epoObsMap;
            string staID = allObs.begin()->_staID;
            for(itObs=allObs.begin();itObs!=allObs.end();)
            {
                const t_satObs& obs = *itObs;

                // Output into the File
                if(m_bWriteAllSta)
                {
                    reopenObsOutFile();
                    *m_obsOutStream << setiosflags(ios::fixed);
                    GPSWeekSecond gs = GPSWeekSecond(obs._time);
                    if(firstObs)
                    {
                        firstObs = false;
                        *m_obsOutStream << "> " << gs.getWeek() << ' '
                                       << setprecision(7) << gs.getSOW() << endl;
                    }
                    *m_obsOutStream << obs._staID << ' '
                                   << NtripObsStream::asciiSatLine(obs) << endl;

                    if((++itObs)==allObs.end())
                    {
                        *m_obsOutStream << endl;
                    }
                    m_obsOutStream->flush();
                }

                // Store the data into m_staEpochObsMap
                epoObsMap[obs._time].push_back(obs);
            }
            m_staEpochObsMap[staID] = epoObsMap;
            m_epoObsMap.erase(it);
            it = m_epoObsMap.begin();
        }
        else
        {
            ++it;
        }
    }
}

void SignalCenter::startPPP()
{
    m_pppMain->start();
}

void SignalCenter::stopPPP()
{
    m_pppMain->stop();
}
