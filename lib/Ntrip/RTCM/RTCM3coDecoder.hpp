// Part of BNC, a utility for retrieving decoding and
// converting GNSS data streams from NTRIP broadcasters.
//
// Copyright (C) 2007
// German Federal Agency for Cartography and Geodesy (BKG)
// http://www.bkg.bund.de
// Czech Technical University Prague, Department of Geodesy
// http://www.fsv.cvut.cz
//
// Email: euref-ip@bkg.bund.de
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation, version 2.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef RTCM3CODECODER_HPP
#define RTCM3CODECODER_HPP

#include <fstream>
#include <string>
#include <list>
#include <map>

#include "CommonTime.hpp"
#include "RTCMDecoder.hpp"

using namespace gpstk;

extern "C" {
# include "clock_orbit_rtcm.h"
}

class RTCM3coDecoder: public RTCMDecoder
{
public:
    RTCM3coDecoder(const std::string& staID);
    virtual ~RTCM3coDecoder();
    virtual bool decode(unsigned char* buff, int len);

private:
   void reset();
   void setEpochTime();
   void sendResults();
   void reopen();
   void checkProviderID();
   std::string codeTypeToRnxType(char system, CodeType type) const;

   std::ofstream*      _out;
   std::string         _staID;
   std::string         _fileNameSkl;
   std::string         _fileName;
   bool                _writeCorrFile;
   //QByteArray                    _buffer;
   ClockOrbit          _clkOrb;
   CodeBias            _codeBias;
   PhaseBias           _phaseBias;
   VTEC                _vTEC;
   int                 _providerID[3];
   CommonTime          _lastTime;
   map<SatID, unsigned int>                    _IODs;
   map<CommonTime, std::list<t_orbCorr> >      _orbCorrections;
   map<CommonTime, std::list<t_clkCorr> >      _clkCorrections;
   map<SatID, t_clkCorr>                       _lastClkCorrections;
   map<CommonTime, std::list<t_satCodeBias> >  _codeBiases;
   map<CommonTime, std::list<t_satPhaseBias> > _phaseBiases;
   map<CommonTime, t_vTec>                     _vTecMap;
};


#endif // RTCM3CODECODER_HPP
