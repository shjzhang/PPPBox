#pragma ident "$Id: Decimate.cpp 1315 2008-07-25 18:35:21Z shjzhang $"

/**
 * @file Decimate.cpp
 * This class decimates GNSS Data Structures data given a sampling interval,
 * a tolerance, and a starting epoch.
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
//  Shoujian Zhang - Wuhan University, 2012
//
//============================================================================
//  
//  Revision
//
//  2014/03/03      The 'new' decimate will decimate the data respect to
//                  the given 'initialTime', which can be set in your program,
//                  such as PPP.
//                  The new version is easy for decimating data for multiple
//                  stations data processing.
//                  The new version can decimate the data, the sampling of 
//                  which are not 'integer', which will cause wrong operation
//                  in the old version.
//
//============================================================================


#include "Decimate.hpp"

using namespace std;

namespace gpstk
{

      // Index initially assigned to this class
   int Decimate::classIndex = 8000000;


      // Returns an index identifying this object.
   int Decimate::getIndex(void) const
   { return index; }


      // Returns a string identifying this object.
   std::string Decimate::getClassName() const
   { return "Decimate"; }



      /* Sets sampling interval.
       *
       * @param sampleInterval      Sampling interval, in seconds.
       */
   Decimate& Decimate::setSampleInterval(const double sampleInterval)
   {

         // Make sure that sample interval is positive
      if( sampleInterval >= 0.0 )
      {
         sampling = sampleInterval;
      }

      return (*this);

   }  // End of method 'Decimate::setSampleInterval()'



      /* Sets tolerance, in seconds.
       *
       * @param tol                 Tolerance, in seconds.
       */
   Decimate& Decimate::setTolerance(const double tol)
   {

         // Make sure that tolerance is positive
      if( tol >= 0.0 )
      {
         tolerance = tol;
      }

      return (*this);

   }  // End of method 'Decimate::setTolerance()'



      /* Returns a satTypeValueMap object, adding the new data generated when
       * calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       * @author          shjzhang
       * @warning         The modified decimate avoids the wrong decimate when
       *                  there is a large data gap more than threshold, which
       *                  is not considered in the old version.
       */
   satTypeValueMap& Decimate::Process( const CommonTime& time,
                                       satTypeValueMap& gData )
      throw(DecimateEpoch)
   {

         // Set a threshold
      double lowerBound( std::abs(tolerance) );

         // Set a threshold
      double upperBound( std::abs(sampling - tolerance) );

         // Time offset relative to the intial epoch time
      double timeoffset( (time - initialTime) );

         // Check if current epoch is whether or not in the integer sampling
         // position with tolerance error.
         // if not, it will be decimated
         
         // Decimate the data with time earlier than the given initial time
      if ( timeoffset < 0.0)
      {
            // If epoch must be decimated, we issue an Exception
         DecimateEpoch e("This epoch must be decimated.");

         GPSTK_THROW(e);
      }
         // According to the definition of the modulo(%) operator in C++,
         // for the positive integers, the remainder will also be positive
         // for example, 301 % 30 = 1, and 299 % 30 = 29. 
         // So, given the tolerence, if the the remainder of the time  
         // is greater than (sampling - tolerance), or less than (tolerance), 
         // the time is near the integer sampling position. Or else, 
         // we will decimate the data. 
      else if ( ( (int)(timeoffset) % (int)(sampling) > lowerBound) && 
                ( (int)(timeoffset) % (int)(sampling) < upperBound) )
      {
            // If epoch must be decimated, we issue an Exception
         DecimateEpoch e("This epoch must be decimated.");

         GPSTK_THROW(e);

      }

      return gData;

   }  // End of method 'Decimate::Process()'



}  // End of namespace gpstk
