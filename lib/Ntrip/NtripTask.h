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

	 NtripTask();
	~NtripTask();

public:

	virtual void run();
	void setMountPoint(MountPoint pt ){m_MP = pt;}
    void setRawOutOpt(bool output) {m_bOutputRaw = output;}
    void setRawOutFile(string& file){m_sRawOutFile = file;}

    // Instantiate the decoder
    bool initDecoder();

    // Current decoder in use
    RTCMDecoder* decoder();

private:

       // unused now
    mutex m_mutex;

       // MountPoint
    MountPoint m_MP;

       // The choice that if output the raw file
    bool m_bOutputRaw;

       // The path to save the raw file
    string m_sRawOutFile;

       // RTCM decoder
    RTCMDecoder* m_decoder;

       // Stream format
    string m_sStreamFormat;

      // RINEX file savepath
    string m_sRnxPath;

      // RINEX file version
    double m_dRnxVer;
      // Last epoch time record
    /// ????????????????????????
    //map<string,int> m_prnLastEpo;
};

#endif
