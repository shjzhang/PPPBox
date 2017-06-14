/**
* @file satObs.cpp
* Class to realize the functions of Correction data structure.
*/
#include <iostream>
#include <iomanip>
#include <sstream>

#include "GNSSconstants.hpp"
#include "RinexSatID.hpp"
#include "CivilTime.hpp"
#include "satObs.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;
// Constructor
////////////////////////////////////////////////////////////////////////////
t_clkCorr::t_clkCorr()
{
    _updateInt  = 0;
    _iod        = 0;
    _dClk       = 0.0;
    _dotDClk    = 0.0;
    _dotDotDClk = 0.0;
}

//
////////////////////////////////////////////////////////////////////////////
void t_clkCorr::writeEpoch(ostream* out, const list<t_clkCorr>& corrList)
{
    if (!out || corrList.size() == 0)
    {
      return;
    }
    out->setf(ios::fixed);
    CommonTime epoTime;
    list<t_clkCorr>::const_iterator it;
	cout << "Write the clkCorr for , " << (corrList.begin())->_staID << endl;
    for(it=corrList.begin();it!=corrList.end();++it)
    {
        const t_clkCorr& corr = *it;
        if (!(epoTime.getDays()!=0.0 || epoTime.getSecondOfDay()!=0.0))
        {
          epoTime = corr._time;
          string timestr = CivilTime(epoTime).printf("%Y %02m %02d %02H %02M %02S");
          *out << "> CLOCK " << timestr << " " << corr._updateInt <<  " "
               << corrList.size() << ' ' << corr._staID << endl;
        }
        *out << RinexSatID(corr._prn) << ' ' << setw(11) << corr._iod << ' '
             << setw(10) << setprecision(4) << corr._dClk * gpstk::C_MPS << ' '
             << setw(10) << setprecision(4) << corr._dotDClk  * gpstk::C_MPS << ' '
             << setw(10) << setprecision(4) << corr._dotDotDClk  * gpstk::C_MPS<< endl;
    }
    out->flush();
}

//
////////////////////////////////////////////////////////////////////////////
void t_clkCorr::readEpoch(const string& epoLine, istream& inStream, list<t_clkCorr>& corrList)
{
  CommonTime   epoTime;
  unsigned int updateInt;
  int          numCorr;
  string       staID;
  if (t_corrSSR::readEpoLine(epoLine, epoTime, updateInt, numCorr, staID) != t_corrSSR::clkCorr)
  {
      return;
  }
  for (int ii = 0; ii < numCorr; ii++)
  {
      t_clkCorr corr;
      string satsys;
      corr._time      = epoTime;
      corr._updateInt = updateInt;
      corr._staID     = staID;

      string line;
      getline(inStream, line);
      istringstream in(line.c_str());

      RinexSatID rs;
      in >> satsys;
      satsys = satsys.substr(0,1) +" "+ satsys.substr(1);
      rs.fromString(satsys);
      corr._prn = SatID(rs.id,rs.system);
      switch(rs.system)
      {
          case SatID::systemGPS:     corr._time.setTimeSystem(TimeSystem::GPS);
      }

      in >> corr._iod >> corr._dClk >> corr._dotDClk >> corr._dotDotDClk;
      /*if (corr._prn.system() == 'E') {
        corr._prn.setFlags(1);// I/NAV
      }*/
      corr._dClk       /= gpstk::C_MPS;
      corr._dotDClk    /= gpstk::C_MPS;
      corr._dotDotDClk /= gpstk::C_MPS;

      if(rs.system == SatID::systemGPS)
      {
        corrList.push_back(corr);
      }

  }
}

// Constructor
////////////////////////////////////////////////////////////////////////////
t_orbCorr::t_orbCorr()
{
    _updateInt = 0;
    _iod       = 0;
    _system    = 'G';
    _xr = Triple(0.0,0.0,0.0);
    _dotXr = Triple(0.0,0.0,0.0);
}

//
////////////////////////////////////////////////////////////////////////////
void t_orbCorr::writeEpoch(ostream* out, const list<t_orbCorr>& corrList)
{
    if (!out || corrList.size() == 0)
    {
        return;
    }
    out->setf(ios::fixed);
    CommonTime epoTime;
    list<t_orbCorr>::const_iterator it;
    cout << "Write the orbCorr for , " << (corrList.begin())->_staID << endl;
	for(it=corrList.begin();it!=corrList.end();++it)
    {
        const t_orbCorr& corr = *it;
        if (!(epoTime.getDays()!=0.0 || epoTime.getSecondOfDay()!=0.0))
        {
          epoTime = corr._time;
          string timestr = CivilTime(epoTime).printf("%Y %02m %02d %02H %02M %02S");
          *out << "> ORBIT " << timestr << " " << corr._updateInt <<  " "
               << corrList.size() << ' ' << corr._staID << endl;
        }
        *out << RinexSatID(corr._prn)  << ' ' << setw(11) << corr._iod << ' '
             << setw(12) << setprecision(6) << corr._xr[0]     << ' '
             << setw(12) << setprecision(6) << corr._xr[1]     << ' '
             << setw(12) << setprecision(6) << corr._xr[2]     << "    "
             << setw(12) << setprecision(6) << corr._dotXr[0]  << ' '
             << setw(12) << setprecision(6) << corr._dotXr[1]  << ' '
             << setw(12) << setprecision(6) << corr._dotXr[2]  << endl;
    }
    out->flush();
}

//
////////////////////////////////////////////////////////////////////////////
void t_orbCorr::readEpoch(const string& epoLine, istream& inStream, list<t_orbCorr>& corrList)
{
    CommonTime   epoTime;
    unsigned int updateInt;
    int          numCorr;
    string       staID;
    if (t_corrSSR::readEpoLine(epoLine, epoTime, updateInt, numCorr, staID) != t_corrSSR::orbCorr)
    {
        return;
    }
    for (int ii = 0; ii < numCorr; ii++)
    {
        t_orbCorr corr;
        string satsys;
        corr._time      = epoTime;
        corr._updateInt = updateInt;
        corr._staID     = staID;

        string line;
        getline(inStream, line);
        istringstream in(line.c_str());

        in >> satsys;
        satsys = satsys.substr(0,1) +" "+ satsys.substr(1);
        RinexSatID rs;
        rs.fromString(satsys);
        corr._prn = SatID(rs.id,rs.system);

        switch(rs.system)
        {
            case SatID::systemGPS:     corr._time.setTimeSystem(TimeSystem::GPS);
        }

        in >> corr._iod
           >> corr._xr[0]    >> corr._xr[1]    >> corr._xr[2]
           >> corr._dotXr[0] >> corr._dotXr[1] >> corr._dotXr[2];

        //if (corr._prn.system() == 'E') {
        //  corr._prn.setFlags(1);// I/NAV
        //}
        if(rs.system == SatID::systemGPS)
        {
            corrList.push_back(corr);
        }
    }
}

//
////////////////////////////////////////////////////////////////////////////
void t_satCodeBias::readEpoch(const string& epoLine, istream& inStream, list<t_satCodeBias>& biasList)
{
    CommonTime   epoTime;
    unsigned int updateInt;
    int          numCorr;
    string       staID;
    if (t_corrSSR::readEpoLine(epoLine, epoTime, updateInt, numCorr, staID) != t_corrSSR::codeBias)
    {
        return;
    }
    for (int ii = 0; ii < numCorr; ii++)
    {
        string line;
        getline(inStream, line);
    }
}

void t_satPhaseBias::readEpoch(const string& epoLine, istream& inStream, list<t_satPhaseBias>& biasList)
{
    CommonTime   epoTime;
    unsigned int updateInt;
    int          numSat;
    string       staID;
    unsigned int dispInd;
    unsigned int mwInd;
    if (t_corrSSR::readEpoLine(epoLine, epoTime, updateInt, numSat, staID) != t_corrSSR::phaseBias)
    {
        return;
    }
    for (int ii = 0; ii <= numSat; ii++)
    {
      t_satPhaseBias satPhaseBias;
      satPhaseBias._time      = epoTime;
      satPhaseBias._updateInt = updateInt;
      satPhaseBias._staID     = staID;

      string line;
      getline(inStream, line);
      istringstream in(line.c_str());
      string satsys;

      if (ii == 0)
      {
        in >> dispInd >> mwInd;
        continue;
      }
      satPhaseBias._dispBiasConstistInd = dispInd;
      satPhaseBias._MWConsistInd = mwInd;

      int numBias;

      in >> satsys;
      satsys = satsys.substr(0,1) +" "+ satsys.substr(1);
      RinexSatID rs;
      rs.fromString(satsys);
      satPhaseBias._prn = SatID(rs.id,rs.system);

      in >> satPhaseBias._yawDeg >> satPhaseBias._yawDegRate
        >> numBias;

      while (in.good())
      {
        t_frqPhaseBias frqPhaseBias;
        in >> frqPhaseBias._rnxType2ch >> frqPhaseBias._value
           >> frqPhaseBias._fixIndicator >> frqPhaseBias._fixWideLaneIndicator
           >> frqPhaseBias._jumpCounter;
        if (!frqPhaseBias._rnxType2ch.empty())
        {
          satPhaseBias._bias.push_back(frqPhaseBias);
        }
      }

      biasList.push_back(satPhaseBias);
    }
}

void t_vTec::read(const std::string& epoLine, std::istream& inStream, t_vTec& vTec)
{
    CommonTime   epoTime;
    unsigned int updateInt;
    int          numLayers;
    string       staID;
    if (t_corrSSR::readEpoLine(epoLine, epoTime, updateInt, numLayers, staID) != t_corrSSR::vTec)
    {
        return;
    }
    if (numLayers <= 0)
    {
        return;
    }
    vTec._time      = epoTime;
    vTec._updateInt = updateInt;
    vTec._staID     = staID;
    for (int ii = 0; ii < numLayers; ii++)
    {
      t_vTecLayer layer;

      std::string line;
      getline(inStream, line);
      istringstream in(line.c_str());

      int dummy, maxDeg, maxOrd;
      in >> dummy >> maxDeg >> maxOrd >> layer._height;

      layer._C.resize(maxDeg+1, maxOrd+1);
      layer._S.resize(maxDeg+1, maxOrd+1);

      for (int iDeg = 0; iDeg <= maxDeg; iDeg++)
      {
        for (int iOrd = 0; iOrd <= maxOrd; iOrd++)
        {
          inStream >> layer._C[iDeg][iOrd];
        }
      }
      for (int iDeg = 0; iDeg <= maxDeg; iDeg++)
      {
        for (int iOrd = 0; iOrd <= maxOrd; iOrd++)
        {
          inStream >> layer._S[iDeg][iOrd];
        }
      }

      vTec._layers.push_back(layer);
    }
}

//
////////////////////////////////////////////////////////////////////////////
t_corrSSR::e_type t_corrSSR::readEpoLine(const string& line, CommonTime& epoTime,
                                         unsigned int& updateInt, int& numEntries,
                                         string& staID)
{
    istringstream inLine(line.c_str());

    char   epoChar;
    string typeString;
    int    year, month, day, hour, min;
    double sec;

    inLine >> epoChar >> typeString
           >> year >> month >> day >> hour >> min >> sec >> updateInt >> numEntries >> staID;

    if (epoChar == '>')
    {
        CivilTime cvTime(year, month, day, hour, min, sec);
        epoTime = cvTime.convertToCommonTime();
        if      (typeString == "CLOCK") {
          return clkCorr;
        }
        else if (typeString == "ORBIT") {
          return orbCorr;
        }
        else if (typeString == "CODE_BIAS") {
          return codeBias;
        }
        else if (typeString == "PHASE_BIAS") {
          return phaseBias;
        }
        else if (typeString == "VTEC") {
          return vTec;
        }
    }

    return unknown;
}
