#ifndef NTRIPSP3STREAM_HPP
#define NTRIPSP3STREAM_HPP

#include <string>
#include <list>
#include <mutex>

#include "CommonTime.hpp"
#include "RealTimeEphStore.hpp"
#include "SP3Stream.hpp"
#include "SP3Header.hpp"
#include "SP3SatID.hpp"
#include "SP3Data.hpp"
#include "OrbitEph2.hpp"
#include "GPSEphemeris2.hpp"
#include "GPSWeekSecond.hpp"
#include "Xvt.hpp"
#include "NtripNavStream.hpp"

using namespace gpstk;

class NtripSP3Stream  : public NtripFileBase
{
public:

    /// Default constructor
    NtripSP3Stream();

    /// Destructor
    ~NtripSP3Stream();

    /// Print Header of the output File
    void printHeader(const CommonTime &dateTime);

    /// Load the ephmeris data
    void setEph(OrbitEph2 *eph){m_eph = eph;}

    /// Resolve file name according to RINEX standards
    void resolveFileName(const CommonTime &dateTime);

    /// Dump out the data by epoch
    void dumpEpoch();

    /// Close the file writer
    void closeFile();

    /// Write SP3 file.
    void writeFile(CommonTime& epoTime, SatID &prn, Xvt& sv);

    /// Set the time of last clock correction
    void setLastClkCorrTime(CommonTime& time){ m_lastClkCorrTime = time;}

    /// Set the correction mountpoint
    void setCorrMount(const std::string& mntpnt) { m_sCorrMount = mntpnt; }

    /// Set the sp3 file name
    void setSP3FileName(const std::string& filename) { m_sFileName = filename; }

    /// Update the ephmeris store
    void updateEphmerisStore(RealTimeEphStore *ephStore);

    /// Set the sample of output file
    void setOutputSample(double sample) { m_dSample = sample; }

private:

    std::mutex  m_mutex;            ///< Mutex
    RealTimeEphStore m_ephStore;    ///< Ephemeris store
    std::ofstream m_outStream;      ///< Out file stream
    std::string m_sCorrMount;       ///< Correction mountpoint
    SP3Stream   m_sp3Stream;        ///< SP3 stream
    SP3Header   m_header;           ///< SP3 header
    SP3Header::Version m_eSP3Ver;   ///< SP3 file version
    double      m_dSample;          ///< Sample of saving SP3 file(sec)
    CommonTime  m_lastEpoTime;      ///< Last epoch time
    CommonTime  m_lastClkCorrTime;  ///< Last clock correction time

    OrbitEph2*    m_eph;            ///< ephmeris

};


#endif // NTRIPSP3STREAM_HPP
