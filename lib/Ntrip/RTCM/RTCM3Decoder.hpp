#ifndef GPSTK_RTCM3DECODER_HPP
#define GPSTK_RTCM3DECODER_HPP

#include <list>
#include <string>
#include <map>

#include "RTCMDecoder.hpp"
#include "RTCM3coDecoder.hpp"
#include "Rinex3ObsData.hpp"
#include "CommonTime.hpp"
#include "satObs.hpp"

using namespace std;
using namespace gpstk;
class RTCM3Decoder:public RTCMDecoder
{
public:

    /**
     * Default constructor
     */
    RTCM3Decoder();

    /**
     * Common constructor
     */
    RTCM3Decoder(const string& staid);

    /**
     * Destructor
     */
    virtual ~RTCM3Decoder();

    /**
     * Decode the RTCM raw data
     *
     * @param  buff    byte data
     * @param  len     bit length
     * @return the decode state(true:success, false:failure)
     */
    virtual bool decode(unsigned char* buff, int len);

private:

 /**
  * Extract a RTCM3 message. Data is passed in the follow fields:<br>
  * {@link _Message}: contains the message bytes<br>
  * {@link _MessageSize}: contains to current amount of bytes in the buffer<br>
  * {@link _SkipBytes}: amount of bytes to skip at the beginning of the buffer
  *
  * The functions sets following variables:<br>
  * {@link _NeedBytes}: Minimum number of bytes needed on next call<br>
  * {@link _SkipBytes}: internal, Bytes to skip before next call (usually the amount of
  *   found bytes)<br>
  * {@link _MessageSize}: Updated size after processed bytes have been removed from buffer
  * @return message number when message found, 0 otherwise
  */
 int getMessage(void);


 /**
  * compute crc-24q parity for sbas, rtcm3
  * @param  buff    byte data
  * @param  len     data length (bytess)
  * @return return  crc-24Q parity
  */
 unsigned int CRC24(const unsigned char *buff, int len);


 /**
  * Extract data from old 1001-1004 RTCM3 messages.
  * @param buffer the buffer containing an 1001-1004 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block is finished and transfered into
  * {@link RTCMDecoder::_obsList} variable
  * @see DecodeRTCM3GLONASS()
  * @see DecodeRTCM3MSM()
  */
 bool decodeRTCM3GPS(unsigned char* buffer, int bufLen);


 /**
  * Extract data from old 1009-1012 RTCM3 messages.
  * @param buffer the buffer containing an 1009-1012 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block is finished and transfered into
  * {@link RTCMDecoder::_obsList} variable
  * @see DecodeRTCM3GPS()
  * @see DecodeRTCM3MSM()
  */
 bool decodeRTCM3GLONASS(unsigned char* buffer, int bufLen);


 /**
  * Extract data from MSM 1070-1229 RTCM3 messages.
  * @param buffer the buffer containing an 1070-1229 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block is finished and transfered into
  * {@link RTCMDecoder::_obsList} variable
  * @see DecodeRTCM3GPS()
  * @see DecodeRTCM3GLONASS()
  */
 bool decodeRTCM3MSM(unsigned char* buffer, int bufLen);


 /**
  * Extract ephemeris data from 1019 RTCM3 messages.
  * @param buffer the buffer containing an 1019 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeGPSEphemeris(unsigned char* buffer, int bufLen);


 /**
  * Extract ephemeris data from 1020 RTCM3 messages.
  * @param buffer the buffer containing an 1020 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeGLONASSEphemeris(unsigned char* buffer, int bufLen);


 /**
  * Extract ephemeris data from 1043 RTCM3 messages.
  * @param buffer the buffer containing an 1043 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeSBASEphemeris(unsigned char* buffer, int bufLen);


 /**
  * Extract ephemeris data from 1044 RTCM3 messages.
  * @param buffer the buffer containing an 1044 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeQZSSEphemeris(unsigned char* buffer, int bufLen);


 /**
  * Extract ephemeris data from 1045 and 1046 RTCM3 messages.
  * @param buffer the buffer containing an 1045 and 1046 RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeGalileoEphemeris(unsigned char* buffer, int bufLen);


 /**
  * Extract ephemeris data from BDS RTCM3 messages.
  * @param buffer the buffer containing an BDS RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeBDSEphemeris(unsigned char* buffer, int bufLen);


 /**
  * Extract antenna type from 1007, 1008 or 1033 RTCM3 messages.
  * @param buffer the buffer containing an antenna RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeAntenna(unsigned char* buffer, int bufLen);


 /**
  * Extract antenna type from 1005 or 1006 RTCM3 messages.
  * @param buffer the buffer containing an antenna RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeAntennaPosition(unsigned char* buffer, int bufLen);

 /**
  * Extract receiver and antenna descriptors from 1033 RTCM3 messages.
  * @param buffer the buffer containing an antenna RTCM block
  * @param bufLen the length of the buffer (the message length including header+crc)
  * @return <code>true</code> when data block was decodable
  */
 bool decodeRcvAnt(unsigned char* data, int size);

 /** Current station description, dynamic in case of raw input file handling */
 string             staID;
 /** Raw input file for post processing, required to extract station ID */
 //bncRawFile*            rawFile;

 /** List of decoders for Clock and Orbit data */
 //RTCM3coDecoder _coDecoder;
 map<string, RTCM3coDecoder*> coDecoders;
 //map<QByteArray, RTCM3coDecoder*> coDecoders;

 /** Message buffer for input parsing */
 unsigned char Message[2048];
 /** Current size of the message buffer */
 size_t MessageSize;
 /** Minimum bytes required to have success during next {@link GetMessage()} call */
 size_t NeedBytes;
 /** Bytes to skip in next {@link GetMessage()} call, intrnal to that function */
 size_t SkipBytes;
 /** Size of the current RTCM3 block beginning at buffer start after a successful
  *  {@link GetMessage()} call
  */
 size_t BlockSize;

 /**
  * Current observation epoch. Used to link together blocks in one epoch.
  */
 CommonTime currentTime;
 /** Current observation data block list, Filled by {@link DecodeRTCM3GPS()},
  * {@link DecodeRTCM3GLONASS()} and {@link DecodeRTCM3MSM()} functions.
  */
 list<t_satObs> currentObsList;
};

#endif // GPSTK_RTCM3DECODER_HPP
