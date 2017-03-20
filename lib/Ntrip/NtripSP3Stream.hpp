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
    void resolveFileName(CommonTime &dateTime);

    /// Dump out the data by epoch
    void dumpEpoch();

    /// Close the file writer
    void closeFile();

    /// Write SP3 file.
    void writeFile(CommonTime& epoTime, SatID &prn, Xvt& sv);

    /// Set the time of last clock correction
    void setLastClkCorrTime(CommonTime& time){ m_lastClkCorrTime = time;}

    /// Update the ephmeris store
    void updateEphmerisStore(RealTimeEphStore *ephStore);

private:

    //std::mutex  m_mutex;            ///< Mutex
    NtripNavStream* m_ephStream;    ///< Ntrip ephmeris stream
    RealTimeEphStore* m_ephStore;   ///< Ephemeris store
    std::ofstream m_outStream;      ///< Out file stream
    SP3Stream   m_sp3Stream;        ///< SP3 stream
    SP3Header   m_header;           ///< SP3 header
    SP3Header::Version m_eSP3Ver;   ///< SP3 file version
    double      m_dSample;          ///< Sample of saving SP3 file(sec)
    CommonTime  m_lastEpoTime;      ///< Last epoch time
    CommonTime  m_lastClkCorrTime;  ///< Last clock correction time

    OrbitEph2*    m_eph;            ///< ephmeris

};


#endif // NTRIPSP3STREAM_HPP
