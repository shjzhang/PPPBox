#include <vector>
#include <cstdlib>
#include <iostream>
#include <sstream>

#include "NtripObsStream.hpp"

using namespace StringUtils;

NtripObsStream::NtripObsStream(const std::string& staID, const NetUrl& mountPoint,
                                    const std::string& latitude, const std::string& longitude,
                                    const std::string& nmea, const std::string& ntripVersion)
{
    m_sStatID = staID;
    m_mountPoint = mountPoint;
    m_sNmea = nmea;
    m_sLatitude = latitude;
    m_sLongitude = longitude;
    m_sNtripVersion = ntripVersion;

    m_bHeaderSaved = false;
    m_bHeaderWritten = false;
    m_sPrgmName = NTRIPTOOLPGMNAME;

    m_sRnxPath = ".";
    setRnxPath(m_sRnxPath);
#ifdef WIN32
    m_sUserName = ::getenv("USERNAME");
#else
    m_sUserName = ::getenv("USER");
#endif
    m_header.clear();
}

void NtripObsStream::setRnxPath(std::string &path)
{
    if(!path.empty())
    {
        if(path[path.size()-1]!=slash)
        {
            path += slash;
        }
        m_sRnxPath = path;
    }
    else
    {
        m_sRnxPath = "." + slash;
    }
}

// File Name according to RINEX Standards
///////////////////////////////////////////////////////////
void NtripObsStream::resolveFileName(CommonTime &dateTime)
{
    std::string ID4 = m_sStatID.substr(0,4);
    ID4 = StringUtils::upperCase(ID4);

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
    m_sFileName = m_sRnxPath + ID4 + doy + hStr + "." + yy + "O";
}

void NtripObsStream::setStaInfo(const t_staInfo &staInfo)
{
    m_staInfo = staInfo;
}

// Save the RINEX header
///////////////////////////////////////////////////////////
void NtripObsStream::saveHeader()
{

    int navSys = SYS_GPS;
    int freqType = FREQTYPE_L1 | FREQTYPE_L2;
    int obsType = OBSTYPE_PR | OBSTYPE_CP | OBSTYPE_SNR;
    m_dRinexVersion = 3.00;
    m_header.valid = Rinex3ObsHeader::allValid30;
    std::string fileType = "Obs";

    m_rinexOpt.setRnxVer(m_dRinexVersion);
    m_rinexOpt.setFileType(fileType);
    m_rinexOpt.setNavSys(navSys);
    m_rinexOpt.setFreqType(freqType);
    //m_rinexOpt.setObsType(obsType);
    m_rinexOpt.setMarkerName(m_sStatID);
    m_rinexOpt.setPrgName(m_sPrgmName);
    m_rinexOpt.setRunBy(m_sUserName);

    m_rinexOpt.getRnxObsHeader(m_header);

    m_header.antennaDeltaXYZ = Triple(m_staInfo.del[0],
                                      m_staInfo.del[1],
                                      m_staInfo.del[2]);
    m_header.antennaPosition = Triple(m_staInfo.pos[0],
                                      m_staInfo.pos[1],
                                      m_staInfo.pos[2]);
    //m_header.recNo = m_staInfo.rcvNum;
    m_header.recType = m_staInfo.rcvDes;
    m_header.antType = m_staInfo.antDes;
    m_header.antNo = m_staInfo.antNum;
    if(m_header.antennaPosition.mag() != 0.0)
    {
        m_header.valid |= Rinex3ObsHeader::validAntennaPosition;
    }

    m_bHeaderSaved = true;
}

void NtripObsStream::writeHeader(const std::string& format, const CommonTime& firstObsTime)
{
    SystemTime sysTime;
    CommonTime comTime(sysTime);
    resolveFileName(comTime);

    // Append to existing file and return
    // ----------------------------------
    if( FileUtils::fileAccessCheck(m_sFileName))
    {
        m_outStream.open(m_sFileName, ios::app);
        m_bHeaderWritten = true;
    }
    else
    {
        m_outStream.open(m_sFileName, ios::out);
        m_commentList.clear();
    }

    // Save the Rinex header contents
    // ----------------------------------
    if(!m_bHeaderSaved)
    {
        this->saveHeader();
    }

    // Add the additional comments
    // ----------------------------------
    std::string comment = leftJustify(format, 6) + "  " + m_mountPoint.getCasterHost()
                       + "/" + m_mountPoint.getPath() ;
    m_commentList.push_back(comment);
    if(m_sNmea == "yes")
    {
        comment = "NMEA LAT=" + m_sLatitude + " LONG=" + m_sLongitude;
        m_commentList.push_back(comment);
    }
    m_header.commentList = m_commentList;
    m_header.valid |= Rinex3ObsHeader::validComment;

    m_header.firstObs = CivilTime(firstObsTime);

    if(!m_bHeaderWritten)
    {
        m_outStream << m_header;
    }

    m_outStream.header = m_header;
    m_bHeaderWritten = true;
}

// Stores Observation into Internal Array
////////////////////////////////////////////////////////////////////////////
void NtripObsStream::addObs(t_satObs obs)
{
    m_obsList.push_back(obs);
}

void NtripObsStream::dumpEpoch(const string &format, const CommonTime &maxTime)
{
    // Select observations older than maxTime
    // --------------------------------------
    std::list<t_satObs> obsList;
    std::list<t_satObs>::iterator it = m_obsList.begin();
    for(;it!=m_obsList.end();++it)
    {
        t_satObs& obs = *it;
        if(obs._time < maxTime)
        {
            obsList.push_back(obs);
        }
        else
        {
            // Do nothing here
        }
    }

    if(obsList.empty())
    {
        return;
    }

    m_obsList.erase(m_obsList.begin(),--(m_obsList.end()));
    // Time of Epoch
    const t_satObs& fObs = *(obsList.begin());

    // Write RINEX Header
    if(!m_bHeaderWritten)
    {
        this->writeHeader(format, fObs._time);
    }
    if(!m_bHeaderWritten) return;

    Rinex3ObsData rnxObsData = convertToRinexObsData(obsList, m_header);
    m_outStream << rnxObsData;
    m_outStream.flush();
}

