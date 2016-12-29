#ifndef NTRIPNAVSTREAM_HPP
#define NTRIPNAVSTREAM_HPP

#include <string>
#include <map>
#include <mutex>

#include "OrbitEph.hpp"
#include "GPSEphemeris.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"

using namespace gpstk;

class NtripNavStream
{
public:

    /// Default constructor
    NtripNavStream();


    /// Print Header of the output File(s)
    void printEphHeader();


    /// Print One Ephemeris
    void printEph(OrbitEph* eph);


    /// Check the ephmeris before print
    bool checkPrintEph(OrbitEph* eph);


    /// Set the ephmeris file path
    void setEphPath(std::string& path);


    /// Resolve file name according to RINEX standards
    void resolveFileName(CommonTime &dateTime);
private:
    std::string m_sEphPath;         ///< path to save ephmeris file
    std::string m_sFileName;        ///< file name
    int         m_iRinexVer;        ///< RINEX version
    double      m_dRinexVer;        ///< RINEX version
    Rinex3NavStream  m_ephStream;   ///< RINEX navagation ephmeris stream
    bool        m_bHeaderWritten;   ///< if write the header
    std::mutex  m_mutex;            ///< mutex
    std::map<std::string, CommonTime> m_ephTime;  ///< map to store the time
                                                  ///< of every satellite ephmeris


    /// Check and print the ephmeris
    //bool checkAndPrint(OrbitEph* eph);

    /// Check the ephmeris
    void checkEphmeris(OrbitEph* eph);

    /// Store ephmeris time
    bool putNewEph(OrbitEph* eph);
    /// Return the last ephmeris according its prn
    //OrbitEph* ephLast(const std::string& prn);

};

#endif // NTRIPNAVSTREAM_HPP
