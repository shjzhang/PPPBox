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
#include "Rinex3NavData.hpp"

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

void NtripNavStream::printEph(OrbitEph *eph)
{
    if(eph==0)
    {
        return;
    }
    Rinex3NavData *navData;
    navData = 0;
    gpstk::SatID::SatelliteSystem system = eph->satID.system;
    switch(system)
    {
        case gpstk::SatID::systemGPS:
        {
            const GPSEphemeris* nowEph = dynamic_cast<const GPSEphemeris*>(eph);
            navData = new Rinex3NavData(*nowEph);
            break;
        }
        default:
        {

        }
    }

    m_ephStream << *navData;
    m_ephStream.flush();
    delete navData;
}

bool NtripNavStream::putNewEph(OrbitEph *eph)
{
    if(eph == 0)
    {
        return false;
    }

    checkEphmeris(eph);

    std::string prn = StringUtils::asString(eph->satID);
    CommonTime ct = eph->ctToc;
    int cnt = m_ephTime.count(prn);
    int size = m_ephTime.size();
    if(m_ephTime.count(prn)!=0)
    {
        if(ct > m_ephTime[prn])
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        m_ephTime[prn] = ct;
    }
    size = m_ephTime.size();
    return true;
}

void NtripNavStream::checkEphmeris(OrbitEph *eph)
{
    if(!eph)
    {
        return;
    }

    // Simple Check - check satellite radial distance
    // ----------------------------------------------
    // Don't know why???


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
            return;
        }
    }

    // Check consistency with older ephemerides
    // ----------------------------------------
}

bool NtripNavStream::checkPrintEph(OrbitEph* eph)
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
