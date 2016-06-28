/**
 * @file ReferenceSystem.cpp
 * This class ease handling Reference System transformation.
 */

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
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
//  Last Modified:
//
//  create this file, 2013/05/29
//
//  Shoujian Zhang, Wuhan University
//
//============================================================================

#include "ReferenceSystem.hpp"
#include <string>
#include <fstream>
#include <cmath>
#include <iomanip>

#include "Epoch.hpp"

using namespace std;

namespace gpstk
{

      /// Method from EOPDataStore     

      /// Get the x pole displacement parameter, in arcseconds.
   double ReferenceSystem::getXPole(const CommonTime& UTC) const
      throw (InvalidRequest)
   {
      double xpole;

      try
      {
            // Get the x pole from EOPDataStore
         if( pEopStore != NULL )
         {
//            xpole = (*pEopStore).getXPole( UTC );
            xpole = (*pEopStore).getEOPData( UTC ).xp;
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return xpole;

   } // End of method 'ReferenceSystem::getXPole()'


      /// Get the y pole displacement parameter, in arcseconds.
   double ReferenceSystem::getYPole(const CommonTime& UTC) const
      throw (InvalidRequest)
   {
      double ypole;

      try
      {
            // Get the x pole from EOPDataStore
         if( pEopStore != NULL )
         {
//            ypole = (*pEopStore).getYPole( UTC );
            ypole = (*pEopStore).getEOPData( UTC ).yp;
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return ypole;

   } // End of method 'ReferenceSystem::getYPole()'


      /// Get the value of UT1mUTC, in seconds.
   double ReferenceSystem::getUT1mUTC(const CommonTime& UTC) const
      throw (InvalidRequest) 
   {
      double UT1mUTC;

      try
      {
            // Get the UT1mUTC from EOPDataStore
         if( pEopStore != NULL )
         {
//            UT1mUTC = (*pEopStore).getUT1mUTC( UTC );
            UT1mUTC = (*pEopStore).getEOPData( UTC ).UT1mUTC;
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return UT1mUTC;

   } // End of method 'ReferenceSystem::getUT1mUTC()'


      /// Get the value of dPsi, in arcseconds.
   double ReferenceSystem::getDPsi(const CommonTime& UTC) const
       throw(InvalidRequest)
   {
       double dPsi;

       try
       {
           // Get the dPsi from EOPDataStore
           if( pEopStore != NULL )
           {
               dPsi = (*pEopStore).getEOPData( UTC ).dPsi;
           }
       }
       catch(InvalidRequest& ire) 
       {
           GPSTK_RETHROW(ire);
       }

       // return
       return dPsi;

   } // End of method 'ReferenceSystem::getDPsi()'


      /// Get the value of dEps, in arcseconds.
   double ReferenceSystem::getDEps(const CommonTime& UTC) const
       throw(InvalidRequest)
   {
       double dEps;

       try
       {
           // Get the dEps from EOPDataStore
           if( pEopStore != NULL )
           {
               dEps = (*pEopStore).getEOPData( UTC ).dEps;
           }
       }
       catch(InvalidRequest& ire) 
       {
           GPSTK_RETHROW(ire);
       }

       // return
       return dEps;

   } // End of method 'ReferenceSystem::getDEps()'


      /// Get the EOP data at the given UTC time.
   EOPDataStore::EOPData ReferenceSystem::getEOPData(const CommonTime& UTC) const
      throw(InvalidRequest)
   {
      EOPDataStore::EOPData eopData;

      try
      {
            // Get the EOP data from EOPDataStore
         if( pEopStore != NULL )
         {
            eopData = (*pEopStore).getEOPData( UTC );
         }

      }
      catch(InvalidRequest& ire) 
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return eopData;

   } // End of method 'ReferenceSystem::getEOPData()'
   

      /// Method from LeapSecondStore

      /// Get the leap second at the given UTC time.
   double ReferenceSystem::getLeapSec(const CommonTime& UTC) const
      throw(InvalidRequest)
   {
      double leapSec;

      try
      {
            // Get the leap second from EOPDataStore
         if( pLeapSecStore!= NULL )
         {
            leapSec = (*pLeapSecStore).getLeapSec( UTC );
         }

      }
      catch(InvalidRequest& ire)
      {
         GPSTK_RETHROW(ire);
      }

         // return
      return leapSec;

   } // End of method 'ReferenceSystem::getLeapSec()'


      /// Method to get the difference between time systems

      /// Return the value of (TAI - UTC) (= leapsec).
   double ReferenceSystem::getTAImUTC(const CommonTime& UTC)
      throw(InvalidRequest)
   {
          // Leap second = TAI - UTC
       return getLeapSec(UTC);

   } // End of method 'ReferenceSystem::getTAImUTC()


      /// Get the value of (TT - TAI)
   double ReferenceSystem::getTTmTAI( void )
      throw(InvalidRequest)
   {
         // The difference is constant : 32.184 second
      return 32.184;

   } // End of method 'ReferenceSystem::getTTmTAI()


      /// Get the value of (TT - UTC)
   double ReferenceSystem::getTTmUTC(const CommonTime& UTC)
      throw(InvalidRequest)
   {
         // TT - TAI
      double TTmTAI ( getTTmTAI() );

         // TAI - UTC
      double TAImUTC( getTAImUTC(UTC) );

         // TT - UTC
      double TTmUTC( TTmTAI + TAImUTC );

         // return
      return TTmUTC;

   } // End of method 'ReferenceSystem::getTTmTAI()


      /// Get the value of (TAI - GPS)
   double ReferenceSystem::getTAImGPS(void)
      throw(InvalidRequest)
   {
         // The difference is constant : 19.0 second
      return 19.0;

   } // End of method 'ReferenceSystem::getTAImGPS()


      /// Methods to convert between different time systems


      /// Convert GPS to UTC.
   CommonTime ReferenceSystem::GPS2UTC(const CommonTime& GPS)
   {

         // The input time of 'getLeapSec' should be 'UTC'
         // if the input is GPS, you should amends it.
      CommonTime GPS1(GPS);
      GPS1.setTimeSystem(TimeSystem::UTC);

      double leapSec = getLeapSec( GPS1 );
//      cout << leapSec << endl;

         // The formula of GPS and UTC equals with:
         // GPS = UTC + ( leapSec - 19.0s );

      CommonTime UTC1;
      UTC1  = GPS1 - (leapSec - 19.0);

         ///////////////////////////////////////////////////////////////////
         //
         // Now, correct the leap second by feeding the utc instead
         // of the 'gps', if not fix, the following would happen:
         //
         //  01-JUL-1993 00:00:10 GPS TIME (- 9s) = 01-JUL-1993 00:00:01 UTC TIME
         //  01-JUL-1993 00:00:09 GPS TIME (- 9s) = 01-JUL-1993 00:00:00 UTC TIME
         //  01-JUL-1993 00:00:08 GPS TIME (- 9s) = 01-JUL-1993 00:00:59 UTC TIME
         //  01-JUL-1993 00:00:07 GPS TIME (- 9s) = 30-JUN-1993 23:59:58 UTC TIME
         //  01-JUL-1993 00:00:06 GPS TIME (- 9s) = 30-JUN-1993 23:59:57 UTC TIME
         //  01-JUL-1993 00:00:05 GPS TIME (- 9s) = 30-JUN-1993 23:59:56 UTC TIME
         //  01-JUL-1993 00:00:04 GPS TIME (- 9s) = 30-JUN-1993 23:59:55 UTC TIME
         //  01-JUL-1993 00:00:03 GPS TIME (- 9s) = 30-JUN-1993 23:59:54 UTC TIME
         //  01-JUL-1993 00:00:02 GPS TIME (- 9s) = 30-JUN-1993 23:59:53 UTC TIME
         //  01-JUL-1993 00:00:01 GPS TIME (- 9s) = 30-JUN-1993 23:59:52 UTC TIME
         //  01-JUL-1993 00:00:00 GPS TIME (- 9s) = 30-JUN-1993 23:59:51 UTC TIME
         //  30-JUN-1993 23:59:59 GPS TIME (- 8s) = 30-JUN-1993 23:59:51 UTC TIME
         //
         //  
         //  The fix allows the following conversion: 
         //
         //  01-JUL-1993 00:00:10 GPS TIME (- 9s) = 01-JUL-1993 00:00:01 UTC TIME
         //  01-JUL-1993 00:00:09 GPS TIME (- 9s) = 01-JUL-1993 00:00:00 UTC TIME
         //  01-JUL-1993 00:00:08 GPS TIME (- 8s) = 01-JUL-1993 00:00:00 UTC TIME
         //  01-JUL-1993 00:00:07 GPS TIME (- 8s) = 30-JUN-1993 23:59:59 UTC TIME
         //  01-JUL-1993 00:00:06 GPS TIME (- 8s) = 30-JUN-1993 23:59:58 UTC TIME
         //  01-JUL-1993 00:00:05 GPS TIME (- 8s) = 30-JUN-1993 23:59:57 UTC TIME
         //  01-JUL-1993 00:00:04 GPS TIME (- 8s) = 30-JUN-1993 23:59:56 UTC TIME
         //  01-JUL-1993 00:00:03 GPS TIME (- 8s) = 30-JUN-1993 23:59:55 UTC TIME
         //  01-JUL-1993 00:00:02 GPS TIME (- 8s) = 30-JUN-1993 23:59:54 UTC TIME
         //  01-JUL-1993 00:00:01 GPS TIME (- 8s) = 30-JUN-1993 23:59:53 UTC TIME
         //  01-JUL-1993 00:00:00 GPS TIME (- 8s) = 30-JUN-1993 23:59:52 UTC TIME
         //  30-JUN-1993 23:59:59 GPS TIME (- 8s) = 30-JUN-1993 23:59:51 UTC TIME
         //
         //  \reference Please refer to the subroutine 'gps2utc' in GRACE 
         //  data format converting software from GFZ.
         //
         ///////////////////////////////////////////////////////////////////
         
         // New leap second
      leapSec = getLeapSec(UTC1);

      CommonTime UTC;
      UTC = GPS1 -  (leapSec - 19.0);

      // The TimeSystem of UTC is already set to TimeSystem::UTC.
      return UTC;

   } // End of method 'ReferenceSystem::GPS2UTC()'


      /// Convert UTC to GPS.
   CommonTime ReferenceSystem::UTC2GPS(const CommonTime& UTC)
   {
         // Now, set gpst with utc
      CommonTime TAI;

         // Convert the UTC time to TAI time by correcting the leap second
      TAI = UTC + getTAImUTC(UTC);

      CommonTime GPS;

         // Convert the TAI time to GPS time
      GPS = TAI - getTAImGPS();
      GPS.setTimeSystem(TimeSystem::GPS);

         // return gps
      return GPS;

   } // End of method 'ReferenceSystem::UTC2GPS()'


      /// Convert UT1 to UTC.
   CommonTime ReferenceSystem::UT12UTC(const CommonTime& UT1)
   {

         // Warnging the input time should be utc, so
         // you must amends it
      CommonTime UTC1( UT1 );
      UTC1.setTimeSystem(TimeSystem::UTC);
      UTC1 = UT1 -  getUT1mUTC(UTC1);
      UTC1.setTimeSystem(TimeSystem::UTC);

         // Correct utc by computing new UT1mUTC with utc1
      CommonTime UTC2;
      UTC2 = UT1 -  getUT1mUTC(UTC1);
      UTC2.setTimeSystem(TimeSystem::UTC);

         // Corrent utc by computing new UT1mUTC with utc2
      CommonTime UTC;
      UTC  = UT1 -  getUT1mUTC(UTC2);
      UTC.setTimeSystem(TimeSystem::UTC);

      return UTC;

   }  // End of method 'ReferenceSystem::UT12UTC()'


      /// Convert UTC to UT1.
   CommonTime ReferenceSystem::UTC2UT1(const CommonTime& UTC)
   {

      CommonTime UT1;
      UT1 =  UTC + getUT1mUTC(UTC);
      UT1.setTimeSystem(TimeSystem::UT1);

      return UT1;

   }  // End of method 'ReferenceSystem::UTC2UT1()'


   CommonTime ReferenceSystem::UTC2UT1(const CommonTime& UTC, const double& UT1mUTC)
   {
      CommonTime UT1;
      UT1 = UTC + UT1mUTC;
      UT1.setTimeSystem(TimeSystem::UT1);

      return UT1;
   }


      /// Convert TAI to UTC.
   CommonTime ReferenceSystem::TAI2UTC(const CommonTime& TAI)
   {
       // Initial UTC1.
       CommonTime UTC1( TAI );
       UTC1.setTimeSystem(TimeSystem::UTC);

       // Get leapSec.
       double leapSec( getLeapSec(UTC1) );

       // Update UTC1.
       UTC1 = UTC1 - leapSec;

       // Update leapSec.
       leapSec = getLeapSec(UTC1);

       // Get UTC.
       CommonTime UTC( TAI - leapSec );
       UTC.setTimeSystem(TimeSystem::UTC);

       // return UTC.
       return UTC;

   }  // End of method 'ReferenceSystem::TAI2UTC()'


      /// Convert UTC to TAI.
   CommonTime ReferenceSystem::UTC2TAI(const CommonTime& UTC)
   {
       // Get leapSec.
       double leapSec( getLeapSec(UTC) );

       // Get TAI.
       CommonTime TAI( UTC + leapSec );
       TAI.setTimeSystem(TimeSystem::TAI);

       // Return TAI.
       return TAI;

   }  // End of method 'ReferenceSystem::UTC2TAI()'


      /// Convert TT to UTC.
   CommonTime ReferenceSystem::TT2UTC(const CommonTime& TT)
   {
       // Convert TT to TAI.
       CommonTime TAI( TT - getTTmTAI() );
       TAI.setTimeSystem(TimeSystem::TAI);

       // Convert TAI to UTC.
       CommonTime UTC( TAI2UTC(TAI) );


       // Return UTC.
       return UTC;

   }  // End of method 'ReferenceSystem::TT2UTC()'


      /// Convert UTC to TT.
   CommonTime ReferenceSystem::UTC2TT(const CommonTime& UTC)
   {
       // Convert UTC to TAI.
       CommonTime TAI( UTC2TAI(UTC) );
       // Convert TAI to TT.
       CommonTime TT( TAI + getTTmTAI() );
       TT.setTimeSystem(TimeSystem::TT);

       // Return TT.
       return TT;

   }  // End of method 'ReferenceSystem::UTC2TT()'


   // Evaluate the effects of zonal Earth tides on the rotation of the Earth.
   // IERS Conventions 2010
   Vector<double>  ReferenceSystem::RG_ZONT2(const CommonTime& TT)
   { 
      static const struct {
         int nl,nlp,nf,nd,nom;    // Luni-Solar argument multipliers
         double s1, c1;           // sine and cosine of DUT
         double c2, s2;           // cosine and sine of DLOD
         double c3, s3;           // cosine and sine of DOMEGA
      } x[] = {
            // 0 - 20
         {  1,  0,  2,  2,  2,     -0.0235, 0.0000, 0.2617, 0.0000, -0.2209, 0.0000 },
         {  2,  0,  2,  0,  1,     -0.0404, 0.0000, 0.3706, 0.0000, -0.3128, 0.0000 },
         {  2,  0,  2,  0,  2,     -0.0987, 0.0000, 0.9041, 0.0000, -0.7630, 0.0000 },
         {  0,  0,  2,  2,  1,     -0.0508, 0.0000, 0.4499, 0.0000, -0.3797, 0.0000 },
         {  0,  0,  2,  2,  2,     -0.1231, 0.0000, 1.0904, 0.0000, -0.9203, 0.0000 },
         {  1,  0,  2,  0,  0,     -0.0385, 0.0000, 0.2659, 0.0000, -0.2244, 0.0000 },
         {  1,  0,  2,  0,  1,     -0.4108, 0.0000, 2.8298, 0.0000, -2.3884, 0.0000 },
         {  1,  0,  2,  0,  2,     -0.9926, 0.0000, 6.8291, 0.0000, -5.7637, 0.0000 },
         {  3,  0,  0,  0,  0,     -0.0179, 0.0000, 0.1222, 0.0000, -0.1031, 0.0000 },
         { -1,  0,  2,  2,  1,     -0.0818, 0.0000, 0.5384, 0.0000, -0.4544, 0.0000 },
         { -1,  0,  2,  2,  2,     -0.1974, 0.0000, 1.2978, 0.0000, -1.0953, 0.0000 },
         {  1,  0,  0,  2,  0,     -0.0761, 0.0000, 0.4976, 0.0000, -0.4200, 0.0000 },
         {  2,  0,  2, -2,  2,      0.0216, 0.0000,-0.1060, 0.0000,  0.0895, 0.0000 },
         {  0,  1,  2,  0,  2,      0.0254, 0.0000,-0.1211, 0.0000,  0.1022, 0.0000 },
         {  0,  0,  2,  0,  0,     -0.2989, 0.0000, 1.3804, 0.0000, -1.1650, 0.0000 },
         {  0,  0,  2,  0,  1,     -3.1873, 0.2010,14.6890, 0.9266,-12.3974,-0.7820 },
         {  0,  0,  2,  0,  2,     -7.8468, 0.5320,36.0910, 2.4469,-30.4606,-2.0652 },
         {  2,  0,  0,  0, -1,      0.0216, 0.0000,-0.0988, 0.0000,  0.0834, 0.0000 },
         {  2,  0,  0,  0,  0,     -0.3384, 0.0000, 1.5433, 0.0000, -1.3025, 0.0000 },
         {  2,  0,  0,  0,  1,      0.0179, 0.0000,-0.0813, 0.0000,  0.0686, 0.0000 },

            // 21 - 40
         {  0, -1,  2,  0,  2,     -0.0244, 0.0000,  0.1082, 0.0000, -0.0913, 0.0000 },
         {  0,  0,  0,  2, -1,      0.0470, 0.0000, -0.2004, 0.0000,  0.1692, 0.0000 },
         {  0,  0,  0,  2,  0,     -0.7341, 0.0000,  3.1240, 0.0000, -2.6367, 0.0000 },
         {  0,  0,  0,  2,  1,     -0.0526, 0.0000,  0.2235, 0.0000, -0.1886, 0.0000 },
         {  0, -1,  0,  2,  0,     -0.0508, 0.0000,  0.2073, 0.0000, -0.1749, 0.0000 },
         {  1,  0,  2, -2,  1,      0.0498, 0.0000, -0.1312, 0.0000,  0.1107, 0.0000 },
         {  1,  0,  2, -2,  2,      0.1006, 0.0000, -0.2640, 0.0000,  0.2228, 0.0000 },
         {  1,  1,  0,  0,  0,      0.0395, 0.0000, -0.0968, 0.0000,  0.0817, 0.0000 },
         { -1,  0,  2,  0,  0,      0.0470, 0.0000, -0.1099, 0.0000,  0.0927, 0.0000 },
         { -1,  0,  2,  0,  1,      0.1767, 0.0000, -0.4115, 0.0000,  0.3473, 0.0000 },
         { -1,  0,  2,  0,  2,      0.4352, 0.0000, -1.0093, 0.0000,  0.8519, 0.0000 },
         {  1,  0,  0,  0, -1,      0.5339, 0.0000, -1.2224, 0.0000,  1.0317, 0.0000 },
         {  1,  0,  0,  0,  0,     -8.4046, 0.2500, 19.1647, 0.5701,-16.1749,-0.4811 },
         {  1,  0,  0,  0,  1,      0.5443, 0.0000, -1.2360, 0.0000,  1.0432, 0.0000 },
         {  0,  0,  0,  1,  0,      0.0470, 0.0000, -0.1000, 0.0000,  0.0844, 0.0000 },
         {  1, -1,  0,  0,  0,     -0.0555, 0.0000,  0.1169, 0.0000, -0.0987, 0.0000 },
         { -1,  0,  0,  2, -1,      0.1175, 0.0000, -0.2332, 0.0000,  0.1968, 0.0000 },
         { -1,  0,  0,  2,  0,     -1.8236, 0.0000,  3.6018, 0.0000, -3.0399, 0.0000 },
         { -1,  0,  0,  2,  1,      0.1316, 0.0000, -0.2587, 0.0000,  0.2183, 0.0000 },
         {  1,  0, -2,  2, -1,      0.0179, 0.0000, -0.0344, 0.0000,  0.0290, 0.0000 },
 
            // 41 - 60
         { -1, -1,  0,  2,  0,     -0.0855, 0.0000,  0.1542, 0.0000, -0.1302, 0.0000 }
/*
         {  0,  2,  2, -2,  2,     -0.0573, 0.0000,  0.0395, 0.0000, -0.0333, 0.0000 },
         {  0,  1,  2, -2,  1,      0.0329, 0.0000, -0.0173, 0.0000,  0.0146, 0.0000 },
         {  0,  1,  2, -2,  2,     -1.8847, 0.0000,  0.9726, 0.0000, -0.8209, 0.0000 },
         {  0,  0,  2, -2,  0,      0.2510, 0.0000, -0.0910, 0.0000,  0.0768, 0.0000 },
         {  0,  0,  2, -2,  1,      1.1703, 0.0000, -0.4135, 0.0000,  0.3490, 0.0000 },
         {  0,  0,  2, -2,  2,    -49.7174, 0.4330, 17.1056, 0.1490,-14.4370,-0.1257 },
         {  0,  2,  0,  0,  0,     -0.1936, 0.0000,  0.0666, 0.0000, -0.0562, 0.0000 },
         {  2,  0,  0, -2, -1,      0.0489, 0.0000, -0.0154, 0.0000,  0.0130, 0.0000 },
         {  2,  0,  0, -2,  0,     -0.5471, 0.0000,  0.1670, 0.0000, -0.1409, 0.0000 },
         {  2,  0,  0, -2,  1,      0.0367, 0.0000, -0.0108, 0.0000,  0.0092, 0.0000 },
         {  0, -1,  2, -2,  1,     -0.0451, 0.0000,  0.0082, 0.0000, -0.0069, 0.0000 },
         {  0,  1,  0,  0, -1,      0.0921, 0.0000, -0.0167, 0.0000,  0.0141, 0.0000 },
         {  0, -1,  2, -2,  2,      0.8281, 0.0000, -0.1425, 0.0000,  0.1202, 0.0000 },
         {  0,  1,  0,  0,  0,    -15.8887, 0.1530,  2.7332, 0.0267, -2.3068,-0.0222 },
         {  0,  1,  0,  0,  1,     -0.1382, 0.0000,  0.0225, 0.0000, -0.0190, 0.0000 },
         {  1,  0,  0, -1,  0,      0.0348, 0.0000, -0.0053, 0.0000,  0.0045, 0.0000 },
         {  2,  0, -2,  0,  0,     -0.1372, 0.0000, -0.0079, 0.0000,  0.0066, 0.0000 },
         { -2,  0,  2,  0,  1,      0.4211, 0.0000, -0.0203, 0.0000,  0.0171, 0.0000 },
         { -1,  1,  0,  1,  0      -0.0404, 0.0000,  0.0008, 0.0000, -0.0007, 0.0000 },
   
            // 61 - 62
         {  0,  0,  0,  0,  2,      7.8998, 0.0000,  0.1460, 0.0000, -0.1232, 0.0000 },
         {  0,  0,  0,  0,  1,  -1617.2681, 0.0000,-14.9471, 0.0000, 12.6153, 0.0000 }
*/
      };

      /* Number of terms in the series */
      const int NT = (int) (sizeof(x) / sizeof(x[0]));

      double el, elp, f, d, om;

      /* Interval between fundamental epoch J2000.0 and given date (JC). */
      double t = (MJD(TT).mjd - MJD_J2000) / JC;

      /* --------------------- */
      /* Fundamental arguments */
      /* --------------------- */

      /* Mean longitude of Moon minus mean longitude of Moon's perigee. */
      el = Anpm(
            (485866.733 + (715922.633 + (31.310 + 0.064 * t) * t) * t)
            * AS_TO_RAD + fmod(1325.0 * t, 1.0) * TWO_PI);

      /* Mean longitude of Sun minus mean longitude of Sun's perigee. */
      elp = Anpm(
            (1287099.804 + (1292581.224 + (-0.577 - 0.012 * t) * t) * t)
            * AS_TO_RAD + fmod(99.0 * t, 1.0) * TWO_PI);

      /* Mean longitude of Moon minus mean longitude of Moon's node. */
      f = Anpm(
            (335778.877 + (295263.137 + (-13.257 + 0.011 * t) * t) * t)
            * AS_TO_RAD + fmod(1342.0 * t, 1.0) * TWO_PI);

      /* Mean elongation of Moon from Sun. */
      d = Anpm(
            (1072261.307 + (1105601.328 + (-6.891 + 0.019 * t) * t) * t)
            * AS_TO_RAD + fmod(1236.0 * t, 1.0) * TWO_PI);

      /* Longitude of the mean ascending node of the lunar orbit on the */
      /* ecliptic, measured from the mean equinox of date. */
      om = Anpm(
            (450160.280 + (-482890.539 + (7.455 + 0.008 * t) * t) * t)
            * AS_TO_RAD + fmod(-5.0 * t, 1.0) * TWO_PI);

      double arg;
      double dut(0.0), dlod(0.0), domega(0.0);
      for(int i=0; i<NT; i++)
      {
         arg = 0.0;

         // Formation of multiples of arguments.
         arg = x[i].nl*el
               + x[i].nlp*elp
               + x[i].nf*f
               + x[i].nd*d
               + x[i].nom*om;

         arg = std::fmod(arg, TWO_PI);
         if(arg < 0.0) arg += TWO_PI;

         // Evaluate zonal tidal terms.
         dut    += x[i].s1*std::sin(arg) + x[i].c1*std::cos(arg);
         dlod   += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
         domega += x[i].c3*std::cos(arg) + x[i].s3*std::sin(arg);
      }

      // Rescale corrections so that they are in units involving seconds.
      dut    = dut    * 1.0e-4;
      dlod   = dlod   * 1.0e-5;
      domega = domega * 1.0e-14;

      // return
      Vector<double> ret(3,0.0);
      ret(0) = dut;
      ret(1) = dlod;
      ret(2) = domega;

      return ret;

   }


   // Provide the diurnal/subdiurnal tidal effects on polar motion ("), UT1 (s)
   // and LOD (s), in time domain. IERS Conventions 2010
   Vector<double> ReferenceSystem::PMUT1_OCEANS(const CommonTime& UTC)
   {
       static const struct {
           int nchi;            // coefficients of GMST + PI
           int nl,nlp,nf,nd,nom;// coefficients of l,l',F,D,Om
           double s1, c1;       // sine and cosine of xp
           double s2, c2;       // sine and cosine of yp
           double s3, c3;       // sine and cosine of ut1
       } x[] = {
           { 1,-1, 0,-2,-2,-2,  -0.05,   0.94,  -0.94,  -0.05,  0.396, -0.078 },
           { 1,-2, 0,-2, 0,-1,   0.06,   0.64,  -0.64,   0.06,  0.195, -0.059 },
           { 1,-2, 0,-2, 0,-2,   0.30,   3.42,  -3.42,   0.30,  1.034, -0.314 },
           { 1, 0, 0,-2,-2,-1,   0.08,   0.78,  -0.78,   0.08,  0.224, -0.073 },
           { 1, 0, 0,-2,-2,-2,   0.46,   4.15,  -4.15,   0.45,  1.187, -0.387 },
           { 1,-1, 0,-2, 0,-1,   1.19,   4.96,  -4.96,   1.19,  0.966, -0.474 },
           { 1,-1, 0,-2, 0,-2,   6.24,  26.31, -26.31,   6.23,  5.118, -2.499 },
           { 1, 1, 0,-2,-2,-1,   0.24,   0.94,  -0.94,   0.24,  0.172, -0.090 },
           { 1, 1, 0,-2,-2,-2,   1.28,   4.99,  -4.99,   1.28,  0.911, -0.475 },
           { 1, 0, 0,-2, 0, 0,  -0.28,  -0.77,   0.77,  -0.28, -0.093,  0.070 },
           { 1, 0, 0,-2, 0,-1,   9.22,  25.06, -25.06,   9.22,  3.025, -2.280 },
           { 1, 0, 0,-2, 0,-2,  48.82, 132.91,-132.90,  48.82, 16.020,-12.069 },
           { 1,-2, 0, 0, 0, 0,  -0.32,  -0.86,   0.86,  -0.32, -0.103,  0.078 },
           { 1, 0, 0, 0,-2, 0,  -0.66,  -1.72,   1.72,  -0.66, -0.194,  0.154 },
           { 1,-1, 0,-2, 2,-2,  -0.42,  -0.92,   0.92,  -0.42, -0.083,  0.074 },
           { 1, 1, 0,-2, 0,-1,  -0.30,  -0.64,   0.64,  -0.30, -0.057,  0.050 },
           { 1, 1, 0,-2, 0,-2,  -1.61,  -3.46,   3.46,  -1.61, -0.308,  0.271 },
           { 1,-1, 0, 0, 0, 0,  -4.48,  -9.61,   9.61,  -4.48, -0.856,  0.751 },
           { 1,-1, 0, 0, 0,-1,  -0.90,  -1.93,   1.93,  -0.90, -0.172,  0.151 },
           { 1, 1, 0, 0,-2, 0,  -0.86,  -1.81,   1.81,  -0.86, -0.161,  0.137 },
           { 1, 0,-1,-2, 2,-2,   1.54,   3.03,  -3.03,   1.54,  0.315, -0.189 },
           { 1, 0, 0,-2, 2,-1,  -0.29,  -0.58,   0.58,  -0.29, -0.062,  0.035 },
           { 1, 0, 0,-2, 2,-2,  26.13,  51.25, -51.25,  26.13,  5.512, -3.095 },
           { 1, 0, 1,-2, 2,-2,  -0.22,  -0.42,   0.42,  -0.22, -0.047,  0.025 },
           { 1, 0,-1, 0, 0, 0,  -0.61,  -1.20,   1.20,  -0.61, -0.134,  0.070 },
           { 1, 0, 0, 0, 0, 1,   1.54,   3.00,  -3.00,   1.54,  0.348, -0.171 },
           { 1, 0, 0, 0, 0, 0, -77.48,-151.74, 151.74, -77.48,-17.620,  8.548 },
           { 1, 0, 0, 0, 0,-1, -10.52, -20.56,  20.56, -10.52, -2.392,  1.159 },
           { 1, 0, 0, 0, 0,-2,   0.23,   0.44,  -0.44,   0.23,  0.052, -0.025 },
           { 1, 0, 1, 0, 0, 0,  -0.61,  -1.19,   1.19,  -0.61, -0.144,  0.065 },
           { 1, 0, 0, 2,-2, 2,  -1.09,  -2.11,   2.11,  -1.09, -0.267,  0.111 },
           { 1,-1, 0, 0, 2, 0,  -0.69,  -1.43,   1.43,  -0.69, -0.288,  0.043 },
           { 1, 1, 0, 0, 0, 0,  -3.46,  -7.28,   7.28,  -3.46, -1.610,  0.187 },
           { 1, 1, 0, 0, 0,-1,  -0.69,  -1.44,   1.44,  -0.69, -0.320,  0.037 },
           { 1, 0, 0, 0, 2, 0,  -0.37,  -1.06,   1.06,  -0.37, -0.407, -0.005 },
           { 1, 2, 0, 0, 0, 0,  -0.17,  -0.51,   0.51,  -0.17, -0.213, -0.005 },
           { 1, 0, 0, 2, 0, 2,  -1.10,  -3.42,   3.42,  -1.09, -1.436, -0.037 },
           { 1, 0, 0, 2, 0, 1,  -0.70,  -2.19,   2.19,  -0.70, -0.921, -0.023 },
           { 1, 0, 0, 2, 0, 0,  -0.15,  -0.46,   0.46,  -0.15, -0.193, -0.005 },
           { 1, 1, 0, 2, 0, 2,  -0.03,  -0.59,   0.59,  -0.03, -0.396, -0.024 },
           { 1, 1, 0, 2, 0, 1,  -0.02,  -0.38,   0.38,  -0.02, -0.253, -0.015 },
           { 2,-3, 0,-2, 0,-2,  -0.49,  -0.04,   0.63,   0.24, -0.089, -0.011 },
           { 2,-1, 0,-2,-2,-2,  -1.33,  -0.17,   1.53,   0.68, -0.224, -0.032 },
           { 2,-2, 0,-2, 0,-2,  -6.08,  -1.61,   3.13,   3.35, -0.637, -0.177 },
           { 2, 0, 0,-2,-2,-2,  -7.59,  -2.05,   3.44,   4.23, -0.745, -0.222 },
           { 2, 0, 1,-2,-2,-2,  -0.52,  -0.14,   0.22,   0.29, -0.049, -0.015 },
           { 2,-1,-1,-2, 0,-2,   0.47,   0.11,  -0.10,  -0.27,  0.033,  0.013 },
           { 2,-1, 0,-2, 0,-1,   2.12,   0.49,  -0.41,  -1.23,  0.141,  0.058 },
           { 2,-1, 0,-2, 0,-2, -56.87, -12.93,  11.15,  32.88, -3.795, -1.556 },
           { 2,-1, 1,-2, 0,-2,  -0.54,  -0.12,   0.10,   0.31, -0.035, -0.015 },
           { 2, 1, 0,-2,-2,-2, -11.01,  -2.40,   1.89,   6.41, -0.698, -0.298 },
           { 2, 1, 1,-2,-2,-2,  -0.51,  -0.11,   0.08,   0.30, -0.032, -0.014 },
           { 2,-2, 0,-2, 2,-2,   0.98,   0.11,  -0.11,  -0.58,  0.050,  0.022 },
           { 2, 0,-1,-2, 0,-2,   1.13,   0.11,  -0.13,  -0.67,  0.056,  0.025 },
           { 2, 0, 0,-2, 0,-1,  12.32,   1.00,  -1.41,  -7.31,  0.605,  0.266 },
           { 2, 0, 0,-2, 0,-2,-330.15, -26.96,  37.58, 195.92,-16.195, -7.140 },
           { 2, 0, 1,-2, 0,-2,  -1.01,  -0.07,   0.11,   0.60, -0.049, -0.021 },
           { 2,-1, 0,-2, 2,-2,   2.47,  -0.28,  -0.44,  -1.48,  0.111,  0.034 },
           { 2, 1, 0,-2, 0,-2,   9.40,  -1.44,  -1.88,  -5.65,  0.425,  0.117 },
           { 2,-1, 0, 0, 0, 0,  -2.35,   0.37,   0.47,   1.41, -0.106, -0.029 },
           { 2,-1, 0, 0, 0,-1,  -1.04,   0.17,   0.21,   0.62, -0.047, -0.013 },
           { 2, 0,-1,-2, 2,-2,  -8.51,   3.50,   3.29,   5.11, -0.437, -0.019 },
           { 2, 0, 0,-2, 2,-2,-144.13,  63.56,  59.23,  86.56, -7.547, -0.159 },
           { 2, 0, 1,-2, 2,-2,   1.19,  -0.56,  -0.52,  -0.72,  0.064,  0.000 },
           { 2, 0, 0, 0, 0, 1,   0.49,  -0.25,  -0.23,  -0.29,  0.027, -0.001 },
           { 2, 0, 0, 0, 0, 0, -38.48,  19.14,  17.72,  23.11, -2.104,  0.041 },
           { 2, 0, 0, 0, 0,-1, -11.44,   5.75,   5.32,   6.87, -0.627,  0.015 },
           { 2, 0, 0, 0, 0,-2,  -1.24,   0.63,   0.58,   0.75, -0.068,  0.002 },
           { 2, 1, 0, 0, 0, 0,  -1.77,   1.79,   1.71,   1.04, -0.146,  0.037 },
           { 2, 1, 0, 0, 0,-1,  -0.77,   0.78,   0.75,   0.45, -0.064,  0.017 },
           { 2, 0, 0, 2, 0, 2,  -0.33,   0.62,   0.65,   0.19, -0.049,  0.018 }
       };

       /* Number of terms in the series */
       const int NT = (int)(sizeof(x) / sizeof(x[0]));

       /* Interval between fundamental epoch J2000.0 and given date (JC). */
       double t = (MJD(UTC).mjd - MJD_J2000) / JC;
       double t2 = t*t;
       double t3 = t2*t;
       double t4 = t3*t;

       double chi, l, lp, f, d, om;
       double dchi, dl, dlp, df, dd, dom;

       // Arguments in the following order : chi=GMST+PI,l,lp,F,D,Omega

       chi = (67310.54841 + (876600.0*3600 + 8640184.812866)*t + 0.093104*t2 - 6.2e-6*t3)*15.0 + 648000.0;
       chi = std::fmod(chi, TURNAS) * AS_TO_RAD;

       dchi = (876600.0*3600 + 8640184.812866 + 2*0.093104*t - 3*6.2e-6*t2)*15.0;
       dchi = dchi * AS_TO_RAD / JC;    // rad/day

       l = -0.00024470*t4 + 0.051635*t3 + 31.8792*t2 + 1717915923.2178*t + 485868.249036;
       l = std::fmod(l, TURNAS) * AS_TO_RAD;

       dl = -4*0.00024470*t3 + 3*0.051635*t2 + 2*31.8792*t + 1717915923.2178;
       dl = dl * AS_TO_RAD / JC;        // rad/day
 
       lp = -0.00001149*t4 - 0.000136*t3 - 0.5532*t2 + 129596581.0481*t + 1287104.79305;
       lp = std::fmod(lp, TURNAS) * AS_TO_RAD;

       dlp = -4*0.00001149*t3 - 3*0.000136*t2 - 2*0.5532*t + 129596581.0481;
       dlp = dlp * AS_TO_RAD / JC;      // rad/day

       f = 0.00000417*t4 - 0.001037*t3 - 12.7512*t2 + 1739527262.8478*t + 335779.526232;
       f = std::fmod(f, TURNAS) * AS_TO_RAD;

       df = 4*0.00000417*t3 - 3*0.001037*t2 - 2*12.7512*t + 1739527262.8478;
       df = df * AS_TO_RAD / JC;        // rad/day

       d = -0.00003169*t4 + 0.006593*t3 - 6.3706*t2 + 1602961601.2090*t + 1072260.70369;
       d = std::fmod(d, TURNAS) * AS_TO_RAD;

       dd = -4*0.00003169*t3 + 3*0.006593*t2 - 2*6.3706*t + 1602961601.2090;
       dd = dd * AS_TO_RAD / JC;        // rad/day

       om = -0.00005939*t4 + 0.007702*t3 + 7.4722*t2 - 6962890.2665*t + 450160.398036;
       om = std::fmod(om, TURNAS) * AS_TO_RAD;

       dom = -4*0.00005939*t3 + 3*0.007702*t2 + 2*7.4722*t - 6962890.2665;
       dom = dom * AS_TO_RAD / JC;      // rad/day

       // corrections
       double arg, darg;
       double cor_x(0.0), cor_y(0.0), cor_ut1(0.0), cor_lod(0.0);

       for(int i=0; i<NT; i++)
       {
           arg = 0.0; darg = 0.0;
           arg += x[i].nchi*chi
                + x[i].nl*l
                + x[i].nlp*lp
                + x[i].nf*f
                + x[i].nd*d
                + x[i].nom*om;
           darg += x[i].nchi*dchi
                 + x[i].nl*dl
                 + x[i].nlp*dlp
                 + x[i].nf*df
                 + x[i].nd*dd
                 + x[i].nom*dom;

           arg = std::fmod(arg, TWO_PI);

           cor_x    += x[i].c1*std::cos(arg) + x[i].s1*std::sin(arg);
           cor_y    += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
           cor_ut1  += x[i].c3*std::cos(arg) + x[i].s3*std::sin(arg);
           cor_lod  -= (-x[i].c3*std::sin(arg) + x[i].s3*std::cos(arg))*darg;
       }

       cor_x   = cor_x * 1.0e-6;    // arcsecond (")
       cor_y   = cor_y * 1.0e-6;    // arcsecond (")
       cor_ut1 = cor_ut1 * 1.0e-6;  // second (s)
       cor_lod = cor_lod * 1.0e-6;  // second (s)

       // return
       Vector<double> ret(4,0.0);
       ret(0) = cor_x;
       ret(1) = cor_y;
       ret(2) = cor_ut1;
       ret(3) = cor_lod;

       return ret;
   }


   // Provide the diurnal lunisolar effect on polar motion ("), in time domain.
   // IERS Conventions 2010.
   Vector<double> ReferenceSystem::PMSDNUT2(const CommonTime& UTC)
   {
       static const struct {
           int nchi;            // coefficients of GMST+PI
           int nl,nlp,nf,nd,nom;// coefficients of l,l',F,D,Om
           double s1, c1;       // sine and cosine of xp
           double s2, c2;       // sine and cosine of yp
       } x[] = {
           { 1,-1, 0,-2, 0,-1,    -.44,   .25,   -.25,  -.44 },
           { 1,-1, 0,-2, 0,-2,   -2.31,  1.32,  -1.32, -2.31 },
           { 1, 1, 0,-2,-2,-2,    -.44,   .25,   -.25,  -.44 },
           { 1, 0, 0,-2, 0,-1,   -2.14,  1.23,  -1.23, -2.14 },
           { 1, 0, 0,-2, 0,-2,  -11.36,  6.52,  -6.52,-11.36 },
           { 1,-1, 0, 0, 0, 0,     .84,  -.48,    .48,   .84 },
           { 1, 0, 0,-2, 2,-2,   -4.76,  2.73,  -2.73, -4.76 },
           { 1, 0, 0, 0, 0, 0,   14.27, -8.19,   8.19, 14.27 },
           { 1, 0, 0, 0, 0,-1,    1.93, -1.11,   1.11,  1.93 },
           { 1, 1, 0, 0, 0, 0,     .76,  -.43,    .43,   .76 }
       };

       /* Number of terms in the series */
       const int NT = (int)(sizeof(x) / sizeof(x[0]));

       /* Interval between fundamental epoch J2000.0 and given date (JC). */
       double t = (MJD(UTC).mjd - MJD_J2000)/ JC;
       double t2 = t*t;
       double t3 = t2*t;
       double t4 = t3*t;

       double chi, l, lp, f, d, om;
       // Arguments in the following order : chi=GMST+PI,l,lp,F,D,Omega

       chi = (67310.54841 + (876600.0*3600 + 8640184.812866)*t + 0.093104*t2 - 6.2e-6*t3)*15.0 + 648000.0;
       chi = std::fmod(chi, TURNAS) * AS_TO_RAD;

       l = -0.00024470*t4 + 0.051635*t3 + 31.8792*t2 + 1717915923.2178*t + 485868.249036;
       l = std::fmod(l, TURNAS) * AS_TO_RAD;

       lp = -0.00001149*t4 - 0.000136*t3 - 0.5532*t2 + 129596581.0481*t + 1287104.79305;
       lp = std::fmod(lp, TURNAS) * AS_TO_RAD;

       f = 0.00000417*t4 - 0.001037*t3 - 12.7512*t2 + 1739527262.8478*t + 335779.526232;
       f = std::fmod(f, TURNAS) * AS_TO_RAD;

       d = -0.00003169*t4 + 0.006593*t3 - 6.3706*t2 + 1602961601.2090*t + 1072260.70369;
       d = std::fmod(d, TURNAS) * AS_TO_RAD;

       om = -0.00005939*t4 + 0.007702*t3 + 7.4722*t2 - 6962890.2665*t + 450160.398036;
       om = std::fmod(om, TURNAS) * AS_TO_RAD;

       // corrections
       double cor_x = 0.0, cor_y = 0.0;

       double arg;

       for(int i=0; i<NT; i++)
       {
           arg = 0.0;
           arg += x[i].nchi * chi 
                + x[i].nl * l
                + x[i].nlp * lp
                + x[i].nf * f
                + x[i].nd * d
                + x[i].nom * om;

           arg = std::fmod(arg, TWO_PI);

           cor_x += x[i].c1*std::cos(arg) + x[i].s1*std::sin(arg);
           cor_y += x[i].c2*std::cos(arg) + x[i].s2*std::sin(arg);
       }

       cor_x = cor_x * 1.0e-6;   // arcsecond (")
       cor_y = cor_y * 1.0e-6;   // arcsecond (")

       // return 
       Vector<double> ret(2,0.0);
       ret(0) = cor_x;
       ret(1) = cor_y;

       return ret;
   }


   // Evaluate the model of subdiurnal libration in the axial component of
   // rotation, expressed by UT1 and LOD.
   Vector<double> ReferenceSystem::UTLIBR(const CommonTime& UTC)
   {
       static const struct {
           int nchi;            // coefficients of GMST+PI
           int nl,nlp,nf,nd,nom;// coefficients of l,l',F,D,Om
           double period;       // period of the tide
           double s1, c1;       // sine and cosine of UT1
           double s2, c2;       // sine and cosine of LOD
       } x[] = {
           { 2, -2,  0, -2,  0, -2, 0.5377239,  0.05, -0.03,  -0.3,  -0.6 },
           { 2,  0,  0, -2, -2, -2, 0.5363232,  0.06, -0.03,  -0.4,  -0.7 },
           { 2, -1,  0, -2,  0, -2, 0.5274312,  0.35, -0.20,  -2.4,  -4.1 },
           { 2,  1,  0, -2, -2, -2, 0.5260835,  0.07, -0.04,  -0.5,  -0.8 },
           { 2,  0,  0, -2,  0, -1, 0.5175645, -0.07,  0.04,   0.5,   0.8 },
           { 2,  0,  0, -2,  0, -2, 0.5175251,  1.75, -1.01, -12.2, -21.3 },
           { 2,  1,  0, -2,  0, -2, 0.5079842, -0.05,  0.03,   0.3,   0.6 },
           { 2,  0, -1, -2,  2, -2, 0.5006854,  0.04, -0.03,  -0.3,  -0.6 },
           { 2,  0,  0, -2,  2, -2, 0.5000000,  0.76, -0.44,  -5.5,  -9.6 },
           { 2,  0,  0,  0,  0,  0, 0.4986348,  0.21, -0.12,  -1.5,  -2.6 },
           { 2,  0,  0,  0,  0, -1, 0.4985982,  0.06, -0.04,  -0.4,  -0.8 }
       };

       /* Number of terms in the series */
       const int NT = (int)(sizeof(x) / sizeof(x[0]));

       /* Interval between fundamental epoch J2000.0 and given date (JC). */
       double t = (MJD(UTC).mjd - MJD_J2000)/ JC;
       double t2 = t*t;
       double t3 = t2*t;
       double t4 = t3*t;

       double gmst, chi, l, lp, f, d, om;

       // Arguments in the following order : chi=GMST+PI,l,lp,F,D,Omega
       chi = (67310.54841 + (876600.0*3600 + 8640184.812866)*t + 0.093104*t2 - 6.2e-6*t3)*15.0 + 648000.0;
       chi = std::fmod(chi, TURNAS) * AS_TO_RAD;

       l = -0.00024470*t4 + 0.051635*t3 + 31.8792*t2 + 1717915923.2178*t + 485868.249036;
       l = std::fmod(l, TURNAS) * AS_TO_RAD;

       lp = -0.00001149*t4 - 0.000136*t3 - 0.5532*t2 + 129596581.0481*t + 1287104.79305;
       lp = std::fmod(lp, TURNAS) * AS_TO_RAD;

       f = 0.00000417*t4 - 0.001037*t3 - 12.7512*t2 + 1739527262.8478*t + 335779.526232;
       f = std::fmod(f, TURNAS) * AS_TO_RAD;

       d = -0.00003169*t4 + 0.006593*t3 - 6.3706*t2 + 1602961601.2090*t + 1072260.70369;
       d = std::fmod(d, TURNAS) * AS_TO_RAD;

       om = -0.00005939*t4 + 0.007702*t3 + 7.4722*t2 - 6962890.2665*t + 450160.398036;
       om = std::fmod(om, TURNAS) * AS_TO_RAD;

       // corrections
       double arg;
       double dut1 = 0.0, dlod = 0.0;

       for(int i=0; i<NT; i++)
       {
           arg = 0.0;
           arg += x[i].nchi * chi
                + x[i].nl * l
                + x[i].nlp * lp
                + x[i].nf * f
                + x[i].nd * d
                + x[i].nom * om;

           arg = std::fmod(arg, TWO_PI);

           dut1 += x[i].s1 * std::sin(arg) + x[i].c1 * std::cos(arg);
           dlod += x[i].s2 * std::sin(arg) + x[i].c2 * std::cos(arg);
       }

       dut1 = dut1 * 1.0e-6;    // second (s)
       dlod = dlod * 1.0e-6;    // second (s)

       // return
       Vector<double> ret(2,0.0);
       ret(0) = dut1;
       ret(1) = dlod;

       return ret;
   }


   // X,Y coordinates of celestial intermediate pole from series based on IAU
   // 2006 precession and IAU 2006 nutation.
   void ReferenceSystem::XY06(const CommonTime& TT, double& x, double& y)
   {
       /* Maximum power of T in the polynomials for X and Y */
       enum { MAXPT = 5 };

       /* Polynomial coefficients (arcsec, X then Y). */
       static const double xyp[2][MAXPT+1] = {
          {    -0.016617,
             2004.191898,
               -0.4297829,
               -0.19861834,
                0.000007578,
                0.0000059285
          },
          {    -0.006951,
               -0.025896,
              -22.4072747,
                0.00190059,
                0.001112526,
                0.0000001358
          }
       };

       /* Fundamental-argument multipliers:  luni-solar terms */
       static const int mfals[][5] = {

           /* 1-10 */
           {  0,   0,   0,   0,   1 },
           {  0,   0,   2,  -2,   2 },
           {  0,   0,   2,   0,   2 },
           {  0,   0,   0,   0,   2 },
           {  0,   1,   0,   0,   0 },
           {  0,   1,   2,  -2,   2 },
           {  1,   0,   0,   0,   0 },
           {  0,   0,   2,   0,   1 },
           {  1,   0,   2,   0,   2 },
           {  0,   1,  -2,   2,  -2 },

           /* 11-20 */
           {  0,   0,   2,  -2,   1 },
           {  1,   0,  -2,   0,  -2 },
           {  1,   0,   0,  -2,   0 },
           {  1,   0,   0,   0,   1 },
           {  1,   0,   0,   0,  -1 },
           {  1,   0,  -2,  -2,  -2 },
           {  1,   0,   2,   0,   1 },
           {  2,   0,  -2,   0,  -1 },
           {  0,   0,   0,   2,   0 },
           {  0,   0,   2,   2,   2 },

           /* 21-30 */
           {  2,   0,   0,  -2,   0 },
           {  0,   2,  -2,   2,  -2 },
           {  2,   0,   2,   0,   2 },
           {  1,   0,   2,  -2,   2 },
           {  1,   0,  -2,   0,  -1 },
           {  2,   0,   0,   0,   0 },
           {  0,   0,   2,   0,   0 },
           {  0,   1,   0,   0,   1 },
           {  1,   0,   0,  -2,  -1 },
           {  0,   2,   2,  -2,   2 },

           /* 31-40 */
           {  0,   0,   2,  -2,   0 },
           {  1,   0,   0,  -2,   1 },
           {  0,   1,   0,   0,  -1 },
           {  0,   2,   0,   0,   0 },
           {  1,   0,  -2,  -2,  -1 },
           {  1,   0,   2,   2,   2 },
           {  0,   1,   2,   0,   2 },
           {  2,   0,  -2,   0,   0 },
           {  0,   0,   2,   2,   1 },
           {  0,   1,  -2,   0,  -2 },

           /* 41-50 */
           {  0,   0,   0,   2,   1 },
           {  1,   0,   2,  -2,   1 },
           {  2,   0,   0,  -2,  -1 },
           {  2,   0,   2,  -2,   2 },
           {  2,   0,   2,   0,   1 },
           {  0,   0,   0,   2,  -1 },
           {  0,   1,  -2,   2,  -1 },
           {  1,   1,   0,  -2,   0 },
           {  2,   0,   0,  -2,   1 },
           {  1,   0,   0,   2,   0 },

           /* 51-60 */
           {  0,   1,   2,  -2,   1 },
           {  1,  -1,   0,   0,   0 },
           {  0,   1,  -1,   1,  -1 },
           {  2,   0,  -2,   0,  -2 },
           {  0,   1,   0,  -2,   0 },
           {  1,   0,   0,  -1,   0 },
           {  3,   0,   2,   0,   2 },
           {  0,   0,   0,   1,   0 },
           {  1,  -1,   2,   0,   2 },
           {  1,   1,  -2,  -2,  -2 },

           /* 61-70 */
           {  1,   0,  -2,   0,   0 },
           {  2,   0,   0,   0,  -1 },
           {  0,   1,  -2,  -2,  -2 },
           {  1,   1,   2,   0,   2 },
           {  2,   0,   0,   0,   1 },
           {  1,   1,   0,   0,   0 },
           {  1,   0,  -2,   2,  -1 },
           {  1,   0,   2,   0,   0 },
           {  1,  -1,   0,  -1,   0 },
           {  1,   0,   0,   0,   2 },

           /* 71-80 */
           {  1,   0,  -1,   0,  -1 },
           {  0,   0,   2,   1,   2 },
           {  1,   0,  -2,  -4,  -2 },
           {  1,  -1,   0,  -1,  -1 },
           {  1,   0,   2,   2,   1 },
           {  0,   2,  -2,   2,  -1 },
           {  1,   0,   0,   0,  -2 },
           {  2,   0,  -2,  -2,  -2 },
           {  1,   1,   2,  -2,   2 },
           {  2,   0,  -2,  -4,  -2 },

           /* 81-90 */
           {  1,   0,  -4,   0,  -2 },
           {  2,   0,   2,  -2,   1 },
           {  1,   0,   0,  -1,  -1 },
           {  2,   0,   2,   2,   2 },
           {  3,   0,   0,   0,   0 },
           {  1,   0,   0,   2,   1 },
           {  0,   0,   2,  -2,  -1 },
           {  3,   0,   2,  -2,   2 },
           {  0,   0,   4,  -2,   2 },
           {  1,   0,   0,  -4,   0 },

           /* 91-100 */
           {  0,   1,   2,   0,   1 },
           {  2,   0,   0,  -4,   0 },
           {  1,   1,   0,  -2,  -1 },
           {  2,   0,  -2,   0,   1 },
           {  0,   0,   2,   0,  -1 },
           {  0,   1,  -2,   0,  -1 },
           {  0,   1,   0,   0,   2 },
           {  0,   0,   2,  -1,   2 },
           {  0,   0,   2,   4,   2 },
           {  2,   1,   0,  -2,   0 },

           /* 101-110 */
           {  1,   1,   0,  -2,   1 },
           {  1,  -1,   0,  -2,   0 },
           {  1,  -1,   0,  -1,  -2 },
           {  1,  -1,   0,   0,   1 },
           {  0,   1,  -2,   2,   0 },
           {  0,   1,   0,   0,  -2 },
           {  1,  -1,   2,   2,   2 },
           {  1,   0,   0,   2,  -1 },
           {  1,  -1,  -2,  -2,  -2 },
           {  3,   0,   2,   0,   1 },

           /* 111-120 */
           {  0,   1,   2,   2,   2 },
           {  1,   0,   2,  -2,   0 },
           {  1,   1,  -2,  -2,  -1 },
           {  1,   0,   2,  -4,   1 },
           {  0,   1,  -2,  -2,  -1 },
           {  2,  -1,   2,   0,   2 },
           {  0,   0,   0,   2,   2 },
           {  1,  -1,   2,   0,   1 },
           {  1,  -1,  -2,   0,  -2 },
           {  0,   1,   0,   2,   0 },

           /* 121-130 */
           {  0,   1,   2,  -2,   0 },
           {  0,   0,   0,   1,   1 },
           {  1,   0,  -2,  -2,   0 },
           {  0,   3,   2,  -2,   2 },
           {  2,   1,   2,   0,   2 },
           {  1,   1,   0,   0,   1 },
           {  2,   0,   0,   2,   0 },
           {  1,   1,   2,   0,   1 },
           {  1,   0,   0,  -2,  -2 },
           {  1,   0,  -2,   2,   0 },

           /* 131-140 */
           {  1,   0,  -1,   0,  -2 },
           {  0,   1,   0,  -2,   1 },
           {  0,   1,   0,   1,   0 },
           {  0,   0,   0,   1,  -1 },
           {  1,   0,  -2,   2,  -2 },
           {  1,  -1,   0,   0,  -1 },
           {  0,   0,   0,   4,   0 },
           {  1,  -1,   0,   2,   0 },
           {  1,   0,   2,   1,   2 },
           {  1,   0,   2,  -1,   2 },

           /* 141-150 */
           {  0,   0,   2,   1,   1 },
           {  1,   0,   0,  -2,   2 },
           {  1,   0,  -2,   0,   1 },
           {  1,   0,  -2,  -4,  -1 },
           {  0,   0,   2,   2,   0 },
           {  1,   1,   2,  -2,   1 },
           {  1,   0,  -2,   1,  -1 },
           {  0,   0,   1,   0,   1 },
           {  2,   0,  -2,  -2,  -1 },
           {  4,   0,   2,   0,   2 },

           /* 151-160 */
           {  2,  -1,   0,   0,   0 },
           {  2,   1,   2,  -2,   2 },
           {  0,   1,   2,   1,   2 },
           {  1,   0,   4,  -2,   2 },
           {  1,   1,   0,   0,  -1 },
           {  2,   0,   2,   0,   0 },
           {  2,   0,  -2,  -4,  -1 },
           {  1,   0,  -1,   0,   0 },
           {  1,   0,   0,   1,   0 },
           {  0,   1,   0,   2,   1 },

           /* 161-170 */
           {  1,   0,  -4,   0,  -1 },
           {  1,   0,   0,  -4,  -1 },
           {  2,   0,   2,   2,   1 },
           {  2,   1,   0,   0,   0 },
           {  0,   0,   2,  -3,   2 },
           {  1,   2,   0,  -2,   0 },
           {  0,   3,   0,   0,   0 },
           {  0,   0,   4,   0,   2 },
           {  0,   0,   2,  -4,   1 },
           {  2,   0,   0,  -2,  -2 },

           /* 171-180 */
           {  1,   1,  -2,  -4,  -2 },
           {  0,   1,   0,  -2,  -1 },
           {  0,   0,   0,   4,   1 },
           {  3,   0,   2,  -2,   1 },
           {  1,   0,   2,   4,   2 },
           {  1,   1,  -2,   0,  -2 },
           {  0,   0,   4,  -2,   1 },
           {  2,  -2,   0,  -2,   0 },
           {  2,   1,   0,  -2,  -1 },
           {  0,   2,   0,  -2,   0 },

           /* 181-190 */
           {  1,   0,   0,  -1,   1 },
           {  1,   1,   2,   2,   2 },
           {  3,   0,   0,   0,  -1 },
           {  2,   0,   0,  -4,  -1 },
           {  3,   0,   2,   2,   2 },
           {  0,   0,   2,   4,   1 },
           {  0,   2,  -2,  -2,  -2 },
           {  1,  -1,   0,  -2,  -1 },
           {  0,   0,   2,  -1,   1 },
           {  2,   0,   0,   2,   1 },

           /* 191-200 */
           {  1,  -1,  -2,   2,  -1 },
           {  0,   0,   0,   2,  -2 },
           {  2,   0,   0,  -4,   1 },
           {  1,   0,   0,  -4,   1 },
           {  2,   0,   2,  -4,   1 },
           {  4,   0,   2,  -2,   2 },
           {  2,   1,  -2,   0,  -1 },
           {  2,   1,  -2,  -4,  -2 },
           {  3,   0,   0,  -4,   0 },
           {  1,  -1,   2,   2,   1 },

           /* 201-210 */
           {  1,  -1,  -2,   0,  -1 },
           {  0,   2,   0,   0,   1 },
           {  1,   2,  -2,  -2,  -2 },
           {  1,   1,   0,  -4,   0 },
           {  2,   0,   0,  -2,   2 },
           {  0,   2,   2,  -2,   1 },
           {  1,   0,   2,   0,  -1 },
           {  2,   1,   0,  -2,   1 },
           {  2,  -1,  -2,   0,  -1 },
           {  1,  -1,  -2,  -2,  -1 },

           /* 211-220 */
           {  0,   1,  -2,   1,  -2 },
           {  1,   0,  -4,   2,  -2 },
           {  0,   1,   2,   2,   1 },
           {  3,   0,   0,   0,   1 },
           {  2,  -1,   2,   2,   2 },
           {  0,   1,  -2,  -4,  -2 },
           {  1,   0,  -2,  -3,  -2 },
           {  2,   0,   0,   0,   2 },
           {  1,  -1,   0,  -2,  -2 },
           {  2,   0,  -2,   2,  -1 },

           /* 221-230 */
           {  0,   2,  -2,   0,  -2 },
           {  3,   0,  -2,   0,  -1 },
           {  2,  -1,   2,   0,   1 },
           {  1,   0,  -2,  -1,  -2 },
           {  0,   0,   2,   0,   3 },
           {  2,   0,  -4,   0,  -2 },
           {  2,   1,   0,  -4,   0 },
           {  1,   1,  -2,   1,  -1 },
           {  0,   2,   2,   0,   2 },
           {  1,  -1,   2,  -2,   2 },

           /* 231-240 */
           {  1,  -1,   0,  -2,   1 },
           {  2,   1,   2,   0,   1 },
           {  1,   0,   2,  -4,   2 },
           {  1,   1,  -2,   0,  -1 },
           {  1,   1,   0,   2,   0 },
           {  1,   0,   0,  -3,   0 },
           {  2,   0,   2,  -1,   2 },
           {  0,   2,   0,   0,  -1 },
           {  2,  -1,   0,  -2,   0 },
           {  4,   0,   0,   0,   0 },

           /* 241-250 */
           {  2,   1,  -2,  -2,  -2 },
           {  0,   2,  -2,   2,   0 },
           {  1,   0,   2,   1,   1 },
           {  1,   0,  -1,   0,  -3 },
           {  3,  -1,   2,   0,   2 },
           {  2,   0,   2,  -2,   0 },
           {  1,  -2,   0,   0,   0 },
           {  2,   0,   0,   0,  -2 },
           {  1,   0,   0,   4,   0 },
           {  0,   1,   0,   1,   1 },

           /* 251-260 */
           {  1,   0,   2,   2,   0 },
           {  0,   1,   0,   2,  -1 },
           {  0,   1,   0,   1,  -1 },
           {  0,   0,   2,  -2,   3 },
           {  3,   1,   2,   0,   2 },
           {  1,   1,   2,   1,   2 },
           {  1,   1,  -2,   2,  -1 },
           {  2,  -1,   2,  -2,   2 },
           {  1,  -2,   2,   0,   2 },
           {  1,   0,   2,  -4,   0 },

           /* 261-270 */
           {  0,   0,   1,   0,   0 },
           {  1,   0,   2,  -3,   1 },
           {  1,  -2,   0,  -2,   0 },
           {  2,   0,   0,   2,  -1 },
           {  1,   1,   2,  -4,   1 },
           {  4,   0,   2,   0,   1 },
           {  0,   1,   2,   1,   1 },
           {  1,   2,   2,  -2,   2 },
           {  2,   0,   2,   1,   2 },
           {  2,   1,   2,  -2,   1 },

           /* 271-280 */
           {  1,   0,   2,  -1,   1 },
           {  1,   0,   4,  -2,   1 },
           {  1,  -1,   2,  -2,   1 },
           {  0,   1,   0,  -4,   0 },
           {  3,   0,  -2,  -2,  -2 },
           {  0,   0,   4,  -4,   2 },
           {  2,   0,  -4,  -2,  -2 },
           {  2,  -2,   0,  -2,  -1 },
           {  1,   0,   2,  -2,  -1 },
           {  2,   0,  -2,  -6,  -2 },

           /* 281-290 */
           {  1,   0,  -2,   1,  -2 },
           {  1,   0,  -2,   2,   1 },
           {  1,  -1,   0,   2,  -1 },
           {  1,   0,  -2,   1,   0 },
           {  2,  -1,   0,  -2,   1 },
           {  1,  -1,   0,   2,   1 },
           {  2,   0,  -2,  -2,   0 },
           {  1,   0,   2,  -3,   2 },
           {  0,   0,   0,   4,  -1 },
           {  2,  -1,   0,   0,   1 },

           /* 291-300 */
           {  2,   0,   4,  -2,   2 },
           {  0,   0,   2,   3,   2 },
           {  0,   1,   4,  -2,   2 },
           {  0,   1,  -2,   2,   1 },
           {  1,   1,   0,   2,   1 },
           {  1,   0,   0,   4,   1 },
           {  0,   0,   4,   0,   1 },
           {  2,   0,   0,  -3,   0 },
           {  1,   0,   0,  -1,  -2 },
           {  1,  -2,  -2,  -2,  -2 },

           /* 301-310 */
           {  3,   0,   0,   2,   0 },
           {  2,   0,   2,  -4,   2 },
           {  1,   1,  -2,  -4,  -1 },
           {  1,   0,  -2,  -6,  -2 },
           {  2,  -1,   0,   0,  -1 },
           {  2,  -1,   0,   2,   0 },
           {  0,   1,   2,  -2,  -1 },
           {  1,   1,   0,   1,   0 },
           {  1,   2,   0,  -2,  -1 },
           {  1,   0,   0,   1,  -1 },

           /* 311-320 */
           {  0,   0,   1,   0,   2 },
           {  3,   1,   2,  -2,   2 },
           {  1,   0,  -4,  -2,  -2 },
           {  1,   0,   2,   4,   1 },
           {  1,  -2,   2,   2,   2 },
           {  1,  -1,  -2,  -4,  -2 },
           {  0,   0,   2,  -4,   2 },
           {  0,   0,   2,  -3,   1 },
           {  2,   1,  -2,   0,   0 },
           {  3,   0,  -2,  -2,  -1 },

           /* 321-330 */
           {  2,   0,   2,   4,   2 },
           {  0,   0,   0,   0,   3 },
           {  2,  -1,  -2,  -2,  -2 },
           {  2,   0,   0,  -1,   0 },
           {  3,   0,   2,  -4,   2 },
           {  2,   1,   2,   2,   2 },
           {  0,   0,   3,   0,   3 },
           {  1,   1,   2,   2,   1 },
           {  2,   1,   0,   0,  -1 },
           {  1,   2,   0,  -2,   1 },

           /* 331-340 */
           {  3,   0,   2,   2,   1 },
           {  1,  -1,  -2,   2,  -2 },
           {  1,   1,   0,  -1,   0 },
           {  1,   2,   0,   0,   0 },
           {  1,   0,   4,   0,   2 },
           {  1,  -1,   2,   4,   2 },
           {  2,   1,   0,   0,   1 },
           {  1,   0,   0,   2,   2 },
           {  1,  -1,  -2,   2,   0 },
           {  0,   2,  -2,  -2,  -1 },

           /* 341-350 */
           {  2,   0,  -2,   0,   2 },
           {  5,   0,   2,   0,   2 },
           {  3,   0,  -2,  -6,  -2 },
           {  1,  -1,   2,  -1,   2 },
           {  3,   0,   0,  -4,  -1 },
           {  1,   0,   0,   1,   1 },
           {  1,   0,  -4,   2,  -1 },
           {  0,   1,   2,  -4,   1 },
           {  1,   2,   2,   0,   2 },
           {  0,   1,   0,  -2,  -2 },

           /* 351-360 */
           {  0,   0,   2,  -1,   0 },
           {  1,   0,   1,   0,   1 },
           {  0,   2,   0,  -2,   1 },
           {  3,   0,   2,   0,   0 },
           {  1,   1,  -2,   1,   0 },
           {  2,   1,  -2,  -4,  -1 },
           {  3,  -1,   0,   0,   0 },
           {  2,  -1,  -2,   0,   0 },
           {  4,   0,   2,  -2,   1 },
           {  2,   0,  -2,   2,   0 },

           /* 361-370 */
           {  1,   1,   2,  -2,   0 },
           {  1,   0,  -2,   4,  -1 },
           {  1,   0,  -2,  -2,   1 },
           {  2,   0,   2,  -4,   0 },
           {  1,   1,   0,  -2,  -2 },
           {  1,   1,  -2,  -2,   0 },
           {  1,   0,   1,  -2,   1 },
           {  2,  -1,  -2,  -4,  -2 },
           {  3,   0,  -2,   0,  -2 },
           {  0,   1,  -2,  -2,   0 },

           /* 371-380 */
           {  3,   0,   0,  -2,  -1 },
           {  1,   0,  -2,  -3,  -1 },
           {  0,   1,   0,  -4,  -1 },
           {  1,  -2,   2,  -2,   1 },
           {  0,   1,  -2,   1,  -1 },
           {  1,  -1,   0,   0,   2 },
           {  2,   0,   0,   1,   0 },
           {  1,  -2,   0,   2,   0 },
           {  1,   2,  -2,  -2,  -1 },
           {  0,   0,   4,  -4,   1 },

           /* 381-390 */
           {  0,   1,   2,   4,   2 },
           {  0,   1,  -4,   2,  -2 },
           {  3,   0,  -2,   0,   0 },
           {  2,  -1,   2,   2,   1 },
           {  0,   1,  -2,  -4,  -1 },
           {  4,   0,   2,   2,   2 },
           {  2,   0,  -2,  -3,  -2 },
           {  2,   0,   0,  -6,   0 },
           {  1,   0,   2,   0,   3 },
           {  3,   1,   0,   0,   0 },

           /* 391-400 */
           {  3,   0,   0,  -4,   1 },
           {  1,  -1,   2,   0,   0 },
           {  1,  -1,   0,  -4,   0 },
           {  2,   0,  -2,   2,  -2 },
           {  1,   1,   0,  -2,   2 },
           {  4,   0,   0,  -2,   0 },
           {  2,   2,   0,  -2,   0 },
           {  0,   1,   2,   0,   0 },
           {  1,   1,   0,  -4,   1 },
           {  1,   0,   0,  -4,  -2 },

           /* 401-410 */
           {  0,   0,   0,   1,   2 },
           {  3,   0,   0,   2,   1 },
           {  1,   1,   0,  -4,  -1 },
           {  0,   0,   2,   2,  -1 },
           {  1,   1,   2,   0,   0 },
           {  1,  -1,   2,  -4,   1 },
           {  1,   1,   0,   0,   2 },
           {  0,   0,   2,   6,   2 },
           {  4,   0,  -2,  -2,  -1 },
           {  2,   1,   0,  -4,  -1 },

           /* 411-420 */
           {  0,   0,   0,   3,   1 },
           {  1,  -1,  -2,   0,   0 },
           {  0,   0,   2,   1,   0 },
           {  1,   0,   0,   2,  -2 },
           {  3,  -1,   2,   2,   2 },
           {  3,  -1,   2,  -2,   2 },
           {  1,   0,   0,  -1,   2 },
           {  1,  -2,   2,  -2,   2 },
           {  0,   1,   0,   2,   2 },
           {  0,   1,  -2,  -1,  -2 },

           /* 421-430 */
           {  1,   1,  -2,   0,   0 },
           {  0,   2,   2,  -2,   0 },
           {  3,  -1,  -2,  -1,  -2 },
           {  1,   0,   0,  -6,   0 },
           {  1,   0,  -2,  -4,   0 },
           {  2,   1,   0,  -4,   1 },
           {  2,   0,   2,   0,  -1 },
           {  2,   0,  -4,   0,  -1 },
           {  0,   0,   3,   0,   2 },
           {  2,   1,  -2,  -2,  -1 },

           /* 431-440 */
           {  1,  -2,   0,   0,   1 },
           {  2,  -1,   0,  -4,   0 },
           {  0,   0,   0,   3,   0 },
           {  5,   0,   2,  -2,   2 },
           {  1,   2,  -2,  -4,  -2 },
           {  1,   0,   4,  -4,   2 },
           {  0,   0,   4,  -1,   2 },
           {  3,   1,   0,  -4,   0 },
           {  3,   0,   0,  -6,   0 },
           {  2,   0,   0,   2,   2 },

           /* 441-450 */
           {  2,  -2,   2,   0,   2 },
           {  1,   0,   0,  -3,   1 },
           {  1,  -2,  -2,   0,  -2 },
           {  1,  -1,  -2,  -3,  -2 },
           {  0,   0,   2,  -2,  -2 },
           {  2,   0,  -2,  -4,   0 },
           {  1,   0,  -4,   0,   0 },
           {  0,   1,   0,  -1,   0 },
           {  4,   0,   0,   0,  -1 },
           {  3,   0,   2,  -1,   2 },

           /* 451-460 */
           {  3,  -1,   2,   0,   1 },
           {  2,   0,   2,  -1,   1 },
           {  1,   2,   2,  -2,   1 },
           {  1,   1,   0,   2,  -1 },
           {  0,   2,   2,   0,   1 },
           {  3,   1,   2,   0,   1 },
           {  1,   1,   2,   1,   1 },
           {  1,   1,   0,  -1,   1 },
           {  1,  -2,   0,  -2,  -1 },
           {  4,   0,   0,  -4,   0 },

           /* 461-470 */
           {  2,   1,   0,   2,   0 },
           {  1,  -1,   0,   4,   0 },
           {  0,   1,   0,  -2,   2 },
           {  0,   0,   2,   0,  -2 },
           {  1,   0,  -1,   0,   1 },
           {  3,   0,   2,  -2,   0 },
           {  2,   0,   2,   2,   0 },
           {  1,   2,   0,  -4,   0 },
           {  1,  -1,   0,  -3,   0 },
           {  0,   1,   0,   4,   0 },

           /* 471 - 480 */
           {  0,   1,  -2,   0,   0 },
           {  2,   2,   2,  -2,   2 },
           {  0,   0,   0,   1,  -2 },
           {  0,   2,  -2,   0,  -1 },
           {  4,   0,   2,  -4,   2 },
           {  2,   0,  -4,   2,  -2 },
           {  2,  -1,  -2,   0,  -2 },
           {  1,   1,   4,  -2,   2 },
           {  1,   1,   2,  -4,   2 },
           {  1,   0,   2,   3,   2 },

           /* 481-490 */
           {  1,   0,   0,   4,  -1 },
           {  0,   0,   0,   4,   2 },
           {  2,   0,   0,   4,   0 },
           {  1,   1,  -2,   2,   0 },
           {  2,   1,   2,   1,   2 },
           {  2,   1,   2,  -4,   1 },
           {  2,   0,   2,   1,   1 },
           {  2,   0,  -4,  -2,  -1 },
           {  2,   0,  -2,  -6,  -1 },
           {  2,  -1,   2,  -1,   2 },

           /* 491-500 */
           {  1,  -2,   2,   0,   1 },
           {  1,  -2,   0,  -2,   1 },
           {  1,  -1,   0,  -4,  -1 },
           {  0,   2,   2,   2,   2 },
           {  0,   2,  -2,  -4,  -2 },
           {  0,   1,   2,   3,   2 },
           {  0,   1,   0,  -4,   1 },
           {  3,   0,   0,  -2,   1 },
           {  2,   1,  -2,   0,   1 },
           {  2,   0,   4,  -2,   1 },

           /* 501-510 */
           {  2,   0,   0,  -3,  -1 },
           {  2,  -2,   0,  -2,   1 },
           {  2,  -1,   2,  -2,   1 },
           {  1,   0,   0,  -6,  -1 },
           {  1,  -2,   0,   0,  -1 },
           {  1,  -2,  -2,  -2,  -1 },
           {  0,   1,   4,  -2,   1 },
           {  0,   0,   2,   3,   1 },
           {  2,  -1,   0,  -1,   0 },
           {  1,   3,   0,  -2,   0 },

           /* 511-520 */
           {  0,   3,   0,  -2,   0 },
           {  2,  -2,   2,  -2,   2 },
           {  0,   0,   4,  -2,   0 },
           {  4,  -1,   2,   0,   2 },
           {  2,   2,  -2,  -4,  -2 },
           {  4,   1,   2,   0,   2 },
           {  4,  -1,  -2,  -2,  -2 },
           {  2,   1,   0,  -2,  -2 },
           {  2,   1,  -2,  -6,  -2 },
           {  2,   0,   0,  -1,   1 },

           /* 521-530 */
           {  2,  -1,  -2,   2,  -1 },
           {  1,   1,  -2,   2,  -2 },
           {  1,   1,  -2,  -3,  -2 },
           {  1,   0,   3,   0,   3 },
           {  1,   0,  -2,   1,   1 },
           {  1,   0,  -2,   0,   2 },
           {  1,  -1,   2,   1,   2 },
           {  1,  -1,   0,   0,  -2 },
           {  1,  -1,  -4,   2,  -2 },
           {  0,   3,  -2,  -2,  -2 },

           /* 531-540 */
           {  0,   1,   0,   4,   1 },
           {  0,   0,   4,   2,   2 },
           {  3,   0,  -2,  -2,   0 },
           {  2,  -2,   0,   0,   0 },
           {  1,   1,   2,  -4,   0 },
           {  1,   1,   0,  -3,   0 },
           {  1,   0,   2,  -3,   0 },
           {  1,  -1,   2,  -2,   0 },
           {  0,   2,   0,   2,   0 },
           {  0,   0,   2,   4,   0 },

           /* 541-550 */
           {  1,   0,   1,   0,   0 },
           {  3,   1,   2,  -2,   1 },
           {  3,   0,   4,  -2,   2 },
           {  3,   0,   2,   1,   2 },
           {  3,   0,   0,   2,  -1 },
           {  3,   0,   0,   0,   2 },
           {  3,   0,  -2,   2,  -1 },
           {  2,   0,   4,  -4,   2 },
           {  2,   0,   2,  -3,   2 },
           {  2,   0,   0,   4,   1 },

           /* 551-560 */
           {  2,   0,   0,  -3,   1 },
           {  2,   0,  -4,   2,  -1 },
           {  2,   0,  -2,  -2,   1 },
           {  2,  -2,   2,   2,   2 },
           {  2,  -2,   0,  -2,  -2 },
           {  2,  -1,   0,   2,   1 },
           {  2,  -1,   0,   2,  -1 },
           {  1,   1,   2,   4,   2 },
           {  1,   1,   0,   1,   1 },
           {  1,   1,   0,   1,  -1 },

           /* 561-570 */
           {  1,   1,  -2,  -6,  -2 },
           {  1,   0,   0,  -3,  -1 },
           {  1,   0,  -4,  -2,  -1 },
           {  1,   0,  -2,  -6,  -1 },
           {  1,  -2,   2,   2,   1 },
           {  1,  -2,  -2,   2,  -1 },
           {  1,  -1,  -2,  -4,  -1 },
           {  0,   2,   0,   0,   2 },
           {  0,   1,   2,  -4,   2 },
           {  0,   1,  -2,   4,  -1 },

           /* 571-580 */
           {  5,   0,   0,   0,   0 },
           {  3,   0,   0,  -3,   0 },
           {  2,   2,   0,  -4,   0 },
           {  1,  -1,   2,   2,   0 },
           {  0,   1,   0,   3,   0 },
           {  4,   0,  -2,   0,  -1 },
           {  3,   0,  -2,  -6,  -1 },
           {  3,   0,  -2,  -1,  -1 },
           {  2,   1,   2,   2,   1 },
           {  2,   1,   0,   2,   1 },

           /* 581-590 */
           {  2,   0,   2,   4,   1 },
           {  2,   0,   2,  -6,   1 },
           {  2,   0,   2,  -2,  -1 },
           {  2,   0,   0,  -6,  -1 },
           {  2,  -1,  -2,  -2,  -1 },
           {  1,   2,   2,   0,   1 },
           {  1,   2,   0,   0,   1 },
           {  1,   0,   4,   0,   1 },
           {  1,   0,   2,  -6,   1 },
           {  1,   0,   2,  -4,  -1 },

           /* 591-600 */
           {  1,   0,  -1,  -2,  -1 },
           {  1,  -1,   2,   4,   1 },
           {  1,  -1,   2,  -3,   1 },
           {  1,  -1,   0,   4,   1 },
           {  1,  -1,  -2,   1,  -1 },
           {  0,   1,   2,  -2,   3 },
           {  3,   0,   0,  -2,   0 },
           {  1,   0,   1,  -2,   0 },
           {  0,   2,   0,  -4,   0 },
           {  0,   0,   2,  -4,   0 },

           /* 601-610 */
           {  0,   0,   1,  -1,   0 },
           {  0,   0,   0,   6,   0 },
           {  0,   2,   0,   0,  -2 },
           {  0,   1,  -2,   2,  -3 },
           {  4,   0,   0,   2,   0 },
           {  3,   0,   0,  -1,   0 },
           {  3,  -1,   0,   2,   0 },
           {  2,   1,   0,   1,   0 },
           {  2,   1,   0,  -6,   0 },
           {  2,  -1,   2,   0,   0 },

           /* 611-620 */
           {  1,   0,   2,  -1,   0 },
           {  1,  -1,   0,   1,   0 },
           {  1,  -1,  -2,  -2,   0 },
           {  0,   1,   2,   2,   0 },
           {  0,   0,   2,  -3,   0 },
           {  2,   2,   0,  -2,  -1 },
           {  2,  -1,  -2,   0,   1 },
           {  1,   2,   2,  -4,   1 },
           {  0,   1,   4,  -4,   2 },
           {  0,   0,   0,   3,   2 },

           /* 621-630 */
           {  5,   0,   2,   0,   1 },
           {  4,   1,   2,  -2,   2 },
           {  4,   0,  -2,  -2,   0 },
           {  3,   1,   2,   2,   2 },
           {  3,   1,   0,  -2,   0 },
           {  3,   1,  -2,  -6,  -2 },
           {  3,   0,   0,   0,  -2 },
           {  3,   0,  -2,  -4,  -2 },
           {  3,  -1,   0,  -3,   0 },
           {  3,  -1,   0,  -2,   0 },

           /* 631-640 */
           {  2,   1,   2,   0,   0 },
           {  2,   1,   2,  -4,   2 },
           {  2,   1,   2,  -2,   0 },
           {  2,   1,   0,  -3,   0 },
           {  2,   1,  -2,   0,  -2 },
           {  2,   0,   0,  -4,   2 },
           {  2,   0,   0,  -4,  -2 },
           {  2,   0,  -2,  -5,  -2 },
           {  2,  -1,   2,   4,   2 },
           {  2,  -1,   0,  -2,   2 },

           /* 641-650 */
           {  1,   3,  -2,  -2,  -2 },
           {  1,   1,   0,   0,  -2 },
           {  1,   1,   0,  -6,   0 },
           {  1,   1,  -2,   1,  -2 },
           {  1,   1,  -2,  -1,  -2 },
           {  1,   0,   2,   1,   0 },
           {  1,   0,   0,   3,   0 },
           {  1,   0,   0,  -4,   2 },
           {  1,   0,  -2,   4,  -2 },
           {  1,  -2,   0,  -1,   0 },

           /* 651-NFLS */
           {  0,   1,  -4,   2,  -1 },
           {  1,   0,  -2,   0,  -3 },
           {  0,   0,   4,  -4,   4 }
       };

       /* Number of frequencies:  luni-solar */
       static const int NFLS = (int) (sizeof mfals / sizeof (int) / 5);

       /* Fundamental-argument multipliers:  planetary terms */
       static const int mfapl[][14] = {

           /* 1-10 */
           {  0,  0,  1, -1,  1,  0,  0, -1,  0, -2,  5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -5,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  1,  0, -8, 12,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  8,-16,  4,  5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0, -1,  2,  0,  0,  0,  0,  0 },

           /* 11-20 */
           {  0,  0,  0,  0,  0,  0,  8,-13,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  2, -5,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0, -5,  6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  4, -6,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0, -1,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -8,  3,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  2, -4,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  6, -8,  3,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  2, -3,  0,  0,  0,  0,  0,  0 },

           /* 21-30 */
           {  0,  0,  0,  0,  0,  0,  2, -2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  1,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  1, -1,  1,  0,  0,  0, -2,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2, -1,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  1 },
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  2,  0,  0,  0,  0 },

           /* 31-40 */
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8,-13,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  5, -8,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -5,  0,  0,  1 },
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0, -1,  0,  0,  0 },

           /* 41-50 */
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -7,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  0,  0,  0,  0, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4,  0, -2,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  8,-13,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  2, -1,  0,  0,  0,  0,  0,  2 },
           {  1,  0,  0,  0,  0,  0,-18, 16,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  1,  0,  0,  0,  2 },

           /* 51-60 */
           {  0,  0,  1, -1,  1,  0, -5,  7,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0,-10,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  0,  0, -5,  6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -1,  0,  0,  0,  2 },
           {  1,  0,  2,  0,  2,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -2,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  1 },
           {  1,  0, -2,  0, -2,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0,  2,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0, -3,  3,  0,  0,  0,  0,  0,  0 },

           /* 61-70 */
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  8,-16,  4,  5,  0,  0, -2 },
           {  0,  0,  1, -1,  1,  0,  0,  3, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8,-11,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  8,-16,  4,  5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  4, -6,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -3,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -4,  0,  0,  0,  0,  0 },

           /* 71-80 */
           {  0,  0,  0,  0,  0,  0,  6, -8,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  3, -2,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  8,-15,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  2, -5,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  1, -3,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0, -2,  0,  0,  0,  2 },
           {  0,  0,  1, -1,  1,  0,  0, -5,  8, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  2,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -2,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0,  0,  0,  0 },

           /* 81-90 */
           {  2,  0,  0, -2,  1,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -8,  0,  0,  0,  0,  0, -1 },
           {  2,  0,  0, -2,  0,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  8,-13,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  0,  0, -2,  5,  0,  0,  0 },
           {  1,  0,  0, -1,  0,  0, -3,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  2 },
           {  1,  0,  0,  0, -1,  0,-18, 16,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  0,  0,  2, -5,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  0,  0,  1,  0,  0,  0,  0 },

           /* 91-100 */
           {  1,  0,  0, -2,  0,  0, 19,-21,  3,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0, -8, 13,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0,  1,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  7, -9,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  2 },
           {  1,  0,  0,  0,  1,  0,-18, 16,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  2, -4,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  6,-16,  4,  5,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  4, -7,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  3, -7,  0,  0,  0,  0,  0, -2 },

           
           /* 101-110 */
           {  0,  0,  0,  0,  0,  0,  2, -2,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  1 },
           {  2,  0,  0, -2,  1,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -4,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2, -1,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  2,  0,  0,  0,  2 },
           
           
           /* 111-120 */
           {  0,  0,  0,  0,  1,  0,  0,  1, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  2 },
           {  0,  0,  2, -2,  1,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  3, -3,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  4, -4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0, -1,  0, -1,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0, -6,  8,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -2,  2,  0,  0,  0,  0,  0 },
           
           
           /* 121-130 */
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -3,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -4,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, -1,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8,-10,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  1,  0, -3,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  6, -9,  0,  0,  0,  0,  0, -2 },
           {  1,  0,  0, -1,  1,  0,  0, -1,  0,  2,  0,  0,  0,  0 },
           
           
           /* 131-140 */
           {  0,  0,  0,  0,  0,  0,  5, -7,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  5, -5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  3, -3,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4,  0, -3,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  1,  0,  2, -3,  0,  0,  0,  0,  0,  0 },
           
           
           /* 141-150 */
           {  1,  0,  0, -1,  0,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1, -3,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -4,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -4,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  9,-11,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  2, -3,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  8,-15,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -4,  5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  4, -6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4,  0, -1,  0,  0,  0,  2 },
           
           
           /* 151-160 */
           {  1,  0,  0, -1,  1,  0, -3,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1,  1,  1,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0, -4, 10,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  1, -1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0, -1,  0,  0, -1,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -1,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -4,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -5,  0,  0, -2 },
           {  0,  0,  2, -2,  1,  0, -4,  4,  0,  0,  0,  0,  0,  0 },
           
           
           /* 161-170 */
           {  0,  0,  0,  0,  0,  0,  0,  3,  0,  0, -1,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -3,  0,  0,  0,  0,  2 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0,  0,  0,  2,  0 },
           {  0,  0,  0,  0,  0,  0,  4, -4,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -4,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  5, -8,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  1,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -9, 13,  0,  0,  0,  0,  0 },
           {  2,  0,  2,  0,  2,  0,  0,  2,  0, -3,  0,  0,  0,  0 },
           
           /* 171-180 */
           {  0,  0,  0,  0,  0,  0,  3, -6,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  2,  0,  0, -1,  0,  0,  2,  0,  0,  0 },
           {  1,  0,  0, -1, -1,  0, -3,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -6,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  6, -6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  1 },
           {  1,  0,  2,  0,  1,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  1,  0, -2,  0, -1,  0,  0, -1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0, -2,  4,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0,  0,  0 },

           /* 181-190 */
           {  0,  0,  0,  0,  0,  0,  2,  1,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  2,  0,  2,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1, -8,  3,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  6,-10,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  7, -8,  3,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  1,  0, -3,  5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -1,  0,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0, -5,  7,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -2,  0,  0,  0,  1 },
           
           /* 191-200 */
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  7,-10,  0,  0,  0,  0,  0, -2 },
           {  1,  0,  0, -2,  0,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  2, -5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  6, -8,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  1, -1,  1,  0,  0, -9, 15,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0, -2,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0, -1,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -6,  0,  0,  0,  0,  0 },
           
           /* 201-210 */
           {  0,  0,  0,  0,  0,  0,  0,  1, -4,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0, -1,  0,  0,  2 },
           {  2,  0,  0, -2,  1,  0, -6,  8,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -5,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  1, -1,  1,  0,  3, -6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  8,-14,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           
           /* 211-220 */
           {  0,  0,  0,  0,  1,  0,  0,  8,-15,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -6,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  7, -7,  0,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  1,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -1,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  1,  0,  0,  2 },
           {  2,  0, -1, -1,  0,  0,  0,  3, -7,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -7,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -3,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -3,  4,  0,  0,  0,  0,  0 },
           
           /* 221-230 */
           {  2,  0,  0, -2,  0,  0,  0, -6,  8,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0,  0, -5,  6,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  0,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  2,  1,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  1,  2,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  1,  0,  0,  1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -9,  4,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0,  0, -2 },
           
           /* 231-240 */
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -4,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  1 },
           {  0,  0,  0,  0,  0,  0,  7,-11,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  3, -5,  4,  0,  0,  0,  0,  2 },
           {  0,  0,  1, -1,  0,  0,  0, -1,  0, -1,  1,  0,  0,  0 },
           {  2,  0,  0,  0,  0,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  8,-15,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  2,  0,  0, -2,  2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  6, -6,  0,  0,  0,  0,  0, -1 },
           
           /* 241-250 */
           {  0,  0,  1, -1,  1,  0,  0, -1,  0, -1,  1,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  2, -2,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -7,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  2, -4,  0, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  3, -5,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0, -3,  0,  0,  0,  2 },
           {  0,  0,  2, -2,  2,  0, -8, 11,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, -2,  0,  0,  0 },
           
           /* 251-260 */
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  1,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -9,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -5,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  7, -9,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  4, -7,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  2, -1,  0,  0,  0,  0,  0,  0 },
           {  1,  0, -2, -2, -2,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -2,  5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  3, -3,  0,  0,  0,  0,  0,  1 },
           
           /* 261-270 */
           {  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  2, -5,  0,  0,  2 },
           {  2,  0,  0, -2, -1,  0,  0, -2,  0,  0,  5,  0,  0,  0 },
           {  2,  0,  0, -2, -1,  0, -6,  8,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8, -8,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0,  2, -5,  0,  0,  2 },
           {  0,  0,  0,  0,  1,  0,  3, -7,  4,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           
           /* 271-280 */
           {  0,  0,  1, -1,  0,  0,  0, -1,  0, -2,  5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -1,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  2, -3,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0, 11,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  6,-15,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0,  1,  0,  0,  0,  2 },
           {  1,  0,  0, -1,  0,  0,  0, -3,  4,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0, -3,  7, -4,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5,  0, -2,  0,  0,  0,  2 },
           
           /* 281-290 */
           {  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  2, -2,  2,  0, -5,  6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  2,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  4, -4,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -8,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -5,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -7,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  6,-11,  0,  0,  0,  0, -2 },
           
           /* 291-300 */
           {  0,  0,  0,  0,  0,  0,  0,  1, -3,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0, -1,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  9,-12,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  4, -4,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  1, -1,  0,  0, -8, 12,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -2,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  7, -7,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -6,  0,  0,  0,  0, -1 },
           
           /* 301-310 */
           {  0,  0,  0,  0,  0,  0,  0,  6, -6,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  1,  0, -4,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  1,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  6, -9,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  1, -1, -1,  0,  0,  0, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1, -5,  0,  0,  0,  0, -2 },
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  3, -1,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0, -2,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -9,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -6,  0,  0,  0,  0,  0,  2 },
           
           /* 311-320 */
           {  0,  0,  0,  0,  0,  0,  9, -9,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0,  3,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  2, -4,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -3,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  1 },
           {  0,  0,  1, -1,  2,  0,  0, -1,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -9,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -3,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  2 },
           {  0,  0,  2,  0,  2,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           
           /* 321-330 */
           {  0,  0,  2,  0,  2,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5,  0, -3,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0 },
           {  2,  0, -1, -1, -1,  0,  0, -1,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  4, -3,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  4, -2,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  5,-10,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  8,-13,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  2, -2,  1, -1,  0,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0,  0,  2,  0,  0 },
           
           /* 331-340 */
           {  0,  0,  0,  0,  1,  0,  3, -5,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  0,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  9, -9,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2,  0,  2,  0,  1, -1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -8, 11,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -2,  0,  0,  2,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0, -1,  2,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -5,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  2, -6,  0,  0,  0,  0,  0, -2 },
           
           /* 341-350 */
           {  0,  0,  0,  0,  0,  0,  0,  8,-15,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -2,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  7,-13,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0, -2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  3,  0,  0,  0,  2 },
           {  0,  0,  2, -2,  1,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8, -8,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  8,-10,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  4, -2,  0,  0,  0,  0,  0,  1 },
           
           /* 351-360 */
           {  0,  0,  0,  0,  0,  0,  3, -6,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  3, -4,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -4,  0,  0,  0,  0 },
           {  2,  0,  0, -2, -1,  0,  0, -5,  6,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -5,  0,  0,  0,  0, -2 },
           {  2,  0, -1, -1, -1,  0,  0,  3, -7,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -8,  0,  0,  0,  0,  0 },
           {  0,  0,  2,  0,  2,  0, -1,  1,  0,  0,  0,  0,  0,  0 },
           
           /* 361-370 */
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  4, -3,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  6,-11,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  1,  0,  0, -6,  8,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -8,  1,  5,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  6, -5,  0,  0,  0,  0,  2 },
           {  1,  0, -2, -2, -2,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  2,  0,  0,  0, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  2,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  2,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  6,  0,  0,  0,  0,  0,  1 },
           
           /* 371-380 */
           {  0,  0,  0,  0,  0,  0,  0,  6, -7,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4,  0,  0, -2,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0,  0, -2,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -1,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  1, -6,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  4, -5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  3, -5,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  7,-13,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -2,  0,  0,  0,  2 },
           
           /* 381-390 */
           {  0,  0,  1, -1,  0,  0,  0, -1,  0,  0,  2,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0, -8, 15,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2, -2,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  2,  0, -1, -1, -1,  0,  0, -1,  0,  2,  0,  0,  0,  0 },
           {  1,  0,  2, -2,  2,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  1,  0, -1,  1, -1,  0,-18, 17,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2,  0,  2,  0,  0,  1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  2,  0,  2,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  2, -2, -1,  0, -5,  6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  2,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           
           /* 391-400 */
           {  0,  0,  0,  0,  1,  0,  2, -2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8,-16,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  2 },
           {  0,  0,  0,  0,  2,  0,  0, -1,  2,  0,  0,  0,  0,  0 },
           {  2,  0, -1, -1, -2,  0,  0, -1,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  6,-10,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0, -2,  4,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  2,  0,  0,  0,  0,  2 },
           {  2,  0,  0, -2, -1,  0,  0, -2,  0,  4, -5,  0,  0,  0 },
           
           /* 401-410 */
           {  2,  0,  0, -2, -1,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  2,  0, -1, -1, -1,  0,  0, -1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  1, -1,  1,  0,  0, -1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0, -1, -1,  0,  0, -2,  2,  0,  0,  0,  0,  0 },
           {  1,  0, -1, -1, -1,  0, 20,-20,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  1, -2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -2,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  5, -8,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  0,  0,  0, -1,  0,  0,  0 },
           
           /* 411-420 */
           {  0,  0,  0,  0,  0,  0,  9,-11,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  5, -3,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -3,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  6, -7,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0, -2,  0,  0,  0 },
           {  0,  0,  1, -1,  2,  0,  0, -1,  0, -2,  5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -7,  0,  0,  0,  0,  0 },
           
           /* 421-430 */
           {  0,  0,  0,  0,  0,  0,  1, -3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -8,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -6,  0,  0,  0,  0, -2 },
           {  1,  0,  0, -2,  0,  0, 20,-21,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8,-12,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  5, -6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -4,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  2,  0,  0, -1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  8,-12,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  9,-17,  0,  0,  0,  0,  0 },
           
           /* 431-440 */
           {  0,  0,  0,  0,  0,  0,  0,  5, -6,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -8,  1,  5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -6,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -7,  0,  0,  0,  0, -2 },
           {  1,  0,  0, -1,  1,  0,  0, -3,  4,  0,  0,  0,  0,  0 },
           {  1,  0, -2,  0, -2,  0,-10,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0, -9, 17,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1, -4,  0,  0,  0,  0,  0, -2 },
           {  1,  0, -2, -2, -2,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  1,  0, -1,  1, -1,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           
           /* 441-450 */
           {  0,  0,  2, -2,  2,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  2,  0,  0, -1,  0,  0,  1,  0,  0,  0 },
           {  0,  0,  1, -1,  2,  0, -5,  7,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  2, -2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  4, -5,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  3, -4,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  2, -4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5,-10,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  4,  0, -4,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0, -5,  0,  0,  0, -2 },
           
           /* 451-460 */
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -5,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -2,  5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -2,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  2, -3,  0,  0,  0,  0,  0,  1 },
           {  1,  0,  0, -2,  0,  0,  0,  1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -7,  4,  0,  0,  0,  0,  0 },
           {  2,  0,  2,  0,  1,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1, -1,  0,  0, -1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  1,  0, -2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  6,-10,  0,  0,  0,  0, -2 },
           
           
           /* 461-470 */
           {  1,  0,  0, -1,  1,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0,  1,  0, -1,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -3,  0,  3,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0, -5,  5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  1, -3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -4,  6,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  0,  0, -1,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -5,  6,  0,  0,  0,  0,  0,  0 },
           
           
           /* 471-480 */
           {  0,  0,  0,  0,  1,  0,  3, -4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  7,-10,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  5, -5,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  4, -5,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  3, -8,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  2, -5,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  7, -9,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  7, -8,  0,  0,  0,  0,  2 },
           
           
           /* 481-490 */
           {  0,  0,  0,  0,  0,  0,  0,  3,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -8,  3,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0, -2,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -4,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -1,  0,  0,  0, -1 },
           {  2,  0,  0, -2, -1,  0,  0, -6,  8,  0,  0,  0,  0,  0 },
           {  2,  0, -1, -1,  1,  0,  0,  3, -7,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -7,  9,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0,  0, -1 },
           
           
           /* 491-500 */
           {  0,  0,  1, -1,  2,  0, -8, 12,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0,  2, -2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  7, -8,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  1,  0,  0, -5,  6,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2, -1,  0,  0, -2,  0,  3, -1,  0,  0,  0 },
           {  1,  0,  1,  1,  1,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  1,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  1,  0,  0, -2, -1,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           
           
           /* 501-510 */
           {  1,  0,  0, -1, -1,  0,  0, -3,  4,  0,  0,  0,  0,  0 },
           {  1,  0, -1,  0, -1,  0, -3,  5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -4,  4,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0, -8, 11,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  0,  0,  0, -9, 13,  0,  0,  0,  0,  0 },
           {  0,  0,  1,  1,  2,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0,  1, -4,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0,  0, -1,  0,  1, -3,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0,  7,-13,  0,  0,  0,  0,  0 },
           
           
           /* 511-520 */
           {  0,  0,  0,  0,  1,  0,  0,  2,  0, -2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0, -2,  2,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0, -3,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  1,  0, -4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  7,-11,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  6, -6,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  6, -4,  0,  0,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  5, -6,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  4, -2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -4,  0,  0,  0,  0,  0,  1 },
           
           
           /* 521-530 */
           {  0,  0,  0,  0,  0,  0,  1, -4,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  9,-17,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  7, -7,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -8,  3,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -8,  3,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -8,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4, -7,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  1 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -4,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           
           
           /* 531-540 */
           {  2,  0,  0, -2,  0,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0, -1,  1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0, 17,-16,  0, -2,  0,  0,  0,  0 },
           {  1,  0,  0, -1,  0,  0,  0, -2,  2,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  0,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  6, -9,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  3,  0, -4,  0,  0,  0,  0 },
           
           
           /* 541-550 */
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -2, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  1,  0,  0,  0,  0,  2 },
           {  2,  0,  0, -2,  0,  0,  0, -4,  4,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  2,  2,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0,  1, -1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           
           
           /* 551-560 */
           {  1,  0,  0, -2,  0,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  0,  0, -4,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  3, -6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0, -2,  2,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0, -1,  0,  0,  1,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0, -4,  5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0, -3,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  2,  0,  0,  0, -1,  0,  1,  0,  0,  0,  0 },
           
           
           /* 561-570 */
           {  0,  0,  0,  0,  0,  0,  8, -9,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  3, -5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -2,  0,  0,  0 },
           {  2,  0, -2, -2, -2,  0,  0, -2,  0,  2,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  1,  0,-10,  3,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0,  0, -1,  0,-10,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2,  0,  2,  0,  2, -3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2,  0,  2,  0,  2, -2,  0,  0,  0,  0,  0,  0 },
           
           
           /* 571-580 */
           {  0,  0,  2,  0,  2,  0, -2,  3,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2,  0,  2,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  2,  0,  0,  0,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0,  0, -1,  0,  2,  0,  0,  0,  0 },
           {  2,  0,  2, -2,  2,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  2,  0,  1, -3,  1,  0, -6,  7,  0,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0,  2, -5,  0,  0,  0,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  5, -5,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  1,  5,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  0,  5,  0,  0,  0 },
           
           
           /* 581-590 */
           {  2,  0,  0, -2,  0,  0,  0, -2,  0,  0,  2,  0,  0,  0 },
           {  2,  0,  0, -2,  0,  0, -4,  4,  0,  0,  0,  0,  0,  0 },
           {  2,  0, -2,  0, -2,  0,  0,  5, -9,  0,  0,  0,  0,  0 },
           {  2,  0, -1, -1,  0,  0,  0, -1,  0,  3,  0,  0,  0,  0 },
           {  1,  0,  2,  0,  2,  0,  1, -1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  2,  0,  2,  0,  0,  4, -8,  3,  0,  0,  0,  0 },
           {  1,  0,  2,  0,  2,  0,  0, -4,  8, -3,  0,  0,  0,  0 },
           {  1,  0,  2,  0,  2,  0, -1,  1,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  2, -2,  2,  0, -3,  3,  0,  0,  0,  0,  0,  0 },
           {  1,  0,  0,  0,  0,  0,  0,  1,  0, -1,  0,  0,  0,  0 },

           /* 591-600 */
           {  1,  0,  0,  0,  0,  0,  0, -2,  0,  3,  0,  0,  0,  0 },
           {  1,  0,  0, -2,  0,  0,  0,  2,  0, -2,  0,  0,  0,  0 },
           {  1,  0, -2, -2, -2,  0,  0,  1,  0, -1,  0,  0,  0,  0 },
           {  1,  0, -1,  1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  1,  0, -1, -1,  0,  0,  0,  8,-15,  0,  0,  0,  0,  0 },
           {  0,  0,  2,  2,  2,  0,  0,  2,  0, -2,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  1, -1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0, -2,  0,  1,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  1,  0,  0,-10, 15,  0,  0,  0,  0,  0 },
           {  0,  0,  2, -2,  0, -1,  0,  2,  0,  0,  0,  0,  0,  0 },

           /* 601-610 */
           {  0,  0,  1, -1,  2,  0,  0, -1,  0,  0, -1,  0,  0,  0 },
           {  0,  0,  1, -1,  2,  0, -3,  4,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -4,  6,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  1,  0, -1,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0,  0, -1,  0,  0, -2,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1,  0,  0, -1,  0,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  1, -1, -1,  0, -5,  7,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  2,  0,  0,  0,  2,  0, -2,  0,  0,  0,  0 },

           /* 611-620 */
           {  0,  0,  0,  2,  0,  0, -2,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  2,  0, -3,  5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  1,  0, -1,  2,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  9,-13,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  8,-14,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  8,-11,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  6, -9,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  6, -8,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  6, -7,  0,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  5, -6,  0,  0,  0,  0,  0, -2 },

           /* 621-630 */
           {  0,  0,  0,  0,  0,  0,  5, -6, -4,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  5, -4,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  4, -8,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  4, -5,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  3, -3,  0,  2,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  3, -1,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  2,  0,  0,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  1, -1,  0,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  7,-12,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  6, -9,  0,  0,  0,  0, -2 },

           /* 631-640 */
           {  0,  0,  0,  0,  0,  0,  0,  6, -8,  1,  5,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  6, -4,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  6,-10,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5,  0, -4,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -9,  0,  0,  0,  0, -1 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -8,  3,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -7,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  5, -6,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  5,-16,  4,  5,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  5,-13,  0,  0,  0,  0, -2 },

           /* 641-650 */
           {  0,  0,  0,  0,  0,  0,  0,  3,  0, -5,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -9,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  3, -7,  0,  0,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  2,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  2,  0,  0, -3,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  2, -8,  1,  5,  0,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  1, -5,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  2,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0,  0, -3,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  1,  0, -3,  5,  0,  0,  0 },

           /* 651-NFPL */
           {  0,  0,  0,  0,  0,  0,  0,  1, -3,  0,  0,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  2, -6,  3,  0, -2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  1, -2,  0,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  2 },
           {  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0 }
       };

       /* Number of frequencies:  planetary */
       static const int NFPL = (int) (sizeof mfapl / sizeof (int) / 14);

       /* Pointers into amplitudes array, one pointer per frequency */
       static const int nc[] = {

           /* 1-100 */
           1,    21,    37,    51,    65,    79,    91,   103,   115,   127,
         139,   151,   163,   172,   184,   196,   207,   219,   231,   240,
         252,   261,   273,   285,   297,   309,   318,   327,   339,   351,
         363,   372,   384,   396,   405,   415,   423,   435,   444,   452,
         460,   467,   474,   482,   490,   498,   506,   513,   521,   528,
         536,   543,   551,   559,   566,   574,   582,   590,   597,   605,
         613,   620,   628,   636,   644,   651,   658,   666,   674,   680,
         687,   695,   702,   710,   717,   725,   732,   739,   746,   753,
         760,   767,   774,   782,   790,   798,   805,   812,   819,   826,
         833,   840,   846,   853,   860,   867,   874,   881,   888,   895,

         /* 101-200 */
         901,   908,   914,   921,   928,   934,   941,   948,   955,   962,
         969,   976,   982,   989,   996,  1003,  1010,  1017,  1024,  1031,
         1037,  1043,  1050,  1057,  1064,  1071,  1078,  1084,  1091,  1098,
         1104,  1112,  1118,  1124,  1131,  1138,  1145,  1151,  1157,  1164,
         1171,  1178,  1185,  1192,  1199,  1205,  1212,  1218,  1226,  1232,
         1239,  1245,  1252,  1259,  1266,  1272,  1278,  1284,  1292,  1298,
         1304,  1310,  1316,  1323,  1329,  1335,  1341,  1347,  1353,  1359,
         1365,  1371,  1377,  1383,  1389,  1396,  1402,  1408,  1414,  1420,
         1426,  1434,  1440,  1446,  1452,  1459,  1465,  1471,  1477,  1482,
         1488,  1493,  1499,  1504,  1509,  1514,  1520,  1527,  1532,  1538,

         /* 201-300 */
         1543,  1548,  1553,  1558,  1564,  1569,  1574,  1579,  1584,  1589,
         1594,  1596,  1598,  1600,  1602,  1605,  1608,  1610,  1612,  1617,
         1619,  1623,  1625,  1627,  1629,  1632,  1634,  1640,  1642,  1644,
         1646,  1648,  1650,  1652,  1654,  1658,  1660,  1662,  1664,  1668,
         1670,  1672,  1673,  1675,  1679,  1681,  1683,  1684,  1686,  1688,
         1690,  1693,  1695,  1697,  1701,  1703,  1705,  1707,  1709,  1711,
         1712,  1715,  1717,  1721,  1723,  1725,  1727,  1729,  1731,  1733,
         1735,  1737,  1739,  1741,  1743,  1745,  1747,  1749,  1751,  1753,
         1755,  1757,  1759,  1761,  1762,  1764,  1766,  1768,  1769,  1771,
         1773,  1775,  1777,  1779,  1781,  1783,  1785,  1787,  1788,  1790,

         /* 301-400 */
         1792,  1794,  1796,  1798,  1800,  1802,  1804,  1806,  1807,  1809,
         1811,  1815,  1817,  1819,  1821,  1823,  1825,  1827,  1829,  1831,
         1833,  1835,  1837,  1839,  1840,  1842,  1844,  1848,  1850,  1852,
         1854,  1856,  1858,  1859,  1860,  1862,  1864,  1866,  1868,  1869,
         1871,  1873,  1875,  1877,  1879,  1881,  1883,  1885,  1887,  1889,
         1891,  1892,  1896,  1898,  1900,  1901,  1903,  1905,  1907,  1909,
         1910,  1911,  1913,  1915,  1919,  1921,  1923,  1927,  1929,  1931,
         1933,  1935,  1937,  1939,  1943,  1945,  1947,  1948,  1949,  1951,
         1953,  1955,  1957,  1958,  1960,  1962,  1964,  1966,  1968,  1970,
         1971,  1973,  1974,  1975,  1977,  1979,  1980,  1981,  1982,  1984,

         /* 401-500 */
         1986,  1988,  1990,  1992,  1994,  1995,  1997,  1999,  2001,  2003,
         2005,  2007,  2008,  2009,  2011,  2013,  2015,  2017,  2019,  2021,
         2023,  2024,  2025,  2027,  2029,  2031,  2033,  2035,  2037,  2041,
         2043,  2045,  2046,  2047,  2049,  2051,  2053,  2055,  2056,  2057,
         2059,  2061,  2063,  2065,  2067,  2069,  2070,  2071,  2072,  2074,
         2076,  2078,  2080,  2082,  2084,  2086,  2088,  2090,  2092,  2094,
         2095,  2096,  2097,  2099,  2101,  2105,  2106,  2107,  2108,  2109,
         2110,  2111,  2113,  2115,  2119,  2121,  2123,  2125,  2127,  2129,
         2131,  2133,  2135,  2136,  2137,  2139,  2141,  2143,  2145,  2147,
         2149,  2151,  2153,  2155,  2157,  2159,  2161,  2163,  2165,  2167,

         /* 501-600 */
         2169,  2171,  2173,  2175,  2177,  2179,  2181,  2183,  2185,  2186,
         2187,  2188,  2192,  2193,  2195,  2197,  2199,  2201,  2203,  2205,
         2207,  2209,  2211,  2213,  2217,  2219,  2221,  2223,  2225,  2227,
         2229,  2231,  2233,  2234,  2235,  2236,  2237,  2238,  2239,  2240,
         2241,  2244,  2246,  2248,  2250,  2252,  2254,  2256,  2258,  2260,
         2262,  2264,  2266,  2268,  2270,  2272,  2274,  2276,  2278,  2280,
         2282,  2284,  2286,  2288,  2290,  2292,  2294,  2296,  2298,  2300,
         2302,  2303,  2304,  2305,  2306,  2307,  2309,  2311,  2313,  2315,
         2317,  2319,  2321,  2323,  2325,  2327,  2329,  2331,  2333,  2335,
         2337,  2341,  2343,  2345,  2347,  2349,  2351,  2352,  2355,  2356,

         /* 601-700 */
         2357,  2358,  2359,  2361,  2363,  2364,  2365,  2366,  2367,  2368,
         2369,  2370,  2371,  2372,  2373,  2374,  2376,  2378,  2380,  2382,
         2384,  2385,  2386,  2387,  2388,  2389,  2390,  2391,  2392,  2393,
         2394,  2395,  2396,  2397,  2398,  2399,  2400,  2401,  2402,  2403,
         2404,  2405,  2406,  2407,  2408,  2409,  2410,  2411,  2412,  2413,
         2414,  2415,  2417,  2418,  2430,  2438,  2445,  2453,  2460,  2468,
         2474,  2480,  2488,  2496,  2504,  2512,  2520,  2527,  2535,  2543,
         2550,  2558,  2566,  2574,  2580,  2588,  2596,  2604,  2612,  2619,
         2627,  2634,  2642,  2648,  2656,  2664,  2671,  2679,  2685,  2693,
         2701,  2709,  2717,  2725,  2733,  2739,  2747,  2753,  2761,  2769,

         /* 701-800 */
         2777,  2785,  2793,  2801,  2809,  2817,  2825,  2833,  2841,  2848,
         2856,  2864,  2872,  2878,  2884,  2892,  2898,  2906,  2914,  2922,
         2930,  2938,  2944,  2952,  2958,  2966,  2974,  2982,  2988,  2996,
         3001,  3009,  3017,  3025,  3032,  3039,  3045,  3052,  3059,  3067,
         3069,  3076,  3083,  3090,  3098,  3105,  3109,  3111,  3113,  3120,
         3124,  3128,  3132,  3136,  3140,  3144,  3146,  3150,  3158,  3161,
         3165,  3166,  3168,  3172,  3176,  3180,  3182,  3185,  3189,  3193,
         3194,  3197,  3200,  3204,  3208,  3212,  3216,  3219,  3221,  3222,
         3226,  3230,  3234,  3238,  3242,  3243,  3247,  3251,  3254,  3258,
         3262,  3266,  3270,  3274,  3275,  3279,  3283,  3287,  3289,  3293,

         /* 801-900 */
         3296,  3300,  3303,  3307,  3311,  3315,  3319,  3321,  3324,  3327,
         3330,  3334,  3338,  3340,  3342,  3346,  3350,  3354,  3358,  3361,
         3365,  3369,  3373,  3377,  3381,  3385,  3389,  3393,  3394,  3398,
         3402,  3406,  3410,  3413,  3417,  3421,  3425,  3429,  3433,  3435,
         3439,  3443,  3446,  3450,  3453,  3457,  3458,  3461,  3464,  3468,
         3472,  3476,  3478,  3481,  3485,  3489,  3493,  3497,  3501,  3505,
         3507,  3511,  3514,  3517,  3521,  3524,  3525,  3527,  3529,  3533,
         3536,  3540,  3541,  3545,  3548,  3551,  3555,  3559,  3563,  3567,
         3569,  3570,  3574,  3576,  3578,  3582,  3586,  3590,  3593,  3596,
         3600,  3604,  3608,  3612,  3616,  3620,  3623,  3626,  3630,  3632,

         /* 901-1000 */
         3636,  3640,  3643,  3646,  3648,  3652,  3656,  3660,  3664,  3667,
         3669,  3671,  3675,  3679,  3683,  3687,  3689,  3693,  3694,  3695,
         3699,  3703,  3705,  3707,  3710,  3713,  3717,  3721,  3725,  3729,
         3733,  3736,  3740,  3744,  3748,  3752,  3754,  3757,  3759,  3763,
         3767,  3770,  3773,  3777,  3779,  3783,  3786,  3790,  3794,  3798,
         3801,  3805,  3809,  3813,  3817,  3821,  3825,  3827,  3831,  3835,
         3836,  3837,  3840,  3844,  3848,  3852,  3856,  3859,  3863,  3867,
         3869,  3871,  3875,  3879,  3883,  3887,  3890,  3894,  3898,  3901,
         3905,  3909,  3913,  3917,  3921,  3922,  3923,  3924,  3926,  3930,
         3932,  3936,  3938,  3940,  3944,  3948,  3952,  3956,  3959,  3963,

         /* 1001-1100 */
         3965,  3969,  3973,  3977,  3979,  3981,  3982,  3986,  3989,  3993,
         3997,  4001,  4004,  4006,  4009,  4012,  4016,  4020,  4024,  4026,
         4028,  4032,  4036,  4040,  4044,  4046,  4050,  4054,  4058,  4060,
         4062,  4063,  4064,  4068,  4071,  4075,  4077,  4081,  4083,  4087,
         4089,  4091,  4095,  4099,  4101,  4103,  4105,  4107,  4111,  4115,
         4119,  4123,  4127,  4129,  4131,  4135,  4139,  4141,  4143,  4145,
         4149,  4153,  4157,  4161,  4165,  4169,  4173,  4177,  4180,  4183,
         4187,  4191,  4195,  4198,  4201,  4205,  4209,  4212,  4213,  4216,
         4217,  4221,  4223,  4226,  4230,  4234,  4236,  4240,  4244,  4248,
         4252,  4256,  4258,  4262,  4264,  4266,  4268,  4270,  4272,  4276,

         /* 1101-1200 */
         4279,  4283,  4285,  4287,  4289,  4293,  4295,  4299,  4300,  4301,
         4305,  4309,  4313,  4317,  4319,  4323,  4325,  4329,  4331,  4333,
         4335,  4337,  4341,  4345,  4349,  4351,  4353,  4357,  4361,  4365,
         4367,  4369,  4373,  4377,  4381,  4383,  4387,  4389,  4391,  4395,
         4399,  4403,  4407,  4411,  4413,  4414,  4415,  4418,  4419,  4421,
         4423,  4427,  4429,  4431,  4433,  4435,  4437,  4439,  4443,  4446,
         4450,  4452,  4456,  4458,  4460,  4462,  4466,  4469,  4473,  4477,
         4481,  4483,  4487,  4489,  4491,  4493,  4497,  4499,  4501,  4504,
         4506,  4510,  4513,  4514,  4515,  4518,  4521,  4522,  4525,  4526,
         4527,  4530,  4533,  4534,  4537,  4541,  4542,  4543,  4544,  4545,

         /* 1201-1300 */
         4546,  4547,  4550,  4553,  4554,  4555,  4558,  4561,  4564,  4567,
         4568,  4571,  4574,  4575,  4578,  4581,  4582,  4585,  4586,  4588,
         4590,  4592,  4596,  4598,  4602,  4604,  4608,  4612,  4613,  4616,
         4619,  4622,  4623,  4624,  4625,  4626,  4629,  4632,  4633,  4636,
         4639,  4640,  4641,  4642,  4643,  4644,  4645,  4648,  4649,  4650,
         4651,  4652,  4653,  4656,  4657,  4660,  4661,  4664,  4667,  4670,
         4671,  4674,  4675,  4676,  4677,  4678,  4681,  4682,  4683,  4684,
         4687,  4688,  4689,  4692,  4693,  4696,  4697,  4700,  4701,  4702,
         4703,  4704,  4707,  4708,  4711,  4712,  4715,  4716,  4717,  4718,
         4719,  4720,  4721,  4722,  4723,  4726,  4729,  4730,  4733,  4736,

         /* 1301-(NFLS+NFPL) */
         4737,  4740,  4741,  4742,  4745,  4746,  4749,  4752,  4753
       };

       /* Amplitude coefficients (microarcsec);  indexed using the nc array. */
       static const double a[] = {

           /* 1-105 */
         -6844318.44,     9205236.26,1328.67,1538.18,      205833.11,
           153041.79,       -3309.73, 853.32,2037.98,       -2301.27,
       81.46, 120.56, -20.39, -15.22,   1.73,  -1.61,  -0.10,   0.11,
       -0.02,  -0.02,     -523908.04,      573033.42,-544.75,-458.66,
            12814.01,       11714.49, 198.97,-290.91, 155.74,-143.27,
       -2.75,  -1.03,  -1.27,  -1.16,   0.00,  -0.01,      -90552.22,
            97846.69, 111.23, 137.41,2187.91,2024.68,  41.44, -51.26,
       26.92, -24.46,  -0.46,  -0.28,  -0.22,  -0.20,       82168.76,
           -89618.24, -27.64, -29.05,       -2004.36,       -1837.32,
      -36.07,  48.00, -24.43,  22.41,   0.47,   0.24,   0.20,   0.18,
            58707.02,7387.02, 470.05,-192.40, 164.33,       -1312.21,
     -179.73, -28.93, -17.36,  -1.83,  -0.50,   3.57,   0.00,   0.13,
           -20557.78,       22438.42, -20.84, -17.40, 501.82, 459.68,
       59.20, -67.30,   6.08,  -5.61,  -1.36,  -1.19,       28288.28,
     -674.99, -34.69,  35.80, -15.07,-632.54, -11.19,   0.78,  -8.41,
        0.17,   0.01,   0.07,      -15406.85,       20069.50,  15.12,

        /* 106-219 */
       31.80, 448.76, 344.50,  -5.77,   1.41,   4.59,  -5.02,   0.17,
        0.24,      -11991.74,       12902.66,  32.46,  36.70, 288.49,
      268.14,   5.70,  -7.06,   3.57,  -3.23,  -0.06,  -0.04,
            -8584.95,       -9592.72,   4.42, -13.20,-214.50, 192.06,
       23.87,  29.83,   2.54,   2.40,   0.60,  -0.48,5095.50,
            -6918.22,   7.19,   3.92,-154.91,-113.94,   2.86,  -1.04,
       -1.52,   1.73,  -0.07,  -0.10,       -4910.93,       -5331.13,
        0.76,   0.40,-119.21, 109.81,   2.16,   3.20,   1.46,   1.33,
        0.04,  -0.02,       -6245.02,-123.48,  -6.68,  -8.20,  -2.76,
      139.64,   2.71,   0.15,   1.86,2511.85,       -3323.89,   1.07,
       -0.90, -74.33, -56.17,   1.16,  -0.01,  -0.75,   0.83,  -0.02,
       -0.04,2307.58,3143.98,  -7.52,   7.50,  70.31, -51.60,   1.46,
        0.16,  -0.69,  -0.79,   0.02,  -0.05,2372.58,2554.51,   5.93,
       -6.60,  57.12, -53.05,  -0.96,  -1.24,  -0.71,  -0.64,  -0.01,
            -2053.16,2636.13,   5.13,   7.80,  58.94,  45.91,  -0.42,
       -0.12,   0.61,  -0.66,   0.02,   0.03,       -1825.49,

       /* 220-339 */
            -2423.59,   1.23,  -2.00, -54.19,  40.82,  -1.07,  -1.02,
        0.54,   0.61,  -0.04,   0.04,2521.07,-122.28,  -5.97,   2.90,
       -2.73, -56.37,  -0.82,   0.13,  -0.75,       -1534.09,1645.01,
        6.29,   6.80,  36.78,  34.30,   0.92,  -1.25,   0.46,  -0.41,
       -0.02,  -0.01,1898.27,  47.70,  -0.72,   2.50,   1.07, -42.45,
       -0.94,   0.02,  -0.56,       -1292.02,       -1387.00,   0.00,
        0.00, -31.01,  28.89,   0.68,   0.00,   0.38,   0.35,  -0.01,
       -0.01,       -1234.96,1323.81,   5.21,   5.90,  29.60,  27.61,
        0.74,  -1.22,   0.37,  -0.33,  -0.02,  -0.01,1137.48,
            -1233.89,  -0.04,  -0.30, -27.59, -25.43,  -0.61,   1.00,
       -0.34,   0.31,   0.01,   0.01,-813.13,       -1075.60,   0.40,
        0.30, -24.05,  18.18,  -0.40,  -0.01,   0.24,   0.27,  -0.01,
        0.01,1163.22, -60.90,  -2.94,   1.30,  -1.36, -26.01,  -0.58,
        0.07,  -0.35,1029.70, -55.55,  -2.63,   1.10,  -1.25, -23.02,
       -0.52,   0.06,  -0.31,-556.26, 852.85,   3.16,  -4.48,  19.06,
       12.44,  -0.81,  -0.27,   0.17,  -0.21,   0.00,   0.02,-603.52,

       /* 340-467 */
     -800.34,   0.44,   0.10, -17.90,  13.49,  -0.08,  -0.01,   0.18,
        0.20,  -0.01,   0.01,-628.24, 684.99,  -0.64,  -0.50,  15.32,
       14.05,   3.18,  -4.19,   0.19,  -0.17,  -0.09,  -0.07,-866.48,
      -16.26,   0.52,  -1.30,  -0.36,  19.37,   0.43,  -0.01,   0.26,
     -512.37, 695.54,  -1.47,  -1.40,  15.55,  11.46,  -0.16,   0.03,
        0.15,  -0.17,   0.01,   0.01, 506.65, 643.75,   2.54,  -2.62,
       14.40, -11.33,  -0.77,  -0.06,  -0.15,  -0.16,   0.00,   0.01,
      664.57,  16.81,  -0.40,   1.00,   0.38, -14.86,  -3.71,  -0.09,
       -0.20, 405.91, 522.11,   0.99,  -1.50,  11.67,  -9.08,  -0.25,
       -0.02,  -0.12,  -0.13,-305.78, 326.60,   1.75,   1.90,   7.30,
        6.84,   0.20,  -0.04, 300.99,-325.03,  -0.44,  -0.50,  -7.27,
       -6.73,  -1.01,   0.01,   0.00,   0.08,   0.00,   0.02, 438.51,
       10.47,  -0.56,  -0.20,   0.24,  -9.81,  -0.24,   0.01,  -0.13,
     -264.02, 335.24,   0.99,   1.40,   7.49,   5.90,  -0.27,  -0.02,
      284.09, 307.03,   0.32,  -0.40,   6.87,  -6.35,  -0.99,  -0.01,
     -250.54, 327.11,   0.08,   0.40,   7.31,   5.60,  -0.30, 230.72,

     /* 468-595 */
     -304.46,   0.08,  -0.10,  -6.81,  -5.16,   0.27, 229.78, 304.17,
       -0.60,   0.50,   6.80,  -5.14,   0.33,   0.01, 256.30,-276.81,
       -0.28,  -0.40,  -6.19,  -5.73,  -0.14,   0.01,-212.82, 269.45,
        0.84,   1.20,   6.02,   4.76,   0.14,  -0.02, 196.64, 272.05,
       -0.84,   0.90,   6.08,  -4.40,   0.35,   0.02, 188.95, 272.22,
       -0.12,   0.30,   6.09,  -4.22,   0.34,-292.37,  -5.10,  -0.32,
       -0.40,  -0.11,   6.54,   0.14,   0.01, 161.79,-220.67,   0.24,
        0.10,  -4.93,  -3.62,  -0.08, 261.54, -19.94,  -0.95,   0.20,
       -0.45,  -5.85,  -0.13,   0.02, 142.16,-190.79,   0.20,   0.10,
       -4.27,  -3.18,  -0.07, 187.95,  -4.11,  -0.24,   0.30,  -0.09,
       -4.20,  -0.09,   0.01,   0.00,   0.00, -79.08, 167.90,   0.04,
        0.00,   3.75,   1.77, 121.98, 131.04,  -0.08,   0.10,   2.93,
       -2.73,  -0.06,-172.95,  -8.11,  -0.40,  -0.20,  -0.18,   3.87,
        0.09,   0.01,-160.15, -55.30, -14.04,  13.90,  -1.23,   3.58,
        0.40,   0.31,-115.40, 123.20,   0.60,   0.70,   2.75,   2.58,
        0.08,  -0.01,-168.26,  -2.00,   0.20,  -0.20,  -0.04,   3.76,

        /* 596-723 */
        0.08,-114.49, 123.20,   0.32,   0.40,   2.75,   2.56,   0.07,
       -0.01, 112.14, 120.70,   0.28,  -0.30,   2.70,  -2.51,  -0.07,
       -0.01, 161.34,   4.03,   0.20,   0.20,   0.09,  -3.61,  -0.08,
       91.31, 126.64,  -0.40,   0.40,   2.83,  -2.04,  -0.04,   0.01,
      105.29, 112.90,   0.44,  -0.50,   2.52,  -2.35,  -0.07,  -0.01,
       98.69,-106.20,  -0.28,  -0.30,  -2.37,  -2.21,  -0.06,   0.01,
       86.74,-112.94,  -0.08,  -0.20,  -2.53,  -1.94,  -0.05,-134.81,
        3.51,   0.20,  -0.20,   0.08,   3.01,   0.07,  79.03, 107.31,
       -0.24,   0.20,   2.40,  -1.77,  -0.04,   0.01, 132.81, -10.77,
       -0.52,   0.10,  -0.24,  -2.97,  -0.07,   0.01,-130.31,  -0.90,
        0.04,   0.00,   0.00,   2.91, -78.56,  85.32,   0.00,   0.00,
        1.91,   1.76,   0.04,   0.00,   0.00, -41.53,  89.10,   0.02,
        0.00,   1.99,   0.93,  66.03, -71.00,  -0.20,  -0.20,  -1.59,
       -1.48,  -0.04,  60.50,  64.70,   0.36,  -0.40,   1.45,  -1.35,
       -0.04,  -0.01, -52.27, -70.01,   0.00,   0.00,  -1.57,   1.17,
        0.03, -52.95,  66.29,   0.32,   0.40,   1.48,   1.18,   0.04,

        /* 724-851 */
       -0.01,  51.02,  67.25,   0.00,   0.00,   1.50,  -1.14,  -0.03,
      -55.66, -60.92,   0.16,  -0.20,  -1.36,   1.24,   0.03, -54.81,
      -59.20,  -0.08,   0.20,  -1.32,   1.23,   0.03,  51.32, -55.60,
        0.00,   0.00,  -1.24,  -1.15,  -0.03,  48.29,  51.80,   0.20,
       -0.20,   1.16,  -1.08,  -0.03, -45.59, -49.00,  -0.12,   0.10,
       -1.10,   1.02,   0.03,  40.54, -52.69,  -0.04,  -0.10,  -1.18,
       -0.91,  -0.02, -40.58, -49.51,  -1.00,   1.00,  -1.11,   0.91,
        0.04,   0.02, -43.76,  46.50,   0.36,   0.40,   1.04,   0.98,
        0.03,  -0.01,  62.65,  -5.00,  -0.24,   0.00,  -0.11,  -1.40,
       -0.03,   0.01, -38.57,  49.59,   0.08,   0.10,   1.11,   0.86,
        0.02, -33.22, -44.04,   0.08,  -0.10,  -0.98,   0.74,   0.02,
       37.15, -39.90,  -0.12,  -0.10,  -0.89,  -0.83,  -0.02,  36.68,
      -39.50,  -0.04,  -0.10,  -0.88,  -0.82,  -0.02, -53.22,  -3.91,
       -0.20,   0.00,  -0.09,   1.19,   0.03,  32.43, -42.19,  -0.04,
       -0.10,  -0.94,  -0.73,  -0.02, -51.00,  -2.30,  -0.12,  -0.10,
        0.00,   1.14, -29.53, -39.11,   0.04,   0.00,  -0.87,   0.66,

        /* 852-979 */
        0.02,  28.50, -38.92,  -0.08,  -0.10,  -0.87,  -0.64,  -0.02,
       26.54,  36.95,  -0.12,   0.10,   0.83,  -0.59,  -0.01,  26.54,
       34.59,   0.04,  -0.10,   0.77,  -0.59,  -0.02,  28.35, -32.55,
       -0.16,   0.20,  -0.73,  -0.63,  -0.01, -28.00,  30.40,   0.00,
        0.00,   0.68,   0.63,   0.01, -27.61,  29.40,   0.20,   0.20,
        0.66,   0.62,   0.02,  40.33,   0.40,  -0.04,   0.10,   0.00,
       -0.90, -23.28,  31.61,  -0.08,  -0.10,   0.71,   0.52,   0.01,
       37.75,   0.80,   0.04,   0.10,   0.00,  -0.84,  23.66,  25.80,
        0.00,   0.00,   0.58,  -0.53,  -0.01,  21.01, -27.91,   0.00,
        0.00,  -0.62,  -0.47,  -0.01, -34.81,   2.89,   0.04,   0.00,
        0.00,   0.78, -23.49, -25.31,   0.00,   0.00,  -0.57,   0.53,
        0.01, -23.47,  25.20,   0.16,   0.20,   0.56,   0.52,   0.02,
       19.58,  27.50,  -0.12,   0.10,   0.62,  -0.44,  -0.01, -22.67,
      -24.40,  -0.08,   0.10,  -0.55,   0.51,   0.01, -19.97,  25.00,
        0.12,   0.20,   0.56,   0.45,   0.01,  21.28, -22.80,  -0.08,
       -0.10,  -0.51,  -0.48,  -0.01, -30.47,   0.91,   0.04,   0.00,

       /* 980-1107 */
        0.00,   0.68,  18.58,  24.00,   0.04,  -0.10,   0.54,  -0.42,
       -0.01, -18.02,  24.40,  -0.04,  -0.10,   0.55,   0.40,   0.01,
       17.74,  22.50,   0.08,  -0.10,   0.50,  -0.40,  -0.01, -19.41,
       20.70,   0.08,   0.10,   0.46,   0.43,   0.01, -18.64,  20.11,
        0.00,   0.00,   0.45,   0.42,   0.01, -16.75,  21.60,   0.04,
        0.10,   0.48,   0.37,   0.01, -18.42, -20.00,   0.00,   0.00,
       -0.45,   0.41,   0.01, -26.77,   1.41,   0.08,   0.00,   0.00,
        0.60, -26.17,  -0.19,   0.00,   0.00,   0.00,   0.59, -15.52,
       20.51,   0.00,   0.00,   0.46,   0.35,   0.01, -25.42,  -1.91,
       -0.08,   0.00,  -0.04,   0.57,   0.45, -17.42,  18.10,   0.00,
        0.00,   0.40,   0.39,   0.01,  16.39, -17.60,  -0.08,  -0.10,
       -0.39,  -0.37,  -0.01, -14.37,  18.91,   0.00,   0.00,   0.42,
        0.32,   0.01,  23.39,  -2.40,  -0.12,   0.00,   0.00,  -0.52,
       14.32, -18.50,  -0.04,  -0.10,  -0.41,  -0.32,  -0.01,  15.69,
       17.08,   0.00,   0.00,   0.38,  -0.35,  -0.01, -22.99,   0.50,
        0.04,   0.00,   0.00,   0.51,   0.00,   0.00,  14.47, -17.60,

        /* 1108-1235 */
       -0.01,   0.00,  -0.39,  -0.32, -13.33,  18.40,  -0.04,  -0.10,
        0.41,   0.30,  22.47,  -0.60,  -0.04,   0.00,   0.00,  -0.50,
      -12.78, -17.41,   0.04,   0.00,  -0.39,   0.29,   0.01, -14.10,
      -15.31,   0.04,   0.00,  -0.34,   0.32,   0.01,  11.98,  16.21,
       -0.04,   0.00,   0.36,  -0.27,  -0.01,  19.65,  -1.90,  -0.08,
        0.00,   0.00,  -0.44,  19.61,  -1.50,  -0.08,   0.00,   0.00,
       -0.44,  13.41, -14.30,  -0.04,  -0.10,  -0.32,  -0.30,  -0.01,
      -13.29,  14.40,   0.00,   0.00,   0.32,   0.30,   0.01,  11.14,
      -14.40,  -0.04,   0.00,  -0.32,  -0.25,  -0.01,  12.24, -13.38,
        0.04,   0.00,  -0.30,  -0.27,  -0.01,  10.07, -13.81,   0.04,
        0.00,  -0.31,  -0.23,  -0.01,  10.46,  13.10,   0.08,  -0.10,
        0.29,  -0.23,  -0.01,  16.55,  -1.71,  -0.08,   0.00,   0.00,
       -0.37,   9.75, -12.80,   0.00,   0.00,  -0.29,  -0.22,  -0.01,
        9.11,  12.80,   0.00,   0.00,   0.29,  -0.20,   0.00,   0.00,
       -6.44, -13.80,   0.00,   0.00,  -0.31,   0.14,  -9.19, -12.00,
        0.00,   0.00,  -0.27,   0.21, -10.30,  10.90,   0.08,   0.10,

        /* 1236-1363 */
        0.24,   0.23,   0.01,  14.92,  -0.80,  -0.04,   0.00,   0.00,
       -0.33,  10.02, -10.80,   0.00,   0.00,  -0.24,  -0.22,  -0.01,
       -9.75,  10.40,   0.04,   0.00,   0.23,   0.22,   0.01,   9.67,
      -10.40,  -0.04,   0.00,  -0.23,  -0.22,  -0.01,  -8.28, -11.20,
        0.04,   0.00,  -0.25,   0.19,  13.32,  -1.41,  -0.08,   0.00,
        0.00,  -0.30,   8.27,  10.50,   0.04,   0.00,   0.23,  -0.19,
        0.00,   0.00,  13.13,   0.00,   0.00,   0.00,   0.00,  -0.29,
      -12.93,   0.70,   0.04,   0.00,   0.00,   0.29,   7.91, -10.20,
        0.00,   0.00,  -0.23,  -0.18,  -7.84, -10.00,  -0.04,   0.00,
       -0.22,   0.18,   7.44,   9.60,   0.00,   0.00,   0.21,  -0.17,
       -7.64,   9.40,   0.08,   0.10,   0.21,   0.17,   0.01, -11.38,
        0.60,   0.04,   0.00,   0.00,   0.25,  -7.48,   8.30,   0.00,
        0.00,   0.19,   0.17, -10.98,  -0.20,   0.00,   0.00,   0.00,
        0.25,  10.98,   0.20,   0.00,   0.00,   0.00,  -0.25,   7.40,
       -7.90,  -0.04,   0.00,  -0.18,  -0.17,  -6.09,   8.40,  -0.04,
        0.00,   0.19,   0.14,  -6.94,  -7.49,   0.00,   0.00,  -0.17,

        /* 1364-1491 */
        0.16,   6.92,   7.50,   0.04,   0.00,   0.17,  -0.15,   6.20,
        8.09,   0.00,   0.00,   0.18,  -0.14,  -6.12,   7.80,   0.04,
        0.00,   0.17,   0.14,   5.85,  -7.50,   0.00,   0.00,  -0.17,
       -0.13,  -6.48,   6.90,   0.08,   0.10,   0.15,   0.14,   0.01,
        6.32,   6.90,   0.00,   0.00,   0.15,  -0.14,   5.61,  -7.20,
        0.00,   0.00,  -0.16,  -0.13,   9.07,   0.00,   0.00,   0.00,
        0.00,  -0.20,   5.25,   6.90,   0.00,   0.00,   0.15,  -0.12,
       -8.47,  -0.40,   0.00,   0.00,   0.00,   0.19,   6.32,  -5.39,
       -1.11,   1.10,  -0.12,  -0.14,   0.02,   0.02,   5.73,  -6.10,
       -0.04,   0.00,  -0.14,  -0.13,   4.70,   6.60,  -0.04,   0.00,
        0.15,  -0.11,  -4.90,  -6.40,   0.00,   0.00,  -0.14,   0.11,
       -5.33,   5.60,   0.04,   0.10,   0.13,   0.12,   0.01,  -4.81,
        6.00,   0.04,   0.00,   0.13,   0.11,   5.13,   5.50,   0.04,
        0.00,   0.12,  -0.11,   4.50,   5.90,   0.00,   0.00,   0.13,
       -0.10,  -4.22,   6.10,   0.00,   0.00,   0.14,  -4.53,   5.70,
        0.00,   0.00,   0.13,   0.10,   4.18,   5.70,   0.00,   0.00,

        /* 1492-1619 */
        0.13,  -4.75,  -5.19,   0.00,   0.00,  -0.12,   0.11,  -4.06,
        5.60,   0.00,   0.00,   0.13,  -3.98,   5.60,  -0.04,   0.00,
        0.13,   4.02,  -5.40,   0.00,   0.00,  -0.12,   4.49,  -4.90,
       -0.04,   0.00,  -0.11,  -0.10,  -3.62,  -5.40,  -0.16,   0.20,
       -0.12,   0.00,   0.01,   4.38,   4.80,   0.00,   0.00,   0.11,
       -6.40,  -0.10,   0.00,   0.00,   0.00,   0.14,  -3.98,   5.00,
        0.04,   0.00,   0.11,  -3.82,  -5.00,   0.00,   0.00,  -0.11,
       -3.71,   5.07,   0.00,   0.00,   0.11,   4.14,   4.40,   0.00,
        0.00,   0.10,  -6.01,  -0.50,  -0.04,   0.00,   0.00,   0.13,
       -4.04,   4.39,   0.00,   0.00,   0.10,   3.45,  -4.72,   0.00,
        0.00,  -0.11,   3.31,   4.71,   0.00,   0.00,   0.11,   3.26,
       -4.50,   0.00,   0.00,  -0.10,  -3.26,  -4.50,   0.00,   0.00,
       -0.10,  -3.34,  -4.40,   0.00,   0.00,  -0.10,  -3.74,  -4.00,
        3.70,   4.00,   3.34,  -4.30,   3.30,  -4.30,  -3.66,   3.90,
        0.04,   3.66,   3.90,   0.04,  -3.62,  -3.90,  -3.61,   3.90,
       -0.20,   5.30,   0.00,   0.00,   0.12,   3.06,   4.30,   3.30,

       /* 1620-1747 */
        4.00,   0.40,   0.20,   3.10,   4.10,  -3.06,   3.90,  -3.30,
       -3.60,  -3.30,   3.36,   0.01,   3.14,   3.40,  -4.57,  -0.20,
        0.00,   0.00,   0.00,   0.10,  -2.70,  -3.60,   2.94,  -3.20,
       -2.90,   3.20,   2.47,  -3.40,   2.55,  -3.30,   2.80,  -3.08,
        2.51,   3.30,  -4.10,   0.30,  -0.12,  -0.10,   4.10,   0.20,
       -2.74,   3.00,   2.46,   3.23,  -3.66,   1.20,  -0.20,   0.20,
        3.74,  -0.40,  -2.51,  -2.80,  -3.74,   2.27,  -2.90,   0.00,
        0.00,  -2.50,   2.70,  -2.51,   2.60,  -3.50,   0.20,   3.38,
       -2.22,  -2.50,   3.26,  -0.40,   1.95,  -2.60,   3.22,  -0.40,
       -0.04,  -1.79,  -2.60,   1.91,   2.50,   0.74,   3.05,  -0.04,
        0.08,   2.11,  -2.30,  -2.11,   2.20,  -1.87,  -2.40,   2.03,
       -2.20,  -2.03,   2.20,   2.98,   0.00,   0.00,   2.98,  -1.71,
        2.40,   2.94,  -0.10,  -0.12,   0.10,   1.67,   2.40,  -1.79,
        2.30,  -1.79,   2.20,  -1.67,   2.20,   1.79,  -2.00,   1.87,
       -1.90,   1.63,  -2.10,  -1.59,   2.10,   1.55,  -2.10,  -1.55,
        2.10,  -2.59,  -0.20,  -1.75,  -1.90,  -1.75,   1.90,  -1.83,

        /* 1748-1875 */
       -1.80,   1.51,   2.00,  -1.51,  -2.00,   1.71,   1.80,   1.31,
        2.10,  -1.43,   2.00,   1.43,   2.00,  -2.43,  -1.51,   1.90,
       -1.47,   1.90,   2.39,   0.20,  -2.39,   1.39,   1.90,   1.39,
       -1.80,   1.47,  -1.60,   1.47,  -1.60,   1.43,  -1.50,  -1.31,
        1.60,   1.27,  -1.60,  -1.27,   1.60,   1.27,  -1.60,   2.03,
        1.35,   1.50,  -1.39,  -1.40,   1.95,  -0.20,  -1.27,   1.49,
        1.19,   1.50,   1.27,   1.40,   1.15,   1.50,   1.87,  -0.10,
       -1.12,  -1.50,   1.87,  -1.11,  -1.50,  -1.11,  -1.50,   0.00,
        0.00,   1.19,   1.40,   1.27,  -1.30,  -1.27,  -1.30,  -1.15,
        1.40,  -1.23,   1.30,  -1.23,  -1.30,   1.22,  -1.29,   1.07,
       -1.40,   1.75,  -0.20,  -1.03,  -1.40,  -1.07,   1.20,  -1.03,
        1.15,   1.07,   1.10,   1.51,  -1.03,   1.10,   1.03,  -1.10,
        0.00,   0.00,  -1.03,  -1.10,   0.91,  -1.20,  -0.88,  -1.20,
       -0.88,   1.20,  -0.95,   1.10,  -0.95,  -1.10,   1.43,  -1.39,
        0.95,  -1.00,  -0.95,   1.00,  -0.80,   1.10,   0.91,  -1.00,
       -1.35,   0.88,   1.00,  -0.83,   1.00,  -0.91,   0.90,   0.91,

       /* 1876-2003 */
        0.90,   0.88,  -0.90,  -0.76,  -1.00,  -0.76,   1.00,   0.76,
        1.00,  -0.72,   1.00,   0.84,  -0.90,   0.84,   0.90,   1.23,
        0.00,   0.00,  -0.52,  -1.10,  -0.68,   1.00,   1.19,  -0.20,
        1.19,   0.76,   0.90,   1.15,  -0.10,   1.15,  -0.10,   0.72,
       -0.90,  -1.15,  -1.15,   0.68,   0.90,  -0.68,   0.90,  -1.11,
        0.00,   0.00,   0.20,   0.79,   0.80,  -1.11,  -0.10,   0.00,
        0.00,  -0.48,  -1.00,  -0.76,  -0.80,  -0.72,  -0.80,  -1.07,
       -0.10,   0.64,   0.80,  -0.64,  -0.80,   0.64,   0.80,   0.40,
        0.60,   0.52,  -0.50,  -0.60,  -0.80,  -0.71,   0.70,  -0.99,
        0.99,   0.56,   0.80,  -0.56,   0.80,   0.68,  -0.70,   0.68,
        0.70,  -0.95,  -0.64,   0.70,   0.64,   0.70,  -0.60,   0.70,
       -0.60,  -0.70,  -0.91,  -0.10,  -0.51,   0.76,  -0.91,  -0.56,
        0.70,   0.88,   0.88,  -0.63,  -0.60,   0.55,  -0.60,  -0.80,
        0.80,  -0.80,  -0.52,   0.60,   0.52,   0.60,   0.52,  -0.60,
       -0.48,   0.60,   0.48,   0.60,   0.48,   0.60,  -0.76,   0.44,
       -0.60,   0.52,  -0.50,  -0.52,   0.50,   0.40,   0.60,  -0.40,

       /* 2004-2131 */
       -0.60,   0.40,  -0.60,   0.72,  -0.72,  -0.51,  -0.50,  -0.48,
        0.50,   0.48,  -0.50,  -0.48,   0.50,  -0.48,   0.50,   0.48,
       -0.50,  -0.48,  -0.50,  -0.68,  -0.68,   0.44,   0.50,  -0.64,
       -0.10,  -0.64,  -0.10,  -0.40,   0.50,   0.40,   0.50,   0.40,
        0.50,   0.00,   0.00,  -0.40,  -0.50,  -0.36,  -0.50,   0.36,
       -0.50,   0.60,  -0.60,   0.40,  -0.40,   0.40,   0.40,  -0.40,
        0.40,  -0.40,   0.40,  -0.56,  -0.56,   0.36,  -0.40,  -0.36,
        0.40,   0.36,  -0.40,  -0.36,  -0.40,   0.36,   0.40,   0.36,
        0.40,  -0.52,   0.52,   0.52,   0.32,   0.40,  -0.32,   0.40,
       -0.32,   0.40,  -0.32,   0.40,   0.32,  -0.40,  -0.32,  -0.40,
        0.32,  -0.40,   0.28,  -0.40,  -0.28,   0.40,   0.28,  -0.40,
        0.28,   0.40,   0.48,  -0.48,   0.48,   0.36,  -0.30,  -0.36,
       -0.30,   0.00,   0.00,   0.20,   0.40,  -0.44,   0.44,  -0.44,
       -0.44,  -0.44,  -0.44,   0.32,  -0.30,   0.32,   0.30,   0.24,
        0.30,  -0.12,  -0.10,  -0.28,   0.30,   0.28,   0.30,   0.28,
        0.30,   0.28,  -0.30,   0.28,  -0.30,   0.28,  -0.30,   0.28,

        /* 2132-2259 */
        0.30,  -0.28,   0.30,   0.40,   0.40,  -0.24,   0.30,   0.24,
       -0.30,   0.24,  -0.30,  -0.24,  -0.30,   0.24,   0.30,   0.24,
       -0.30,  -0.24,   0.30,   0.24,  -0.30,  -0.24,  -0.30,   0.24,
       -0.30,   0.24,   0.30,  -0.24,   0.30,  -0.24,   0.30,   0.20,
       -0.30,   0.20,  -0.30,   0.20,  -0.30,   0.20,   0.30,   0.20,
       -0.30,   0.20,  -0.30,   0.20,   0.30,   0.20,   0.30,  -0.20,
       -0.30,   0.20,  -0.30,   0.20,  -0.30,  -0.36,  -0.36,  -0.36,
       -0.04,   0.30,   0.12,  -0.10,  -0.32,  -0.24,   0.20,   0.24,
        0.20,   0.20,  -0.20,  -0.20,  -0.20,  -0.20,  -0.20,   0.20,
        0.20,   0.20,  -0.20,   0.20,   0.20,   0.20,   0.20,  -0.20,
       -0.20,   0.00,   0.00,  -0.20,  -0.20,  -0.20,   0.20,  -0.20,
        0.20,   0.20,  -0.20,  -0.20,  -0.20,   0.20,   0.20,   0.20,
        0.20,   0.20,  -0.20,   0.20,  -0.20,   0.28,   0.28,   0.28,
        0.28,   0.28,   0.28,  -0.28,   0.28,   0.12,   0.00,   0.24,
        0.16,  -0.20,   0.16,  -0.20,   0.16,  -0.20,   0.16,   0.20,
       -0.16,   0.20,   0.16,   0.20,  -0.16,   0.20,  -0.16,   0.20,

       /* 2260-2387 */
       -0.16,   0.20,   0.16,  -0.20,   0.16,   0.20,   0.16,  -0.20,
       -0.16,   0.20,  -0.16,  -0.20,  -0.16,   0.20,   0.16,   0.20,
        0.16,  -0.20,   0.16,  -0.20,   0.16,   0.20,   0.16,   0.20,
        0.16,   0.20,  -0.16,  -0.20,   0.16,   0.20,  -0.16,   0.20,
        0.16,   0.20,  -0.16,  -0.20,   0.16,  -0.20,   0.16,  -0.20,
       -0.16,  -0.20,   0.24,  -0.24,  -0.24,   0.24,   0.24,   0.12,
        0.20,   0.12,   0.20,  -0.12,  -0.20,   0.12,  -0.20,   0.12,
       -0.20,  -0.12,   0.20,  -0.12,   0.20,  -0.12,  -0.20,   0.12,
        0.20,   0.12,   0.20,   0.12,  -0.20,  -0.12,   0.20,   0.12,
       -0.20,  -0.12,   0.20,   0.12,   0.20,   0.00,   0.00,  -0.12,
        0.20,  -0.12,   0.20,   0.12,  -0.20,  -0.12,   0.20,   0.12,
        0.20,   0.00,  -0.21,  -0.20,   0.00,   0.00,   0.20,  -0.20,
       -0.20,  -0.20,   0.20,  -0.16,  -0.10,   0.00,   0.17,   0.16,
        0.16,   0.16,   0.16,  -0.16,   0.16,   0.16,  -0.16,   0.16,
       -0.16,   0.16,   0.12,   0.10,   0.12,  -0.10,  -0.12,   0.10,
       -0.12,   0.10,   0.12,  -0.10,  -0.12,   0.12,  -0.12,   0.12,

       /* 2388-2515 */
       -0.12,   0.12,  -0.12,  -0.12,  -0.12,  -0.12,  -0.12,  -0.12,
       -0.12,   0.12,   0.12,   0.12,   0.12,  -0.12,  -0.12,   0.12,
        0.12,   0.12,  -0.12,   0.12,  -0.12,  -0.12,  -0.12,   0.12,
       -0.12,  -0.12,   0.12,   0.00,   0.11,   0.11,-122.67, 164.70,
      203.78, 273.50,   3.58,   2.74,   6.18,  -4.56,   0.00,  -0.04,
        0.00,  -0.07,  57.44, -77.10,  95.82, 128.60,  -1.77,  -1.28,
        2.85,  -2.14,  82.14,  89.50,   0.00,   0.00,   2.00,  -1.84,
       -0.04,  47.73, -64.10,  23.79,  31.90,  -1.45,  -1.07,   0.69,
       -0.53, -46.38,  50.50,   0.00,   0.00,   1.13,   1.04,   0.02,
      -18.38,   0.00,  63.80,   0.00,   0.00,   0.41,   0.00,  -1.43,
       59.07,   0.00,   0.00,   0.00,   0.00,  -1.32,  57.28,   0.00,
        0.00,   0.00,   0.00,  -1.28, -48.65,   0.00,  -1.15,   0.00,
        0.00,   1.09,   0.00,   0.03, -18.30,  24.60, -17.30, -23.20,
        0.56,   0.41,  -0.51,   0.39, -16.91,  26.90,   8.43,  13.30,
        0.60,   0.38,   0.31,  -0.19,   1.23,  -1.70, -19.13, -25.70,
       -0.03,  -0.03,  -0.58,   0.43,  -0.72,   0.90, -17.34, -23.30,

       /* 2516-2643 */
        0.03,   0.02,  -0.52,   0.39, -19.49, -21.30,   0.00,   0.00,
       -0.48,   0.44,   0.01,  20.57, -20.10,   0.64,   0.70,  -0.45,
       -0.46,   0.00,  -0.01,   4.89,   5.90, -16.55,  19.90,   0.14,
       -0.11,   0.44,   0.37,  18.22,  19.80,   0.00,   0.00,   0.44,
       -0.41,  -0.01,   4.89,  -5.30, -16.51, -18.00,  -0.11,  -0.11,
       -0.41,   0.37, -17.86,   0.00,  17.10,   0.00,   0.00,   0.40,
        0.00,  -0.38,   0.32,   0.00,  24.42,   0.00,   0.00,  -0.01,
        0.00,  -0.55, -23.79,   0.00,   0.00,   0.00,   0.00,   0.53,
       14.72, -16.00,  -0.32,   0.00,  -0.36,  -0.33,  -0.01,   0.01,
        3.34,  -4.50,  11.86,  15.90,  -0.11,  -0.07,   0.35,  -0.27,
       -3.26,   4.40,  11.62,  15.60,   0.09,   0.07,   0.35,  -0.26,
      -19.53,   0.00,   5.09,   0.00,   0.00,   0.44,   0.00,  -0.11,
      -13.48,  14.70,   0.00,   0.00,   0.33,   0.30,   0.01,  10.86,
      -14.60,   3.18,   4.30,  -0.33,  -0.24,   0.09,  -0.07, -11.30,
      -15.10,   0.00,   0.00,  -0.34,   0.25,   0.01,   2.03,  -2.70,
       10.82,  14.50,  -0.07,  -0.05,   0.32,  -0.24,  17.46,   0.00,

       /* 2644-2771 */
        0.00,   0.00,   0.00,  -0.39,  16.43,   0.00,   0.52,   0.00,
        0.00,  -0.37,   0.00,  -0.01,   9.35,   0.00,  13.29,   0.00,
        0.00,  -0.21,   0.00,  -0.30, -10.42,  11.40,   0.00,   0.00,
        0.25,   0.23,   0.01,   0.44,   0.50, -10.38,  11.30,   0.02,
       -0.01,   0.25,   0.23, -14.64,   0.00,   0.00,   0.00,   0.00,
        0.33,   0.56,   0.80,  -8.67,  11.70,   0.02,  -0.01,   0.26,
        0.19,  13.88,   0.00,  -2.47,   0.00,   0.00,  -0.31,   0.00,
        0.06,  -1.99,   2.70,   7.72,  10.30,   0.06,   0.04,   0.23,
       -0.17,  -0.20,   0.00,  13.05,   0.00,   0.00,   0.00,   0.00,
       -0.29,   6.92,  -9.30,   3.34,   4.50,  -0.21,  -0.15,   0.10,
       -0.07,  -6.60,   0.00,  10.70,   0.00,   0.00,   0.15,   0.00,
       -0.24,  -8.04,  -8.70,   0.00,   0.00,  -0.19,   0.18, -10.58,
        0.00,  -3.10,   0.00,   0.00,   0.24,   0.00,   0.07,  -7.32,
        8.00,  -0.12,  -0.10,   0.18,   0.16,   1.63,   1.70,   6.96,
       -7.60,   0.03,  -0.04,  -0.17,  -0.16,  -3.62,   0.00,   9.86,
        0.00,   0.00,   0.08,   0.00,  -0.22,   0.20,  -0.20,  -6.88,

        /* 2772-2899 */
       -7.50,   0.00,   0.00,  -0.17,   0.15,  -8.99,   0.00,   4.02,
        0.00,   0.00,   0.20,   0.00,  -0.09,  -1.07,   1.40,  -5.69,
       -7.70,   0.03,   0.02,  -0.17,   0.13,   6.48,  -7.20,  -0.48,
       -0.50,  -0.16,  -0.14,  -0.01,   0.01,   5.57,  -7.50,   1.07,
        1.40,  -0.17,  -0.12,   0.03,  -0.02,   8.71,   0.00,   3.54,
        0.00,   0.00,  -0.19,   0.00,  -0.08,   0.40,   0.00,   9.27,
        0.00,   0.00,  -0.01,   0.00,  -0.21,  -6.13,   6.70,  -1.19,
       -1.30,   0.15,   0.14,  -0.03,   0.03,   5.21,  -5.70,  -2.51,
       -2.60,  -0.13,  -0.12,  -0.06,   0.06,   5.69,  -6.20,  -0.12,
       -0.10,  -0.14,  -0.13,  -0.01,   2.03,  -2.70,   4.53,   6.10,
       -0.06,  -0.05,   0.14,  -0.10,   5.01,   5.50,  -2.51,   2.70,
        0.12,  -0.11,   0.06,   0.06,  -1.91,   2.60,  -4.38,  -5.90,
        0.06,   0.04,  -0.13,   0.10,   4.65,  -6.30,   0.00,   0.00,
       -0.14,  -0.10,  -5.29,   5.70,   0.00,   0.00,   0.13,   0.12,
       -2.23,  -4.00,  -4.65,   4.20,  -0.09,   0.05,   0.10,   0.10,
       -4.53,   6.10,   0.00,   0.00,   0.14,   0.10,   2.47,   2.70,

       /* 2900-3027 */
       -4.46,   4.90,   0.06,  -0.06,   0.11,   0.10,  -5.05,   5.50,
        0.84,   0.90,   0.12,   0.11,   0.02,  -0.02,   4.97,  -5.40,
       -1.71,   0.00,  -0.12,  -0.11,   0.00,   0.04,  -0.99,  -1.30,
        4.22,  -5.70,  -0.03,   0.02,  -0.13,  -0.09,   0.99,   1.40,
        4.22,  -5.60,   0.03,  -0.02,  -0.13,  -0.09,  -4.69,  -5.20,
        0.00,   0.00,  -0.12,   0.10,  -3.42,   0.00,   6.09,   0.00,
        0.00,   0.08,   0.00,  -0.14,  -4.65,  -5.10,   0.00,   0.00,
       -0.11,   0.10,   0.00,   0.00,  -4.53,  -5.00,   0.00,   0.00,
       -0.11,   0.10,  -2.43,  -2.70,  -3.82,   4.20,  -0.06,   0.05,
        0.10,   0.09,   0.00,   0.00,  -4.53,   4.90,   0.00,   0.00,
        0.11,   0.10,  -4.49,  -4.90,   0.00,   0.00,  -0.11,   0.10,
        2.67,  -2.90,  -3.62,  -3.90,  -0.06,  -0.06,  -0.09,   0.08,
        3.94,  -5.30,   0.00,   0.00,  -0.12,  -3.38,   3.70,  -2.78,
       -3.10,   0.08,   0.08,  -0.07,   0.06,   3.18,  -3.50,  -2.82,
       -3.10,  -0.08,  -0.07,  -0.07,   0.06,  -5.77,   0.00,   1.87,
        0.00,   0.00,   0.13,   0.00,  -0.04,   3.54,  -4.80,  -0.64,

        /* 3028-3155 */
       -0.90,  -0.11,   0.00,  -0.02,  -3.50,  -4.70,   0.68,  -0.90,
       -0.11,   0.00,  -0.02,   5.49,   0.00,   0.00,   0.00,   0.00,
       -0.12,   1.83,  -2.50,   2.63,   3.50,  -0.06,   0.00,   0.08,
        3.02,  -4.10,   0.68,   0.90,  -0.09,   0.00,   0.02,   0.00,
        0.00,   5.21,   0.00,   0.00,   0.00,   0.00,  -0.12,  -3.54,
        3.80,   2.70,   3.60,  -1.35,   1.80,   0.08,   0.00,   0.04,
       -2.90,   3.90,   0.68,   0.90,   0.09,   0.00,   0.02,   0.80,
       -1.10,  -2.78,  -3.70,  -0.02,   0.00,  -0.08,   4.10,   0.00,
       -2.39,   0.00,   0.00,  -0.09,   0.00,   0.05,  -1.59,   2.10,
        2.27,   3.00,   0.05,   0.00,   0.07,  -2.63,   3.50,  -0.48,
       -0.60,  -2.94,  -3.20,  -2.94,   3.20,   2.27,  -3.00,  -1.11,
       -1.50,  -0.07,   0.00,  -0.03,  -0.56,  -0.80,  -2.35,   3.10,
        0.00,  -0.60,  -3.42,   1.90,  -0.12,  -0.10,   2.63,  -2.90,
        2.51,   2.80,  -0.64,   0.70,  -0.48,  -0.60,   2.19,  -2.90,
        0.24,  -0.30,   2.15,   2.90,   2.15,  -2.90,   0.52,   0.70,
        2.07,  -2.80,  -3.10,   0.00,   1.79,   0.00,   0.00,   0.07,

        /* 3156-3283 */
        0.00,  -0.04,   0.88,   0.00,  -3.46,   2.11,   2.80,  -0.36,
        0.50,   3.54,  -0.20,  -3.50,  -1.39,   1.50,  -1.91,  -2.10,
       -1.47,   2.00,   1.39,   1.90,   2.07,  -2.30,   0.91,   1.00,
        1.99,  -2.70,   3.30,   0.00,   0.60,  -0.44,  -0.70,  -1.95,
        2.60,   2.15,  -2.40,  -0.60,  -0.70,   3.30,   0.84,   0.00,
       -3.10,  -3.10,   0.00,  -0.72,  -0.32,   0.40,  -1.87,  -2.50,
        1.87,  -2.50,   0.32,   0.40,  -0.24,   0.30,  -1.87,  -2.50,
       -0.24,  -0.30,   1.87,  -2.50,  -2.70,   0.00,   1.55,   2.03,
        2.20,  -2.98,  -1.99,  -2.20,   0.12,  -0.10,  -0.40,   0.50,
        1.59,   2.10,   0.00,   0.00,  -1.79,   2.00,  -1.03,   1.40,
       -1.15,  -1.60,   0.32,   0.50,   1.39,  -1.90,   2.35,  -1.27,
        1.70,   0.60,   0.80,  -0.32,  -0.40,   1.35,  -1.80,   0.44,
        0.00,   2.23,  -0.84,   0.90,  -1.27,  -1.40,  -1.47,   1.60,
       -0.28,  -0.30,  -0.28,   0.40,  -1.27,  -1.70,   0.28,  -0.40,
       -1.43,  -1.50,   0.00,   0.00,  -1.27,  -1.70,   2.11,  -0.32,
       -0.40,  -1.23,   1.60,   1.19,  -1.30,  -0.72,  -0.80,   0.72,

       /* 3284-3411 */
       -0.80,  -1.15,  -1.30,  -1.35,  -1.50,  -1.19,  -1.60,  -0.12,
        0.20,   1.79,   0.00,  -0.88,  -0.28,   0.40,   1.11,   1.50,
       -1.83,   0.00,   0.56,  -0.12,   0.10,  -1.27,  -1.40,   0.00,
        0.00,   1.15,   1.50,  -0.12,   0.20,   1.11,   1.50,   0.36,
       -0.50,  -1.07,  -1.40,  -1.11,   1.50,   1.67,   0.00,   0.80,
       -1.11,   0.00,   1.43,   1.23,  -1.30,  -0.24,  -1.19,  -1.30,
       -0.24,   0.20,  -0.44,  -0.90,  -0.95,   1.10,   1.07,  -1.40,
        1.15,  -1.30,   1.03,  -1.10,  -0.56,  -0.60,  -0.68,   0.90,
       -0.76,  -1.00,  -0.24,  -0.30,   0.95,  -1.30,   0.56,   0.70,
        0.84,  -1.10,  -0.56,   0.00,  -1.55,   0.91,  -1.30,   0.28,
        0.30,   0.16,  -0.20,   0.95,   1.30,   0.40,  -0.50,  -0.88,
       -1.20,   0.95,  -1.10,  -0.48,  -0.50,   0.00,   0.00,  -1.07,
        1.20,   0.44,  -0.50,   0.95,   1.10,   0.00,   0.00,   0.92,
       -1.30,   0.95,   1.00,  -0.52,   0.60,   1.59,   0.24,  -0.40,
        0.91,   1.20,   0.84,  -1.10,  -0.44,  -0.60,   0.84,   1.10,
       -0.44,   0.60,  -0.44,   0.60,  -0.84,  -1.10,  -0.80,   0.00,

       /* 3412-3539 */
        1.35,   0.76,   0.20,  -0.91,  -1.00,   0.20,  -0.30,  -0.91,
       -1.20,  -0.95,   1.00,  -0.48,  -0.50,   0.88,   1.00,   0.48,
       -0.50,  -0.95,  -1.10,   0.20,  -0.20,  -0.99,   1.10,  -0.84,
        1.10,  -0.24,  -0.30,   0.20,  -0.30,   0.84,   1.10,  -1.39,
        0.00,  -0.28,  -0.16,   0.20,   0.84,   1.10,   0.00,   0.00,
        1.39,   0.00,   0.00,  -0.95,   1.00,   1.35,  -0.99,   0.00,
        0.88,  -0.52,   0.00,  -1.19,   0.20,   0.20,   0.76,  -1.00,
        0.00,   0.00,   0.76,   1.00,   0.00,   0.00,   0.76,   1.00,
       -0.76,   1.00,   0.00,   0.00,   1.23,   0.76,   0.80,  -0.32,
        0.40,  -0.72,   0.80,  -0.40,  -0.40,   0.00,   0.00,  -0.80,
       -0.90,  -0.68,   0.90,  -0.16,  -0.20,  -0.16,  -0.20,   0.68,
       -0.90,  -0.36,   0.50,  -0.56,  -0.80,   0.72,  -0.90,   0.44,
       -0.60,  -0.48,  -0.70,  -0.16,   0.00,  -1.11,   0.32,   0.00,
       -1.07,   0.60,  -0.80,  -0.28,  -0.40,  -0.64,   0.00,   0.91,
        1.11,   0.64,  -0.90,   0.76,  -0.80,   0.00,   0.00,  -0.76,
       -0.80,   1.03,   0.00,  -0.36,  -0.64,  -0.70,   0.36,  -0.40,

       /* 3540-3667 */
        1.07,   0.36,  -0.50,  -0.52,  -0.70,   0.60,   0.00,   0.88,
        0.95,   0.00,   0.48,   0.16,  -0.20,   0.60,   0.80,   0.16,
       -0.20,  -0.60,  -0.80,   0.00,  -1.00,   0.12,   0.20,   0.16,
       -0.20,   0.68,   0.70,   0.59,  -0.80,  -0.99,  -0.56,  -0.60,
        0.36,  -0.40,  -0.68,  -0.70,  -0.68,  -0.70,  -0.36,  -0.50,
       -0.44,   0.60,   0.64,   0.70,  -0.12,   0.10,  -0.52,   0.60,
        0.36,   0.40,   0.00,   0.00,   0.95,  -0.84,   0.00,   0.44,
        0.56,   0.60,   0.32,  -0.30,   0.00,   0.00,   0.60,   0.70,
        0.00,   0.00,   0.60,   0.70,  -0.12,  -0.20,   0.52,  -0.70,
        0.00,   0.00,   0.56,   0.70,  -0.12,   0.10,  -0.52,  -0.70,
        0.00,   0.00,   0.88,  -0.76,   0.00,  -0.44,   0.00,   0.00,
       -0.52,  -0.70,   0.52,  -0.70,   0.36,  -0.40,  -0.44,  -0.50,
        0.00,   0.00,   0.60,   0.60,   0.84,   0.00,   0.12,  -0.24,
        0.00,   0.80,  -0.56,   0.60,  -0.32,  -0.30,   0.48,  -0.50,
        0.28,  -0.30,  -0.48,  -0.50,   0.12,   0.20,   0.48,  -0.60,
        0.48,   0.60,  -0.12,   0.20,   0.24,   0.00,   0.76,  -0.52,

        /* 3668-3795 */
       -0.60,  -0.52,   0.60,   0.48,  -0.50,  -0.24,  -0.30,   0.12,
       -0.10,   0.48,   0.60,   0.52,  -0.20,   0.36,   0.40,  -0.44,
        0.50,  -0.24,  -0.30,  -0.48,  -0.60,  -0.44,  -0.60,  -0.12,
        0.10,   0.76,   0.76,   0.20,  -0.20,   0.48,   0.50,   0.40,
       -0.50,  -0.24,  -0.30,   0.44,  -0.60,   0.44,  -0.60,   0.36,
        0.00,  -0.64,   0.72,   0.00,  -0.12,   0.00,  -0.10,  -0.40,
       -0.60,  -0.20,  -0.20,  -0.44,   0.50,  -0.44,   0.50,   0.20,
        0.20,  -0.44,  -0.50,   0.20,  -0.20,  -0.20,   0.20,  -0.44,
       -0.50,   0.64,   0.00,   0.32,  -0.36,   0.50,  -0.20,  -0.30,
        0.12,  -0.10,   0.48,   0.50,  -0.12,   0.30,  -0.36,  -0.50,
        0.00,   0.00,   0.48,   0.50,  -0.48,   0.50,   0.68,   0.00,
       -0.12,   0.56,  -0.40,   0.44,  -0.50,  -0.12,  -0.10,   0.24,
        0.30,  -0.40,   0.40,   0.64,   0.00,  -0.24,   0.64,   0.00,
       -0.20,   0.00,   0.00,   0.44,  -0.50,   0.44,   0.50,  -0.12,
        0.20,  -0.36,  -0.50,   0.12,   0.00,   0.64,  -0.40,   0.50,
        0.00,   0.10,   0.00,   0.00,  -0.40,   0.50,   0.00,   0.00,

        /* 3796-3923 */
       -0.40,  -0.50,   0.56,   0.00,   0.28,   0.00,   0.10,   0.36,
        0.50,   0.00,  -0.10,   0.36,  -0.50,   0.36,   0.50,   0.00,
       -0.10,   0.24,  -0.20,  -0.36,  -0.40,   0.16,   0.20,   0.40,
       -0.40,   0.00,   0.00,  -0.36,  -0.50,  -0.36,  -0.50,  -0.32,
       -0.50,  -0.12,   0.10,   0.20,   0.20,  -0.36,   0.40,  -0.60,
        0.60,   0.28,   0.00,   0.52,   0.12,  -0.10,   0.40,   0.40,
        0.00,  -0.50,   0.20,  -0.20,  -0.32,   0.40,   0.16,   0.20,
       -0.16,   0.20,   0.32,   0.40,   0.56,   0.00,  -0.12,   0.32,
       -0.40,  -0.16,  -0.20,   0.00,   0.00,   0.40,   0.40,  -0.40,
       -0.40,  -0.40,   0.40,  -0.36,   0.40,   0.12,   0.10,   0.00,
        0.10,   0.36,   0.40,   0.00,  -0.10,   0.36,   0.40,  -0.36,
        0.40,   0.00,   0.10,   0.32,   0.00,   0.44,   0.12,   0.20,
        0.28,  -0.40,   0.00,   0.00,   0.36,   0.40,   0.32,  -0.40,
       -0.16,   0.12,   0.10,   0.32,  -0.40,   0.20,   0.30,  -0.24,
        0.30,   0.00,   0.10,   0.32,   0.40,   0.00,  -0.10,  -0.32,
       -0.40,  -0.32,   0.40,   0.00,   0.10,  -0.52,  -0.52,   0.52,

       /* 3924-4051 */
        0.32,  -0.40,   0.00,   0.00,   0.32,   0.40,   0.32,  -0.40,
        0.00,   0.00,  -0.32,  -0.40,  -0.32,   0.40,   0.32,   0.40,
        0.00,   0.00,   0.32,   0.40,   0.00,   0.00,  -0.32,  -0.40,
        0.00,   0.00,   0.32,   0.40,   0.16,   0.20,   0.32,  -0.30,
       -0.16,   0.00,  -0.48,  -0.20,   0.20,  -0.28,  -0.30,   0.28,
       -0.40,   0.00,   0.00,   0.28,  -0.40,   0.00,   0.00,   0.28,
       -0.40,   0.00,   0.00,  -0.28,  -0.40,   0.28,   0.40,  -0.28,
       -0.40,  -0.48,  -0.20,   0.20,   0.24,   0.30,   0.44,   0.00,
        0.16,   0.24,   0.30,   0.16,  -0.20,   0.24,   0.30,  -0.12,
        0.20,   0.20,   0.30,  -0.16,   0.20,   0.00,   0.00,   0.44,
       -0.32,   0.30,   0.24,   0.00,  -0.36,   0.36,   0.00,   0.24,
        0.12,  -0.20,   0.20,   0.30,  -0.12,   0.00,  -0.28,   0.30,
       -0.24,   0.30,   0.12,   0.10,  -0.28,  -0.30,  -0.28,   0.30,
        0.00,   0.00,  -0.28,  -0.30,   0.00,   0.00,  -0.28,  -0.30,
        0.00,   0.00,   0.28,   0.30,   0.00,   0.00,  -0.28,  -0.30,
       -0.28,   0.30,   0.00,   0.00,  -0.28,  -0.30,   0.00,   0.00,

       /* 4052-4179 */
        0.28,   0.30,   0.00,   0.00,  -0.28,   0.30,   0.28,  -0.30,
       -0.28,   0.30,   0.40,   0.40,  -0.24,   0.30,   0.00,  -0.10,
        0.16,   0.00,   0.36,  -0.20,   0.30,  -0.12,  -0.10,  -0.24,
       -0.30,   0.00,   0.00,  -0.24,   0.30,  -0.24,   0.30,   0.00,
        0.00,  -0.24,   0.30,  -0.24,   0.30,   0.24,  -0.30,   0.00,
        0.00,   0.24,  -0.30,   0.00,   0.00,   0.24,   0.30,   0.24,
       -0.30,   0.24,   0.30,  -0.24,   0.30,  -0.24,   0.30,  -0.20,
        0.20,  -0.16,  -0.20,   0.00,   0.00,  -0.32,   0.20,   0.00,
        0.10,   0.20,  -0.30,   0.20,  -0.20,   0.12,   0.20,  -0.16,
        0.20,   0.16,   0.20,   0.20,   0.30,   0.20,   0.30,   0.00,
        0.00,  -0.20,   0.30,   0.00,   0.00,   0.20,   0.30,  -0.20,
       -0.30,  -0.20,  -0.30,   0.20,  -0.30,   0.00,   0.00,   0.20,
        0.30,   0.00,   0.00,   0.20,   0.30,   0.00,   0.00,   0.20,
        0.30,   0.00,   0.00,   0.20,   0.30,   0.00,   0.00,   0.20,
       -0.30,   0.00,   0.00,  -0.20,  -0.30,   0.00,   0.00,  -0.20,
        0.30,   0.00,   0.00,  -0.20,   0.30,   0.00,   0.00,   0.36,

        /* 4180-4307 */
        0.00,   0.00,   0.36,   0.12,   0.10,  -0.24,   0.20,   0.12,
       -0.20,  -0.16,  -0.20,  -0.13,   0.10,   0.22,   0.21,   0.20,
        0.00,  -0.28,   0.32,   0.00,  -0.12,  -0.20,  -0.20,   0.12,
       -0.10,   0.12,   0.10,  -0.20,   0.20,   0.00,   0.00,  -0.32,
        0.32,   0.00,   0.00,   0.32,   0.32,   0.00,   0.00,  -0.24,
       -0.20,   0.24,   0.20,   0.20,   0.00,  -0.24,   0.00,   0.00,
       -0.24,  -0.20,   0.00,   0.00,   0.24,   0.20,  -0.24,  -0.20,
        0.00,   0.00,  -0.24,   0.20,   0.16,  -0.20,   0.12,   0.10,
        0.20,   0.20,   0.00,  -0.10,  -0.12,   0.10,  -0.16,  -0.20,
       -0.12,  -0.10,  -0.16,   0.20,   0.20,   0.20,   0.00,   0.00,
       -0.20,   0.20,  -0.20,   0.20,  -0.20,   0.20,  -0.20,   0.20,
        0.20,  -0.20,  -0.20,  -0.20,   0.00,   0.00,  -0.20,   0.20,
        0.20,   0.00,  -0.20,   0.00,   0.00,  -0.20,   0.20,  -0.20,
        0.20,  -0.20,  -0.20,  -0.20,  -0.20,   0.00,   0.00,   0.20,
        0.20,   0.20,   0.20,   0.12,  -0.20,  -0.12,  -0.10,   0.28,
       -0.28,   0.16,  -0.20,   0.00,  -0.10,   0.00,   0.10,  -0.16,

       /* 4308-4435 */
        0.20,   0.00,  -0.10,  -0.16,  -0.20,   0.00,  -0.10,   0.16,
       -0.20,   0.16,  -0.20,   0.00,   0.00,   0.16,   0.20,  -0.16,
        0.20,   0.00,   0.00,   0.16,   0.20,   0.16,  -0.20,   0.16,
       -0.20,  -0.16,   0.20,   0.16,  -0.20,   0.00,   0.00,   0.16,
        0.20,   0.00,   0.00,   0.16,   0.20,   0.00,   0.00,  -0.16,
       -0.20,   0.16,  -0.20,  -0.16,  -0.20,   0.00,   0.00,  -0.16,
       -0.20,   0.00,   0.00,  -0.16,   0.20,   0.00,   0.00,   0.16,
       -0.20,   0.16,   0.20,   0.16,   0.20,   0.00,   0.00,  -0.16,
       -0.20,   0.00,   0.00,  -0.16,  -0.20,   0.00,   0.00,   0.16,
        0.20,   0.16,   0.20,   0.00,   0.00,   0.16,   0.20,   0.16,
       -0.20,   0.16,   0.20,   0.00,   0.00,  -0.16,   0.20,   0.00,
        0.10,   0.12,  -0.20,   0.12,  -0.20,   0.00,  -0.10,   0.00,
       -0.10,   0.12,   0.20,   0.00,  -0.10,  -0.12,   0.20,  -0.15,
        0.20,  -0.24,   0.24,   0.00,   0.00,   0.24,   0.24,   0.12,
       -0.20,  -0.12,  -0.20,   0.00,   0.00,   0.12,   0.20,   0.12,
       -0.20,   0.12,   0.20,   0.12,   0.20,   0.12,   0.20,   0.12,

       /* 4436-4563 */
       -0.20,  -0.12,   0.20,   0.00,   0.00,   0.12,   0.20,   0.12,
        0.00,  -0.20,   0.00,   0.00,  -0.12,  -0.20,   0.12,  -0.20,
        0.00,   0.00,   0.12,   0.20,  -0.12,   0.20,  -0.12,   0.20,
        0.12,  -0.20,   0.00,   0.00,   0.12,   0.20,   0.20,   0.00,
        0.12,   0.00,   0.00,  -0.12,   0.20,   0.00,   0.00,  -0.12,
       -0.20,   0.00,   0.00,  -0.12,  -0.20,  -0.12,  -0.20,   0.00,
        0.00,   0.12,  -0.20,   0.12,  -0.20,   0.12,   0.20,  -0.12,
       -0.20,   0.00,   0.00,   0.12,  -0.20,   0.12,  -0.20,   0.12,
        0.20,   0.12,   0.00,   0.20,  -0.12,  -0.20,   0.00,   0.00,
        0.12,   0.20,  -0.16,   0.00,   0.16,  -0.20,   0.20,   0.00,
        0.00,  -0.20,   0.00,   0.00,  -0.20,   0.20,   0.00,   0.00,
        0.20,   0.20,  -0.20,   0.00,   0.00,  -0.20,   0.12,   0.00,
       -0.16,   0.20,   0.00,   0.00,   0.20,   0.12,  -0.10,   0.00,
        0.10,   0.16,  -0.16,  -0.16,  -0.16,  -0.16,  -0.16,   0.00,
        0.00,  -0.16,   0.00,   0.00,  -0.16,  -0.16,  -0.16,   0.00,
        0.00,  -0.16,   0.00,   0.00,   0.16,   0.00,   0.00,   0.16,

        /* 4564-4691 */
        0.00,   0.00,   0.16,   0.16,   0.00,   0.00,  -0.16,   0.00,
        0.00,  -0.16,  -0.16,   0.00,   0.00,   0.16,   0.00,   0.00,
       -0.16,  -0.16,   0.00,   0.00,  -0.16,  -0.16,   0.12,   0.10,
        0.12,  -0.10,   0.12,   0.10,   0.00,   0.00,   0.12,   0.10,
       -0.12,   0.10,   0.00,   0.00,   0.12,   0.10,   0.12,  -0.10,
        0.00,   0.00,  -0.12,  -0.10,   0.00,   0.00,   0.12,   0.10,
        0.12,   0.00,   0.00,   0.12,   0.00,   0.00,  -0.12,   0.00,
        0.00,   0.12,   0.12,   0.12,   0.12,   0.12,   0.00,   0.00,
        0.12,   0.00,   0.00,   0.12,   0.12,   0.00,   0.00,   0.12,
        0.00,   0.00,   0.12,  -0.12,  -0.12,   0.12,   0.12,  -0.12,
       -0.12,   0.00,   0.00,   0.12,  -0.12,   0.12,   0.12,  -0.12,
       -0.12,   0.00,   0.00,  -0.12,  -0.12,   0.00,   0.00,  -0.12,
        0.12,   0.00,   0.00,   0.12,   0.00,   0.00,   0.12,   0.00,
        0.00,   0.12,  -0.12,   0.00,   0.00,  -0.12,   0.12,  -0.12,
       -0.12,   0.12,   0.00,   0.00,   0.12,   0.12,   0.12,  -0.12,
        0.00,   0.00,  -0.12,  -0.12,  -0.12,   0.00,   0.00,  -0.12,

        /* 4692-NA */
       -0.12,   0.00,   0.00,   0.12,   0.12,   0.00,   0.00,  -0.12,
       -0.12,  -0.12,  -0.12,   0.12,   0.00,   0.00,   0.12,  -0.12,
        0.00,   0.00,  -0.12,  -0.12,   0.00,   0.00,   0.12,  -0.12,
       -0.12,  -0.12,  -0.12,   0.12,   0.12,  -0.12,  -0.12,   0.00,
        0.00,  -0.12,   0.00,   0.00,  -0.12,   0.12,   0.00,   0.00,
        0.12,   0.00,   0.00,  -0.12,  -0.12,   0.00,   0.00,  -0.12,
       -0.12,   0.12,   0.00,   0.00,   0.12,   0.12,   0.00,   0.00,
        0.12,   0.00,   0.00,   0.12,   0.12,   0.08,   0.00,   0.04
       };

       /* Number of amplitude coefficients */
       static const int NA = (int) (sizeof a / sizeof (double));

       /* Amplitude usage: X or Y, sin or cos, power of T. */
       static const int jaxy[] = {0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1};
       static const int jasc[] = {0,1,1,0,1,0,0,1,0,1,1,0,1,0,0,1,0,1,1,0};
       static const int japt[] = {0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4};

       /* Miscellaneous */
       double t, w, pt[MAXPT+1], fa[14], xypr[2], xypl[2], xyls[2], arg,
              sc[2];
       int jpt, i, j, jxy, ialast, ifreq, m, ia, jsc;

       /*--------------------------------------------------------------------*/

       /* Interval between fundamental date J2000.0 and given date (JC). */
       t = (MJD(TT).mjd - MJD_J2000) / JC;

       /* Powers of T. */
       w = 1.0;
       for (jpt = 0; jpt <= MAXPT; jpt++) {
           pt[jpt] = w;
           w *= t;
       }

       /* Initialize totals in X and Y:  polynomial, luni-solar, planetary. */
       for (jxy = 0; jxy < 2; jxy++) {
           xypr[jxy] = 0.0;
           xyls[jxy] = 0.0;
           xypl[jxy] = 0.0;
       }

       /* --------------------------------- */
       /* Fundamental arguments (IERS 2003) */
       /* --------------------------------- */

       /* Mean anomaly of the Moon. */
       fa[0] = std::fmod(         485868.249036  +
                        t * ( 1717915923.2178 +
                 	    t * (         31.8792 +
                 	    t * (          0.051635 +
                 	    t * (        - 0.00024470 ) ) ) ), TURNAS ) * AS_TO_RAD;

       /* Mean anomaly of the Sun. */
       fa[1] = std::fmod(       1287104.793048 +
                        t * ( 129596581.0481 +
             	        t * (       - 0.5532 +
             	        t * (         0.000136 +
             	        t * (       - 0.00001149 ) ) ) ), TURNAS ) * AS_TO_RAD;

       /* Mean argument of the latitude of the Moon. */
       fa[2] = std::fmod(         335779.526232 +
                 	    t * ( 1739527262.8478 +
                        t * (       - 12.7512 +
                 	    t * (        - 0.001037 +
                 	    t * (          0.00000417 ) ) ) ), TURNAS ) * AS_TO_RAD;


       /* Mean elongation of the Moon from the Sun. */
       fa[3] = std::fmod(        1072260.703692 +
                 	    t * ( 1602961601.2090 +
                 	    t * (        - 6.3706 +
                 	    t * (          0.006593 +
             	        t * (        - 0.00003169 ) ) ) ), TURNAS ) * AS_TO_RAD;
 
       /* Mean longitude of the ascending node of the Moon. */
       fa[4] = std::fmod(        450160.398036 +
                 	    t * ( - 6962890.5431 +
                 	    t * (         7.4722 +
                 	    t * (         0.007702 +
                 	    t * (       - 0.00005939 ) ) ) ), TURNAS ) * AS_TO_RAD;

       /* Planetary longitudes, Mercury through Neptune. */
       fa[5] = std::fmod(4.402608842 + 2608.7903141574 * t, TWO_PI);
       fa[6] = std::fmod(3.176146697 + 1021.3285546211 * t, TWO_PI);
       fa[7] = std::fmod(1.753470314 + 628.3075849991 * t, TWO_PI);
       fa[8] = std::fmod(6.203480913 + 334.0612426700 * t, TWO_PI);
       fa[9] = std::fmod(0.599546497 + 52.9690962641 * t, TWO_PI);
       fa[10] = std::fmod(0.874016757 + 21.3299104960 * t, TWO_PI);
       fa[11] = std::fmod(5.481293872 + 7.4781598567 * t, TWO_PI);
       fa[12] = std::fmod(5.311886287 + 3.8133035638 * t, TWO_PI);

       /* General accumulated precession in longitude. */
       fa[13] = (0.024381750 + 0.00000538691 * t) * t;

       /* -------------------------------------- */
       /* Polynomial part of precession-nutation */
       /* -------------------------------------- */

       for (jxy = 0; jxy < 2; jxy++) {
           for (j = MAXPT; j >= 0; j--) {
               xypr[jxy] += xyp[jxy][j] * pt[j];
           }
       }

       /* ---------------------------------- */
       /* Nutation periodic terms, planetary */
       /* ---------------------------------- */

       /* Work backwards through the coefficients per frequency list. */
       ialast = NA;
       for (ifreq = NFPL-1; ifreq >= 0; ifreq--) {

           /* Obtain the argument functions. */
           arg = 0.0;
           for (i = 0; i < 14; i++) {
               m = mfapl[ifreq][i];
               if (m != 0) arg += (double)m * fa[i];
           }
           sc[0] = std::sin(arg);
           sc[1] = std::cos(arg);

           /* Work backwards through the amplitudes at this frequency. */
           ia = nc[ifreq+NFLS];
           for (i = ialast; i >= ia; i--) {

               /* Coefficient number (0 = 1st). */
               j = i-ia;

               /* X or Y. */
               jxy = jaxy[j];

               /* Sin or cos. */
               jsc = jasc[j];

               /* Power of T. */
               jpt = japt[j];

               /* Accumulate the component. */
               xypl[jxy] += a[i-1] * sc[jsc] * pt[jpt];
           }
           ialast = ia-1;
       }

       /* ----------------------------------- */
       /* Nutation periodic terms, luni-solar */
       /* ----------------------------------- */

       /* Continue working backwards through the number of coefficients list. */
       for (ifreq = NFLS-1; ifreq >= 0; ifreq--) {

           /* Obtain the argument functions. */
           arg = 0.0;
           for (i = 0; i < 5; i++) {
               m = mfals[ifreq][i];
               if (m != 0) arg += (double)m * fa[i];
           }
           sc[0] = std::sin(arg);
           sc[1] = std::cos(arg);

           /* Work backwards through the amplitudes at this frequency. */
           ia = nc[ifreq];
           for (i = ialast; i >= ia; i--) {

               /* Coefficient number (0 = 1st). */
               j = i-ia;

               /* X or Y. */
               jxy = jaxy[j];

               /* Sin or cos. */
               jsc = jasc[j];

               /* Power of T. */
               jpt = japt[j];

               /* Accumulate the component. */
               xyls[jxy] += a[i-1] * sc[jsc] * pt[jpt];
           }
           ialast = ia-1;
       }

       /* ------------------------------------ */
       /* Results:  CIP unit vector components */
       /* ------------------------------------ */

       x = AS_TO_RAD * (xypr[0] + (xyls[0] + xypl[0]) / 1e6);
       y = AS_TO_RAD * (xypr[1] + (xyls[1] + xypl[1]) / 1e6);

   }


   // The CIO locator s, positioning the Celestial Intermediate Origin on
   // the equator of the Celestial Intermediate Pole, given the CIP's X,Y
   // coordinates.  Compatible with IAU 2006/2000A precession-nutation.
   double ReferenceSystem::S06(const CommonTime& TT, const double& x, const double& y)
   {
       /* Time since J2000.0, in Julian centuries */
       double t;

       /* Miscellaneous */
       int i, j;
       double a, w0, w1, w2, w3, w4, w5;

       /* Fundamental arguments */
       double fa[8];

       /* Returned value */
       double s;

       /* --------------------- */
       /* The series for s+XY/2 */
       /* --------------------- */

       typedef struct {
           int nfa[8];      /* coefficients of l,l',F,D,Om,LVe,LE,pA */
           double s, c;     /* sine and cosine coefficients */
       } TERM;

       /* Polynomial coefficients */
       static const double sp[] = {

           /* 1-6 */
           94.00e-6,
         3808.65e-6,
         -122.68e-6,
       -72574.11e-6,
           27.98e-6,
           15.62e-6
       };

       /* Terms of order t^0 */
       static const TERM s0[] = {

           /* 1-10 */
           {{ 0,  0,  0,  0,  1,  0,  0,  0}, -2640.73e-6,   0.39e-6 },
           {{ 0,  0,  0,  0,  2,  0,  0,  0},   -63.53e-6,   0.02e-6 },
           {{ 0,  0,  2, -2,  3,  0,  0,  0},   -11.75e-6,  -0.01e-6 },
           {{ 0,  0,  2, -2,  1,  0,  0,  0},   -11.21e-6,  -0.01e-6 },
           {{ 0,  0,  2, -2,  2,  0,  0,  0},     4.57e-6,   0.00e-6 },
           {{ 0,  0,  2,  0,  3,  0,  0,  0},    -2.02e-6,   0.00e-6 },
           {{ 0,  0,  2,  0,  1,  0,  0,  0},    -1.98e-6,   0.00e-6 },
           {{ 0,  0,  0,  0,  3,  0,  0,  0},     1.72e-6,   0.00e-6 },
           {{ 0,  1,  0,  0,  1,  0,  0,  0},     1.41e-6,   0.01e-6 },
           {{ 0,  1,  0,  0, -1,  0,  0,  0},     1.26e-6,   0.01e-6 },

           /* 11-20 */
           {{ 1,  0,  0,  0, -1,  0,  0,  0},     0.63e-6,   0.00e-6 },
           {{ 1,  0,  0,  0,  1,  0,  0,  0},     0.63e-6,   0.00e-6 },
           {{ 0,  1,  2, -2,  3,  0,  0,  0},    -0.46e-6,   0.00e-6 },
           {{ 0,  1,  2, -2,  1,  0,  0,  0},    -0.45e-6,   0.00e-6 },
           {{ 0,  0,  4, -4,  4,  0,  0,  0},    -0.36e-6,   0.00e-6 },
           {{ 0,  0,  1, -1,  1, -8, 12,  0},     0.24e-6,   0.12e-6 },
           {{ 0,  0,  2,  0,  0,  0,  0,  0},    -0.32e-6,   0.00e-6 },
           {{ 0,  0,  2,  0,  2,  0,  0,  0},    -0.28e-6,   0.00e-6 },
           {{ 1,  0,  2,  0,  3,  0,  0,  0},    -0.27e-6,   0.00e-6 },
           {{ 1,  0,  2,  0,  1,  0,  0,  0},    -0.26e-6,   0.00e-6 },

           /* 21-30 */
           {{ 0,  0,  2, -2,  0,  0,  0,  0},     0.21e-6,   0.00e-6 },
           {{ 0,  1, -2,  2, -3,  0,  0,  0},    -0.19e-6,   0.00e-6 },
           {{ 0,  1, -2,  2, -1,  0,  0,  0},    -0.18e-6,   0.00e-6 },
           {{ 0,  0,  0,  0,  0,  8,-13, -1},     0.10e-6,  -0.05e-6 },
           {{ 0,  0,  0,  2,  0,  0,  0,  0},    -0.15e-6,   0.00e-6 },
           {{ 2,  0, -2,  0, -1,  0,  0,  0},     0.14e-6,   0.00e-6 },
           {{ 0,  1,  2, -2,  2,  0,  0,  0},     0.14e-6,   0.00e-6 },
           {{ 1,  0,  0, -2,  1,  0,  0,  0},    -0.14e-6,   0.00e-6 },
           {{ 1,  0,  0, -2, -1,  0,  0,  0},    -0.14e-6,   0.00e-6 },
           {{ 0,  0,  4, -2,  4,  0,  0,  0},    -0.13e-6,   0.00e-6 },

           /* 31-33 */
           {{ 0,  0,  2, -2,  4,  0,  0,  0},     0.11e-6,   0.00e-6 },
           {{ 1,  0, -2,  0, -3,  0,  0,  0},    -0.11e-6,   0.00e-6 },
           {{ 1,  0, -2,  0, -1,  0,  0,  0},    -0.11e-6,   0.00e-6 }
       };

       /* Terms of order t^1 */
       static const TERM s1[] = {

           /* 1 - 3 */
           {{ 0,  0,  0,  0,  2,  0,  0,  0},    -0.07e-6,   3.57e-6 },
           {{ 0,  0,  0,  0,  1,  0,  0,  0},     1.73e-6,  -0.03e-6 },
           {{ 0,  0,  2, -2,  3,  0,  0,  0},     0.00e-6,   0.48e-6 }
       };

       /* Terms of order t^2 */
       static const TERM s2[] = {

           /* 1-10 */
           {{ 0,  0,  0,  0,  1,  0,  0,  0},   743.52e-6,  -0.17e-6 },
           {{ 0,  0,  2, -2,  2,  0,  0,  0},    56.91e-6,   0.06e-6 },
           {{ 0,  0,  2,  0,  2,  0,  0,  0},     9.84e-6,  -0.01e-6 },
           {{ 0,  0,  0,  0,  2,  0,  0,  0},    -8.85e-6,   0.01e-6 },
           {{ 0,  1,  0,  0,  0,  0,  0,  0},    -6.38e-6,  -0.05e-6 },
           {{ 1,  0,  0,  0,  0,  0,  0,  0},    -3.07e-6,   0.00e-6 },
           {{ 0,  1,  2, -2,  2,  0,  0,  0},     2.23e-6,   0.00e-6 },
           {{ 0,  0,  2,  0,  1,  0,  0,  0},     1.67e-6,   0.00e-6 },
           {{ 1,  0,  2,  0,  2,  0,  0,  0},     1.30e-6,   0.00e-6 },
           {{ 0,  1, -2,  2, -2,  0,  0,  0},     0.93e-6,   0.00e-6 },

           /* 11-20 */
           {{ 1,  0,  0, -2,  0,  0,  0,  0},     0.68e-6,   0.00e-6 },
           {{ 0,  0,  2, -2,  1,  0,  0,  0},    -0.55e-6,   0.00e-6 },
           {{ 1,  0, -2,  0, -2,  0,  0,  0},     0.53e-6,   0.00e-6 },
           {{ 0,  0,  0,  2,  0,  0,  0,  0},    -0.27e-6,   0.00e-6 },
           {{ 1,  0,  0,  0,  1,  0,  0,  0},    -0.27e-6,   0.00e-6 },
           {{ 1,  0, -2, -2, -2,  0,  0,  0},    -0.26e-6,   0.00e-6 },
           {{ 1,  0,  0,  0, -1,  0,  0,  0},    -0.25e-6,   0.00e-6 },
           {{ 1,  0,  2,  0,  1,  0,  0,  0},     0.22e-6,   0.00e-6 },
           {{ 2,  0,  0, -2,  0,  0,  0,  0},    -0.21e-6,   0.00e-6 },
           {{ 2,  0, -2,  0, -1,  0,  0,  0},     0.20e-6,   0.00e-6 },

           /* 21-25 */
           {{ 0,  0,  2,  2,  2,  0,  0,  0},     0.17e-6,   0.00e-6 },
           {{ 2,  0,  2,  0,  2,  0,  0,  0},     0.13e-6,   0.00e-6 },
           {{ 2,  0,  0,  0,  0,  0,  0,  0},    -0.13e-6,   0.00e-6 },
           {{ 1,  0,  2, -2,  2,  0,  0,  0},    -0.12e-6,   0.00e-6 },
           {{ 0,  0,  2,  0,  0,  0,  0,  0},    -0.11e-6,   0.00e-6 }
       };

       /* Terms of order t^3 */
       static const TERM s3[] = {

           /* 1-4 */
           {{ 0,  0,  0,  0,  1,  0,  0,  0},     0.30e-6, -23.42e-6 },
           {{ 0,  0,  2, -2,  2,  0,  0,  0},    -0.03e-6,  -1.46e-6 },
           {{ 0,  0,  2,  0,  2,  0,  0,  0},    -0.01e-6,  -0.25e-6 },
           {{ 0,  0,  0,  0,  2,  0,  0,  0},     0.00e-6,   0.23e-6 }
       };

       /* Terms of order t^4 */
       static const TERM s4[] = {

           /* 1-1 */
           {{ 0,  0,  0,  0,  1,  0,  0,  0},    -0.26e-6,  -0.01e-6 }
       };

       /* Number of terms in the series */
       static const int NS0 = (int) (sizeof s0 / sizeof (TERM));
       static const int NS1 = (int) (sizeof s1 / sizeof (TERM));
       static const int NS2 = (int) (sizeof s2 / sizeof (TERM));
       static const int NS3 = (int) (sizeof s3 / sizeof (TERM));
       static const int NS4 = (int) (sizeof s4 / sizeof (TERM));

       /*--------------------------------------------------------------------*/

       /* Interval between fundamental epoch J2000.0 and current date (JC). */
       t = (MJD(TT).mjd - MJD_J2000) / JC;

       /* Fundamental Arguments (from IERS Conventions 2003) */

       /* Mean anomaly of the Moon. */
       fa[0] = std::fmod(         485868.249036  +
                        t * ( 1717915923.2178 +
                 	    t * (         31.8792 +
                 	    t * (          0.051635 +
                 	    t * (        - 0.00024470 ) ) ) ), TURNAS ) * AS_TO_RAD;

       /* Mean anomaly of the Sun. */
       fa[1] = std::fmod(       1287104.793048 +
                        t * ( 129596581.0481 +
             	        t * (       - 0.5532 +
             	        t * (         0.000136 +
             	        t * (       - 0.00001149 ) ) ) ), TURNAS ) * AS_TO_RAD;

       /* Mean argument of the latitude of the Moon. */
       fa[2] = std::fmod(         335779.526232 +
                 	    t * ( 1739527262.8478 +
                        t * (       - 12.7512 +
                 	    t * (        - 0.001037 +
                 	    t * (          0.00000417 ) ) ) ), TURNAS ) * AS_TO_RAD;


       /* Mean elongation of the Moon from the Sun. */
       fa[3] = std::fmod(        1072260.703692 +
                 	    t * ( 1602961601.2090 +
                 	    t * (        - 6.3706 +
                 	    t * (          0.006593 +
             	        t * (        - 0.00003169 ) ) ) ), TURNAS ) * AS_TO_RAD;
 
       /* Mean longitude of the ascending node of the Moon. */
       fa[4] = std::fmod(        450160.398036 +
                 	    t * ( - 6962890.5431 +
                 	    t * (         7.4722 +
                 	    t * (         0.007702 +
                 	    t * (       - 0.00005939 ) ) ) ), TURNAS ) * AS_TO_RAD;

       /* Mean longitude of Venus. */
       fa[5] = std::fmod(3.176146697 + 1021.3285546211 * t, TWO_PI);

       /* Mean longitude of Earth. */
       fa[6] = std::fmod(1.753470314 + 628.3075849991 * t, TWO_PI);

       /* General precession in longitude. */
       fa[7] = (0.024381750 + 0.00000538691 * t) * t;

       /* Evaluate s. */
       w0 = sp[0];
       w1 = sp[1];
       w2 = sp[2];
       w3 = sp[3];
       w4 = sp[4];
       w5 = sp[5];

       for (i = NS0-1; i >= 0; i--) {
           a = 0.0;
           for (j = 0; j < 8; j++) {
               a += (double)s0[i].nfa[j] * fa[j];
           }
           w0 += s0[i].s * std::sin(a) + s0[i].c * std::cos(a);
       }

       for (i = NS1-1; i >= 0; i--) {
           a = 0.0;
           for (j = 0; j < 8; j++) {
               a += (double)s1[i].nfa[j] * fa[j];
           }
           w1 += s1[i].s * std::sin(a) + s1[i].c * std::cos(a);
       }

       for (i = NS2-1; i >= 0; i--) {
           a = 0.0;
           for (j = 0; j < 8; j++) {
               a += (double)s2[i].nfa[j] * fa[j];
           }
           w2 += s2[i].s * std::sin(a) + s2[i].c * std::cos(a);
       }

       for (i = NS3-1; i >= 0; i--) {
           a = 0.0;
           for (j = 0; j < 8; j++) {
               a += (double)s3[i].nfa[j] * fa[j];
           }
           w3 += s3[i].s * std::sin(a) + s3[i].c * std::cos(a);
       }

       for (i = NS4-1; i >= 0; i--) {
           a = 0.0;
           for (j = 0; j < 8; j++) {
               a += (double)s4[i].nfa[j] * fa[j];
           }
           w4 += s4[i].s * std::sin(a) + s4[i].c * std::cos(a);
       }

       s = (w0 +
           (w1 +
           (w2 +
           (w3 +
           (w4 +
            w5 * t) * t) * t) * t) * t) * AS_TO_RAD - x*y/2.0;

       return s;
   }


   // Form the celestial to intermediate-frame-of-date matrix given the CIP
   // X,Y and CIO locator s.
   Matrix<double> ReferenceSystem::C2IXYS(const double& x, const double& y, const double& s)
   {
       double r2, e, d;

       /* Obtaion the spherical angles E and d. */
       r2 = x*x + y*y;
       e = (r2 > 0.0) ? std::atan2(y,x) : 0.0;
       d = std::atan(std::sqrt(r2 / (1.0 - r2)));

       /* Form the matrix. */
       Matrix<double> rc2i(3,3,0.0);
       rc2i(0,0) = 1.0; rc2i(1,1) = 1.0; rc2i(2,2) = 1.0;
       rc2i = rotation(-(e+s),3) * rotation(d,2) * rotation(e,3);

       return rc2i;
   }


   // Earth rotation angle (IAU 2000 model).
   double ReferenceSystem::ERA00(const CommonTime& UT1)
   {
       double d1, d2, t, f, theta;

       /* Days since fundamental epoch. */
/*       
       long day, sod; double fsod;
       ut1.get(day, sod, fsod);

       double dj1, dj2;
       dj1 = double(day-0.5);       // Julian Date, Integer Part
       dj2 = (sod+fsod) / 86400.0;  // Julian Date, Fractional Part
       d1 = dj2;
       d2 = dj1;

       t = d1 + (d2 - JD_J2000);
*/
       t = (MJD(UT1).mjd - MJD_J2000);

       /* Fractional part of T (days). */
//       f = std::fmod(d1,1.0) + std::fmod(d2,1.0);
       f = std::fmod(t,1.0);

       /* Earth rotation angle at this UT1. */
       theta = Anp(TWO_PI * (f + 0.7790572732640
                               + 0.00273781191135448 * t));

       return theta;
   }

   // The TIO locator s', positioning the Terrestrial Intermediate Origin
   // on the equator of the Celestial Intermediate Pole.
   double ReferenceSystem::SP00(const CommonTime& TT)
   {
       double t, sp;

       /* Interval between fundamental epoch J2000.0 and current date (JC). */
       t = (MJD(TT).mjd - MJD_J2000) / JC;

       /* Approximate s'. */
       sp = -47e-6 * t * AS_TO_RAD;

       return sp;
   }


   // Form the matrix of polar motion for a given date, IAU 2000.
   Matrix<double> ReferenceSystem::POM00(const double& xp, const double& yp, const double& sp)
   {
       Matrix<double> rpom(3,3,0.0);
       rpom(0,0) = 1.0; rpom(1,1) = 1.0; rpom(2,2) = 1.0;

       /* Construct the matrix. */
       rpom = rotation(-yp,1) * rotation(-xp,2) * rotation(sp,3);

       return rpom;
   }


   // CRS to TRS Matrix
   Matrix<double> ReferenceSystem::C2TMatrix(const CommonTime& utc)
   {
       // TT
       CommonTime tt( UTC2TT(utc) );

       // day, sod, fsod and timeSystem of utc
       long day, sod;
       double fsod;
       TimeSystem timeSystem;

       utc.get(day, sod, fsod, timeSystem);

       // get the epoch before and after utc
       CommonTime utc1, utc2;
       utc1.set(day, timeSystem);
       utc2.set(day+1, timeSystem);

       // get the eop before and after utc
       EOPDataStore::EOPData eop1, eop2;
       eop1 = getEOPData(utc1);
       eop2 = getEOPData(utc2);
/*
       cout << eop1 << endl;
       cout << eop2 << endl;
       eop1.xp = 0.030770; eop1.yp = 0.280745;
       eop1.UT1mUTC = -0.4599141;
       eop1.dPsi = -0.000125; eop1.dEps = -0.000016;
       eop2.xp = 0.029627; eop2.yp = 0.281248;
       eop2.UT1mUTC = -0.4608362;
       eop2.dPsi = -0.000156; eop2.dEps = 0.000025;
       cout << eop1 << endl;
       cout << eop2 << endl;
*/
//       cout << "EOP before reduce UT1-UTC to UT1R-UTC:" << endl;
//       cout << CivilTime(utc1) << eop1 << endl;
//       cout << CivilTime(utc2) << eop2 << endl;


       // reduce UT1-UTC to UT1R-UTC for interpolation
       CommonTime tt1(UTC2TT(utc1)), tt2(UTC2TT(utc2));

       Vector<double> cor_zonalTide1( RG_ZONT2(tt1) );
       Vector<double> cor_zonalTide2( RG_ZONT2(tt2) );

       eop1.UT1mUTC -= cor_zonalTide1(0);
       eop2.UT1mUTC -= cor_zonalTide2(0);

//       cout << "EOP after reduce UT1-UTC to UT1R-UTC (subroutine RG_ZONT2):" << endl;
//       cout << CivilTime(utc1) << eop1 << endl;
//       cout << CivilTime(utc2) << eop2 << endl;

       // interpolation of eop
       EOPDataStore::EOPData eop;
       eop.xp       =   eop1.xp + (eop2.xp-eop1.xp) * ((utc-utc1)/(utc2-utc1));
       eop.yp       =   eop1.yp + (eop2.yp-eop1.yp) * ((utc-utc1)/(utc2-utc1));
       eop.UT1mUTC  =   eop1.UT1mUTC + (eop2.UT1mUTC-eop1.UT1mUTC) * ((utc-utc1)/(utc2-utc1));
       eop.dPsi     =   eop1.dPsi + (eop2.dPsi-eop1.dPsi) * ((utc-utc1)/(utc2-utc1));
       eop.dEps     =   eop1.dEps + (eop2.dEps-eop1.dEps) * ((utc-utc1)/(utc2-utc1));
//       cout << "EOP interpolation:" << endl;
//       cout << CivilTime(utc) << eop << endl;

       // add subdaily pole model to pole values
       // Tidal variations in the Earth's rotation
       // 1. diurnal/subdiurnal tidal effect on polar motion ("), UT1 (s) and
       //    LOD (s).
       // 2. diurnal lunisolar effect on polar motion (").
       Vector<double> cor_oceanTides = PMUT1_OCEANS(utc);   // xp, yp, ut1, lod
       Vector<double> cor_libration1 = PMSDNUT2(utc);       // xp, yp
       Vector<double> cor_libration2 = UTLIBR(utc);         // ut1, lod

//       cout << "OceanTides from PMUT1_OCEANS subroutine: " << endl
//            << setw(12) << cor_oceanTides << endl;
//       cout << "Libration1 from PMSDNUT2     subroutine: " << endl
//            << setw(12) << cor_libration1 << endl;
//       cout << "Libration2 from UTLIBR       subroutine: " << endl
//            << setw(12) << cor_libration2 << endl;

       eop.UT1mUTC  +=  cor_oceanTides(2) + cor_libration2(0);
       eop.xp       +=  cor_oceanTides(0) + cor_libration1(0);
       eop.yp       +=  cor_oceanTides(1) + cor_libration1(1);
//       cout << "EOP after add subdaily pole model:" << endl;
//       cout << CivilTime(utc) << eop << endl;

       // correct interpolated UT1R-UTC to UT1-UTC
       Vector<double> cor_zonalTide( RG_ZONT2(tt) );
       eop.UT1mUTC += cor_zonalTide(0);
//       cout << "EOP after correct interpolated UT1R-UTC to UT1-UTC:" << endl;
//       cout << CivilTime(utc) << eop << endl;

       eop.xp   *=  AS_TO_RAD;
       eop.yp   *=  AS_TO_RAD;
       eop.dPsi *=  AS_TO_RAD;
       eop.dEps *=  AS_TO_RAD;

       // UT1
       CommonTime ut1(UTC2UT1(utc, eop.UT1mUTC));

       // X,Y and s
       double x,y,s;
       XY06(tt, x, y);
       s = S06(tt, x, y);
       x += eop.dPsi;
       y += eop.dEps;

       // Q
       Matrix<double> Q = C2IXYS(x,y,s);

       // R
       double era = ERA00(ut1);
       Matrix<double> R(3,3,0.0);
//       R(0,0) = 1.0; R(1,1) = 1.0; R(2,2) = 1.0;
//       Rz(era, R);
       R = rotation(era,3);

       // W
       double sp = SP00(tt);
       Matrix<double> W = POM00(eop.xp, eop.yp, sp);

       return W * R * Q;

   }
 

   // TRS to CRS Matrix
   Matrix<double> ReferenceSystem::T2CMatrix(const CommonTime& UTC)
   {
       return transpose( C2TMatrix(UTC) );
   }


   // Earth rotation angle rate (IAU 2000 model)
   double ReferenceSystem::dERA00(const CommonTime& UT1)
   {
       //
       const double t( (MJD(UT1).mjd - MJD_J2000) / JC );

       // 
       double dtheta = 1.00273781191135448 * TWO_PI / DAY_TO_SEC;

       return dtheta;
   }


   // Time derivative of transformation matrix from CRS to TRS coordinates
   // for a given date
   Matrix<double> ReferenceSystem::dC2TMatrix(const CommonTime& utc)
   {
       // UT1
       CommonTime ut1( UTC2UT1(utc) );

       // TT
       CommonTime tt( UTC2TT(utc) );

       // Earth Orientation Parameters
       double xp     =  getXPole(utc);    // arcsecond
       double yp     =  getYPole(utc);    // arcsecond
       double dut1   =  getUT1mUTC(utc);  // second
       double dX     =  getDPsi(utc);     // arcsecond
       double dY     =  getDEps(utc);     // arcsecond

       // Tidal variations in the Earth's rotation
       // 1. diurnal/subdiurnal tidal effect on polar motion ("), UT1 (s) and
       //    LOD (s).
       // 2. diurnal lunisolar effect on polar motion (").
       Vector<double> cor_oceantides = PMUT1_OCEANS(utc);   // xp, yp, ut1, lod
       Vector<double> cor_libration1 = PMSDNUT2(utc);       // xp, yp
       Vector<double> cor_libration2 = UTLIBR(utc);         // ut1, lod

//       cout << "OceanTides: " << cor_oceantides << endl;
//       cout << "Libration1: " << cor_libration1 << endl;
//       cout << "Libration2: " << cor_libration2 << endl;

       dut1 += cor_oceantides(2) + cor_libration2(0);
       xp   += cor_oceantides(0) + cor_libration1(0);
       yp   += cor_oceantides(1) + cor_libration1(1);

//       cout << "dut1: " << dut1 << endl
//            << "xp: " << xp << endl
//            << "yp: " << yp << endl;

       xp = xp * AS_TO_RAD;
       yp = yp * AS_TO_RAD;
       dX = dX * AS_TO_RAD;
       dY = dY * AS_TO_RAD;

       double x,y,s;
       XY06(tt, x, y);
       s = S06(tt, x, y);
       x = x + dX;
       y = y + dY;
//       cout << "X: " << x << endl
//            << "Y: " << y << endl
//            << "S: " << s*RAD_TO_AS << endl;

       // Q
       Matrix<double> Q = C2IXYS(x,y,s);

       // ERA, dERA
       double era    = ERA00(ut1);
       double dera   = dERA00(ut1);

       // dR
       Matrix<double> dR(3,3,0.0);
       dR(0,0) = -std::sin(era);
       dR(0,1) =  std::cos(era);
       dR(1,0) = -std::cos(era);
       dR(1,1) = -std::sin(era);
       dR = dR * dera;

       // W
       double sp = SP00(tt);
       Matrix<double> W = POM00(xp, yp, sp);

       return W * dR * Q;
   }


   // Time derivative of transformation matrix from TRS to CRS coordinates
   // for a given date
   Matrix<double> ReferenceSystem::dT2CMatrix(const CommonTime& UTC)
   {
       return transpose( dC2TMatrix(UTC) );
   }


   // Greenwich mean sidereal time (consistent with IAU 2006 precession)
   double ReferenceSystem::GMST06(const CommonTime& UT1, const CommonTime& TT)
   {
      double t, t2, t3, t4, t5, gmst;

      // TT Julian centuries since J2000.0.
      t = (MJD(TT).mjd - MJD_J2000) / JC;
      t2 = t*t;
      t3 = t2*t;
      t4 = t3*t;
      t5 = t4*t;

      // Greenwich mean sidereal time, IAU 2006.
      gmst = Anp(ERA00(UT1) + (0.014506 + 4612.156534*t + 1.3915817*t2
            - 0.00000044*t3 - 0.000029956*t4 - 0.0000000368*t5)*AS_TO_RAD);

      return gmst;

   }


   // Doodson fundamental arguments
   void ReferenceSystem::DoodsonArguments(const CommonTime& UT1,
                                          const CommonTime& TT, 
                                          double BETA[6],
                                          double FNUT[5]         )
   {
      // GMST, IAU 1980 model
      double THETA = GMST06(UT1,TT);

      // Fundamental Arguments
      //-----------------------------------------------------
      //Julian centuries since J2000
      const double  t = (MJD(TT).mjd - MJD_J2000) / JC;

      // Arcseconds in a full circle
      double TURNAS = 360.0 * 3600.0;


      // Mean anomaly of the Moon.
      double temp = fmod(           485868.249036  +
         t * ( 1717915923.2178 +
         t * (         31.8792 +
         t * (          0.051635 +
         t * (        - 0.00024470 ) ) ) ), TURNAS ) * AS_TO_RAD;

      double F1 = Anpm(temp);           // [-PI, +PI]

      // Mean anomaly of the Sun
      temp = fmod(         1287104.793048 +
         t * ( 129596581.0481 +
         t * (       - 0.5532 +
         t * (         0.000136 +
         t * (       - 0.00001149 ) ) ) ), TURNAS ) * AS_TO_RAD;

      double F2 = Anpm(temp);           // [-PI, +PI]
      
      // Mean longitude of the Moon minus that of the ascending node.
      temp = fmod(           335779.526232 +
         t * ( 1739527262.8478 +
         t * (       - 12.7512 +
         t * (        - 0.001037 +
         t * (          0.00000417 ) ) ) ), TURNAS ) * AS_TO_RAD;
      double F3 = Anpm(temp);           // [-PI, +PI]
      
      // Mean elongation of the Moon from the Sun. 
      temp = fmod(          1072260.703692 +
         t * ( 1602961601.2090 +
         t * (        - 6.3706 +
         t * (          0.006593 +
         t * (        - 0.00003169 ) ) ) ), TURNAS ) * AS_TO_RAD;

      double F4 = Anpm(temp);           // [-PI, +PI]
      
      // Mean longitude of the ascending node of the Moon.
      temp = fmod(          450160.398036 +
         t * ( - 6962890.5431 +
         t * (         7.4722 +
         t * (         0.007702 +
         t * (       - 0.00005939 ) ) ) ), TURNAS ) * AS_TO_RAD;
      double F5 = Anpm(temp);           // [-PI, +PI]

      FNUT[0] = F1;
      FNUT[1] = F2;
      FNUT[2] = F3;
      FNUT[3] = F4;
      FNUT[4] = F5;

      double S = F3+F5;

      BETA[0] = THETA + PI - S;
      BETA[1] = F3 + F5;
      BETA[2] = S - F4;
      BETA[3] = S - F1;
      BETA[4] = -F5;
      BETA[5] = S - F4 - F2;

   }



}  // End of namespace 'gpstk'
