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

#include "bits.h"
#include "RTCM3Decoder.hpp"
#include "SatID.hpp"
#include "GNSSconstants.hpp"

// #include <stdint.h> //uint64_t,UINT_LEAST64_MAX的定义场所。


RTCM3Decoder::RTCM3Decoder()
{
    MessageSize = SkipBytes = BlockSize = NeedBytes = 0;
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
        /*if (!_coDecoders.contains(_staID.toAscii()))
          _coDecoders[_staID.toAscii()] = new RTCM3coDecoder(_staID);
        RTCM3coDecoder* coDecoder = _coDecoders[_staID.toAscii()];
        if(coDecoder->Decode(reinterpret_cast<char *>(_Message), _BlockSize,
        errmsg) == success)
        {
          decoded = true;
        }*/
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
        case 1007: case 1008: case 1033:
          decodeAntenna(Message, BlockSize);
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
  if(currObsTime != currentTime)
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
    CivilTime cv = CivilTime(currObsTime);

    RinexSatID sat;

    GETBITS(sv, 6);
    if(sv < 40)
    {
      // gps
      sat =  RinexSatID(sv, SatID::systemGPS);
    }
    else
    {
      // sbas
      sat = RinexSatID(sv-20, SatID::systemGeosync);
    }
    currObs._prn = sat;

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
    currentObsList.push_back(currObs);
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

  return true;
}

//
////////////////////////////////////////////////////////////////////////////
bool RTCM3Decoder::decodeAntennaPosition(unsigned char* data, int size)
{
  /*int type;
  uint64_t numbits = 0, bitfield = 0;
  double x, y, z;

  data += 3; /* header */
  //size -= 6; /* header + crc */

  /*GETBITS(type, 12)
  _antList.push_back(t_antInfo());
  _antList.back().type = t_antInfo::ARP;
  SKIPBITS(22)
  GETBITSSIGN(x, 38)
  _antList.back().xx = x * 1e-4;
  SKIPBITS(2)
  GETBITSSIGN(y, 38)
  _antList.back().yy = y * 1e-4;
  SKIPBITS(2)
  GETBITSSIGN(z, 38)
  _antList.back().zz = z * 1e-4;
  if(type == 1006)
  {
    double h;
    GETBITS(h, 16)
    _antList.back().height = h * 1e-4;
    _antList.back().height_f = true;
  }
  _antList.back().message  = type;*/

  return true;
}

