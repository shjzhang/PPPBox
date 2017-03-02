#ifndef NTRIPSP3STREAM_HPP
#define NTRIPSP3STREAM_HPP

#include <string>
#include <list>
#include <mutex>

#include "CommonTime.hpp"
#include "OrbitEphStore2.hpp"
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

class NtripSP3Stream
{
public:

    /// Default constructor
    NtripSP3Stream();

    /// Destructor
    ~NtripSP3Stream();

    /// Print Header of the output File
    void printHeader(const CommonTime &dateTime);

    /// Set the SP3 file saving path
    void setSavePath(std::string& path);

    /// Load the ephmeris data
    void setEph(OrbitEph2 *eph){m_eph = eph;}

    /// Resolve file name according to RINEX standards
    void resolveFileName(CommonTime &dateTime);

    /// Close the file writer
    void closeFile();

    /// Add ephmeris data to the store
    void printSP3Ephmeris();

    /// Write SP3 file.
    void writeFile(CommonTime& epoTime, std::string& prn, Xvt& sv);

    /// Put ephmeris data
    void putEphmeris(const OrbitEph2* eph);

    /// New GPS ephmeris comes
    void newGPSEph(GPSEphemeris2& eph);

    /// New orbit correction comes
    void newOrbCorr(std::list<t_orbCorr> orbCorr);

    /// New clock correction comes
    void newClkCorr(std::list<t_clkCorr> clkCorr);

    /// Set the time of last clock correction
    void setLastClkCorrTime(CommonTime& time){ m_lastClkCorrTime = time;}

    /// Update the ephmeris store
    void updateEphmerisStore(NtripNavStream* ephStore);

private:

    std::mutex  m_mutex;            ///< Mutex
    std::string m_sPath;            ///< Path to save SP3 file
    std::string m_sFileName;        ///< File name

    NtripNavStream* m_ephStream;    ///< Ntrip ephmeris stream
    std::ofstream m_outStream;      ///< Out file stram
    SP3Stream   m_sp3Stream;        ///< SP3 stream
    SP3Header   m_header;           ///< SP3 header
    SP3Header::Version m_eSP3Ver;   ///< SP3 file version
    bool        m_bHeaderWritten;   ///< If write the header
    double      m_dSample;          ///< Sample of saving SP3 file(sec)
    CommonTime  m_lastEpoTime;      ///< Last epoch time
    CommonTime  m_lastClkCorrTime;  ///< Last clock correction time

    OrbitEph2*    m_eph;            ///< ephmeris

};

    /// Add ephmeris data to the store
    NtripSP3Stream& operator>> (NtripSP3Stream& sp3Stream,
                                OrbitEphStore2& sp3EphStore);

#endif // NTRIPSP3STREAM_HPP
