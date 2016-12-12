#pragma ident "$Id 2016-11-24 $"
/**
 * @file RTCMDecoder.cpp
 * Class to decode the RTCM data
 */

#include "RTCMDecoder.hpp"


RTCMDecoder::RTCMDecoder()
{
    m_rnx = 0;
}

RTCMDecoder::~RTCMDecoder()
{
    delete m_rnx;
}

void RTCMDecoder::dumpRinexEpoch(const t_satObs& obs, const string &format)
{
    if(m_rnx)
    {
        m_rnx->addObs(obs);
    }
    m_rnx->dumpEpoch(format, obs._time);
}

void RTCMDecoder::initRinex(const std::string& staID, const NetUrl& mountPoint,
               const std::string& latitude, const std::string& longitude,
               const std::string& nmea, const std::string& ntripVersion)
{
    if(m_rnx)
    {
        return;
    }
    m_rnx = new NtripObsStream(staID,mountPoint,latitude, longitude,
                               nmea, ntripVersion);
}


