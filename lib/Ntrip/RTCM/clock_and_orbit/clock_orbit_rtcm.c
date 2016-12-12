/* Programheader

        Name:           clock_orbit_rtcm.c
        Project:        RTCM3
        Version:        $Id: clock_orbit_rtcm.c 7777 2016-02-16 08:57:27Z stuerze $
        Authors:        Dirk Stöcker
        Description:    state space approach for RTCM3
*/

#include <math.h>
#include <stdio.h>
#include <string.h>
#ifndef sparc
#include <stdint.h>
#else
#include <sys/types.h>
#endif
#include "clock_orbit_rtcm.h"

static uint32_t CRC24(long size, const unsigned char *buf)
{
  uint32_t crc = 0;
  int i;

  while(size--)
  {
    crc ^= (*buf++) << (16);
    for(i = 0; i < 8; i++)
    {
      crc <<= 1;
      if(crc & 0x1000000)
        crc ^= 0x01864cfb;
    }
  }
  return crc;
}

/* NOTE: These defines are interlinked with below functions and directly modify
the values. This may not be optimized in terms of final program code size but
should be optimized in terms of speed.

modified variables are:
- everything defined in STARTDATA (only use ressize outside of the defines,
  others are private)
- buffer
- size
*/

#ifndef NOENCODE
#define STOREBITS \
  while(numbits >= 8) \
  { \
    if(!size) return 0; \
    *(buffer++) = bitbuffer>>(numbits-8); \
    numbits -= 8; \
    ++ressize; \
    --size; \
  }

#define ADDBITS(a, b) \
  { \
    bitbuffer = (bitbuffer<<(a))|((b)&((1<<a)-1)); \
    numbits += (a); \
    STOREBITS \
  }

#define STARTDATA \
  size_t ressize=0; \
  char *blockstart; \
  int numbits; \
  uint64_t bitbuffer=0;

#define INITBLOCK \
  numbits = 0; \
  blockstart = buffer; \
  ADDBITS(8, 0xD3) \
  ADDBITS(6, 0) \
  ADDBITS(10, 0)

#define ENDBLOCK \
  if(numbits) { ADDBITS((8-numbits), 0) } \
  { \
    int len = buffer-blockstart-3; \
    blockstart[1] |= len>>8; \
    blockstart[2] = len; \
    if(len > 1023) \
      return 0; \
    len = CRC24(len+3, (const unsigned char *) blockstart); \
    ADDBITS(24, len) \
  }

#define SCALEADDBITS(a, b, c) ADDBITS(a, (int64_t)(c > 0 ? b*c+0.5 : b*c-0.5))

#define MPI         3.141592653589793

/* GPS macros also used for other systems when matching! */
#define T_MESSAGE_NUMBER(a)              ADDBITS(12, a)     /* DF002 */
#define T_GPS_SATELLITE_ID(a)            ADDBITS(6, a)      /* DF068 */
#define T_QZSS_SATELLITE_ID(a)           ADDBITS(4, a)      /* DF249 */
#define T_GLONASS_SATELLITE_ID(a)        ADDBITS(5, a)

#define T_GPS_IODE(a)                    ADDBITS(8, a)      /* DF071 */
#define T_GLONASS_IOD(a)                 ADDBITS(8, a)      /* DF237 */
#define T_GALILEO_IOD(a)                 ADDBITS(10, a)     /* DF459 */
#define T_SBAS_T0MOD(a)                  ADDBITS(9, (a/16)) /* DF468 */
#define T_SBAS_IODCRC(a)                 ADDBITS(24, a)     /* DF469 */
#define T_BDS_TOEMOD(a)                  ADDBITS(10, (a/8)) /* DF470 */
#define T_BDS_IODCRC(a)                  ADDBITS(24, a)     /* DF471 */

#define T_DELTA_RADIAL(a)                SCALEADDBITS(22,    10000.0, a)
#define T_DELTA_ALONG_TRACK(a)           SCALEADDBITS(20,     2500.0, a)
#define T_DELTA_CROSS_TRACK(a)           SCALEADDBITS(20,     2500.0, a)
#define T_DELTA_DOT_RADIAL(a)            SCALEADDBITS(21,  1000000.0, a)
#define T_DELTA_DOT_ALONG_TRACK(a)       SCALEADDBITS(19,   250000.0, a)
#define T_DELTA_DOT_CROSS_TRACK(a)       SCALEADDBITS(19,   250000.0, a)

#define T_SATELLITE_REFERENCE_DATUM(a)   ADDBITS(1, a)
#define T_DELTA_CLOCK_C0(a)              SCALEADDBITS(22,    10000.0, a)
#define T_DELTA_CLOCK_C1(a)              SCALEADDBITS(21,  1000000.0, a)
#define T_DELTA_CLOCK_C2(a)              SCALEADDBITS(27, 50000000.0, a)
#define T_NO_OF_CODE_BIASES(a)           ADDBITS(5, a)
#define T_NO_OF_PHASE_BIASES(a)          ADDBITS(5, a)
#define T_SIGNAL_IDENTIFIER(a)           ADDBITS(5, a)
#define T_CODE_BIAS(a)                   SCALEADDBITS(14,      100.0, a)
#define T_YAW_ANGLE(a)                   SCALEADDBITS(9,   256.0/MPI, a)
#define T_YAW_RATE(a)                    SCALEADDBITS(8,  8192.0/MPI, a)
#define T_PHASE_BIAS(a)                  SCALEADDBITS(20,    10000.0, a)

#define T_GPS_EPOCH_TIME(a)              ADDBITS(20, a)
#define T_GLONASS_EPOCH_TIME(a)          ADDBITS(17, a)
#define T_NO_OF_SATELLITES(a)            ADDBITS(6, a)
#define T_MULTIPLE_MESSAGE_INDICATOR(a)  ADDBITS(1, a)
#define T_DISPERSIVE_BIAS_INDICATOR(a)   ADDBITS(1, a)
#define T_MW_CONSISTENCY_INDICATOR(a)    ADDBITS(1, a)
#define T_INTEGER_INDICATOR(a)           ADDBITS(1, a)
#define T_WIDE_LANE_INDICATOR(a)         ADDBITS(2, a)
#define T_DISCONTINUITY_COUNTER(a)       ADDBITS(4, a)
#define T_SSR_URA(a)                     ADDBITS(6, a)
#define T_HR_CLOCK_CORRECTION(a)         SCALEADDBITS(22,    10000.0, a)
#define T_SSR_UPDATE_INTERVAL(a)         ADDBITS(4, a)

#define T_SSR_IOD(a)                     ADDBITS(4, a)
#define T_SSR_PROVIDER_ID(a)             ADDBITS(16, a)
#define T_SSR_SOLUTION_ID(a)             ADDBITS(4, a)

#define T_NO_IONO_LAYERS(a)              ADDBITS(2, a-1)
#define T_VTEC_QUALITY_INDICATOR(a)      SCALEADDBITS(9,        20.0, a)
#define T_IONO_COEFF(a)                  SCALEADDBITS(16,      200.0, a)
#define T_IONO_DEGREE(a)                 ADDBITS(4, a-1)
#define T_IONO_ORDER(a)                  ADDBITS(4, a-1)
#define T_IONO_HEIGHT(a)                 SCALEADDBITS(8,   1/10000.0, a)

static double URAToValue(int ura)
{
  int urac, urav;
  urac = ura >> 3;
  urav = ura & 7;
  if(!ura)
    return 0;
  else if(ura == 63)
    return SSR_MAXURA;
  return (pow(3,urac)*(1.0+urav/4.0)-1.0)/1000.0;
}

static int ValueToURA(double val)
{
  int ura;
  if(!val)
    return 0;
  else if(val > 5.4665)
    return 63;
  for(ura = 1; ura < 63 && val > URAToValue(ura); ++ura)
    ;
  return ura;
}

static const enum ClockOrbitType corbase[CLOCKORBIT_SATNUM] =
{
  (int) COBBASE_GPS, (int) COBBASE_GLONASS, (int) COBBASE_GALILEO,
  (int) COBBASE_QZSS, (int) COBBASE_SBAS, (int) COBBASE_BDS
};
static const enum COR_OFFSETS satoffset[CLOCKORBIT_SATNUM+1] =
{
  CLOCKORBIT_OFFSETGPS, CLOCKORBIT_OFFSETGLONASS, CLOCKORBIT_OFFSETGALILEO,
  CLOCKORBIT_OFFSETQZSS, CLOCKORBIT_OFFSETSBAS, CLOCKORBIT_OFFSETBDS,
  CLOCKORBIT_COUNTSAT
};

size_t MakeClockOrbit(const struct ClockOrbit *co, enum ClockOrbitType type,
int moremessagesfollow, char *buffer, size_t size)
{
  unsigned int status[CLOCKORBIT_SATNUM][COBOFS_NUM], i, s;

  memset(status, 0, sizeof(status));
  STARTDATA

  for(s = 0; s < CLOCKORBIT_SATNUM; ++s)
  {
    for(i = 0; i < COBOFS_NUM; ++i)
    {
      if(co->NumberOfSat[s] && (type == COTYPE_AUTO
      || type == corbase[s]+i) && (co->Supplied[i] || (i <= COBOFS_CLOCK
      && co->Supplied[COBOFS_COMBINED]) || (i == COBOFS_COMBINED
      && co->Supplied[COBOFS_ORBIT] && co->Supplied[COBOFS_CLOCK])))
      {
        status[s][i] = 1;
        if(i == COBOFS_COMBINED)
        {
          status[s][COBOFS_ORBIT] = status[s][COBOFS_CLOCK] = 0;
        } /* disable single blocks for combined type */
      } /* check for data */
    } /* iterate over RTCM data types */
  } /* iterate over satellite systems */

  for(s = 0; s < CLOCKORBIT_SATNUM; ++s)
  {
    if(status[s][COBOFS_ORBIT])
    {
      INITBLOCK
      T_MESSAGE_NUMBER(corbase[s]+COBOFS_ORBIT)
      switch(s)
      {
      case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
      case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
      case CLOCKORBIT_SATBDS:
        T_GPS_EPOCH_TIME(co->EpochTime[s])
        break;
      case CLOCKORBIT_SATGLONASS:
        T_GLONASS_EPOCH_TIME(co->EpochTime[s])
        break;
      }
      T_SSR_UPDATE_INTERVAL(co->UpdateInterval)
      T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
      T_SATELLITE_REFERENCE_DATUM(co->SatRefDatum)
      T_SSR_IOD(co->SSRIOD)
      T_SSR_PROVIDER_ID(co->SSRProviderID)
      T_SSR_SOLUTION_ID(co->SSRSolutionID)
      T_NO_OF_SATELLITES(co->NumberOfSat[s])
      for(i = satoffset[s]; i < satoffset[s]+co->NumberOfSat[s]; ++i)
      {
        switch(s)
        {
        case CLOCKORBIT_SATGPS:
          T_GPS_SATELLITE_ID(co->Sat[i].ID)
          T_GPS_IODE(co->Sat[i].IOD)
          break;
        case CLOCKORBIT_SATGLONASS:
          T_GLONASS_SATELLITE_ID(co->Sat[i].ID)
          T_GLONASS_IOD(co->Sat[i].IOD)
          break;
        case CLOCKORBIT_SATGALILEO:
          T_GPS_SATELLITE_ID(co->Sat[i].ID)
          T_GALILEO_IOD(co->Sat[i].IOD)
          break;
        case CLOCKORBIT_SATQZSS:
          T_QZSS_SATELLITE_ID(co->Sat[i].ID)
          T_GPS_IODE(co->Sat[i].IOD)
          break;
        case CLOCKORBIT_SATSBAS:
          T_GPS_SATELLITE_ID(co->Sat[i].ID)
          T_SBAS_T0MOD(co->Sat[i].toe)
          T_SBAS_IODCRC(co->Sat[i].IOD)
          break;
        case CLOCKORBIT_SATBDS:
          T_GPS_SATELLITE_ID(co->Sat[i].ID)
          T_BDS_TOEMOD(co->Sat[i].toe)
          T_BDS_IODCRC(co->Sat[i].IOD)
          break;
        }
        T_DELTA_RADIAL(co->Sat[i].Orbit.DeltaRadial)
        T_DELTA_ALONG_TRACK(co->Sat[i].Orbit.DeltaAlongTrack)
        T_DELTA_CROSS_TRACK(co->Sat[i].Orbit.DeltaCrossTrack)
        T_DELTA_DOT_RADIAL(co->Sat[i].Orbit.DotDeltaRadial)
        T_DELTA_DOT_ALONG_TRACK(co->Sat[i].Orbit.DotDeltaAlongTrack)
        T_DELTA_DOT_CROSS_TRACK(co->Sat[i].Orbit.DotDeltaCrossTrack)
      }
      ENDBLOCK
    }
    if(status[s][COBOFS_CLOCK])
    {
      INITBLOCK
      T_MESSAGE_NUMBER(corbase[s]+COBOFS_CLOCK)
      switch(s)
      {
      case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
      case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
      case CLOCKORBIT_SATBDS:
        T_GPS_EPOCH_TIME(co->EpochTime[s])
        break;
      case CLOCKORBIT_SATGLONASS:
        T_GLONASS_EPOCH_TIME(co->EpochTime[s])
        break;
      }
      T_SSR_UPDATE_INTERVAL(co->UpdateInterval)
      T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
      T_SSR_IOD(co->SSRIOD)
      T_SSR_PROVIDER_ID(co->SSRProviderID)
      T_SSR_SOLUTION_ID(co->SSRSolutionID)
      T_NO_OF_SATELLITES(co->NumberOfSat[s])
      for(i = satoffset[s]; i < satoffset[s]+co->NumberOfSat[s]; ++i)
      {
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
          T_GPS_SATELLITE_ID(co->Sat[i].ID)
          break;
        case CLOCKORBIT_SATQZSS:
          T_QZSS_SATELLITE_ID(co->Sat[i].ID)
          break;
        case CLOCKORBIT_SATGLONASS:
          T_GLONASS_SATELLITE_ID(co->Sat[i].ID)
          break;
        }
        T_DELTA_CLOCK_C0(co->Sat[i].Clock.DeltaA0)
        T_DELTA_CLOCK_C1(co->Sat[i].Clock.DeltaA1)
        T_DELTA_CLOCK_C2(co->Sat[i].Clock.DeltaA2)
      }
      ENDBLOCK
    }
    if(status[s][COBOFS_COMBINED])
    {
#ifdef SPLITBLOCK
      int nums = co->NumberOfSat[s];
      int left, start = satoffset[s];
      if(nums > 28) /* split block when more than 28 sats */
      {
        left = nums - 28;
        nums = 28;
      }
      else
      {
        left = 0;
      }
      while(nums)
      {
#endif
        INITBLOCK
        T_MESSAGE_NUMBER(corbase[s]+COBOFS_COMBINED)
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
        case CLOCKORBIT_SATBDS:
          T_GPS_EPOCH_TIME(co->EpochTime[s])
          break;
        case CLOCKORBIT_SATGLONASS:
          T_GLONASS_EPOCH_TIME(co->EpochTime[s])
          break;
        }
        T_SSR_UPDATE_INTERVAL(co->UpdateInterval)
#ifdef SPLITBLOCK
        T_MULTIPLE_MESSAGE_INDICATOR((moremessagesfollow || left) ? 1 : 0)
#else
        T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
#endif
        T_SATELLITE_REFERENCE_DATUM(co->SatRefDatum)
        T_SSR_IOD(co->SSRIOD)
        T_SSR_PROVIDER_ID(co->SSRProviderID)
        T_SSR_SOLUTION_ID(co->SSRSolutionID)
#ifdef SPLITBLOCK
        T_NO_OF_SATELLITES(nums)
        for(i = start; i < start+nums; ++i)
#else
        T_NO_OF_SATELLITES(co->NumberOfSat[s])
        for(i = satoffset[s]; i < satoffset[s]+co->NumberOfSat[s]; ++i)
#endif
        {
          switch(s)
          {
          case CLOCKORBIT_SATGPS:
            T_GPS_SATELLITE_ID(co->Sat[i].ID)
            T_GPS_IODE(co->Sat[i].IOD)
            break;
          case CLOCKORBIT_SATGLONASS:
            T_GLONASS_SATELLITE_ID(co->Sat[i].ID)
            T_GLONASS_IOD(co->Sat[i].IOD)
            break;
          case CLOCKORBIT_SATGALILEO:
            T_GPS_SATELLITE_ID(co->Sat[i].ID)
            T_GALILEO_IOD(co->Sat[i].IOD)
            break;
          case CLOCKORBIT_SATQZSS:
            T_QZSS_SATELLITE_ID(co->Sat[i].ID)
            T_GPS_IODE(co->Sat[i].IOD)
            break;
          case CLOCKORBIT_SATSBAS:
            T_GPS_SATELLITE_ID(co->Sat[i].ID)
            T_SBAS_T0MOD(co->Sat[i].toe)
            T_SBAS_IODCRC(co->Sat[i].IOD)
            break;
          case CLOCKORBIT_SATBDS:
            T_GPS_SATELLITE_ID(co->Sat[i].ID)
            T_BDS_TOEMOD(co->Sat[i].toe)
            T_BDS_IODCRC(co->Sat[i].IOD)
            break;
          }
          T_DELTA_RADIAL(co->Sat[i].Orbit.DeltaRadial)
          T_DELTA_ALONG_TRACK(co->Sat[i].Orbit.DeltaAlongTrack)
          T_DELTA_CROSS_TRACK(co->Sat[i].Orbit.DeltaCrossTrack)
          T_DELTA_DOT_RADIAL(co->Sat[i].Orbit.DotDeltaRadial)
          T_DELTA_DOT_ALONG_TRACK(co->Sat[i].Orbit.DotDeltaAlongTrack)
          T_DELTA_DOT_CROSS_TRACK(co->Sat[i].Orbit.DotDeltaCrossTrack)
          T_DELTA_CLOCK_C0(co->Sat[i].Clock.DeltaA0)
          T_DELTA_CLOCK_C1(co->Sat[i].Clock.DeltaA1)
          T_DELTA_CLOCK_C2(co->Sat[i].Clock.DeltaA2)
        }
        ENDBLOCK
#ifdef SPLITBLOCK
        start += nums;
        nums = left;
        left = 0;
      }
#endif
    }
    if(status[s][COBOFS_HR])
    {
      INITBLOCK
      T_MESSAGE_NUMBER(corbase[s]+COBOFS_HR)
      switch(s)
      {
      case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
      case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
      case CLOCKORBIT_SATBDS:
        T_GPS_EPOCH_TIME(co->EpochTime[s])
        break;
      case CLOCKORBIT_SATGLONASS:
        T_GLONASS_EPOCH_TIME(co->EpochTime[s])
        break;
      }
      T_SSR_UPDATE_INTERVAL(co->UpdateInterval)
      T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
      T_SSR_IOD(co->SSRIOD)
      T_SSR_PROVIDER_ID(co->SSRProviderID)
      T_SSR_SOLUTION_ID(co->SSRSolutionID)
      T_NO_OF_SATELLITES(co->NumberOfSat[s])
      for(i = satoffset[s]; i < satoffset[s]+co->NumberOfSat[s]; ++i)
      {
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
          T_GPS_SATELLITE_ID(co->Sat[i].ID)
          break;
        case CLOCKORBIT_SATQZSS:
          T_QZSS_SATELLITE_ID(co->Sat[i].ID)
          break;
        case CLOCKORBIT_SATGLONASS:
          T_GLONASS_SATELLITE_ID(co->Sat[i].ID)
          break;
        }
        T_HR_CLOCK_CORRECTION(co->Sat[i].hrclock)
      }
      ENDBLOCK
    }
    if(status[s][COBOFS_URA])
    {
      INITBLOCK
      T_MESSAGE_NUMBER(corbase[s]+COBOFS_URA)
      switch(s)
      {
      case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
      case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
      case CLOCKORBIT_SATBDS:
        T_GPS_EPOCH_TIME(co->EpochTime[s])
        break;
      case CLOCKORBIT_SATGLONASS:
        T_GLONASS_EPOCH_TIME(co->EpochTime[s])
        break;
      }
      T_SSR_UPDATE_INTERVAL(co->UpdateInterval)
      T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
      T_SSR_IOD(co->SSRIOD)
      T_SSR_PROVIDER_ID(co->SSRProviderID)
      T_SSR_SOLUTION_ID(co->SSRSolutionID)
      T_NO_OF_SATELLITES(co->NumberOfSat[s])
      for(i = satoffset[s]; i < satoffset[s]+co->NumberOfSat[s]; ++i)
      {
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
          T_GPS_SATELLITE_ID(co->Sat[i].ID)
          break;
        case CLOCKORBIT_SATQZSS:
          T_QZSS_SATELLITE_ID(co->Sat[i].ID)
          break;
        case CLOCKORBIT_SATGLONASS:
          T_GLONASS_SATELLITE_ID(co->Sat[i].ID)
          break;
        }
        T_SSR_URA(ValueToURA(co->Sat[i].UserRangeAccuracy))
      }
      ENDBLOCK
    }
  }
  return ressize;
}

size_t MakeCodeBias(const struct CodeBias *b, enum CodeBiasType type,
int moremessagesfollow, char *buffer, size_t size)
{
  unsigned int s, i, j;

  STARTDATA

  for(s = 0; s < CLOCKORBIT_SATNUM; ++s)
  {
    if(b->NumberOfSat[s] && (type == BTYPE_AUTO || type == corbase[s]+COBOFS_BIAS))
    {
      INITBLOCK
      T_MESSAGE_NUMBER(corbase[s]+COBOFS_BIAS)
      switch(s)
      {
      case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
      case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
      case CLOCKORBIT_SATBDS:
        T_GPS_EPOCH_TIME(b->EpochTime[s])
        break;
      case CLOCKORBIT_SATGLONASS:
        T_GLONASS_EPOCH_TIME(b->EpochTime[s])
        break;
      }
      T_SSR_UPDATE_INTERVAL(b->UpdateInterval)
      T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
      T_SSR_IOD(b->SSRIOD)
      T_SSR_PROVIDER_ID(b->SSRProviderID)
      T_SSR_SOLUTION_ID(b->SSRSolutionID)
      T_NO_OF_SATELLITES(b->NumberOfSat[s])
      for(i = satoffset[s]; i < satoffset[s]+b->NumberOfSat[s]; ++i)
      {
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
          T_GPS_SATELLITE_ID(b->Sat[i].ID)
          break;
        case CLOCKORBIT_SATQZSS:
          T_QZSS_SATELLITE_ID(b->Sat[i].ID)
          break;
        case CLOCKORBIT_SATGLONASS:
          T_GLONASS_SATELLITE_ID(b->Sat[i].ID)
          break;
        }
        T_NO_OF_CODE_BIASES(b->Sat[i].NumberOfCodeBiases)
        for(j = 0; j < b->Sat[i].NumberOfCodeBiases; ++j)
        {
          T_SIGNAL_IDENTIFIER(b->Sat[i].Biases[j].Type)
          T_CODE_BIAS(b->Sat[i].Biases[j].Bias)
        }
      }
      ENDBLOCK
    }
  }
  return ressize;
}

size_t MakePhaseBias(const struct PhaseBias *b, enum PhaseBiasType type,
int moremessagesfollow, char *buffer, size_t size)
{
  unsigned int s, i, j;

  STARTDATA

  for(s = 0; s < CLOCKORBIT_SATNUM; ++s)
  {
    if(b->NumberOfSat[s] && (type == PBTYPE_AUTO || type == s+PBTYPE_BASE))
    {
      INITBLOCK
      T_MESSAGE_NUMBER(s+PBTYPE_BASE)
      switch(s)
      {
      case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
      case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
      case CLOCKORBIT_SATBDS:
        T_GPS_EPOCH_TIME(b->EpochTime[s])
        break;
      case CLOCKORBIT_SATGLONASS:
        T_GLONASS_EPOCH_TIME(b->EpochTime[s])
        break;
      }
      T_SSR_UPDATE_INTERVAL(b->UpdateInterval)
      T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
      T_SSR_IOD(b->SSRIOD)
      T_SSR_PROVIDER_ID(b->SSRProviderID)
      T_SSR_SOLUTION_ID(b->SSRSolutionID)
      T_DISPERSIVE_BIAS_INDICATOR(b->DispersiveBiasConsistencyIndicator ? 1 : 0)
      T_MW_CONSISTENCY_INDICATOR(b->MWConsistencyIndicator ? 1 : 0)
      T_NO_OF_SATELLITES(b->NumberOfSat[s])
      for(i = satoffset[s]; i < satoffset[s]+b->NumberOfSat[s]; ++i)
      {
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
          T_GPS_SATELLITE_ID(b->Sat[i].ID)
          break;
        case CLOCKORBIT_SATQZSS:
          T_QZSS_SATELLITE_ID(b->Sat[i].ID)
          break;
        case CLOCKORBIT_SATGLONASS:
          T_GLONASS_SATELLITE_ID(b->Sat[i].ID)
          break;
        }
        T_NO_OF_PHASE_BIASES(b->Sat[i].NumberOfPhaseBiases)
        T_YAW_ANGLE(b->Sat[i].YawAngle)
        T_YAW_RATE(b->Sat[i].YawRate)
        for(j = 0; j < b->Sat[i].NumberOfPhaseBiases; ++j)
        {
          T_SIGNAL_IDENTIFIER(b->Sat[i].Biases[j].Type)
          T_INTEGER_INDICATOR(b->Sat[i].Biases[j].SignalIntegerIndicator ? 1 : 0)
          T_WIDE_LANE_INDICATOR(b->Sat[i].Biases[j].SignalsWideLaneIntegerIndicator)
          T_DISCONTINUITY_COUNTER(b->Sat[i].Biases[j].SignalDiscontinuityCounter)
          T_PHASE_BIAS(b->Sat[i].Biases[j].Bias)
        }
      }
      ENDBLOCK
    }
  }
  return ressize;
}

size_t MakeVTEC(const struct VTEC *v, int moremessagesfollow, char *buffer,
size_t size)
{
  unsigned int l, o, d;

  STARTDATA
  INITBLOCK
  T_MESSAGE_NUMBER(VTEC_BASE)

  T_GPS_EPOCH_TIME(v->EpochTime)
  T_SSR_UPDATE_INTERVAL(v->UpdateInterval)
  T_MULTIPLE_MESSAGE_INDICATOR(moremessagesfollow ? 1 : 0)
  T_SSR_IOD(v->SSRIOD)
  T_SSR_PROVIDER_ID(v->SSRProviderID)
  T_SSR_SOLUTION_ID(v->SSRSolutionID)
  T_VTEC_QUALITY_INDICATOR(v->Quality)
  T_NO_IONO_LAYERS(v->NumLayers)
  for(l = 0; l < v->NumLayers; ++l)
  {
    T_IONO_HEIGHT(v->Layers[l].Height)
    T_IONO_DEGREE(v->Layers[l].Degree)
    T_IONO_ORDER(v->Layers[l].Order)
    for(o = 0; o <= v->Layers[l].Order; ++o)
    {
      for(d = o; d <= v->Layers[l].Degree; ++d)
      {
        T_IONO_COEFF(v->Layers[l].Cosinus[d][o])
      }
    }
    for(o = 1; o <= v->Layers[l].Order; ++o)
    {
      for(d = o; d <= v->Layers[l].Degree; ++d)
      {
        T_IONO_COEFF(v->Layers[l].Sinus[d][o])
      }
    }
  }
  ENDBLOCK
  return ressize;
}
#endif /* NOENCODE */

#ifndef NODECODE

#define DECODESTART \
  int numbits=0; \
  uint64_t bitbuffer=0;

#define LOADBITS(a) \
{ \
  while((a) > numbits) \
  { \
    if(!size--) return GCOBR_SHORTMESSAGE; \
    bitbuffer = (bitbuffer<<8)|((unsigned char)*(buffer++)); \
    numbits += 8; \
  } \
}

/* extract bits from data stream
   b = variable to store result, a = number of bits */
#define GETBITS(b, a) \
{ \
  LOADBITS(a) \
  b = (bitbuffer<<(64-numbits))>>(64-(a)); \
  numbits -= (a); \
}

/* extract bits from data stream
   b = variable to store result, a = number of bits */
#define GETBITSFACTOR(b, a, c) \
{ \
  LOADBITS(a) \
  b = ((bitbuffer<<(64-numbits))>>(64-(a)))*(c); \
  numbits -= (a); \
}

/* extract signed floating value from data stream
   b = variable to store result, a = number of bits */
#define GETFLOATSIGN(b, a, c) \
{ \
  LOADBITS(a) \
  b = ((double)(((int64_t)(bitbuffer<<(64-numbits)))>>(64-(a))))*(c); \
  numbits -= (a); \
}

/* extract floating value from data stream
   b = variable to store result, a = number of bits, c = scale factor */
#define GETFLOAT(b, a, c) \
{ \
  LOADBITS(a) \
  b = ((double)((bitbuffer<<(sizeof(bitbuffer)*8-numbits))>>(sizeof(bitbuffer)*8-(a))))*(c); \
  numbits -= (a); \
}

#define SKIPBITS(b) { LOADBITS(b) numbits -= (b); }

/* GPS macros also used for other systems when matching! */
#define G_HEADER(a)                      GETBITS(a,8)
#define G_RESERVEDH(a)                   GETBITS(a,6)
#define G_SIZE(a)                        GETBITS(a, 10)
#define G_MESSAGE_NUMBER(a)              GETBITS(a, 12)          /* DF002 */
#define G_GPS_SATELLITE_ID(a)            GETBITS(a, 6)           /* DF068 */
#define G_QZSS_SATELLITE_ID(a)           GETBITS(a, 4)           /* DF249 */
#define G_GLONASS_SATELLITE_ID(a)        GETBITS(a, 5)

#define G_GPS_IODE(a)                    GETBITS(a, 8)           /* DF071 */
#define G_GLONASS_IOD(a)                 GETBITS(a, 8)           /* DF237 */
#define G_GALILEO_IOD(a)                 GETBITS(a, 10)          /* DF459 */
#define G_SBAS_T0MOD(a)                  GETBITSFACTOR(a, 9, 16) /* DF468 */
#define G_SBAS_IODCRC(a)                 GETBITS(a, 24)          /* DF469 */
#define G_BDS_TOEMOD(a)                  GETBITSFACTOR(a, 10, 8) /* DF470 */
#define G_BDS_IODCRC(a)                  GETBITS(a, 24)          /* DF471 */

/* defined values */
#define G_DELTA_RADIAL(a)                GETFLOATSIGN(a, 22, 1/10000.0)
#define G_DELTA_ALONG_TRACK(a)           GETFLOATSIGN(a, 20, 1/2500.0)
#define G_DELTA_CROSS_TRACK(a)           GETFLOATSIGN(a, 20, 1/2500.0)
#define G_DELTA_DOT_RADIAL(a)            GETFLOATSIGN(a, 21, 1/1000000.0)
#define G_DELTA_DOT_ALONG_TRACK(a)       GETFLOATSIGN(a, 19, 1/250000.0)
#define G_DELTA_DOT_CROSS_TRACK(a)       GETFLOATSIGN(a, 19, 1/250000.0)

#define G_SATELLITE_REFERENCE_DATUM(a)   GETBITS(a, 1)
#define G_DELTA_CLOCK_C0(a)              GETFLOATSIGN(a, 22, 1/10000.0)
#define G_DELTA_CLOCK_C1(a)              GETFLOATSIGN(a, 21, 1/1000000.0)
#define G_DELTA_CLOCK_C2(a)              GETFLOATSIGN(a, 27, 1/50000000.0)
#define G_NO_OF_CODE_BIASES(a)           GETBITS(a, 5)
#define G_NO_OF_PHASE_BIASES(a)          GETBITS(a, 5)
#define G_SIGNAL_IDENTIFIER(a)           GETBITS(a, 5)
#define G_CODE_BIAS(a)                   GETFLOATSIGN(a, 14, 1/100.0)
#define G_YAW_ANGLE(a)                   GETFLOAT(a, 9, MPI/256.0)
#define G_YAW_RATE(a)                    GETFLOATSIGN(a, 8, MPI/8192.0)
#define G_PHASE_BIAS(a)                  GETFLOATSIGN(a, 20, 1/10000.0)

#define G_GPS_EPOCH_TIME(a, b)           {unsigned int temp; GETBITS(temp, 20) \
 if(b && a != temp) return GCOBR_TIMEMISMATCH; a = temp;}
#define G_GLONASS_EPOCH_TIME(a, b)       {unsigned int temp; GETBITS(temp, 17) \
 if(b && a != temp) return GCOBR_TIMEMISMATCH; a = temp;}
#define G_EPOCH_TIME(a)                  GETBITS(a, 20)
#define G_NO_OF_SATELLITES(a)            GETBITS(a, 6)
#define G_MULTIPLE_MESSAGE_INDICATOR(a)  GETBITS(a, 1)
#define G_DISPERSIVE_BIAS_INDICATOR(a)   GETBITS(a, 1)
#define G_MW_CONSISTENCY_INDICATOR(a)    GETBITS(a, 1)
#define G_INTEGER_INDICATOR(a)           GETBITS(a, 1)
#define G_WIDE_LANE_INDICATOR(a)         GETBITS(a, 2)
#define G_DISCONTINUITY_COUNTER(a)       GETBITS(a, 4)
#define G_SSR_URA(a)                     {int temp; GETBITS(temp, 6) \
 (a) = URAToValue(temp);}
#define G_HR_CLOCK_CORRECTION(a)         GETFLOATSIGN(a, 22, 1/10000.0)
#define G_SSR_UPDATE_INTERVAL(a)         GETBITS(a, 4)

#define G_SSR_IOD(a)                     GETBITS(a, 4)
#define G_SSR_PROVIDER_ID(a)             GETBITS(a, 16)
#define G_SSR_SOLUTION_ID(a)             GETBITS(a, 4)

#define G_NO_IONO_LAYERS(a)              {unsigned int temp; GETBITS(temp, 2) a = temp+1; }
#define G_VTEC_QUALITY_INDICATOR(a)      GETFLOAT(a, 9,      1/20.0)
#define G_IONO_COEFF(a)                  GETFLOATSIGN(a, 16,1/200.0)
#define G_IONO_DEGREE(a)                 {unsigned int temp; GETBITS(temp, 4) a = temp+1; }
#define G_IONO_ORDER(a)                  {unsigned int temp; GETBITS(temp, 4) a = temp+1; }
#define G_IONO_HEIGHT(a)                 GETFLOAT(a, 8 ,    10000.0)

enum GCOB_RETURN GetSSR(struct ClockOrbit *co, struct CodeBias *b, struct VTEC *v,
struct PhaseBias *pb, const char *buffer, size_t size, int *bytesused)
{
  int mmi=0, h, rs;
  unsigned int type, pos, i, j, s, nums, id;
  size_t sizeofrtcmblock;
  const char *blockstart = buffer;
  DECODESTART

  if(size < 7)
    return GCOBR_SHORTBUFFER;

#ifdef DEBUG
  fprintf(stderr, "GetClockOrbitBias START: size %d, numbits %d\n",size, numbits);
#endif

  G_HEADER(h)
  G_RESERVEDH(rs)
  G_SIZE(sizeofrtcmblock);

  if((unsigned char)h != 0xD3 || rs)
    return GCOBR_UNKNOWNDATA;
  if(size < sizeofrtcmblock + 3) /* 3 header bytes already removed */
    return GCOBR_MESSAGEEXCEEDSBUFFER;
  if(CRC24(sizeofrtcmblock+3, (const unsigned char *) blockstart) !=
  (uint32_t)((((unsigned char)buffer[sizeofrtcmblock])<<16)|
  (((unsigned char)buffer[sizeofrtcmblock+1])<<8)|
  (((unsigned char)buffer[sizeofrtcmblock+2]))))
    return GCOBR_CRCMISMATCH;
  size = sizeofrtcmblock; /* reduce size, so overflows are detected */

  G_MESSAGE_NUMBER(type)
#ifdef DEBUG
fprintf(stderr, "type %d size %d\n",type,sizeofrtcmblock);
#endif
  if(bytesused)
    *bytesused = sizeofrtcmblock+6;
  if(type == VTEC_BASE)
  {
    unsigned int l, o, d;
    if(!v) return GCOBR_NOVTECPARAMETER;
    memset(v, 0, sizeof(*v));
    G_EPOCH_TIME(v->EpochTime)
    G_SSR_UPDATE_INTERVAL(v->UpdateInterval)
    G_MULTIPLE_MESSAGE_INDICATOR(mmi)
    G_SSR_IOD(v->SSRIOD)
    G_SSR_PROVIDER_ID(v->SSRProviderID)
    G_SSR_SOLUTION_ID(v->SSRSolutionID)
    G_VTEC_QUALITY_INDICATOR(v->Quality)
    G_NO_IONO_LAYERS(v->NumLayers)
    for(l = 0; l < v->NumLayers; ++l)
    {
      G_IONO_HEIGHT(v->Layers[l].Height)
      G_IONO_DEGREE(v->Layers[l].Degree)
      G_IONO_ORDER(v->Layers[l].Order)
      for(o = 0; o <= v->Layers[l].Order; ++o)
      {
        for(d = o; d <= v->Layers[l].Degree; ++d)
        {
          G_IONO_COEFF(v->Layers[l].Cosinus[d][o])
        }
      }
      for(o = 1; o <= v->Layers[l].Order; ++o)
      {
        for(d = o; d <= v->Layers[l].Degree; ++d)
        {
          G_IONO_COEFF(v->Layers[l].Sinus[d][o])
        }
      }
    }
#ifdef DEBUG
    for(type = 0; type < (int)size && (unsigned char)buffer[type] != 0xD3; ++type)
      numbits += 8;
    fprintf(stderr, "numbits left %d\n",numbits);
#endif
    return mmi ? GCOBR_MESSAGEFOLLOWS : GCOBR_OK;
  }
  for(s = CLOCKORBIT_SATNUM; s-- > 0;)
  {
    if(type == PBTYPE_BASE+s)
    {
      if(!pb) return GCOBR_NOPHASEBIASPARAMETER;
      pb->messageType = type;
      switch(s)
      {
      case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
      case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
      case CLOCKORBIT_SATBDS:
        G_GPS_EPOCH_TIME(pb->EpochTime[s], pb->NumberOfSat[s])
        break;
      case CLOCKORBIT_SATGLONASS:
        G_GLONASS_EPOCH_TIME(pb->EpochTime[s], pb->NumberOfSat[s])
        break;
      }
      G_SSR_UPDATE_INTERVAL(pb->UpdateInterval)
      G_MULTIPLE_MESSAGE_INDICATOR(mmi)
      G_SSR_IOD(pb->SSRIOD)
      G_SSR_PROVIDER_ID(pb->SSRProviderID)
      G_SSR_SOLUTION_ID(pb->SSRSolutionID)
      G_DISPERSIVE_BIAS_INDICATOR(pb->DispersiveBiasConsistencyIndicator)
      G_MW_CONSISTENCY_INDICATOR(pb->MWConsistencyIndicator)
      G_NO_OF_SATELLITES(nums)
      for(i = 0; i < nums; ++i)
      {
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
          G_GPS_SATELLITE_ID(id)
          break;
        case CLOCKORBIT_SATQZSS:
          G_QZSS_SATELLITE_ID(id)
          break;
        case CLOCKORBIT_SATGLONASS:
          G_GLONASS_SATELLITE_ID(id)
          break;
        }
        for(pos = satoffset[s]; pos < satoffset[s]+pb->NumberOfSat[s] && pb->Sat[pos].ID != id; ++pos)
          ;
        if(pos >= satoffset[s+1]) return GCOBR_DATAMISMATCH;
        else if(pos == pb->NumberOfSat[s] + satoffset[s]) ++pb->NumberOfSat[s];
        pb->Sat[pos].ID = id;

        G_NO_OF_PHASE_BIASES(pb->Sat[pos].NumberOfPhaseBiases)
        G_YAW_ANGLE(pb->Sat[pos].YawAngle)
        G_YAW_RATE(pb->Sat[pos].YawRate)
        for(j = 0; j < pb->Sat[pos].NumberOfPhaseBiases; ++j)
        {
          G_SIGNAL_IDENTIFIER(pb->Sat[pos].Biases[j].Type)
          G_INTEGER_INDICATOR(pb->Sat[pos].Biases[j].SignalIntegerIndicator)
          G_WIDE_LANE_INDICATOR(pb->Sat[pos].Biases[j].SignalsWideLaneIntegerIndicator)
          G_DISCONTINUITY_COUNTER(pb->Sat[pos].Biases[j].SignalDiscontinuityCounter)
          G_PHASE_BIAS(pb->Sat[pos].Biases[j].Bias)
        }
      }
#ifdef DEBUG
      for(type = 0; type < (int)size && (unsigned char)buffer[type] != 0xD3; ++type)
        numbits += 8;
      fprintf(stderr, "numbits left %d\n",numbits);
#endif
      return mmi ? GCOBR_MESSAGEFOLLOWS : GCOBR_OK;
    }
    else if(type >= corbase[s])
    {
      switch(type-corbase[s])
      {
      case COBOFS_ORBIT:
        if(!co) return GCOBR_NOCLOCKORBITPARAMETER;
        co->messageType = type;
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
        case CLOCKORBIT_SATBDS:
          G_GPS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        case CLOCKORBIT_SATGLONASS:
          G_GLONASS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        }
        G_SSR_UPDATE_INTERVAL(co->UpdateInterval)
        G_MULTIPLE_MESSAGE_INDICATOR(mmi)
        G_SATELLITE_REFERENCE_DATUM(co->SatRefDatum)
        G_SSR_IOD(co->SSRIOD)
        G_SSR_PROVIDER_ID(co->SSRProviderID)
        G_SSR_SOLUTION_ID(co->SSRSolutionID)
        G_NO_OF_SATELLITES(nums)
        co->Supplied[COBOFS_ORBIT] |= 1;
#ifdef DEBUG
        fprintf(stderr, "epochtime %d ui %d mmi %d sats %d/%d rd %d\n",co->EpochTime[s],
        co->UpdateInterval,mmi,co->NumberOfSat[s],nums, co->SatRefDatum);
#endif
        for(i = 0; i < nums; ++i)
        {
          switch(s)
          {
          case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
          case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
            G_GPS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATQZSS:
            G_QZSS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_SATELLITE_ID(id)
            break;
          }
          for(pos = satoffset[s]; pos < satoffset[s]+co->NumberOfSat[s] && co->Sat[pos].ID != id; ++pos)
            ;
          if(pos >= satoffset[s+1]) return GCOBR_DATAMISMATCH;
          else if(pos == co->NumberOfSat[s] + satoffset[s]) ++co->NumberOfSat[s];
          co->Sat[pos].ID = id;

          switch(s)
          {
          case CLOCKORBIT_SATGPS:
          case CLOCKORBIT_SATQZSS:
            G_GPS_IODE(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_IOD(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATGALILEO:
            G_GALILEO_IOD(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATSBAS:
            G_SBAS_T0MOD(co->Sat[pos].toe)
            G_SBAS_IODCRC(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATBDS:
            G_BDS_TOEMOD(co->Sat[pos].toe)
            G_BDS_IODCRC(co->Sat[pos].IOD)
            break;
          }
          G_DELTA_RADIAL(co->Sat[pos].Orbit.DeltaRadial)
          G_DELTA_ALONG_TRACK(co->Sat[pos].Orbit.DeltaAlongTrack)
          G_DELTA_CROSS_TRACK(co->Sat[pos].Orbit.DeltaCrossTrack)
          G_DELTA_DOT_RADIAL(co->Sat[pos].Orbit.DotDeltaRadial)
          G_DELTA_DOT_ALONG_TRACK(co->Sat[pos].Orbit.DotDeltaAlongTrack)
          G_DELTA_DOT_CROSS_TRACK(co->Sat[pos].Orbit.DotDeltaCrossTrack)
#ifdef DEBUG
          fprintf(stderr, "id %2d iod %3d dr %8.3f da %8.3f dc %8.3f dr %8.3f da %8.3f dc %8.3f\n",
          co->Sat[pos].ID,co->Sat[pos].IOD,co->Sat[pos].Orbit.DeltaRadial,
          co->Sat[pos].Orbit.DeltaAlongTrack,co->Sat[pos].Orbit.DeltaCrossTrack,
          co->Sat[pos].Orbit.DotDeltaRadial,
          co->Sat[pos].Orbit.DotDeltaAlongTrack,
          co->Sat[pos].Orbit.DotDeltaCrossTrack);
#endif
        }
        break;
      case COBOFS_CLOCK:
        if(!co) return GCOBR_NOCLOCKORBITPARAMETER;
        co->messageType = type;
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
        case CLOCKORBIT_SATBDS:
          G_GPS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        case CLOCKORBIT_SATGLONASS:
          G_GLONASS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        }
        G_SSR_UPDATE_INTERVAL(co->UpdateInterval)
        G_MULTIPLE_MESSAGE_INDICATOR(mmi)
        G_SSR_IOD(co->SSRIOD)
        G_SSR_PROVIDER_ID(co->SSRProviderID)
        G_SSR_SOLUTION_ID(co->SSRSolutionID)
        G_NO_OF_SATELLITES(nums)
        co->Supplied[COBOFS_CLOCK] |= 1;
#ifdef DEBUG
        fprintf(stderr, "epochtime %d ui %d mmi %d sats %d/%d\n",co->EpochTime[s],
        co->UpdateInterval,mmi,co->NumberOfSat[s],nums);
#endif
        for(i = 0; i < nums; ++i)
        {
          switch(s)
          {
          case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
          case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
            G_GPS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATQZSS:
            G_QZSS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_SATELLITE_ID(id)
            break;
          }
          for(pos = satoffset[s]; pos < satoffset[s]+co->NumberOfSat[s] && co->Sat[pos].ID != id; ++pos)
            ;
          if(pos >= satoffset[s+1]) return GCOBR_DATAMISMATCH;
          else if(pos == co->NumberOfSat[s] + satoffset[s]) ++co->NumberOfSat[s];
          co->Sat[pos].ID = id;

          G_DELTA_CLOCK_C0(co->Sat[pos].Clock.DeltaA0)
          G_DELTA_CLOCK_C1(co->Sat[pos].Clock.DeltaA1)
          G_DELTA_CLOCK_C2(co->Sat[pos].Clock.DeltaA2)
#ifdef DEBUG
          fprintf(stderr, "id %2d c0 %8.3f c1 %8.3f c2 %8.3f\n",
          co->Sat[pos].ID, co->Sat[pos].Clock.DeltaA0, co->Sat[pos].Clock.DeltaA1,
          co->Sat[pos].Clock.DeltaA2);
#endif
        }
        break;
      case COBOFS_COMBINED:
        if(!co) return GCOBR_NOCLOCKORBITPARAMETER;
        co->messageType = type;
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
        case CLOCKORBIT_SATBDS:
          G_GPS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        case CLOCKORBIT_SATGLONASS:
          G_GLONASS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        }
        G_SSR_UPDATE_INTERVAL(co->UpdateInterval)
        G_MULTIPLE_MESSAGE_INDICATOR(mmi)
        G_SATELLITE_REFERENCE_DATUM(co->SatRefDatum)
        G_SSR_IOD(co->SSRIOD)
        G_SSR_PROVIDER_ID(co->SSRProviderID)
        G_SSR_SOLUTION_ID(co->SSRSolutionID)
        G_NO_OF_SATELLITES(nums)
        co->Supplied[COBOFS_ORBIT] |= 1;
        co->Supplied[COBOFS_CLOCK] |= 1;
        for(i = 0; i < nums; ++i)
        {
          switch(s)
          {
          case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
          case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
            G_GPS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATQZSS:
            G_QZSS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_SATELLITE_ID(id)
            break;
          }
          for(pos = satoffset[s]; pos < satoffset[s]+co->NumberOfSat[s] && co->Sat[pos].ID != id; ++pos)
            ;
          if(pos >= satoffset[s+1]) return GCOBR_DATAMISMATCH;
          else if(pos == co->NumberOfSat[s] + satoffset[s]) ++co->NumberOfSat[s];
          co->Sat[pos].ID = id;

          switch(s)
          {
          case CLOCKORBIT_SATGPS:
          case CLOCKORBIT_SATQZSS:
            G_GPS_IODE(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_IOD(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATGALILEO:
            G_GALILEO_IOD(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATSBAS:
            G_SBAS_T0MOD(co->Sat[pos].toe)
            G_SBAS_IODCRC(co->Sat[pos].IOD)
            break;
          case CLOCKORBIT_SATBDS:
            G_BDS_TOEMOD(co->Sat[pos].toe)
            G_BDS_IODCRC(co->Sat[pos].IOD)
            break;
          }
          G_DELTA_RADIAL(co->Sat[pos].Orbit.DeltaRadial)
          G_DELTA_ALONG_TRACK(co->Sat[pos].Orbit.DeltaAlongTrack)
          G_DELTA_CROSS_TRACK(co->Sat[pos].Orbit.DeltaCrossTrack)
          G_DELTA_DOT_RADIAL(co->Sat[pos].Orbit.DotDeltaRadial)
          G_DELTA_DOT_ALONG_TRACK(co->Sat[pos].Orbit.DotDeltaAlongTrack)
          G_DELTA_DOT_CROSS_TRACK(co->Sat[pos].Orbit.DotDeltaCrossTrack)
          G_DELTA_CLOCK_C0(co->Sat[pos].Clock.DeltaA0)
          G_DELTA_CLOCK_C1(co->Sat[pos].Clock.DeltaA1)
          G_DELTA_CLOCK_C2(co->Sat[pos].Clock.DeltaA2)
        }
        break;
      case COBOFS_URA:
        if(!co) return GCOBR_NOCLOCKORBITPARAMETER;
        co->messageType = type;
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
        case CLOCKORBIT_SATBDS:
          G_GPS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        case CLOCKORBIT_SATGLONASS:
          G_GLONASS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        }
        G_SSR_UPDATE_INTERVAL(co->UpdateInterval)
        G_MULTIPLE_MESSAGE_INDICATOR(mmi)
        G_SSR_IOD(co->SSRIOD)
        G_SSR_PROVIDER_ID(co->SSRProviderID)
        G_SSR_SOLUTION_ID(co->SSRSolutionID)
        G_NO_OF_SATELLITES(nums)
        co->Supplied[COBOFS_URA] |= 1;
        for(i = 0; i < nums; ++i)
        {
          switch(s)
          {
          case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
          case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
            G_GPS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATQZSS:
            G_QZSS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_SATELLITE_ID(id)
            break;
          }
          for(pos = satoffset[s]; pos < satoffset[s]+co->NumberOfSat[s] && co->Sat[pos].ID != id; ++pos)
            ;
          if(pos >= satoffset[s+1]) return GCOBR_DATAMISMATCH;
          else if(pos == co->NumberOfSat[s] + satoffset[s]) ++co->NumberOfSat[s];
          co->Sat[pos].ID = id;

          G_SSR_URA(co->Sat[pos].UserRangeAccuracy)
        }
        break;
      case COBOFS_HR:
        if(!co) return GCOBR_NOCLOCKORBITPARAMETER;
        co->messageType = type;
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
        case CLOCKORBIT_SATBDS:
          G_GPS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        case CLOCKORBIT_SATGLONASS:
          G_GLONASS_EPOCH_TIME(co->EpochTime[s], co->NumberOfSat[s])
          break;
        }
        G_SSR_UPDATE_INTERVAL(co->UpdateInterval)
        G_MULTIPLE_MESSAGE_INDICATOR(mmi)
        G_SSR_IOD(co->SSRIOD)
        G_SSR_PROVIDER_ID(co->SSRProviderID)
        G_SSR_SOLUTION_ID(co->SSRSolutionID)
        G_NO_OF_SATELLITES(nums)
        co->Supplied[COBOFS_HR] |= 1;
        for(i = 0; i < nums; ++i)
        {
          switch(s)
          {
          case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
          case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
            G_GPS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATQZSS:
            G_QZSS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_SATELLITE_ID(id)
            break;
          }
          for(pos = satoffset[s]; pos < satoffset[s]+co->NumberOfSat[s] && co->Sat[pos].ID != id; ++pos)
            ;
          if(pos >= satoffset[s+1]) return GCOBR_DATAMISMATCH;
          else if(pos == co->NumberOfSat[s] + satoffset[s]) ++co->NumberOfSat[s];
          co->Sat[pos].ID = id;

          G_HR_CLOCK_CORRECTION(co->Sat[pos].hrclock)
        }
        break;
      case COBOFS_BIAS:
        if(!b) return GCOBR_NOCODEBIASPARAMETER;
        b->messageType = type;
        switch(s)
        {
        case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
        case CLOCKORBIT_SATQZSS: case CLOCKORBIT_SATSBAS:
        case CLOCKORBIT_SATBDS:
          G_GPS_EPOCH_TIME(b->EpochTime[s], b->NumberOfSat[s])
          break;
        case CLOCKORBIT_SATGLONASS:
          G_GLONASS_EPOCH_TIME(b->EpochTime[s], b->NumberOfSat[s])
          break;
        }
        G_SSR_UPDATE_INTERVAL(b->UpdateInterval)
        G_MULTIPLE_MESSAGE_INDICATOR(mmi)
        G_SSR_IOD(b->SSRIOD)
        G_SSR_PROVIDER_ID(b->SSRProviderID)
        G_SSR_SOLUTION_ID(b->SSRSolutionID)
        G_NO_OF_SATELLITES(nums)
        for(i = 0; i < nums; ++i)
        {
          switch(s)
          {
          case CLOCKORBIT_SATGPS: case CLOCKORBIT_SATGALILEO:
          case CLOCKORBIT_SATSBAS: case CLOCKORBIT_SATBDS:
            G_GPS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATQZSS:
            G_QZSS_SATELLITE_ID(id)
            break;
          case CLOCKORBIT_SATGLONASS:
            G_GLONASS_SATELLITE_ID(id)
            break;
          }
          for(pos = satoffset[s]; pos < satoffset[s]+b->NumberOfSat[s] && b->Sat[pos].ID != id; ++pos)
            ;
          if(pos >= satoffset[s+1]) return GCOBR_DATAMISMATCH;
          else if(pos == b->NumberOfSat[s] + satoffset[s]) ++b->NumberOfSat[s];
          b->Sat[pos].ID = id;

          G_NO_OF_CODE_BIASES(b->Sat[pos].NumberOfCodeBiases)
          for(j = 0; j < b->Sat[pos].NumberOfCodeBiases; ++j)
          {
            G_SIGNAL_IDENTIFIER(b->Sat[pos].Biases[j].Type)
            G_CODE_BIAS(b->Sat[pos].Biases[j].Bias)
          }
        }
        break;
      default:
        continue;
      }
#ifdef COR_LATENCY
      if(s == CLOCKORBIT_SATGPS && type-corbase[s] != COBOFS_BIAS)
      {
        co->epochGPS[co->epochSize] = co->EpochTime[s];
        if(co->epochSize < COR_LATENCYCOUNT)
          ++co->epochSize;
      }
#endif
#ifdef DEBUG
      for(type = 0; type < (int)size && (unsigned char)buffer[type] != 0xD3; ++type)
        numbits += 8;
      fprintf(stderr, "numbits left %d\n",numbits);
#endif
      return mmi ? GCOBR_MESSAGEFOLLOWS : GCOBR_OK;
    }
  }
  return GCOBR_UNKNOWNTYPE;
}
#endif /* NODECODE */
