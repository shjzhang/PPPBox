#ifndef NTRIPNAVSTREAM_HPP
#define NTRIPNAVSTREAM_HPP

#include <string>
#include <map>
#include <deque>
#include <mutex>

#include "NtripFileBase.hpp"
#include "OrbitEph2.hpp"
#include "GPSEphemeris2.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "RealTimeEphStore.hpp"

using namespace gpstk;

class NtripNavStream : public NtripFileBase
{
public:

    /// Default constructor
    NtripNavStream();

    /// Destructor
    ~NtripNavStream();

    /// Copy another NtripNavStream object
    NtripNavStream(NtripNavStream& right);

    /// Store the new ephmeris
    bool addNewEph(OrbitEph2* eph, bool check);

    /// Print ephemeris at one epoch
    void printEph(OrbitEph2* eph);

    /// Print Header of the output File
    void printHeader();

    /// Resolve file name according to specified standards
    void resolveFileName(CommonTime& dateTime);

private:
    int         m_iRinexVer;        ///< RINEX version
    double      m_dRinexVer;        ///< RINEX version
    Rinex3NavStream  m_ephStream;   ///< RINEX navagation ephmeris stream
    RealTimeEphStore* m_ephStore;   ///< Ephmeris store
    std::mutex  m_mutex;            ///< Mutex

};

#endif // NTRIPNAVSTREAM_HPP
