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

using namespace gpstk;

class NtripNavStream
{
public:

    /// Default constructor
    NtripNavStream();


    /// Print Header of the output File
    void printEphHeader();


    /// Print One Ephemeris
    void printEph(OrbitEph2* eph);


    /// Check the ephmeris before print
    bool checkPrintEph(OrbitEph2* eph);


    /// Set the ephmeris file path
    void setEphPath(std::string& path);


    /// Resolve file name according to RINEX standards
    void resolveFileName(CommonTime &dateTime);

    /// Get the last ephmeris in the queue
    OrbitEph2* ephLast(const std::string& prn);

    /// Get the last ephmeris in the queue
    OrbitEph2* ephPrev(const std::string& prn);

private:
    std::string m_sEphPath;         ///< Path to save ephmeris file
    std::string m_sFileName;        ///< File name
    int         m_iRinexVer;        ///< RINEX version
    double      m_dRinexVer;        ///< RINEX version
    Rinex3NavStream  m_ephStream;   ///< RINEX navagation ephmeris stream
    bool        m_bHeaderWritten;   ///< If write the header
    std::mutex  m_mutex;            ///< Mutex
    std::map<std::string, CommonTime> m_ephTime;  ///< Map to store the time
                                                  ///< of every satellite ephmeris
    static const unsigned   m_iMaxQueueSize = 5;  ///< Maximun size of ephmeris data queue
    std::map<std::string, std::deque<OrbitEph2*> > m_eph; ///< Map to store the ephmeris

    /// Check and print the ephmeris
    //bool checkAndPrint(OrbitEph2* eph);

    /// Check the ephmeris
    void checkEphmeris(OrbitEph2* eph);

    /// Store ephmeris time
    bool putNewEph(OrbitEph2* eph);

};

#endif // NTRIPNAVSTREAM_HPP
