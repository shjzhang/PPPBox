#ifndef GPSTK_RTCMDECODER_HPP
#define GPSTK_RTCMDECODER_HPP

#include <list>
#include <math.h>

#include "NetUrl.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"
#include "Rinex3ObsHeader.hpp"
#include "Rinex3ObsData.hpp"
#include "CommonTime.hpp"
#include "SystemTime.hpp"
#include "satObs.hpp"
#include "SatID.hpp"
#include "NtripObsStream.hpp"


using namespace gpstk;


/*  Constants  */
#define STRFMT_RTCM2 0                  /* stream format: RTCM 2 */
#define STRFMT_RTCM3 1                  /* stream format: RTCM 3 */
#define MAXANT      32                  /* max length of station name/antenna type *
#define RTCM2PREAMB 0x66                /* rtcm ver.2 frame preamble */
#define RTCM3PREAMB 0xD3                /* rtcm ver.3 frame preamble */
#define PRUNIT_GPS  299792.458          /* rtcm ver.3 unit of gps pseudorange (m) */
#define PRUNIT_GLO  599584.916          /* rtcm ver.3 unit of glonass pseudorange (m) */
#define RANGE_MS    (CLIGHT*0.001)      /* range in 1 ms */
#define BDSEPHF_SATH1      (1<<0)       /* SatH1 is 1 */

#define POLYCRC32   0xEDB88320u /* CRC32 polynomial */
#define POLYCRC24Q  0x1864CFBu  /* CRC24Q polynomial */

#define P2_5        0.03125               /* 2^-5  */
#define P2_6        0.015625              /* 2^-6  */
#define P2_10       0.0009765625          /* 2^-10 */
#define P2_11       4.882812500000000E-04 /* 2^-11 */
#define P2_15       3.051757812500000E-05 /* 2^-15 */
#define P2_17       7.629394531250000E-06 /* 2^-17 */
#define P2_19       1.907348632812500E-06 /* 2^-19 */
#define P2_20       9.536743164062500E-07 /* 2^-20 */
#define P2_21       4.768371582031250E-07 /* 2^-21 */
#define P2_23       1.192092895507810E-07 /* 2^-23 */
#define P2_24       5.960464477539063E-08 /* 2^-24 */
#define P2_27       7.450580596923828E-09 /* 2^-27 */
#define P2_29       1.862645149230957E-09 /* 2^-29 */
#define P2_30       9.313225746154785E-10 /* 2^-30 */
#define P2_31       4.656612873077393E-10 /* 2^-31 */
#define P2_32       2.328306436538696E-10 /* 2^-32 */
#define P2_33       1.164153218269348E-10 /* 2^-33 */
#define P2_66       1.355252715606881E-20 /* 2^-66 */
#define P2_34       5.820766091346740E-11 /* 2^-34 */
#define P2_35       2.910383045673370E-11 /* 2^-35 */
#define P2_38       3.637978807091710E-12 /* 2^-38 */
#define P2_39       1.818989403545856E-12 /* 2^-39 */
#define P2_40       9.094947017729280E-13 /* 2^-40 */
#define P2_43       1.136868377216160E-13 /* 2^-43 */
#define P2_46       1.421085471520200E-14 /* 2^-46 */
#define P2_48       3.552713678800501E-15 /* 2^-48 */
#define P2_50       8.881784197001252E-16 /* 2^-50 */
#define P2_55       2.775557561562891E-17 /* 2^-55 */
#define P2_59       1.734723475976810E-18 /* 2^-59 */
#define P2_60       8.673617379884035E-19 /* 2^-60 */


/* RTCM Decoder Base class */
class RTCMDecoder
{
public:


    /**
     * Default constructor
     */
    RTCMDecoder();

    /**
     * Destructor(must be virtual)
     */
    virtual ~RTCMDecoder();

    /**
     * Decode the RTCM raw data
     *
     * @param  buff    byte data
     * @param  len     bit length
     * @return the decode state(true:success, false:failure)
     */
    virtual bool decode(unsigned char* buff, int len) = 0;


    /**
     * Initialize RINEX Writer
     * @param  staID    the ID of mountpoint
     * @param  mountPoint  mountpoint url
     * @param  latitude
     * @param  longitude
     * @param  nmea
     * @param  ntripVersion
     */
    void initRinex(const std::string& staID, const NetUrl& mountPoint,
                   const std::string& latitude, const std::string& longitude,
                   const std::string& nmea, const std::string& ntripVersion);

    /**
     * Write Rinex Espoch
     *
     */
    void dumpRinexEpoch(const t_satObs &obs, const std::string &format);

       // antenna information type
    struct t_antInfo {
      enum t_type { ARP, APC };

      t_antInfo() {
        xx = yy = zz = height = 0.0;
        type = ARP;
        height_f = false;
        message  = 0;
      }

      double xx;
      double yy;
      double zz;
      t_type type;
      double height;
      bool   height_f;
      int    message;
    };

    // station information type
    struct t_staInfo
    {
        t_staInfo()
        {
            antSetup = 0;
            itrf = 0;
            height = 0.0;
            for(int i=0;i<3;++i)
            {
                pos[i] = del[i] = 0.0;
            }
            for(int i=0;i<MAXANT;++i)
            {
                antDes[i] = '0';
                antNum[i] = '0';
                rcvDes[i] = '0';
                rcvVer[i] = '0';
                rcvNum[i] = '0';
            }
        }
        char antDes   [MAXANT]; /* antenna descriptor */
        char antNum   [MAXANT]; /* antenna serial number */
        char rcvDes   [MAXANT]; /* receiver type descriptor */
        char rcvVer   [MAXANT]; /* receiver firmware version */
        char rcvNum   [MAXANT]; /* receiver serial number */

        int staID;          /* station id  */
        int antSetup;       /* antenna setup id */
        int itrf;           /* ITRF realization year */
        double pos[3];      /* station position (ecef) (m) */
        double del[3];      /* antenna position delta (x/y/z) (m) */
        double height;      /* antenna height (m) */
    };

       // List of observations
    std::list<t_satObs> m_obsList;

       // RTCM message types
    std::list<int> m_typeList;

       // RTCM antenna descriptor
    std::list<std::string> m_antType;

       // RTCM antenna XYZ
    std::list<t_antInfo> m_antList;



       // GLONASS frequency
    std::string m_sGloFreq;

       // Rinex writer
    NtripObsStream* m_rnx;

};	// End of class 'RTCMDecoder'



/* Common functions for RTCM Decoding ----------------------*/

// get the GPS time from GPS week and second of week
///////////////////////////////////////////////////////////
inline CommonTime setTime(int week, double sec)
{
    GPSWeekSecond weeksec(week,sec);
    return weeksec.convertToCommonTime();
}

// get the GPS time from second of week
///////////////////////////////////////////////////////////
inline CommonTime setGPS(int msec)
{
    int week;
    double sec;

    SystemTime system;
    CommonTime ct(system);
    GPSWeekSecond gws;
    gws.convertFromCommonTime(ct);

    week = gws.getWeek();
    sec = gws.getSOW();
    if(msec/1000.0 < sec - 86400.0)
      ++week;
    return setTime(week, msec/1000.0);
}

// get the current GPS week and seconds from civil time
///////////////////////////////////////////////////////////
inline void currentGPSWeeks(int& week, double& sec)
{
    SystemTime system;
    CommonTime ct(system);
    GPSWeekSecond gws;
    gws.convertFromCommonTime(ct);
    week = gws.getWeek();
    sec = gws.getSOW();
}

inline double accuracyFromIndex(int index, gpstk::SatID::SatelliteSystem system)
{

    if (system == gpstk::SatID::systemGPS || system == gpstk::SatID::systemBeiDou
            || system == gpstk::SatID::systemGeosync || system == gpstk::SatID::systemQZSS) {

      if ((index >= 0) && (index <= 6)) {
        if (index == 3) {
          return ceil(10.0 * pow(2.0, (double(index) / 2.0) + 1.0)) / 10.0;
        }
        else {
          return floor(10.0 * pow(2.0, (double(index) / 2.0) + 1.0)) / 10.0;
        }
      }
      else if ((index > 6) && (index <= 15)) {
        return (10.0 * pow(2.0, (double(index) - 2.0))) / 10.0;
      }
      else {
        return 8192.0;
      }
    }

    if (system == gpstk::SatID::systemGalileo) {

      if ((index >= 0) && (index <= 49)) {
        return (double(index) / 100.0);
      }
      else if ((index > 49) && (index <= 74)) {
        return (50.0 + (double(index) - 50.0) * 2.0) / 100.0;
      }
      else if ((index > 74) && (index <= 99)) {
        return 1.0 + (double(index) - 75.0) * 0.04;
      }
      else if ((index > 99) && (index <= 125)) {
        return 2.0 + (double(index) - 100.0) * 0.16;
      }
      else {
        return -1.0;
      }
    }

    return double(index);
  }


#endif // GPSTK_RTCMDECODER_H
