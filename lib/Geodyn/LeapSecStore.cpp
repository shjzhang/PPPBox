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
