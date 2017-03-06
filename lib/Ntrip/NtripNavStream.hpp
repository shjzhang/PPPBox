#ifndef NTRIPNAVSTREAM_HPP
#define NTRIPNAVSTREAM_HPP

#include <string>
#include <map>
#include <deque>
#include <mutex>

#include "OrbitEph2.hpp"
#include "GPSEphemeris2.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "RealTimeEphStore.hpp"

using namespace gpstk;

class NtripNavStream
{
public:

    /// Default constructor
    NtripNavStream();

    /// Destructor
    ~NtripNavStream();

    /// Copy another NtripNavStream object
    NtripNavStream(NtripNavStream& right);

    /// Resolve file name according to RINEX standards
    void resolveFileName(CommonTime &dateTime);

    /// Print Header of the output File
    void printEphHeader();

    /// Store the new ephmeris
    bool addNewEph(OrbitEph2* eph, bool check);

    /// Print ephemeris at one epoch
    void printEph(OrbitEph2* eph);

    /// Set the ephmeris file path
    void setEphPath(std::string& path);

    /// Set the choice if write the rinex file
    void setWriteRinexFile(bool flag)
    { m_bWriteFile = flag; }

private:
    std::string m_sEphPath;         ///< Path to save ephmeris file
    std::string m_sFileName;        ///< File name
    bool        m_bWriteFile;       ///< If write the ephemeris data to RINEX file
    int         m_iRinexVer;        ///< RINEX version
    double      m_dRinexVer;        ///< RINEX version
    Rinex3NavStream  m_ephStream;   ///< RINEX navagation ephmeris stream
    RealTimeEphStore* m_ephStore;   ///< Ephmeris store
    bool        m_bHeaderWritten;   ///< If had writen the RINEX header
    std::mutex  m_mutex;            ///< Mutex


};

#endif // NTRIPNAVSTREAM_HPP
