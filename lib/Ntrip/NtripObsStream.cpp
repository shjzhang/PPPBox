#include <vector>
#include <cstdlib>
#include <iostream>

#include "NtripObsStream.hpp"
#include "NtripToolVersion.hpp"
#include "StringUtils.hpp"
#include "SystemTime.hpp"
#include "FileUtils.hpp"
#include "FileSpec.hpp"

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
    m_rinexOpt.setObsType(obsType);
    m_rinexOpt.setMarkerName(m_sStatID);
    m_rinexOpt.setPrgName(m_sPrgmName);
    m_rinexOpt.setRunBy(m_sUserName);

    m_rinexOpt.getRnxObsHeader(m_header);

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

    m_obsList.clear();
    int ss = m_obsList.size();
    // Time of Epoch
    const t_satObs& fObs = *(obsList.begin());

    // Write RINEX Header
    if(!m_bHeaderWritten)
    {
        this->writeHeader(format, fObs._time);
    }
    if(!m_bHeaderWritten) return;

    Rinex3ObsData rnxObsData;
    rnxObsData.time = fObs._time;
    rnxObsData.numSVs = obsList.size();
    rnxObsData.epochFlag = 0;

    // loop over all satellites
    for(it = obsList.begin();it!=obsList.end();++it)
    {
        const t_satObs& satObs = *it;
        // get the satellite ID
        RinexSatID prn = satObs._prn;
        std::vector<RinexSatID> satIndex;
        satIndex.push_back(satObs._prn);
        std::string sys = asString(satObs._prn.systemChar());
        int freqNum = satObs._obs.size();
        int typeSize  = m_header.mapObsTypes[sys].size();

        // loop over all frequencies
        std::vector<RinexDatum> data;
        for(int i = 0; i < freqNum; ++i)
        {
            const t_frqObs* frqObs = satObs._obs[i];
            // loop over all types
            for(int j = 0; j < typeSize; ++j)
            {
                std::string type = m_header.mapObsTypes[sys][j].asString(); /// occurs bug
                RinexDatum tempData;
                if(frqObs == 0)
                {
                    continue;
                }
                if(type.substr(1) != frqObs->_rnxType2ch)
                {
                    data.push_back(tempData);
                    continue;
                }
                if(type[0] == 'C' && frqObs->_codeValid)
                {
                    tempData.data = frqObs->_code;
                }
                else if(type[0] == 'L' && frqObs->_phaseValid)
                {
                    tempData.data = frqObs->_phase;
                    if(frqObs->_slip)
                    {
                        tempData.lli |= 1;
                    }
                }
                else if(type[0] == 'D' && frqObs->_dopplerValid)
                {
                    tempData.data = frqObs->_doppler;
                }
                else if(type[0] == 'S' &&  frqObs->_snrValid)
                {
                    tempData.data = frqObs->_snr;
                }
                data.push_back(tempData);
            }
        }
        rnxObsData.obs[prn] = data;
    }

    m_outStream << rnxObsData;
    m_outStream.flush();
}


