#pragma ident "$Id 2016-11-24 $"
/**
 * @file RTCM3Decoder.cpp
 * Class to decode the RTCM_3 format raw data
 */

#include <iostream>
#include <iomanip>
#include <sstream>
#include <math.h>
#include <string.h>

#include "SignalCenter.hpp"
#include "bits.h"
#include "RTCM3Decoder.hpp"
#include "SatID.hpp"
#include "GNSSconstants.hpp"

// #include <stdint.h> //uint64_t,UINT_LEAST64_MAX的定义场所。

// RTCM station information
RTCMDecoder::t_staInfo m_sta;

RTCM3Decoder::RTCM3Decoder()
{
    MessageSize = SkipBytes = BlockSize = NeedBytes = 0;
    //_coDecoder = 0;
}

RTCM3Decoder::RTCM3Decoder(const string& staid)
{
    MessageSize = SkipBytes = BlockSize = NeedBytes = 0;
    staID = staid;
}

RTCM3Decoder::~RTCM3Decoder()
{
    //delete _coDecoder;
    while(coDecoders.size()!=0)
    {
        delete (coDecoders.begin())->second;
        coDecoders.erase(coDecoders.begin());
    }
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decode(unsigned char* buffer, int bufLen)
{
  bool decoded = false;

  while(bufLen && MessageSize < sizeof(Message))
  {
    int l = sizeof(Message) - MessageSize;
    if(l > bufLen)
      l = bufLen;
    memcpy(Message+MessageSize, buffer, l);
    MessageSize += l;
    bufLen -= l;
    buffer += l;
    int id;
    while((id = getMessage()))
    {
      /* reset station ID for file loading as it can change */
      //if(_rawFile)
       // _staID = _rawFile->staID();
      /* store the id into the list of loaded blocks */
      m_typeList.push_back(id);

      /* SSR I+II data handled in another function, already pass the
       * extracted data block. That does no harm, as it anyway skip everything
       * else. */
      if((id >= 1057 && id <= 1068) || (id >= 1240 && id <= 1270))
      {
        if(coDecoders.count(staID)==0)
        {
          coDecoders[staID] = new RTCM3coDecoder(staID);
        }
        RTCM3coDecoder* decoder = coDecoders[staID];
        if(decoder->decode(Message, BlockSize))
        {
          decoded = true;
        }
      }
      else if(id >= 1070 && id <= 1229) /* MSM */
      {
        //if(decodeRTCM3MSM(_Message, _BlockSize))
        // decoded = true;
      }
      else
      {
        switch(id)
        {
        case 1001: case 1003:
          //emit(newMessage(QString("%1: Block %2 contain partial data! Ignored!")
          //.arg(staID).arg(id).toAscii(), true));
          break; /* no use decoding partial data ATM, remove break when data can be used */
        case 1002: case 1004:
          if(decodeRTCM3GPS(Message, BlockSize))
            decoded = true;
          break;
        case 1009: case 1011:
          //emit(newMessage(QString("%1: Block %2 contain partial data! Ignored!")
          //.arg(_staID).arg(id).toAscii(), true));
          break; /* no use decoding partial data ATM, remove break when data can be used */
        case 1010: case 1012:
          if(decodeRTCM3GLONASS(Message, BlockSize))
            decoded = true;
          break;
        case 1019:
          if(decodeGPSEphemeris(Message, BlockSize))
            decoded = true;
          break;
        case 1020:
          if(decodeGLONASSEphemeris(Message, BlockSize))
            decoded = true;
          break;
        case 1043:
          if(decodeSBASEphemeris(Message, BlockSize))
            decoded = true;
          break;
        case 1044:
          if(decodeQZSSEphemeris(Message, BlockSize))
            decoded = true;
          break;
        case 1045: case 1046:
          if(decodeGalileoEphemeris(Message, BlockSize))
            decoded = true;
          break;
        case 63:
          //if(decodeBDSEphemeris(Message, BlockSize))
          //  decoded = true;
          break;
        case 1007: case 1008:
          decodeAntenna(Message, BlockSize);
          break;
        case 1033:
          decodeRcvAnt(Message, BlockSize);
          break;
        case 1005: case 1006:
          decodeAntennaPosition(Message, BlockSize);
          break;
        }
      }
    }
  }
  return decoded ;
}

/**
 * compute crc-24q parity for sbas, rtcm3
 * @param  buff    byte data
 * @param  len     data length (bytess)
 * @return return  crc-24Q parity
 */
unsigned int RTCM3Decoder::CRC24(const unsigned char *buff, int len)
{
    unsigned int crc=0;
    int i;

    while(len--)
    {
      crc ^= (*buff++) << (16);
      for(i = 0; i < 8; i++)
      {
        crc <<= 1;
        if(crc & 0x1000000)
          crc ^= 0x01864cfb;
      }
    }
    return crc;
}

//
////////////////////////////////////////////////////////////////////////////
int RTCM3Decoder::getMessage(void)
{
  unsigned char *m, *e;
  int i;

  m = Message+SkipBytes;
  e = Message+MessageSize;
  NeedBytes = SkipBytes = 0;
  while(e-m >= 3)
  {
    if(m[0] == RTCM3PREAMB)
    {
      BlockSize = ((m[1]&3)<<8)|m[2];
      if(e-m >= static_cast<int>(BlockSize+6))
      {
        if(static_cast<unsigned int>((m[3+BlockSize]<<16)|(m[3+BlockSize+1]<<8)
        |(m[3+BlockSize+2])) == CRC24(m, BlockSize+3))
        {
          BlockSize +=6;
          SkipBytes = BlockSize;
          break;
        }
        else
          ++m;
      }
      else
      {
        NeedBytes = BlockSize;
        break;
      }
    }
    else
      ++m;
  }
  if(e-m < 3)
    NeedBytes = 3;

  /* copy buffer to front */
  i = m - Message;
  if(i && m < e)
    memmove(Message, m, static_cast<size_t>(MessageSize-i));
  MessageSize -= i;

  return !NeedBytes ? ((Message[3]<<4)|(Message[4]>>4)) : 0;
}


// Decode GPS Observation RTCM Message
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeRTCM3GPS(unsigned char* data, int size)
{
  bool decoded = false;
  CommonTime currObsTime;
  int i, numsats, syncf, type;
  uint64_t numbits = 0, bitfield = 0;

  data += 3; /* header */
  size -= 6; /* header + crc */

  GETBITS(type, 12)
  SKIPBITS(12) /* id */
  GETBITS(i,30)

  currObsTime = setGPS(i);
  if(currObsTime != currentTime && (currObsTime.getDays() != 0.0
                        || currObsTime.getSecondOfDay() != 0.0) )
  {
    decoded = true;
    for(auto& obs:currentObsList)
    {
      m_obsList.push_back(obs);
    }
    currentObsList.clear();
  }

  currentTime = currObsTime;

  GETBITS(syncf,1) /* sync */
  GETBITS(numsats,5)
  SKIPBITS(4) /* smind, smint */

  while(numsats--)
  {
    int sv, code, l1range, amb=0;
    t_satObs currObs;
    currObs._time = currObsTime;

    RinexSatID sat;

    GETBITS(sv, 6);
    if(sv < 40 && sv != 0)
    {
      // gps
      sat =  RinexSatID(sv, SatID::systemGPS);
      currObs._prn = sat;
    }
    else
    {
      // sbas
      sat = RinexSatID(sv-20, SatID::systemGeosync);
      currObs._prn = sat;
    }

    t_frqObs *frqObs = new t_frqObs;
    GETBITS(code, 1);
    (code) ? frqObs->_rnxType2ch.assign("1W") : frqObs->_rnxType2ch.assign("1C");
    GETBITS(l1range, 24);
    GETBITSSIGN(i, 20);
    if((i&((1<<20)-1)) != 0x80000)
    {
        frqObs->_code = l1range*0.02;
        frqObs->_phase = (l1range*0.02+i*0.0005)/L1_WAVELENGTH_GPS;
        frqObs->_codeValid = frqObs->_phaseValid = true;
    }
    GETBITS(i, 7);
    frqObs->_slipCounter = i;
    if(type == 1002 || type == 1004)
    {
      GETBITS(amb,8);
      if(amb)
      {
        frqObs->_code += amb*299792.458;
        frqObs->_phase += (amb*299792.458)/L1_WAVELENGTH_GPS;
      }
      GETBITS(i, 8);
      if(i)
      {
        frqObs->_snr = i*0.25;
        frqObs->_snrValid = true;
      }
    }
    currObs._obs.push_back(frqObs);
    if(type == 1003 || type == 1004)
    {
      frqObs = new t_frqObs;
      /* L2 */
      GETBITS(code,2);
      switch(code)
      {
      case 3: frqObs->_rnxType2ch.assign("2W"); /* or "2Y"? */ break;
      case 2: frqObs->_rnxType2ch.assign("2W"); break;
      case 1: frqObs->_rnxType2ch.assign("2P"); break;
      case 0: frqObs->_rnxType2ch.assign("2X"); /* or "2S" or "2L"? */ break;
      }
      GETBITSSIGN(i,14);
      if((i&((1<<14)-1)) != 0x2000)
      {
        frqObs->_code = l1range*0.02+i*0.02+amb*299792.458;
        frqObs->_codeValid = true;
      }
      GETBITSSIGN(i,20);
      if((i&((1<<20)-1)) != 0x80000)
      {
        frqObs->_phase = (l1range*0.02+i*0.0005+amb*299792.458)/L2_WAVELENGTH_GPS;
        frqObs->_phaseValid = true;
      }
      GETBITS(i,7);
      frqObs->_slipCounter = i;
      if(type == 1004)
      {
        GETBITS(i, 8);
        if(i)
        {
          frqObs->_snr = i*0.25;
          frqObs->_snrValid = true;
        }
      }
      currObs._obs.push_back(frqObs);
    }
    // Only put the GPS obs but not sbas
    if(currObs._prn.system==SatID::systemGPS)
    {
      currentObsList.push_back(currObs);
    }
  }

  if(!syncf)
  {
    decoded = true;
    for(auto& obs:currentObsList)
    {
      m_obsList.push_back(obs);
    }
    //CurrentTime.reset();
    currentObsList.clear();
  }
  return decoded;
}




//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeRTCM3MSM(unsigned char* data, int size)
{
  bool decoded = false;
  int type, syncf, i;

  return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeRTCM3GLONASS(unsigned char* data, int size)
{
  bool decoded = false;

  return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeGPSEphemeris(unsigned char* data, int size)
{
    bool decoded = false;

    if(size == 67)
    {
      GPSEphemeris2 eph;
      int i, week, toeWeek;
      double sqrtA;
      uint64_t numbits = 0, bitfield = 0;

      data += 3; /* header */
      size -= 6; /* header + crc */
      SKIPBITS(12);

      //eph.transmitTime = ;

      // prn
      GETBITS(i, 6);
      eph.satID = SatID(i, SatID::systemGPS);
      // week
      GETBITS(week, 10);
      week += 1024;
      // SV accuracy (URA index)
      GETBITS(i, 4);
      eph.accuracyFlag = accuracyFromIndex(i, eph.satID.system);
      GETBITS(eph.codeflags, 2);
      GETFLOATSIGN(eph.idot, 14, PI*P2_43);
      GETBITS(eph.IODE, 8);
      GETBITS(i, 16);
      i <<= 4;
      eph.ctToc = setGPS(i*1000);
      eph.transmitTime = eph.ctToc;
      GETFLOATSIGN(eph.af2, 8, P2_55)
      GETFLOATSIGN(eph.af1, 16, P2_43)
      GETFLOATSIGN(eph.af0, 22, P2_31)
      GETBITS(eph.IODC, 10)
      GETFLOATSIGN(eph.Crs, 16, P2_5)
      GETFLOATSIGN(eph.dn, 16, PI*P2_43)
      GETFLOATSIGN(eph.M0, 32, PI*P2_31)
      GETFLOATSIGN(eph.Cuc, 16, P2_29)
      GETFLOAT(eph.ecc, 32, P2_33)
      GETFLOATSIGN(eph.Cus, 16, P2_29)
      GETFLOAT(sqrtA, 32, P2_19);
      eph.A = sqrtA*sqrtA;

      // toe second of week
      GETBITS(i, 16)
      i <<= 4;
      eph.ctToe = setGPS(i*1000);
      GPSWeekSecond weeksec;
      weeksec.convertFromCommonTime(eph.ctToe);
      /* week from HOW, differs from TOC, TOE week, we use adapted value instead */
      toeWeek = weeksec.week;
      if(toeWeek > week + 1 || toeWeek < week-1)
        return false;
      GETFLOATSIGN(eph.Cic, 16, P2_29);
      GETFLOATSIGN(eph.OMEGA0, 32, PI*P2_31);
      GETFLOATSIGN(eph.Cis, 16, P2_29);
      GETFLOATSIGN(eph.i0, 32, PI*P2_31);
      GETFLOATSIGN(eph.Crc, 16, P2_5);
      GETFLOATSIGN(eph.w, 32, PI*P2_31);
      GETFLOATSIGN(eph.OMEGAdot, 24, PI*P2_43);
      GETFLOATSIGN(eph.Tgd, 8, P2_31);
      GETBITS(eph.health, 6);
      GETBITS(eph.L2Pdata, 1);
      GETBITS(eph.fitint, 1);

      decoded = true;
      //std::lock_guard<std::mutex> guard(SIG_CENTER->m_gpsEphMutex);
      SIG_CENTER->newGPSEph(eph);
    } 
    return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeGLONASSEphemeris(unsigned char* data, int size)
{
  bool decoded = false;

  return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeQZSSEphemeris(unsigned char* data, int size)
{
  bool decoded = false;

  return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeSBASEphemeris(unsigned char* data, int size)
{
  bool decoded = false;

  return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeGalileoEphemeris(unsigned char* data, int size)
{
  bool decoded = false;

  return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeBDSEphemeris(unsigned char* data, int size)
{
  bool decoded = false;


  return decoded;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeAntenna(unsigned char* data, int size)
{
\
    int type;
    char *antenna, *serialNum;
    int antnum = -1, snum = -1;
    int staID, setup;
    uint64_t numbits = 0, bitfield = 0;

    data += 3; /* header */
    size -= 6; /* header + crc */

    // get message type number
    GETBITS(type, 12);

    // get station ID
    GETBITS(staID, 12);
    m_sta.staID = staID;

    // get antenna description
    GETSTRING(antnum, antenna);  // Here has a question, if this function has skipped right bits
    if (antnum > -1 && antnum < MAXANT)
    {
      memcpy(m_sta.antDes, antenna, antnum);
      m_sta.antDes[antnum] = 0;
    }

    // get setup ID
    GETBITS(setup, 8);
    m_sta.antSetup = setup;

    if(type == 1008)
    {
        // get antenna serial number
      GETSTRING(snum, serialNum);
      {
        if(snum > -1 && snum < 32)
        {
          memcpy(m_sta.antNum, serialNum, snum);
          m_sta.antNum[snum] = 0;
        }
      }
    }
    return true;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeAntennaPosition(unsigned char* data, int size)
{

    int type;
    int staID, itrf;
    uint64_t numbits = 0, bitfield = 0;
    double x, y, z;

    data += 3; /* header */
    size -= 6; /* header + crc */

    // get message type number
    GETBITS(type, 12);

    // get station ID
    GETBITS(staID,12);
    m_sta.staID = staID;

    // get the year for ITRF realization
    GETBITS(itrf, 6);
    m_sta.itrf = itrf;

    SKIPBITS(4);

    // get the antenna position
    GETBITSSIGN(x, 38);
    SKIPBITS(2);
    GETBITSSIGN(y, 38);
    SKIPBITS(2);
    GETBITSSIGN(z, 38);


    m_sta.pos[0] = x;
    m_sta.pos[1] = y;
    m_sta.pos[2] = z;
    if(type == 1006)
    {
      double h;
      GETBITS(h, 16);
      m_sta.height = h;
    }

    return true;
}

// type = 1033
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeRcvAnt(unsigned char* data, int size)
{
    char *antenna, *antSerialNum, *receiver, *rcvFirmware, *rcvSerialNum;
    int strNum = -1, setup,staID;
    uint64_t numbits = 0, bitfield = 0;

    data += 3; /* header */
    size -= 6; /* header + crc */
    SKIPBITS(12);

    // get station ID
    GETBITS(staID, 12);
    m_sta.staID = staID;

    // get antenna descriptor
    GETSTRING(strNum,antenna);
    if(strNum > -1 && strNum < MAXANT)
    {
      memcpy(m_sta.antDes, antenna, strNum);
      m_sta.antDes[strNum] = 0;
    }

    // get setup ID
    GETBITS(setup, 8);
    m_sta.antSetup = setup;

    // get antenna serial number
    strNum = -1;
    GETSTRING(strNum, antSerialNum);
    if(strNum > -1 && strNum < MAXANT)
    {
      memcpy(m_sta.antNum, antSerialNum, strNum);
      m_sta.antNum[strNum] = 0;
    }

    // get receiver descriptor
    strNum = -1;
    GETSTRING(strNum, receiver);
    if(strNum > -1 && strNum < MAXANT)
    {
      memcpy(m_sta.rcvDes, receiver, strNum);
      m_sta.rcvDes[strNum] = 0;
    }

    // get receiver firmware version
    strNum = -1;
    GETSTRING(strNum, rcvFirmware);
    if(strNum > -1 && strNum < MAXANT)
    {
      memcpy(m_sta.rcvVer, rcvFirmware, strNum);
      m_sta.rcvVer[strNum] = 0;
    }

    // get receiver serial number
    strNum = -1;
    GETSTRING(strNum, rcvSerialNum);
    if(strNum > -1 && strNum < MAXANT)
    {
      memcpy(m_sta.rcvNum, rcvSerialNum, strNum);
      m_sta.antNum[strNum] = 0;
    }
}

