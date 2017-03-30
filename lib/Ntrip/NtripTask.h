#ifndef NTRIP_TASK_H_H_
#define NTRIP_TASK_H_H_

#include <mutex>
#include <map>

#include "CommonTime.hpp"
#include "TaskBase.h"
#include "MountPoint.hpp"
#include "RTCMDecoder.hpp"

using namespace gpstk;
using namespace std;



class NtripTask : public TaskBase{

public:

     /// Constructor
	 NtripTask();

     /// Destructor
	~NtripTask();

public:

	/// Get class name
	string getClassName()
	{return "NtripTask";}

    /// run the task
    virtual bool run();

    /// Set the mountpoint
	void setMountPoint(MountPoint pt ){m_MP = pt;}

    /// Set the choice if output the raw data
    void setRawOutOpt(bool output)
    { m_bOutputRaw = output;}

    /// Set the path to save raw data
    void setRawOutFile(string& file)
    { m_sRawOutFile = file;}

    /// Set the choice if write the observation file
    void setWriteObsFile(bool choice)
    { m_bWriteObsFile = choice;}

    /// Set the path to save the observation
    void setObsFilePath(string& path)
    { m_sRinexPath = path; }

    void setRinexVersion(double version)
    { m_dRnxVer = version; }

    /// Instantiate the decoder
    bool initDecoder();

    /// Current decoder in use
    RTCMDecoder* decoder();

private:

    mutex m_mutex;          ///< unused now
    MountPoint m_MP;        ///< // MountPoint
    bool m_bOutputRaw;      ///< The choice that if output the raw file
    bool m_bWriteObsFile;   ///< The choice that if write observation RINEX file
    string m_sRawOutFile;   ///< The path to save the raw file
    RTCMDecoder* m_decoder; ///< RTCM decoder
    string m_sStreamFormat; ///< Stream format
    string m_sRinexPath;    ///< RINEX file savepath
    double m_dRnxVer;       ///< RINEX file version
    map<string, CommonTime> m_prnLastEpoch; ///< Observation prn and its epoch map
};

#endif
