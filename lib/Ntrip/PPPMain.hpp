#ifndef PPPMAIN_HPP
#define PPPMAIN_HPP

#include <vector>
#include <list>
#include <deque>
#include <mutex>
#include <string>
#include <iostream>
#include <fstream>

#include "WorkThread.h"
#include "PPPTask.hpp"
#include "CommonTime.hpp"
#include "Rinex3ObsHeader.hpp"

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

    /// Set the time of last clock correction
    void setLastClkCorrTime(CommonTime& time)
    { m_pppTask->setLastClkCorrTime(time); }

    /// Set real-time flag
    void setRealTimeFlag(bool flag)
    {  m_pppTask->setRealTimeFlag(flag);  }

    /// Set the correction mountpoint
    void setCorrMount(const string& mntpnt)
    {  m_pppTask->setCorrMount(mntpnt); }

    /// Set the correction type
    void setCorrType(const string& type)
    { m_pppTask->setCorrType(type); }

    /// Set station list
    void setStationList(const list<string>& staList)
    { m_pppTask->setStationList(staList);}

    /// New obs
    void newObs(StaObsMap& staObsMap)
    {
        m_pppTask->newObs(staObsMap);
    }

	/// Save rinex obs header
	void setObsHeader(const Rinex3ObsHeader& header)
	{
		m_pppTask->setObsHeader(header);
	}

	PPPTask* m_pppTask;                 ///< PPP task
private:

    /// Read the PPP options
    void readOptions();

    /// Wait for the corrections data
    //bool waitForCorr(const CommonTime& epoTime) const;

    mutex m_mutex;                      ///< Mutex
    CommonTime m_lastClkCorrTime;       ///< Time of last clock correction
    WorkThread* m_pppThread;            ///< PPP thread
    string m_sConfFile;                 ///< Configuration file name
    string m_sEopFileListName;          ///< File Name of EOP data files list

    //NtripNavStream* m_eph;              ///< Ephmeris user
    bool m_bRunning;                    ///< If the ppp process is running
    double m_dCorrWaitTime;             ///< Time of correction stream waiting (sec)
    string m_sCorrMount;                ///< Name of the correction mountpoint
    bool m_bRealTime;                   ///< Flag of real-time mode
};

#endif // PPPMAIN_HPP
