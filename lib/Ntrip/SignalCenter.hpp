#ifndef SIGNALCENTER_HPP
#define SIGNALCENTER_HPP

#include <list>
#include <string>
#include <map>
#include <mutex>
#include <condition_variable>
#include <fstream>

#include "GPSEphemeris2.hpp"
#include "NtripNavStream.hpp"
#include "NtripSP3Stream.hpp"
#include "NtripObsStream.hpp"
#include "RealTimeEphStore.hpp"
#include "PPPMain.hpp"

using namespace std;
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

    /// New observation data comes
    void newObs(const string& staID, list<t_satObs> obsList);

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

    /// Start PPP(precise point positioning) process
    void startPPP();

    /// Stop PPP process
    void stopPPP();

public:
    mutex m_obsMutex;                        ///< Mutex for observation data
    mutex m_gpsEphMutex;                     ///< Mutex for gps ephmeris data
    mutex m_orbCorrMutex;                    ///< Mutex for orbit correction data
    mutex m_clkCorrMutex;                    ///< Mutex for clock correction data

    condition_variable m_condObsDecoded;     ///< condition variable for obs data
    condition_variable m_condGPSEphDecoded;  ///< condition variable for gps eph data
    condition_variable m_condOrbCorrDecoded; ///< condition variable for orb corr data
    condition_variable m_condClkCorrDecoded; ///< condition variable for clk corr data

    NtripObsStream* m_obsStream;             ///< Ntrip observation stream
    NtripNavStream* m_navStream;             ///< Ntrip ephmeris stream
    NtripSP3Stream* m_sp3Stream;             ///< Ntrip SP3 stream
    RealTimeEphStore* m_ephStore;            ///< Ephemeris store
    std::string m_sCorrPath;                 ///< Path to save the correction data

    CommonTime m_lastClkCorrTime;            ///< Time of last clock correction
    bool m_bRunning;                         ///< If the ppp process is running
    double m_dCorrWaitTime;                  ///< Time of correction stream waiting (sec)
    string m_sCorrMount;                     ///< Name of the correction mountpoint
    bool m_bRealTime;                        ///< Flag of real-time mode

    PPPMain* m_pppMain;                      ///< PPP main controller

    // Epoch observation map type
    typedef map<CommonTime,list<t_satObs> > EpochObsMap;

    // For observation data
    map<string, EpochObsMap> m_staEpochObsMap;   ///< Station observation data list at specified epoch
    map<CommonTime,list<t_satObs> > m_epoObsMap; ///< Observation data list at specified epoch
    ofstream *m_obsOutStream;                    ///< Stream of outputing observations
    bool m_bWriteAllSta;                         ///< If write all stations' observation data to a file
    CommonTime m_lastObsDumpTime;                ///< Time of last dump observation
    double m_dOutWait;                           ///< Time of waiting next observation's comming
    typedef map<string, CommonTime> PrnEpoch;    ///< Satellite prn and its epoch type
    map<string, PrnEpoch> m_staPrnLastEpo;       ///< Station satellite prn epoch
};

#define SIG_CENTER (SignalCenter::instance())

#endif // SIGNALCENTER_HPP
