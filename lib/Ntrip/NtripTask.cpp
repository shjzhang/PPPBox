#include <mutex>
#include <fstream>
#include "NtripTask.h"
#include "NetUrl.hpp"
#include "NetQueryBase.hpp"
#include "NetQueryNtrip1.hpp"
#include "RTCM3Decoder.hpp"
#include "StringUtils.hpp"


//using namespace gpstk;

NtripTask::NtripTask()
{
    m_bOutputRaw = false;
    m_sRawOutFile = "";
    m_sStreamFormat = "RTCM_3";
    m_decoder = 0;
    m_sRnxPath = "./";
    m_dRnxVer = 3.01;
    //m_prnLastEpo.clear();
}

NtripTask::~NtripTask()
{
    delete m_decoder;
}

RTCMDecoder* NtripTask::decoder()
{
    if (m_decoder != 0) {
      return m_decoder;
    }

    else {
      if (initDecoder() == true)
      {
          return m_decoder;
      }
    }
    return 0;
}

bool NtripTask::initDecoder()
{
    m_decoder = 0;
    string staID = m_MP.getMountPointID();

    NetUrl mntpntUrl = m_MP.getMountPointUrl();
    string lat  = StringUtils::asString(m_MP.getLatitude());
    string lon  = StringUtils::asString(m_MP.getLongitude());
    string ntripVer = m_MP.getNtripVersion();
    string nmea = "no";

    if(m_sStreamFormat.find("RTCM_3") != string::npos)
    {
        m_decoder = new RTCM3Decoder();
    }
    m_decoder->initRinex(staID, mntpntUrl, lat, lon,
                         nmea, ntripVer);
    return true;
}

void NtripTask::run()
{
    string tempVersion = m_MP.getNtripVersion();
    int tempNmeaFlag = m_MP.getnmeaFlag();
    NetUrl tempURL = m_MP.getMountPointUrl();

    /// According to the ntrip version of mountpoint,
    /// select corresponding model of network request.
    /// By default: Ntrip Version 1.0
    NetQueryBase* query = 0;
    if(tempVersion == "1")
    {
        query = new NetQueryNtrip1();
    }
    else if(tempVersion == "2")
    {
    }
    else if(tempVersion == "2S")
    {
    }

    ofstream out;
    if(m_bOutputRaw)
    {
        out.open(m_sRawOutFile.c_str(),ios::out|ios::binary);
    }
    cout << "pid=" << this_thread::get_id() << endl;
    while(1)
    {
        if(tempNmeaFlag)
        {
            // added gga into string
            string gga = "";
            query->startRequest(tempURL,gga);
        }
        else
        {
            try
            {
                query->startRequest(tempURL,"");
                if(m_bOutputRaw)
                {
                    query->writeRawData(out);
                }

                if (!decoder() || query->getStatus() != NetQueryBase::dataReceiveable)
                {
                  continue;
                }

                // Delete old observations
                // -----------------------
                m_decoder->m_obsList.clear();

                // Decode Data
                // -------------
                unsigned char* data = (unsigned char *)malloc(4096);
                int buffLen = query->waitForReadyRead(data);

                if(buffLen > 0)
                {
                    bool decodeState = m_decoder->decode(data, buffLen);
                    if(!decodeState)
                    {
                        continue;
                    }
                }

                // Loop over all observations (observations output)
                // ------------------------------------------------
                list<t_satObs>::iterator it = m_decoder->m_obsList.begin();
                for(;it!=(m_decoder->m_obsList.end());++it)
                {
                    const t_satObs& obs = *it;
                    string prn = obs._prn.toString();
                    CommonTime obsTime = obs._time;
                    /*
                    {
                        map<string, CommonTime>::iterator mt = m_prnLastEpo.begin();
                        for(;mt!=(m_prnLastEpo.end());++mt)
                        {
                            CommonTime oldTime = m_prnLastEpo[prn];
                            // observation coming more than once
                            if( obsTime <= oldTime )
                            {
                                continue;
                            }
                        }
                        m_prnLastEpo[prn] = obsTime;
                    }*/
                    string format = "RTCM_3";
                    m_decoder->dumpRinexEpoch(obs, format);
                }

            }
            catch(MountPointNotFound& e)
            {
                cout << e.what();
                continue;
            }
        }
    }
    out.close();
}

