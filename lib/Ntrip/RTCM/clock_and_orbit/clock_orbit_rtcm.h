#ifndef RTCM3_CLOCK_ORBIT_RTCM_H
#define RTCM3_CLOCK_ORBIT_RTCM_H

/* Programheader

        Name:           clock_orbit_rtcm.h
        Project:        RTCM3
        Version:        $Id: clock_orbit_rtcm.h 7902 2016-05-12 13:57:11Z stuerze $
        Authors:        Dirk Stöcker
        Description:    state space approach for RTCM3
*/

#include <string.h>

enum SatelliteReferenceDatum { DATUM_ITRF=0, DATUM_LOCAL=1 };

enum COR_BASE {
  COBBASE_GPS = 1057,
  COBBASE_GLONASS = 1063,
  COBBASE_GALILEO = 1240,
  COBBASE_QZSS = 1246,
  COBBASE_SBAS = 1252,
  COBBASE_BDS = 1258,
};

enum COR_OFFSET {
  COBOFS_ORBIT = 0,
  COBOFS_CLOCK,
  COBOFS_BIAS,
  COBOFS_COMBINED,
  COBOFS_URA,
  COBOFS_HR,
  COBOFS_NUM
};

enum ClockOrbitType {
  COTYPE_GPSORBIT = COBBASE_GPS + COBOFS_ORBIT,
  COTYPE_GPSCLOCK,
  COTYPE_GPSCOMBINED = COBBASE_GPS + COBOFS_COMBINED,
  COTYPE_GPSURA,
  COTYPE_GPSHR,

  COTYPE_GLONASSORBIT = COBBASE_GLONASS + COBOFS_ORBIT,
  COTYPE_GLONASSCLOCK,
  COTYPE_GLONASSCOMBINED = COBBASE_GLONASS + COBOFS_COMBINED,
  COTYPE_GLONASSURA,
  COTYPE_GLONASSHR,

  COTYPE_GALILEOORBIT = COBBASE_GALILEO + COBOFS_ORBIT,
  COTYPE_GALILEOCLOCK,
  COTYPE_GALILEOCOMBINED = COBBASE_GALILEO + COBOFS_COMBINED,
  COTYPE_GALILEOURA,
  COTYPE_GALILEOHR,

  COTYPE_QZSSORBIT = COBBASE_QZSS + COBOFS_ORBIT,
  COTYPE_QZSSCLOCK,
  COTYPE_QZSSCOMBINED = COBBASE_QZSS + COBOFS_COMBINED,
  COTYPE_QZSSURA,
  COTYPE_QZSSHR,

  COTYPE_SBASORBIT = COBBASE_SBAS + COBOFS_ORBIT,
  COTYPE_SBASCLOCK,
  COTYPE_SBASCOMBINED = COBBASE_SBAS + COBOFS_COMBINED,
  COTYPE_SBASURA,
  COTYPE_SBASHR,

  COTYPE_BDSORBIT = COBBASE_BDS + COBOFS_ORBIT,
  COTYPE_BDSCLOCK,
  COTYPE_BDSCOMBINED = COBBASE_BDS + COBOFS_COMBINED,
  COTYPE_BDSURA,
  COTYPE_BDSHR,

  COTYPE_AUTO = 0,
};

enum CodeBiasType {
  BTYPE_GPS = COBBASE_GPS + COBOFS_BIAS,
  BTYPE_GLONASS = COBBASE_GLONASS + COBOFS_BIAS,
  BTYPE_GALILEO = COBBASE_GALILEO + COBOFS_BIAS,
  BTYPE_QZSS = COBBASE_QZSS + COBOFS_BIAS,
  BTYPE_SBAS = COBBASE_SBAS + COBOFS_BIAS,
  BTYPE_BDS = COBBASE_BDS + COBOFS_BIAS,
  BTYPE_AUTO = 0
};

enum PhaseBiasType {
  PBTYPE_BASE = 1265,
  PBTYPE_GPS = PBTYPE_BASE,
  PBTYPE_GLONASS,
  PBTYPE_GALILEO,
  PBTYPE_QZSS,
  PBTYPE_SBAS,
  PBTYPE_BDS,
  PBTYPE_AUTO = 0
};

enum VTECType {
  VTEC_BASE = 1264
};

/* if some systems aren't supported at all, change the following numbers to zero
for these systems to save space */
enum COR_CONSTANTS {
  CLOCKORBIT_BUFFERSIZE=2048,
  CLOCKORBIT_NUMGPS=32,
  CLOCKORBIT_NUMGLONASS=26,
  CLOCKORBIT_NUMGALILEO=36,
  CLOCKORBIT_NUMQZSS=10,
  CLOCKORBIT_NUMSBAS=38,
  CLOCKORBIT_NUMBDS=37,
  CLOCKORBIT_NUMBIAS=17,
  CLOCKORBIT_NUMIONOLAYERS=4,
  CLOCKORBIT_MAXIONOORDER=16,
  CLOCKORBIT_MAXIONODEGREE=16
};

enum COR_SATSYSTEM {
  CLOCKORBIT_SATGPS=0,
  CLOCKORBIT_SATGLONASS,
  CLOCKORBIT_SATGALILEO,
  CLOCKORBIT_SATQZSS,
  CLOCKORBIT_SATSBAS,
  CLOCKORBIT_SATBDS,
  CLOCKORBIT_SATNUM
};

enum COR_OFFSETS {
  CLOCKORBIT_OFFSETGPS=0,
  CLOCKORBIT_OFFSETGLONASS=CLOCKORBIT_NUMGPS,
  CLOCKORBIT_OFFSETGALILEO=CLOCKORBIT_NUMGPS+CLOCKORBIT_NUMGLONASS,
  CLOCKORBIT_OFFSETQZSS=CLOCKORBIT_NUMGPS+CLOCKORBIT_NUMGLONASS
  +CLOCKORBIT_NUMGALILEO,
  CLOCKORBIT_OFFSETSBAS=CLOCKORBIT_NUMGPS+CLOCKORBIT_NUMGLONASS
  +CLOCKORBIT_NUMGALILEO+CLOCKORBIT_NUMQZSS,
  CLOCKORBIT_OFFSETBDS=CLOCKORBIT_NUMGPS+CLOCKORBIT_NUMGLONASS
  +CLOCKORBIT_NUMGALILEO+CLOCKORBIT_NUMQZSS+CLOCKORBIT_NUMSBAS,
  CLOCKORBIT_COUNTSAT=CLOCKORBIT_NUMGPS+CLOCKORBIT_NUMGLONASS
  +CLOCKORBIT_NUMGALILEO+CLOCKORBIT_NUMQZSS+CLOCKORBIT_NUMSBAS
  +CLOCKORBIT_NUMBDS
};

enum CodeType {
  CODETYPEGPS_L1_CA          = 0,
  CODETYPEGPS_L1_P           = 1,
  CODETYPEGPS_L1_Z           = 2,
  /*CODETYPEGPS_L1_Y         = 3,
  CODETYPEGPS_L1_M           = 4,*/
  CODETYPEGPS_L2_CA          = 5,
  CODETYPEGPS_SEMI_CODELESS  = 6,
  CODETYPEGPS_L2_CM          = 7,
  CODETYPEGPS_L2_CL          = 8,
  CODETYPEGPS_L2_CML         = 9,
  CODETYPEGPS_L2_P           = 10,
  CODETYPEGPS_L2_Z           = 11,
  /*CODETYPEGPS_L2_Y         = 12,
  CODETYPEGPS_L2_M           = 13,*/
  CODETYPEGPS_L5_I           = 14,
  CODETYPEGPS_L5_Q           = 15,
  CODETYPEGPS_L5_IQ          = 16,

  CODETYPEGLONASS_L1_CA      = 0,
  CODETYPEGLONASS_L1_P       = 1,
  CODETYPEGLONASS_L2_CA      = 2,
  CODETYPEGLONASS_L2_P       = 3,

  CODETYPEGALILEO_E1_A       = 0,
  CODETYPEGALILEO_E1_B       = 1,
  CODETYPEGALILEO_E1_C       = 2,
  CODETYPEGALILEO_E5A_I      = 5,
  CODETYPEGALILEO_E5A_Q      = 6,
  CODETYPEGALILEO_E5B_I      = 8,
  CODETYPEGALILEO_E5B_Q      = 9,
  CODETYPEGALILEO_E5_I       = 11,
  CODETYPEGALILEO_E5_Q       = 12,
  CODETYPEGALILEO_E6_A       = 14,
  CODETYPEGALILEO_E6_B       = 15,
  CODETYPEGALILEO_E6_C       = 16,

  CODETYPEQZSS_L1_CA         = 0,
  CODETYPEQZSS_L1C_D         = 1,
  CODETYPEQZSS_L1C_P         = 2,
  CODETYPEQZSS_L2_CM         = 3,
  CODETYPEQZSS_L2_CL         = 4,
  CODETYPEQZSS_L2_CML        = 5,
  CODETYPEQZSS_L5_I          = 6,
  CODETYPEQZSS_L5_Q          = 7,
  CODETYPEQZSS_L5_IQ         = 8,
  CODETYPEQZSS_LEX_S         = 9,
  CODETYPEQZSS_LEX_L         = 10,
  CODETYPEQZSS_LEX_SL        = 11,
  CODETYPEQZSS_L1C_DP        = 12,

  CODETYPE_SBAS_L1_CA        = 0,
  CODETYPE_SBAS_L5_I         = 1,
  CODETYPE_SBAS_L5_Q         = 2,
  CODETYPE_SBAS_L5_IQ        = 3,

  CODETYPE_BDS_B1_I          = 0,
  CODETYPE_BDS_B1_Q          = 1,
  CODETYPE_BDS_B1_IQ         = 2,
  CODETYPE_BDS_B3_I          = 3,
  CODETYPE_BDS_B3_Q          = 4,
  CODETYPE_BDS_B3_IQ         = 5,
  CODETYPE_BDS_B2_I          = 6,
  CODETYPE_BDS_B2_Q          = 7,
  CODETYPE_BDS_B2_IQ         = 8,
};

#define SSR_MAXURA 5.5 /* > 5466.5mm in meter */

/* satellite system data is stored with offset CLOCKORBIT_OFFSET...
in the data structures. So first GLONASS satellite is at
xxx->Sat[CLOCKORBIT_OFFSETGLONASS], first GPS satellite is
xxx->Sat[CLOCKORBIT_OFFSETGPS]. */

#ifdef COR_LEGACY
/* old names */
#define NumberOfGPSSat     NumberOfSat[CLOCKORBIT_SATGPS]
#define NumberOfGLONASSSat NumberOfSat[CLOCKORBIT_SATGLONASS]
#define GPSEpochTime       EpochTime[CLOCKORBIT_SATGPS] /* 0 .. 604799 s */
#define GLONASSEpochTime   EpochTime[CLOCKORBIT_SATGLONASS] /* 0 .. 86399 s (86400 for leap second) */
#define ClockDataSupplied  Supplied[COBOFS_CLOCK]
#define HRDataSupplied     Supplied[COBOFS_HR]
#define OrbitDataSupplied  Supplied[COBOFS_ORBIT]
#define URADataSupplied    Supplied[COBOFS_URA]
#define GetClockOrbitBias(a,b,c,d,e) GetSSR(a,b,0,0,c,d,e)
#endif /* COR_LEGACY */

/* latency check code, disabled by default */
#ifdef COR_LATENCY
#define COR_LATENCYCOUNT 100
#endif

struct ClockOrbit
{
  enum ClockOrbitType messageType;
  unsigned int EpochTime[CLOCKORBIT_SATNUM];   /* 0 .. system specific maximum */
  unsigned int NumberOfSat[CLOCKORBIT_SATNUM]; /* 0 .. CLOCKORBIT_NUM... */

  unsigned int Supplied[COBOFS_NUM];           /* boolean */
#ifdef COR_LATENCY
  unsigned int epochGPS[COR_LATENCYCOUNT+1];   /* Weber, for latency */
  unsigned int epochSize;                      /* Weber, for latency */
#endif
  unsigned int SSRIOD;
  unsigned int SSRProviderID;
  unsigned int SSRSolutionID;
  unsigned int UpdateInterval;
  enum SatelliteReferenceDatum SatRefDatum;
  struct SatData {
    unsigned int ID; /* all */
    unsigned int IOD; /* all */
    unsigned int toe; /* SBAS, BDS */
    double UserRangeAccuracy; /* accuracy values in [m] */
    double hrclock;
    struct OrbitPart
    {
      double DeltaRadial;           /* m */
      double DeltaAlongTrack;       /* m */
      double DeltaCrossTrack;       /* m */
      double DotDeltaRadial;        /* m/s */
      double DotDeltaAlongTrack;    /* m/s */
      double DotDeltaCrossTrack;    /* m/s */
    } Orbit;
    struct ClockPart
    {
      double DeltaA0;               /* m */
      double DeltaA1;               /* m/s */
      double DeltaA2;               /* m/ss */
    } Clock;
  } Sat[CLOCKORBIT_COUNTSAT];
};

struct CodeBias
{
  enum CodeBiasType messageType;
  unsigned int EpochTime[CLOCKORBIT_SATNUM];   /* 0 .. system specific maximum */
  unsigned int NumberOfSat[CLOCKORBIT_SATNUM]; /* 0 .. CLOCKORBIT_NUM... */
  unsigned int UpdateInterval;
  unsigned int SSRIOD;
  unsigned int SSRProviderID;
  unsigned int SSRSolutionID;
  struct BiasSat
  {
    unsigned int ID; /* all */
    unsigned int NumberOfCodeBiases;
    struct CodeBiasEntry
    {
      enum CodeType Type;
      float         Bias;           /* m */
    } Biases[CLOCKORBIT_NUMBIAS];
  } Sat[CLOCKORBIT_COUNTSAT];
};

struct PhaseBias
{
  enum PhaseBiasType messageType;
  unsigned int EpochTime[CLOCKORBIT_SATNUM];   /* 0 .. system specific maximum */
  unsigned int NumberOfSat[CLOCKORBIT_SATNUM]; /* 0 .. CLOCKORBIT_NUM... */
  unsigned int UpdateInterval;
  unsigned int SSRIOD;
  unsigned int SSRProviderID;
  unsigned int SSRSolutionID;
  unsigned int DispersiveBiasConsistencyIndicator;
  unsigned int MWConsistencyIndicator;
  struct PhaseBiasSat
  {
    unsigned int ID; /* all */
    unsigned int NumberOfPhaseBiases;
    double YawAngle; /* radiant */
    double YawRate;  /* radiant/s */
    struct PhaseBiasEntry
    {
      enum CodeType Type;
      unsigned int  SignalIntegerIndicator;
      unsigned int  SignalsWideLaneIntegerIndicator;
      unsigned int  SignalDiscontinuityCounter;
      float         Bias;           /* m */
    } Biases[CLOCKORBIT_NUMBIAS];
  } Sat[CLOCKORBIT_COUNTSAT];
};

struct VTEC
{
  unsigned int EpochTime; /* GPS */
  unsigned int UpdateInterval;
  unsigned int SSRIOD;
  unsigned int SSRProviderID;
  unsigned int SSRSolutionID;
  unsigned int NumLayers; /* 1-4 */
  double Quality;
  struct IonoLayers {
    double       Height; /* m */
    unsigned int Degree; /* 1-16 */
    unsigned int Order; /* 1-16 */
    double       Sinus[CLOCKORBIT_MAXIONODEGREE][CLOCKORBIT_MAXIONOORDER];
    double       Cosinus[CLOCKORBIT_MAXIONODEGREE][CLOCKORBIT_MAXIONOORDER];
  } Layers[CLOCKORBIT_NUMIONOLAYERS];
};

/* return size of resulting data or 0 in case of an error */
size_t MakeClockOrbit(const struct ClockOrbit *co, enum ClockOrbitType type,
       int moremessagesfollow, char *buffer, size_t size);
size_t MakeCodeBias(const struct CodeBias *b, enum CodeBiasType type,
       int moremessagesfollow, char *buffer, size_t size);
size_t MakePhaseBias(const struct PhaseBias *b, enum PhaseBiasType type,
       int moremessagesfollow, char *buffer, size_t size);
size_t MakeVTEC(const struct VTEC *b, int moremessagesfollow, char *buffer,
       size_t size);

enum GCOB_RETURN {
  /* all well */
  GCOBR_MESSAGEFOLLOWS = 1,
  GCOBR_OK = 0,
  /* unknown data, a warning */
  GCOBR_UNKNOWNTYPE = -1,
  GCOBR_UNKNOWNDATA = -2,
  GCOBR_CRCMISMATCH = -3,
  GCOBR_SHORTMESSAGE = -4,
  /* failed to do the work */
  GCOBR_NOCLOCKORBITPARAMETER = -10,
  GCOBR_NOCODEBIASPARAMETER = -11,
  GCOBR_NOPHASEBIASPARAMETER = -12,
  GCOBR_NOVTECPARAMETER = -13,
  /* data mismatch - data in storage does not match new data */
  GCOBR_TIMEMISMATCH = -20,
  GCOBR_DATAMISMATCH = -21,
  /* not enough data - can decode the block completely */
  GCOBR_SHORTBUFFER = -30,
  GCOBR_MESSAGEEXCEEDSBUFFER = -31};

/* NOTE: When an error message has been emitted, the output structures may have been modified. Make a copy of the previous variant before calling the
function to have a clean state. */

/* buffer should point to a RTCM3 block */
enum GCOB_RETURN GetSSR(struct ClockOrbit *co, struct CodeBias *b, struct VTEC *v,
       struct PhaseBias *pb, const char *buffer, size_t size, int *bytesused);

#endif /* RTCM3_CLOCK_ORBIT_RTCM_H */
