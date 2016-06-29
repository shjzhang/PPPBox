/**
* @file LeapSecStore.cpp
* Class to handle interpolatable time serial data 
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
//  Shoujian Zhang, Wuhan University, 2013
//
//============================================================================

#include "StringUtils.hpp"
#include "LeapSecStore.hpp"
#include "MiscMath.hpp"
#include "MJD.hpp"
#include <fstream>

using namespace std;
using namespace gpstk::StringUtils;

#define debug 1

namespace gpstk
{
      
      void LeapSecStore::loadFile(std::string file)
      throw(FileMissingException)
      {
            
            //      cout << file << endl;
            //      cout << initialTime << endl;
            //      cout << finalTime << endl;
            
            ifstream inpf(file.c_str());
            if(!inpf)
            {
                  FileMissingException fme("Could not open leap second file " + file);
                  GPSTK_THROW(fme);
            }
            
            clear();
            
            // first we skip the header section
            // skip the header with following 4 lines:
            // --->
            // MJD        Date        TAI-UTC (s)
            //        day month year
            // ---    --------------   ------
            //
            // <---
            //
            string temp;
            while( getline(inpf,temp) )
            {
                  if(temp[0] != '#') break;
                  if(debug) cout << temp << endl;
            }
            
            // Now, read the leap second data
            bool ok (true);
            while(!inpf.eof() && inpf.good())
            {
                  string line;
                  getline(inpf,line);
                  StringUtils::stripTrailing(line,'\r');
                  
                  if(inpf.eof()) break;
                  
                  if(debug)
                  {
                        cout << "line:" << line << endl;
                  }
                  
                  // line length is actually 185
                  if( inpf.bad() )
                  { ok = false; break; }
                  
                  // Define a string stream to read 'line'
                  istringstream istrm(line);
                  //         cout << line.size() << endl;
                  
                  double mjd(0.0);
                  //         mjd = asDouble( leftJustify(line, 11) );
                  //         cout << mjd << endl;
                  double year(0.0), month(0.0), day(0.0);
                  double leapSec(0.0);
                  
                  // Get the variable values from stream
                  istrm >> mjd
                  >> day >> month >> year
                  >> leapSec;
                  
                  if(debug)
                        cout << "time: "
                        << CommonTime( MJD(mjd).convertToCommonTime() )
                        << " TAI - UTC " << leapSec << endl;
                  
                  //         cout << "mjd: " << mjd << endl;
                  
                  // UTC time corresponding to the leap second
                  CommonTime time( MJD(mjd).convertToCommonTime() );
                  time.setTimeSystem(TimeSystem::UTC);
                  
                  // Add leap second into 'leapSecData'
                  leapSecData[time] = leapSec;
                  
                  //         cout << time << "leap: " << leapSecData[time] << endl; 
                  //         cout << initialTime << endl;
                  //         cout << finalTime << endl;
                  
                  // Modify the initialTime and finalTime for 'leapSecData'
                  if( time < initialTime )
                  {
                        initialTime = time;
                  }
                  else if( time > finalTime)
                  {
                        finalTime = time;
                  }
                  
                  //         cout << "time:"  << time << endl;
                  
            }
            
            inpf.close();
            
            if(!ok) 
            {
                  FileMissingException fme("LeapSec File " + file
                                           + " is corrupted or wrong format");
                  GPSTK_THROW(fme);
            }
      }
      


   double LeapSecStore::getLeapSec(const CommonTime& utc) const
      throw(InvalidRequest)
   {

      double leapSec;

         // check the time
      if( ( utc < initialTime ) || ( utc > finalTime ) )
      {
         InvalidRequest ire(string("Time tag (")
            + utc.asString()
            + string(") not found within the store "));
         GPSTK_THROW(ire);
      }

      for( LeapSecData::const_iterator it = leapSecData.begin();
           it != leapSecData.end();
           ++it)
      {
         if( (it->first) <= utc )
         {
               // Leap second = "TAI2UTC"
            leapSec = (it->second) ;
         }
      }

      return leapSec;
   
   }  // End of method 'LeapSecStore::getLeapSec()'



}  // End of namespace gpstk
