#pragma ident "$Id: RinexUPDXData.cpp 2897 2012-08-10 20:08:46Z shjzhang $"

/**
 * @file RinexUPDXData.cpp
 * Encapsulate satellite upd file data, including I/O
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//  
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================

#include "RinexUPDXStream.hpp"
#include "RinexUPDXHeader.hpp"
#include "RinexUPDXData.hpp"
#include "StringUtils.hpp"
#include "TimeString.hpp"
#include "CivilTime.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{

   void RinexUPDXData::reallyPutRecord(FFStream& ffs) const 
      throw(exception, FFStreamError, StringException)
   {
         // cast the stream to be an RinexUPDXStream
      RinexUPDXStream& strm = dynamic_cast<RinexUPDXStream&>(ffs);

      int i;
      string line;

      line = datatype;
      line += string(1,' ');

      if(datatype == string("AR")) 
      {
         line += rightJustify(site,4);
      }
      else if(datatype == string("AS")) 
      {
         line += string(1,sat.systemChar());
         line += rightJustify(asString(sat.id),2);
         if(line[4] == ' ') line[4] = '0';
         line += string(1,' ');
      }
      else 
      {
         FFStreamError e("Unknown data type: " + datatype);
         GPSTK_THROW(e);
      }

      line += string(1,' ');

      line += printTime(time,"%4Y %02m %02d %02H %02M %9.6f");

         // must count the data to output
      int n(3);

      line += rightJustify(asString(n),3);
      line += string(3,' ');

      line += asString(updSatWL, 3);
      line += string(1,' ');
      line += asString(updSatLC, 3);
      line += string(1,' ');
      line += asString(updSatL1, 3);
      line += string(1,' ');
      line += asString(updSatL2, 3);
      line += string(1,' ');

      strm << line << endl;
      strm.lineNumber++;

   }  // end reallyPutRecord()

   void RinexUPDXData::reallyGetRecord(FFStream& ffs)
      throw(exception, FFStreamError, StringException)
   {
         // cast the stream to be an RinexUPDXStream
      RinexUPDXStream& strm = dynamic_cast<RinexUPDXStream&>(ffs);

      clear();

      string line;
      strm.formattedGetLine(line,true);      // true means 'expect possible EOF'
      stripTrailing(line);
      if(line.length() < 48) 
      {
         FFStreamError e("Short line : " + line);
         GPSTK_THROW(e);
      }

//    cout << "Data Line: /" << line << "/" << endl;

      datatype = line.substr(0,2);
      site = line.substr(3,4);

      if(datatype == string("AS")) 
      {
         strip(site);

         int prn(asInt(site.substr(1,2)));

         if(site[0] == 'G') 
         {
            sat = RinexSatID(prn,RinexSatID::systemGPS);
         }
         else if(site[0] == 'R') 
         {
            sat = RinexSatID(prn,RinexSatID::systemGlonass);
         }
         else 
         {
            FFStreamError e("Invalid sat : /" + site + "/");
            GPSTK_THROW(e);
         }
         site = string();
      }

      CivilTime tc( asInt(line.substr( 8,4)),
                    asInt(line.substr(12,3)),
                    asInt(line.substr(15,3)),
                    asInt(line.substr(18,3)),
                    asInt(line.substr(21,3)),
                    asDouble(line.substr(24,10)),
                    TimeSystem::Any); 

        // Convert to CommonTime
      time = tc.convertToCommonTime();

      int n(asInt(line.substr(34,3)));

         // Read L1 satellite upds
      updSatWL = asDouble(line.substr(41,8));

         // Read L2 satellite upds
      updSatLC = asDouble(line.substr(50,8));

         // Read WL satellite upds
      updSatL1 = asDouble(line.substr(59,8));

         // Read WL satellite upds
      updSatL2 = asDouble(line.substr(68,8));


   }   // end reallyGetRecord()

   void RinexUPDXData::dump(ostream& s) const throw()
   {
      // dump record type, sat id / site, current epoch, and data
      s << " " << datatype;

      if(datatype == string("AR")) 
          s << " " << site;
      else 
          s << " " << sat.toString();

      s << " " << printTime(time,"%Y/%02m/%02d %2H:%02M:%06.3f = %F/%10.3g %P");

      s << scientific << setprecision(3)
         << " " << setw(8) << updSatWL
         << " " << setw(8) << updSatLC
         << " " << setw(8) << updSatL1
         << " " << setw(8) << updSatL2;

      s << endl;

   }  // end dump()

} // namespace

//------------------------------------------------------------------------------------
