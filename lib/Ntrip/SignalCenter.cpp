#include <iostream>
#include <iomanip>
#include "SignalCenter.hpp"
#include "FileUtils.hpp"
#include "PPPMain.hpp"
#include "NtripObsStream.hpp"

SignalCenter* SignalCenter::instance()
{
    static SignalCenter sigCenter;
    return &sigCenter;
}

SignalCenter::SignalCenter()
{
    m_navStream = 0;
    m_sp3Stream = 0;
    m_obsOutStream = 0;
    m_bRealTime = true;
    m_navStream = new NtripNavStream();
    m_sp3Stream = new NtripSP3Stream();
    m_ephStore = new RealTimeEphStore();
    m_pppMain = new PPPMain();
    m_dOutWait = 5;
    m_bWriteAllSta = false;
    m_bWriteCorrFile = false;
    reopenObsOutFile();
}

SignalCenter::~SignalCenter()
{
    delete m_navStream;
    delete m_sp3Stream;
    delete m_ephStore;
    delete m_pppMain;
    delete m_obsOutStream;
}

void SignalCenter::newObs(list<t_satObs> obsList)
{	
    unique_lock<mutex> lock2(m_allObsMutex);

    list<t_satObs>::iterator it = obsList.begin();

    std::string utcTime = CivilTime(it->_time).printf("%02H:%02M:%02S");
    cout << "New Obs for station --> " << it->_staID
         << ", at epoch " << utcTime << endl;

    while(it != obsList.end())
    {
        const t_satObs& obs = *it;


        // First time: set the _lastDumpTime
        if(!m_lastObsDumpTime.valid())
        {
            m_lastObsDumpTime = obs._time - 1.0;
        }

        // An old observation - throw it away
        if (obs._time <= m_lastObsDumpTime)
        {
            ++it;
            continue;
        }

        // Save the observation
        m_epoObsMap[obs._time].push_back(obs);

        // Dump Epochs
        if((obs._time - m_dOutWait)>m_lastObsDumpTime)
        {
            dumpObsEpoch(obs._time - m_dOutWait);
            m_lastObsDumpTime = obs._time - m_dOutWait;

            if(m_staObsMap.size() > 0)
            {
                m_pppMain->newObs(m_staObsMap);

                // Clear older contents
                m_staObsMap.clear();
            }
        }
        ++it;
    }
}

void SignalCenter::dumpObsEpoch(const CommonTime &maxTime)
{
    EpochObsMap::iterator it = m_epoObsMap.begin();
    while(it!=m_epoObsMap.end())
    {
        const CommonTime& epoTime = it->first;
        if(epoTime <= maxTime)
        {
            list<t_satObs>& allObs = it->second;
            list<t_satObs>::iterator itObs = allObs.begin();
            string& staID = itObs->_staID;
            GPSWeekSecond gs = GPSWeekSecond(itObs->_time);
            if(m_bWriteAllSta)
            {
                *m_obsOutStream << "> " << gs.getWeek() << ' '
                                << setprecision(7) << gs.getSOW() << endl;
            }
            while(itObs!=allObs.end())
            {
                const t_satObs& obs = *itObs;

                // Output into the File
                if(m_bWriteAllSta)
                {
                    reopenObsOutFile();
                    *m_obsOutStream << setiosflags(ios::fixed);
                    GPSWeekSecond gs = GPSWeekSecond(obs._time);
                    *m_obsOutStream << obs._staID << ' ' << asciiSatLine(obs) << endl;

                    if((++itObs)==allObs.end())
                    {
                        *m_obsOutStream << endl;
                    }
                    m_obsOutStream->flush();
                }
                else
                {
                    ++itObs;
                }

                // Add the data to m_staObsMap
                m_staObsMap[staID].push_back(obs);
            }
            it = m_epoObsMap.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void SignalCenter::newGPSEph(GPSEphemeris2& eph)
{
    std::unique_lock<std::mutex> lock(m_gpsEphMutex);
    //std::cout << "New GPS ephmeris! " << std::endl;

    m_ephStore->putNewEph(&eph, true);
    m_navStream->addNewEph(&eph, true);
}

void SignalCenter::newOrbCorr(list<t_orbCorr> orbCorr)
{
    std::unique_lock<std::mutex> lock(m_orbCorrMutex);
    std::unique_lock<std::mutex> lock2(m_gpsEphMutex);
//    std::cout << "New OrbCorr!" << std::endl;
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
        else
        {
            return;
        }
    }
    m_sp3Stream->updateEphmerisStore(m_ephStore);
    return;
}

void SignalCenter::newClkCorr(list<t_clkCorr> clkCorr)
{
    std::unique_lock<std::mutex> lock(m_clkCorrMutex);
    std::unique_lock<std::mutex> lock2(m_gpsEphMutex);
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
    CommonTime lastClkCorrTime = it->_time;
	m_pppMain->setLastClkCorrTime(lastClkCorrTime);
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
        else
        {
            return;
        }
    }
    std::string utcTime = CivilTime(lastClkCorrTime).printf("%02H:%02M:%02S");
    std::cout << "New ClkCorr at time -> " << utcTime << std::endl;
    m_sp3Stream->setLastClkCorrTime(lastClkCorrTime);
    m_sp3Stream->updateEphmerisStore(m_ephStore);
    m_sp3Stream->dumpEpoch();
    return;
}


void SignalCenter::reopenObsOutFile()
{
    SystemTime dateTime;
    CommonTime comTime(dateTime);
    string timeStr = CivilTime(comTime).printf("%Y%02m%02d%02H");
    string obsFileName = m_sFilePath + "Obs" + timeStr +".out";

	if(m_bWriteAllSta && !m_obsOutStream)
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

void SignalCenter::saveObsHeader(const Rinex3ObsHeader& header)
{
	m_obsHeader = header;
	m_pppMain->setObsHeader(header);
}


void SignalCenter::setProcessMode(bool realtime)
{
    m_bRealTime = realtime;
    m_pppMain->setRealTimeFlag(realtime);
}

void SignalCenter::setCorrMount(const string& mntpnt)
{
    m_sCorrMount = mntpnt;
    m_pppMain->setCorrMount(mntpnt);
	m_sp3Stream->setCorrMount(mntpnt);
}

void SignalCenter::setCorrType(const string& corrType)
{
    m_pppMain->setCorrType(corrType);
}

void SignalCenter::setStationList(const list<string>& staList)
{
    m_pppMain->setStationList(staList);
}

void SignalCenter::startPPP()
{
    m_pppMain->start();
}

void SignalCenter::stopPPP()
{
    m_pppMain->stop();
}

