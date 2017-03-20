#ifndef NTRIPOBSSTREAM_HPP
#define NTRIPOBSSTREAM_HPP

#include <string>
#include <vector>
#include <list>
#include <map>
#include <istream>

#include "NtripFileBase.hpp"
#include "NtripToolVersion.hpp"
#include "StringUtils.hpp"
#include "SystemTime.hpp"
#include "FileUtils.hpp"
#include "FileSpec.hpp"
#include "Rinex3ObsHeader.hpp"
#include "satObs.hpp"
#include "DataStructures.hpp"
#include "RinexObsOpt.hpp"
#include "NetUrl.hpp"

using namespace gpstk;

class NtripObsStream : public NtripFileBase
{
public:

    /// Common constructor
    NtripObsStream(const std::string& staID, const NetUrl& mountPoint,
                   const std::string& latitude, const std::string& longitude,
                   const std::string& nmea, const std::string& ntripVersion);

    /// Destructor
    ~NtripObsStream(){;}

    /// Resolve the file name
    void resolveFileName(CommonTime& dateTime);

    /// Save the header data
    void saveHeader();

    /// Set the rinex fiel version
    void setRinexVer(double version)
    { m_dRinexVersion = version;}

    /// Add the observation data
    void addObs(t_satObs obs);

    /// Write the header to RINEX file
    void writeHeader(const string &format, const CommonTime &firstObsTime);

    /// Dump out the observation data by epoch
    void dumpEpoch(const std::string& format, const CommonTime& maxTime);

    /// Set the options of RINEX file
    void setRnxObsOpt(RinexObsOpt & opt){m_rinexOpt = opt;}

    /// Set the station information
    void setStaInfo(const t_staInfo& staInfo);

    // Public members
    Rinex3ObsHeader m_header;           ///< RINEX file header

private:
    std::string     m_sStatID;          ///< station identification
    NetUrl          m_mountPoint;       ///< url of mountpoint

    double          m_dRinexVersion;    ///< RINEX file version
    bool            m_bHeaderSaved;     ///< If had already saved the RINEX header
    std::string     m_sPrgmName;        ///< Name of programe
    std::string     m_sUserName;        ///< User's name
    std::string     m_sNmea;            ///< nmea
    std::string     m_sLatitude;        ///< Latitue
    std::string     m_sLongitude;       ///< Longitude
    std::string     m_sNtripVersion;    ///< Ntrip version
    Rinex3ObsStream m_outStream;        ///< RINEX data out stream

    std::list<t_satObs> m_obsList;      ///< Observation data list
    // RTCM station information
    t_staInfo m_staInfo;
    RinexObsOpt m_rinexOpt;             ///< RINEX Observation data option
    std::vector<std::string> m_commentList;///< Comment list
};

    /// Convert the obsList data to Rinex3ObsData object
    inline Rinex3ObsData convertToRinexObsData(std::list<t_satObs> obsList, Rinex3ObsHeader& header)
    {
        // Time of Epoch
        const t_satObs& fObs = *(obsList.begin());

        Rinex3ObsData rnxObsData;
        rnxObsData.time = fObs._time;
        rnxObsData.numSVs = obsList.size();
        rnxObsData.epochFlag = 0;

        // loop over all satellites
        std::list<t_satObs>::iterator it;
        for(it = obsList.begin();it!=obsList.end();++it)
        {
            const t_satObs& satObs = *it;
            // get the satellite ID
            RinexSatID prn = satObs._prn;
            std::vector<RinexSatID> satIndex;
            satIndex.push_back(satObs._prn);
            std::string sys = StringUtils::asString(satObs._prn.systemChar());
            int freqNum = satObs._obs.size();
            int typeSize  = header.mapObsTypes[sys].size();

            std::vector<RinexDatum> data;

            // loop over all types
            for(int j = 0; j < typeSize; ++j)
            {
                std::string type;
                int freq;
                type = header.mapObsTypes[sys][j].asString();
                std::istringstream is(type.substr(1,2));

                // get the frequency
                is >> freq;
                freq--;
                const t_frqObs* frqObs = satObs._obs[freq];
                RinexDatum tempData;
                if(frqObs == 0)
                {
                    continue;
                }
                if(type.substr(1) != frqObs->_rnxType2ch)
                {
                    data.push_back(tempData);  // put back the null data
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
            rnxObsData.obs[prn] = data;
        }
        return rnxObsData;
    }

    /// Convert the obsList data to gnssRinex object
    inline gnssRinex obsList2gnssRinex(std::list<t_satObs> obsList, Rinex3ObsHeader& header)
    {
        gnssRinex g;
        Rinex3ObsData rod = convertToRinexObsData(obsList,header);
        g.header.source.type = SatIDsystem2SourceIDtype(header.fileSysSat);
        g.header.source.sourceName = header.markerName;
        g.header.epochFlag = rod.epochFlag;
        g.header.epoch = rod.time;
        g.body = satTypeValueMapFromRinex3ObsData(header, rod);
        return g;

    }

    /// One Line in ASCII (Internal) Format
    inline std::string asciiSatLine(const t_satObs &obs)
    {
        ostringstream str;
        str.setf(ios::showpoint | ios::fixed);

        str << obs._prn.toString();

        for (unsigned ii = 0; ii < obs._obs.size(); ii++) {
          const t_frqObs* frqObs = obs._obs[ii];
          if (frqObs->_codeValid) {
            str << ' '
                << left  << setw(3)  << "C" + frqObs->_rnxType2ch << ' '
                << right << setw(14) << setprecision(3) << frqObs->_code;
          }
          if (frqObs->_phaseValid) {
            str << ' '
                << left  << setw(3) << "L" + frqObs->_rnxType2ch << ' '
                << right << setw(14) << setprecision(3) << frqObs->_phase << ' '
                << right << setw(4)                     << frqObs->_slipCounter;
          }
          if (frqObs->_dopplerValid) {
            str << ' '
                << left  << setw(3) << "D" + frqObs->_rnxType2ch << ' '
                << right << setw(14) << setprecision(3) << frqObs->_doppler;
          }
          if (frqObs->_snrValid) {
            str << ' '
                << left  << setw(3) << "S" + frqObs->_rnxType2ch << ' '
                << right << setw(8) << setprecision(3) << frqObs->_snr;
          }
        }

        return str.str();
    }








#endif // NTRIPOBSSTREAM_HPP
