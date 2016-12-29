#ifndef NTRIPOBSSTREAM_HPP
#define NTRIPOBSSTREAM_HPP

#include <string>
#include <vector>
#include <list>

#include "Rinex3ObsHeader.hpp"
#include "satObs.hpp"
#include "DataStructures.hpp"
#include "RinexObsOpt.hpp"
#include "NetUrl.hpp"

using namespace gpstk;
using namespace std;

class NtripObsStream
{
public:

    NtripObsStream(const std::string& staID, const NetUrl& mountPoint,
                   const std::string& latitude, const std::string& longitude,
                   const std::string& nmea, const std::string& ntripVersion);
    //NtripObsStream();

    void resolveFileName(CommonTime& dateTime);
    void saveHeader();
    void addObs(t_satObs obs);
    void writeHeader(const string &format, const CommonTime &firstObsTime);
    void dumpEpoch(const std::string& format, const CommonTime& maxTime);
    void setRnxPath(std::string &path);
    void setRnxObsOpt(RinexObsOpt & opt){m_rinexOpt = opt;}
private:
    std::string     m_sStatID;          ///< station identification
    NetUrl          m_mountPoint;       ///< url of mountpoint
    std::string     m_sRnxPath;         ///< path to save the RINEX file

    double          m_dRinexVersion;
    bool            m_bHeaderWritten;
    bool            m_bHeaderSaved;
    std::string     m_sFileName;
    Rinex3ObsStream m_outStream;
    std::string     m_sPrgmName;
    std::string     m_sUserName;
    std::string     m_sNmea;
    std::string     m_sLatitude;
    std::string     m_sLongitude;
    std::string     m_sNtripVersion;

    std::list<t_satObs> m_obsList;
    Rinex3ObsHeader m_header;
    RinexObsOpt m_rinexOpt;
    std::vector<std::string> m_commentList;
};





#endif // NTRIPOBSSTREAM_HPP
