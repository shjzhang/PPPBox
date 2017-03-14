#ifndef SIGNALCENTER_HPP
#define SIGNALCENTER_HPP

#include <list>
#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <future>
#include <exception>
#include <fstream>

#include "NtripObsStream.hpp"
#include "GPSEphemeris2.hpp"
#include "NtripNavStream.hpp"
#include "NtripSP3Stream.hpp"
#include "RealTimeEphStore.hpp"

using namespace std;
using namespace gpstk;

// Epoch observation map type
typedef std::map<CommonTime,std::list<t_satObs> > EpochObsMap;

// Station obslist map valuse
typedef std::map<std::string, std::list<t_satObs> > StaObsMap;


class PPPMain;
//class PPPTask;

// Class "SignalCenter"
class SignalCenter
{
public:
    /// Constructor
    SignalCenter();

    /// Destructor
    ~SignalCenter();

    /// Instance of SiganlCenter class
    static SignalCenter* instance();

    /// New observation data comes
    void newObs(list<t_satObs> obsList);

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

    /// Reopen output file of the observation data
    void reopenObsOutFile();

    /// Dump observations of all stations at specified time
    void dumpObsEpoch(const CommonTime& maxTime);

    /// Set the choice if write the all stations observation into a file
    void setWriteAllStaObs(bool choice)
    { m_bWriteAllSta = choice; }

    /// Get the corrections data file's save path
    string getCorrPath()
    { return m_sCorrPath; }

    /// Set PPP configuration file
    void setPPPConfFile(const string& confFile)
    { m_sPPPConfile = confFile; }

    /// Start PPP(precise point positioning) process
    void startPPP();

    /// Stop PPP process
    void stopPPP();

public:
    mutex m_obsMutex;                        ///< Mutex for observation data
    mutex m_allObsMutex;                        ///< Mutex for observation data
    mutex m_gpsEphMutex;                     ///< Mutex for gps ephmeris data
    mutex m_orbCorrMutex;                    ///< Mutex for orbit correction data
    mutex m_clkCorrMutex;                    ///< Mutex for clock correction data

    condition_variable m_condObsReady;       ///< condition variable for obs data
    condition_variable m_condGPSEphReady;    ///< condition variable for gps eph data
    condition_variable m_condClkCorrReady;   ///< condition variable for clock correction

    NtripObsStream* m_obsStream;             ///< Ntrip observation stream
    NtripNavStream* m_navStream;             ///< Ntrip ephmeris stream
    NtripSP3Stream* m_sp3Stream;             ///< Ntrip SP3 stream
    RealTimeEphStore* m_ephStore;            ///< Ephemeris store
    std::string m_sCorrPath;                 ///< Path to save the correction data

    //CommonTime m_lastClkCorrTime;            ///< Time of last clock correction
    bool m_bRunning;                         ///< If the ppp process is running
    double m_dCorrWaitTime;                  ///< Time of correction stream waiting (sec)
    string m_sCorrMount;                     ///< Name of the correction mountpoint
    bool m_bRealTime;                        ///< Flag of real-time mode

    PPPMain* m_pppMain;                      ///< PPP main controller
    string m_sPPPConfile;                    ///< PPP configuration file

    // For observation data
    StaObsMap m_staObsMap;                   ///< Station observation data list at specified epoch
    EpochObsMap m_epoObsMap;                 ///< Observation data list at specified epoch
    ofstream *m_obsOutStream;                ///< Stream of outputing observations
    bool m_bWriteAllSta;                     ///< If write all stations' observation data to a file
    CommonTime m_lastObsDumpTime;            ///< Time of last dump observation
    double m_dOutWait;                       ///< Time of waiting next observation's comming
};

#define SIG_CENTER (SignalCenter::instance())

#endif // SIGNALCENTER_HPP
