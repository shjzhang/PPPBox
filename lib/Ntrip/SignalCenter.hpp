#ifndef SIGNALCENTER_HPP
#define SIGNALCENTER_HPP

#include <list>
#include <string>
#include <mutex>
#include <condition_variable>

#include "GPSEphemeris2.hpp"
#include "NtripNavStream.hpp"
#include "NtripSP3Stream.hpp"
#include "NtripObsStream.hpp"
#include "PPPMain.hpp"

using namespace gpstk;
class SignalCenter
{
public:
    /// Constructor
    SignalCenter();

    /// Destructor
    ~SignalCenter();

    /// Instance of SiganlCenter class
    static SignalCenter* instance();

    /// New GPS ephmeris data comes
    void newGPSEph(GPSEphemeris2& eph);

    /// New orbit correction data comes
    void newOrbCorr(list<t_orbCorr> orbCorr);

    /// New clock correction data comes
    void newClkCorr(list<t_clkCorr> clkCorr);

    /// Write GPS ephmeris to rinex navigation file
    void writeGPSEph(GPSEphemeris2* eph);

    /// Write GPS SP3 file
    void writeSP3File();

    /// Get the corrections data file's save path
    std::string getCorrPath(){return m_sCorrPath;}

    /// Start PPP(precise point positioning) process
    void startPPP();

    /// Stop PPP process
    void stopPPP();

public:
    std::mutex m_obsMutex;       /// Mutex for observation data
    std::mutex m_gpsEphMutex;    /// Mutex for gps ephmeris data
    std::mutex m_orbCorrMutex;   /// Mutex for orbit correction data
    std::mutex m_clkCorrMutex;   /// Mutex for clock correction data

    std::condition_variable m_condObsDecoded;     ///< condition variable for obs data
    std::condition_variable m_condGPSEphDecoded;  ///< condition variable for gps eph data
    std::condition_variable m_condOrbCorrDecoded; ///< condition variable for orb corr data
    std::condition_variable m_condClkCorrDecoded; ///< condition variable for clk corr data
    NtripNavStream* m_navStream;   ///< Ntrip ephmeris stream
    NtripSP3Stream* m_sp3Stream;   ///< Ntrip SP3 stream
    std::string m_sCorrPath;       ///< Path to save the correction data

    CommonTime m_lastClkCorrTime;       ///< Time of last clock correction
    bool m_bRunning;               ///< If the ppp process is running
    double m_dCorrWaitTime;        ///< Time of correction stream waiting (sec)
    string m_sCorrMount;           ///< Name of the correction mountpoint
    bool m_bRealTime;              ///< Flag of real-time mode

    PPPMain* m_pppMain;            ///< PPP main controller

};

#define SIG_CENTER (SignalCenter::instance())

#endif // SIGNALCENTER_HPP
