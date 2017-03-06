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
    m_bWriteFile = true;
    m_ephStore = new RealTimeEphStore();
    setEphPath(m_sEphPath);
}


NtripNavStream::~NtripNavStream()
{
    delete m_ephStore;
}


NtripNavStream::NtripNavStream(NtripNavStream &right)
//    m_eph(right.m_eph)
{}

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


bool NtripNavStream::addNewEph(OrbitEph2* eph, bool check)
{
    std::lock_guard<std::mutex> guard(m_mutex);
    if(eph == 0)
    {
        return false;
    }

    if(!m_bHeaderWritten && m_bWriteFile)
    {
        printEphHeader();
    }

    if(m_ephStore->putNewEph(eph,check) && m_bWriteFile)
    {
        std::cout << "Write the ephmeris for "
                  << StringUtils::asString(eph->satID) << std::endl;
        printEph(eph);
    }
    return true;
}
