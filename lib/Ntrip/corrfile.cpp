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

/* -------------------------------------------------------------------------
 * BKG NTRIP Client
 * -------------------------------------------------------------------------
 *
 * Class:      t_corrFile
 *
 * Purpose:    Reads DGPS Correction File
 *
 * Author:     L. Mervart
 *
 * Created:    12-Feb-2012
 *
 * Changes:    
 *
 * -----------------------------------------------------------------------*/

#include <iostream>
#include "corrfile.h"
#include "StringUtils.hpp"
#include "RealTimeEphStore.hpp"

#include "NavCorrToSP3.hpp"

// Constructor
////////////////////////////////////////////////////////////////////////////
t_corrFile::t_corrFile(string fileName) {
    m_stream.open(fileName.c_str());
}

// Destructor
////////////////////////////////////////////////////////////////////////////
t_corrFile::~t_corrFile()
{
  m_stream.close();
}

// Read till a given time
////////////////////////////////////////////////////////////////////////////
void t_corrFile::syncRead(CommonTime& corrTime)
{
  if(!m_stream.eof())
  {
  }
  else
  {
      std::cout << "Not good!" << std::endl;
  }
  m_lastEpoTime.setTimeSystem(corrTime.getTimeSystem());
  while (!m_stream.eof() && (!m_lastEpoTime.valid() || m_lastEpoTime <= corrTime))
  {
    if (m_sLastLine.empty()) 
    {
      getline(m_stream, m_sLastLine); 
      StringUtils::stripWhiteSpace(m_sLastLine);
      if      (m_stream.eof())
      {
        throw "t_corrFile: end of file\n";
      }
      else if (m_sLastLine.empty() || m_sLastLine[0] == '!') 
      {
        continue;
      }
      else if (m_sLastLine[0] != '>') 
      {
        throw "t_corrFile: error\n";
      }
    }

    int          numEntries;
    unsigned int updateInt;
    string       staID;
    t_corrSSR::e_type corrType = t_corrSSR::readEpoLine(m_sLastLine, m_lastEpoTime, updateInt, numEntries, staID);
    m_lastEpoTime.setTimeSystem(corrTime.getTimeSystem());
    if      (corrType == t_corrSSR::unknown)
    {
      throw "t_corrFile: unknown line " + m_sLastLine;
    }
    else if(m_lastEpoTime > corrTime)
    {
      break;
    }
    else if (corrType == t_corrSSR::clkCorr)
    {
      list<t_clkCorr> clkCorrList;
      t_clkCorr::readEpoch(m_sLastLine, m_stream, clkCorrList);
      navCorrToSp3->newClkCorrections(clkCorrList);
    }
    else if (corrType == t_corrSSR::orbCorr)
    {
      list<t_orbCorr> orbCorrList;
      t_orbCorr::readEpoch(m_sLastLine, m_stream, orbCorrList);
      list<t_orbCorr>::iterator it = orbCorrList.begin();
      while (it != orbCorrList.end())
      {
        const t_orbCorr& corr = *it;
        m_corrIODs[corr._prn] = corr._iod;
        ++it;
      }
      navCorrToSp3->newOrbCorrections(orbCorrList);
    }

    else if (corrType == t_corrSSR::codeBias)
    {
      list<t_satCodeBias> satCodeBiasList;
      t_satCodeBias::readEpoch(m_sLastLine, m_stream, satCodeBiasList);
      navCorrToSp3->newCodeBiases(satCodeBiasList);
    }
    else if (corrType == t_corrSSR::phaseBias) 
    {
      list<t_satPhaseBias> satPhaseBiasList;
      t_satPhaseBias::readEpoch(m_sLastLine, m_stream, satPhaseBiasList);
      navCorrToSp3->newPhaseBiases(satPhaseBiasList);
    }
    else if (corrType == t_corrSSR::vTec) 
    {
      t_vTec vTec;
      t_vTec::read(m_sLastLine, m_stream, vTec);
      navCorrToSp3->newTec(vTec);
    }

    m_sLastLine.clear();
  }
}
