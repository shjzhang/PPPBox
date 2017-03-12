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
    void setLastClkCorrTime(CommonTime& time)
    {   unique_lock<mutex> lock(m_mutex);
        m_lastClkCorrTime = time;
    }

    /// Set the file name of EOP files list
    void setEOPFileListName(string& eopListFile) {m_sEopFileListName = eopListFile;}

    /// Set the correction mountpoint
    void setCorrMount(string mntpnt)
    {  m_sCorrMount = mntpnt; }


    /// Set station obs map
    void newObs(StaObsMap& staObsMap)
    {
        unique_lock<mutex> lock(m_mutex);
        m_staObsMap = staObsMap;
    }

    mutex m_mutex;
private:

    /// Wait for the corrections data
    bool waitForCorr(const CommonTime& epoTime) const;

    /// Method to print solution values
    void printSolution( ofstream& outfile,
                        const  SolverLMS& solver,
                        const  CommonTime& time,
                        const  ComputeDOP& cDOP,
                        bool   useNEU,
                        int    numSats,
                        double dryTropo,
                        int    precision = 3 );


    /// Method to print model values
    void printModel( ofstream& modelfile,
                     const gnssRinex& gData,
                     int   precision = 4 );

    /// Method that hold code to be run BEFORE processing
    void spinUp();

    /// Method that will really process information
    void process();

    StaObsMap m_staObsMap;            ///< observation data of all stations at the same epoch

    CommonTime m_lastClkCorrTime;     ///< Time of last clock correction
    string m_sCorrMount;              ///< Name of the correction mountpoint
    double m_dCorrWaitTime;             ///< Time of correction stream waiting (sec)

    bool m_bRealTime;                 ///< Real-time process flag
    ConfDataReader m_confReader;      ///< Configuration file reader
    string m_sPPPConfFile;            ///< Configuration file name
    string m_sEopFileListName;        ///< File Name of EOP data files list
};


#endif // PPPTASK_HPP
