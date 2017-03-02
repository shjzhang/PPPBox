#ifndef PPPMAIN_HPP
#define PPPMAIN_HPP

#include <vector>
#include <list>
#include <deque>
#include <mutex>
#include <string>
#include <iostream>
#include <fstream>

#include "PPPTask.hpp"
#include "CommonTime.hpp"
#include "satObs.hpp"
#include "BasicFramework.hpp"
#include "ProcessingList.hpp"
#include "BasicModel.hpp"
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
   // class to get an initial guess of GPS receiver's position
#include "Bancroft.hpp"
#include "MWFilter.hpp"
#include "ConfDataReader.hpp"
#include "ComputeElevWeights.hpp"
#include "MSCStore.hpp"
#include "CC2NONCC.hpp"
#include "RecTypeDataReader.hpp"
#include "NtripNavStream.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


class PPPMain
{
public:

    /// Constructor
    PPPMain();

    /// Destructor
    ~PPPMain();

    /// Start the PPP process
    void start();

    /// Stop the PPP process
    void stop();

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

    /// Set the configuration file
    void setConfFile(string& confFile) {m_sConfFile = confFile;}

    void setEOPFileListName(string& eopListFile) {m_sEopFileListName = eopListFile;}

    /// Method that hold code to be run BEFORE processing
    void spinUp();

    /// Method that will really process information
    void process();

    void newOrbCorr(list<t_orbCorr> orbCorr);

    void newClkCorr(list<t_clkCorr> clkCorr);

    void newObs(string& staID, list<t_satObs> obsList);

private:

    /// Epoch and satellite observation data classs
    class EpochData
    {
    public:
        EpochData() {;}
        ~EpochData()
        {
            for (unsigned i = 0; i < satObs.size(); i++)
            {
              delete satObs[i];
            }
        }

        CommonTime time;                ///< Epoch
        vector<t_satObs*> satObs;       ///< satellite observation
    };

    /// Read the options
    void readOptions();

    /// Wait for the corrections data
    bool waitForCorr(const CommonTime& epoTime) const;

    mutex m_mutex;                      ///< Mutex
    deque<EpochData*> m_epoData;        ///< Epoch observation data
    CommonTime m_lastClkCorrTime;       ///< Time of last clock correction
    PPPTask* m_pppTask;                 ///< PPP Task

    ConfDataReader m_confReader;        ///< Configuration file reader
    string m_sConfFile;                 ///< Configuration file name
    string m_sEopFileListName;          ///< File Name of EOP data files list

    NtripNavStream* m_eph;              ///< Ephmeris user
    bool m_bRunning;                    ///< If the ppp process is running
    double m_dCorrWaitTime;             ///< Time of correction stream waiting (sec)
    string m_sCorrMount;                ///< Name of the correction mountpoint
    bool m_bRealTime;                   ///< Flag of real-time mode
};

#endif // PPPMAIN_HPP
