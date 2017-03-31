#ifndef PPPTASK_HPP
#define PPPTASK_HPP


#include <vector>
#include <list>
#include <deque>
#include <mutex>
#include <string>
#include <iostream>
#include <fstream>

#include "TaskBase.h"
#include "SP3EphemerisStore.hpp"
#include "CommonTime.hpp"
#include "satObs.hpp"
#include "BasicFramework.hpp"
#include "ProcessingList.hpp"
#include "BasicModel.hpp"
#include "RealTimeEphStore.hpp"
#include "TropModel.hpp"
#include "DataStructures.hpp"
#include "RequireObservables.hpp"
#include "SimpleFilter.hpp"
#include "PPPExtendedKalmanFilter.hpp"
#include "XYZ2NEU.hpp"
#include "LICSDetector.hpp"
#include "MWCSDetector.hpp"
#include "SolidTides.hpp"
#include "OceanLoading.hpp"
#include "PoleTides.hpp"
#include "CorrectObservables.hpp"
#include "Antenna.hpp"
#include "AntexReader.hpp"
#include "ComputeWindUp.hpp"
#include "ComputeSatPCenter.hpp"
#include "ComputeTropModel.hpp"
#include "ComputeLinear.hpp"
#include "LinearCombinations.hpp"
#include "ComputeDOP.hpp"
#include "SatArcMarker.hpp"
#include "SatArcMarker2.hpp"
#include "GravitationalDelay.hpp"
#include "PhaseCodeAlignment.hpp"
#include "EclipsedSatFilter.hpp"
#include "Decimate.hpp"
#include "SolverPPP.hpp"
#include "SolverPPPFB.hpp"
#include "SolverPPPPredict.hpp"
#include "SolverPPPCorrect.hpp"
#include "Bancroft.hpp"    // class to get an initial guess of GPS receiver's position
#include "MWFilter.hpp"
#include "ConfDataReader.hpp"
#include "ComputeElevWeights.hpp"
#include "MSCStore.hpp"
#include "CC2NONCC.hpp"
#include "RecTypeDataReader.hpp"
#include "NtripNavStream.hpp"
#include "StringUtils.hpp"
#include "FileUtils.hpp"
#include "ENUUtil.hpp"
#include "MutexLock.h"

#include "SignalCenter.hpp"
#include "NtripObsStream.hpp"

using namespace std;
using namespace gpstk;

class PPPTask : public TaskBase
{
public:

    /// Constructor
    PPPTask();

    /// Destructor
    ~PPPTask();

    /// Run the task
    virtual bool run();

    /// Set real-time flag
    void setRealTimeFlag(bool flag){m_bRealTime = flag;}

    /// Post-process use saved files
    void processFiles();

    /// Set the configuration file
    void setConfFile(string& confFile) {m_sPPPConfFile = confFile;}

    /// Set the time of last clock correction
    void setLastClkCorrTime(CommonTime& time);

    /// Set the file name of EOP files list
    void setEOPFileListName(string& eopListFile) {m_sEOPFileName = eopListFile;}

    /// Set the correction mountpoint
    void setCorrMount(string mntpnt)
    {  m_sCorrMount = mntpnt; }

    /// Set station obs map
    void newObs(StaObsMap& staObsMap);

    /// Save rinex obs header
    void setObsHeader(const Rinex3ObsHeader& header)
    {
        unique_lock<mutex> lock(m_obsHeaderMutex);
        m_obsHeader = header;
    }

    /// Get file name of output results
    string resolveFileName(string &staID);

private:

    /// Wait for the corrections data
    bool waitForCorr(const CommonTime& epoTime);

    /// Method to print solution values
    void printSolution(  ofstream& outfile,
                         const SolverLMS& solver,
                         const CommonTime& time,
                         const ComputeDOP& cDOP,
                         int   numSats,
                         double dryTropo,
                         Position& pos,
                         const string format,
                         int   precision);


    /// Method to print model values
    void printModel( ofstream& modelfile,
                     const gnssRinex& gData,
                     int   precision = 4 );

    /// Method that hold code to be run BEFORE processing
    void spinUp();

    /// Method that will really process information
    void process();

    mutex m_mutex;                    ///< Global mutex
    mutex m_obsQueueMutex;            ///< Observation queue mutex
    mutex m_obsHeaderMutex;           ///< Rinex observation header mutex 

    Rinex3ObsHeader m_obsHeader;      ///< Rinex obs header
    deque<StaObsMap> m_staObsQueue;   ///< observation data queue of all stations during some time
    CommonTime m_lastClkCorrTime;     ///< Time of last clock correction
    string m_sCorrMount;              ///< Name of the correction mountpoint
    double m_dCorrWaitTime;           ///< Time of correction stream waiting (sec)

    bool m_bRealTime;                 ///< Real-time process flag
    ConfDataReader m_confReader;      ///< Configuration file reader
    string m_sPPPConfFile;            ///< Configuration file name
    string m_sEOPFileName;            ///< File Name of EOP data files list
    thread m_processThread;             ///< process thread
    //map<string,vector<ProcessingList> > m_processListMap;   /// stations' processlist map
};


#endif // PPPTASK_HPP
