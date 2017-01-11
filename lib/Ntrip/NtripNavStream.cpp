#pragma ident "$Id$"

/**
* @file NtripNavStream.cpp
* Class to print the decoded ephmeris data.
*/

#include <iostream>
#include <cstdlib>

#include "FileSpec.hpp"
#include "NtripNavStream.hpp"
#include "NtripToolVersion.hpp"
#include "SystemTime.hpp"
#include "YDSTime.hpp"
#include "FileUtils.hpp"
#include "StringUtils.hpp"
#include "SatID.hpp"
#include "Rinex3NavData2.hpp"

NtripNavStream::NtripNavStream()
{
    m_sEphPath = ".";
    m_sFileName = "";
    m_iRinexVer = 3;
    m_dRinexVer = 3.01;
    m_bHeaderWritten = false;
    setEphPath(m_sEphPath);
}


void NtripNavStream::setEphPath(std::string &path)
{
    if(!path.empty())
    {
        if(path[path.size()-1]!=slash)
        {
            path += slash;
        }
        m_sEphPath = path;
    }
    else
    {
        m_sEphPath = "." + slash;
    }
}



void NtripNavStream::resolveFileName(CommonTime &dateTime)
{
    std::string doy  = YDSTime(dateTime).printf("%03j");
    std::string yy = YDSTime(dateTime).printf("%y");
    int hour = CivilTime(dateTime).hour;
    std::string hStr = "";
    if(hour<10)
    {
        hStr = StringUtils::asString(hour);
    }
    else
    {
        hStr = (char)(hour-9)+ 'a' - '1';
    }

    m_sFileName = m_sEphPath + "brdc" + doy + hStr + "." + yy + "n";
}

void NtripNavStream::printEphHeader()
{
    std::string pgmName = NTRIPTOOLPGMNAME;
    std::string userName;
#ifdef WIN32
    userName = ::getenv("USERNAME");
#else
    userName = ::getenv("USER");
#endif

    SystemTime sysTime;
    CommonTime comTime(sysTime);
    resolveFileName(comTime);

    std::string utcTime = CivilTime(comTime).printf("%Y%02m%02d %02H%02M%02S") +
            " UTC";

    // Append to existing file and return
    // ----------------------------------
    if(FileUtils::fileAccessCheck(m_sFileName))
    {
        m_ephStream.open(m_sFileName.c_str(), std::ios::app);
        m_bHeaderWritten = true;
    }
    else
    {
        m_ephStream.open(m_sFileName.c_str(), std::ios::out);
    }

    Rinex3NavHeader header;
    header.fileAgency = userName;
    header.fileProgram = pgmName;
    header.fileSys = "GPS";
    header.fileType = "N: GNSS NAV DATA";
    header.date = utcTime;
    header.valid = Rinex3NavHeader::allValid3;

    if(!m_bHeaderWritten)
    {
        m_ephStream << header;
    }
    m_ephStream.flush();
    m_bHeaderWritten = true;
}

OrbitEph2* NtripNavStream::ephLast(const std::string &prn)
{
    if(m_eph.count(prn)!=0)
    {
        return m_eph[prn].back();
    }
    else
    {
        return 0;
    }
}

OrbitEph2* NtripNavStream::ephPrev(const std::string &prn)
{
    if(m_eph.count(prn)!=0)
    {
        unsigned n = m_eph[prn].size();
        if(n > 1)
        {
            return m_eph[prn].at(n-2);
        }

    }
    return 0;
}

void NtripNavStream::printEph(OrbitEph2 *eph)
{
    if(eph==0)
    {
        return;
    }
    Rinex3NavData2 *navData;
    navData = 0;
    gpstk::SatID::SatelliteSystem system = eph->satID.system;
    switch(system)
    {
        case gpstk::SatID::systemGPS:
        {
            const GPSEphemeris2* nowEph = dynamic_cast<const GPSEphemeris2*>(eph);
            navData = new Rinex3NavData2(*nowEph);
            break;
        }
        default:
        {
            break;
        }
    }

    m_ephStream << *navData;
    m_ephStream.flush();
    delete navData;
}

bool NtripNavStream::putNewEph(OrbitEph2 *eph)
{
    //std::lock_guard<std::mutex> guard(m_mutex);

    if(eph == 0)
    {
        return false;
    }

    checkEphmeris(eph);

    const GPSEphemeris2* ephGPS = dynamic_cast<const GPSEphemeris2*>(eph);

    OrbitEph2* ephNew = 0;

    if(ephGPS)
    {
        ephNew = new GPSEphemeris2(*ephGPS);
    }
    else
    {
        return false;
    }

    std::string prn = StringUtils::asString(eph->satID);
    const OrbitEph2* ephOld = ephLast(prn);

    if(ephOld && (ephOld->getCheckState() == OrbitEph2::bad
               || ephOld->getCheckState() == OrbitEph2::outdated))
    {
        ephOld = 0;
    }

    if((ephOld ==0 || ephNew->isNewerThan(ephOld)) &&
       (eph->getCheckState() != OrbitEph2::bad &&
        eph->getCheckState() != OrbitEph2::outdated))
    {
        std::deque<OrbitEph2*>& qq = m_eph[prn];
        qq.push_back(ephNew);
        if(qq.size() > m_iMaxQueueSize)
        {
            delete qq.front();
            qq.pop_front();
        }

        return true;
    }
    else
    {
        delete ephNew;
        return false;
    }
}

void NtripNavStream::checkEphmeris(OrbitEph2 *eph)
{
    if(!eph)
    {
        return;
    }

    // Simple Check - check satellite radial distance
    // ----------------------------------------------
    Xvt sv;
    double clkcorr;
    eph->dataLoadedFlag = true;
    if(!eph->getCrd(eph->ctToc, sv, clkcorr, false))
    {
        eph->setCheckState(OrbitEph2::bad);
        return;
    }

    double rr = sv.x.mag();


    const double MINDIST = 2.e7;
    const double MAXDIST = 6.e7;

    if (rr < MINDIST || rr > MAXDIST)
    {
        eph->setCheckState(OrbitEph2::bad);
        return;
    }


    // Check whether the epoch is too far away from the current time
    // --------------------------------------------------------

    CommonTime toc = eph->ctToc;
    SystemTime now;
    CommonTime currentTime(now);

    currentTime.setTimeSystem(toc.getTimeSystem());
    double timeDiff = fabs(toc - currentTime);
    SatID::SatelliteSystem system = eph->satID.system;
    if(system == SatID::systemGPS || system == SatID::systemGalileo)
    {
        // update interval: 2h, data sets are valid for 4 hours
        if(timeDiff > 4*3600)
        {
            // outdated
            eph->setCheckState(OrbitEph2::outdated);
            return;
        }
    }
    else if(system == SatID::systemGlonass)
    {
        // updated every 30 minutes
        if(timeDiff > 1*3600)
        {
            // outdated
            eph->setCheckState(OrbitEph2::outdated);
            return;
        }
    }
    else if(system == SatID::systemBeiDou)
    {
        // updates 1 (GEO) up to 6 hours
        if(timeDiff > 6*3600)
        {
            // outdated
            eph->setCheckState(OrbitEph2::outdated);
            return;
        }
    }
    else if(system == SatID::systemQZSS)
    {
        // orbit parameters are valid for 7200 seconds (at minimum)
        if(timeDiff > 4*3600)
        {
            // outdated
            eph->setCheckState(OrbitEph2::outdated);
            return;
        }
    }

    // Check consistency with older ephemerides
    // ----------------------------------------
    const double MAXDIFF = 1000.0;
    std::string prn = StringUtils::asString(eph->satID);
    OrbitEph2* ephL = ephLast(prn);

    if(ephL)
    {
        Xvt svL;
        double clkcorrL;
        ephL->getCrd(eph->ctToc, svL, clkcorrL, false);

        double dt = eph->ctToc - ephL->ctToc;
        double diffX = (sv.x - svL.x).mag();
        double diffC = fabs(clkcorr - clkcorrL) * C_MPS;

        // some lines to allow update of ephemeris data sets after outage
        if(system == SatID::systemGPS && dt > 4*3600)
        {
            ephL->setCheckState(OrbitEph2::outdated);
            return;
        }
        if(system == SatID::systemGalileo && dt > 4*3600)
        {
            ephL->setCheckState(OrbitEph2::outdated);
            return;
        }
        if(system == SatID::systemGlonass && dt > 1*3600)
        {
            ephL->setCheckState(OrbitEph2::outdated);
            return;
        }
        if(system == SatID::systemBeiDou && dt > 6*3600)
        {
            ephL->setCheckState(OrbitEph2::outdated);
            return;
        }
        if(system == SatID::systemQZSS && dt > 4*3600)
        {
            ephL->setCheckState(OrbitEph2::outdated);
            return;
        }

        if (diffX < MAXDIFF && diffC < MAXDIFF)
        {
          if (dt != 0.0) {
            eph->setCheckState(OrbitEph2::ok);
            ephL->setCheckState(OrbitEph2::ok);
          }
          else
          {
              // do nothing here
          }
        }
        else
        {
          if (ephL->getCheckState() == OrbitEph2::ok)
          {
            eph->setCheckState(OrbitEph2::bad);
          }
          else
          {
              // do nothing here
          }
        }
    }
}

bool NtripNavStream::checkPrintEph(OrbitEph2* eph)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if(eph == 0)
    {
        return false;
    }

    if(!m_bHeaderWritten)
    {
        printEphHeader();
    }
    if(putNewEph(eph))
    {
        std::cout << "write the ephmeris! " << std::endl;
        printEph(eph);
    }
    return true;
}
