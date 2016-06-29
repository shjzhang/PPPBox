/**
* @file LeapSecStore.hpp
* Class to read and store the leapsec data.
*/

#ifndef GPSTK_LEAP_SEC_STORE_HPP
#define GPSTK_LEAP_SEC_STORE_HPP


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
//  Last Modifid:
//
//
//  Shoujian Zhang, Wuhan University, 2013
//
//============================================================================


#include <iostream>
#include <string>
#include <set>
#include <map>
#include "CommonTime.hpp"
#include "CivilTime.hpp"

using namespace std;

namespace gpstk
{
   
      /** @addtogroup General Data Tool */
      //@{

      /** This class will read the leap second data from given file and
       *  you can get the leap second by given a time. 
       *
       *  The leap second file can be downloaded from the IERS ftp:
       *
       *  ftp://hpiers.obspm.fr/iers/bul/bulc/Leap_Second_History.dat
       *
       *  \warning You ONLY can get the leapsec at given "UTC" time, so
       *  the time conversion is needed if your time system is 'GPS' or
       *  some other time system.
       *
       *  \warning Different leap second file from difference organizations
       *  would be different. This class only 'distiguish' the file from 
       *  the IERS 'Leap_Second_History.dat'.
       */
    
   class LeapSecStore
   {
   public:

         /// Handy type definition
      typedef std::map<CommonTime, double > LeapSecData;

         /// Default constructor
      LeapSecStore()
      {
               leapSecHistory( 1972, 1, 1, 10 );
               leapSecHistory( 1972, 7, 1, 11 );
               leapSecHistory( 1973, 1, 1, 12 );
               leapSecHistory( 1974, 1, 1, 13 );
               leapSecHistory( 1975, 1, 1, 14 );
               leapSecHistory( 1976, 1, 1, 15 );
               leapSecHistory( 1977, 1, 1, 16 );
               leapSecHistory( 1978, 1, 1, 17 );
               leapSecHistory( 1979, 1, 1, 18 );
               leapSecHistory( 1980, 1, 1, 19 );
               leapSecHistory( 1981, 7, 1, 20 );
               leapSecHistory( 1982, 7, 1, 21 );
               leapSecHistory( 1983, 7, 1, 22 );
               leapSecHistory( 1985, 7, 1, 23 );
               leapSecHistory( 1988, 1, 1, 24 );
               leapSecHistory( 1990, 1, 1, 25 );
               leapSecHistory( 1991, 1, 1, 26 );
               leapSecHistory( 1992, 7, 1, 27 );
               leapSecHistory( 1993, 7, 1, 28 );
               leapSecHistory( 1994, 7, 1, 29 );
               leapSecHistory( 1996, 1, 1, 30 );
               leapSecHistory( 1997, 7, 1, 31 );
               leapSecHistory( 1999, 1, 1, 32 );
               leapSecHistory( 2006, 1, 1, 33 );
               leapSecHistory( 2009, 1, 1, 34 );
               leapSecHistory( 2012, 7, 1, 35 );
               //add 2015 leap second
               leapSecHistory( 2015, 7, 1, 36 );
            
               //
               initialTime=CivilTime(1972,1,1,0,0,0.0,
                                     TimeSystem::UTC).convertToCommonTime();
               finalTime=CivilTime(2015,7,1,0,0,0.0,
                                   TimeSystem::UTC).convertToCommonTime();
               // more leap seconds should be appended here
               // ...
      }
         
         // to set leapSecHistory

      void leapSecHistory(int year, int month, int day,double leap)
      {
         CommonTime time=CivilTime(year,month,day,0,0,0.0,
                                   TimeSystem::UTC).convertToCommonTime();
         leapSecData[time] = leap;
      }

         /// Load the leap second data from input file
         void loadFile(std::string file)
         throw(FileMissingException);
         

         /** Determine the earliest time stored in the object 
          *
          * @return The initial time
          *
          */
      virtual CommonTime getInitialTime()
      { return initialTime; };


         /** Determine the latest time stored in the object
          *
          * @return The final time
          *
          */
      virtual CommonTime getFinalTime()
      { return finalTime; };


         /** Get the leap second at the given UTC time
          *
          *  @param t   the time given in UTC 
          *
          */
      double getLeapSec(const CommonTime& t) const
         throw(InvalidRequest);



         /// clear the all the data
      void clear()
      { leapSecData.clear(); }


         /// Default deconstructor
      virtual ~LeapSecStore()
      { leapSecData.clear(); }


   private:

      
         /// Object holding all the leap second data
      LeapSecData leapSecData;
         
         /// These give the overall span of time for which this object
         /// contains data.
      CommonTime initialTime;
      CommonTime finalTime;

   }; // End of class 'LeapSecStore'

      // @}

}  // End of namespace gpstk


#endif   // GPSTK_EPOCH_DATA_STORE_HPP






