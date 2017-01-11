/**
* @file satObs.cpp
* Class to realize the functions of Correction data structure.
*/
#include <iostream>
#include <iomanip>
#include <sstream>

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
             << setw(10) << setprecision(4) << corr._dClk     << ' '
             << setw(10) << setprecision(4) << corr._dotDClk  << ' '
             << setw(10) << setprecision(4) << corr._dotDotDClk << endl;
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
      int satnum;
      corr._time      = epoTime;
      corr._updateInt = updateInt;
      corr._staID     = staID;

      string line;
      getline(inStream, line);
      istringstream in(line.c_str());

      RinexSatID rs;
      rs.fromString(satsys);
      corr._prn = SatID(rs.id,rs.system);

      in >> corr._iod >> corr._dClk >> corr._dotDClk >> corr._dotDotDClk;
      /*if (corr._prn.system() == 'E') {
        corr._prn.setFlags(1);// I/NAV
      }*/
      //corr._dClk       /= t_CST::c;
      //corr._dotDClk    /= t_CST::c;
      //corr._dotDotDClk /= t_CST::c;

      corrList.push_back(corr);

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
             << setw(10) << setprecision(4) << corr._xr[0]     << ' '
             << setw(10) << setprecision(4) << corr._xr[1]     << ' '
             << setw(10) << setprecision(4) << corr._xr[2]     << "    "
             << setw(10) << setprecision(4) << corr._dotXr[0]  << ' '
             << setw(10) << setprecision(4) << corr._dotXr[1]  << ' '
             << setw(10) << setprecision(4) << corr._dotXr[2]  << endl;
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
        int satnum;
        corr._time      = epoTime;
        corr._updateInt = updateInt;
        corr._staID     = staID;

        string line;
        getline(inStream, line);
        istringstream in(line.c_str());

        in >> satsys;
        RinexSatID rs;
        rs.fromString(satsys);
        corr._prn = SatID(rs.id,rs.system);

        in >> corr._iod
           >> corr._xr[0]    >> corr._xr[1]    >> corr._xr[2]
           >> corr._dotXr[0] >> corr._dotXr[1] >> corr._dotXr[2];

        //if (corr._prn.system() == 'E') {
        //  corr._prn.setFlags(1);// I/NAV
        //}
        corrList.push_back(corr);
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
