#pragma ident "$Id: CycleSlipSimulation.cpp 1315 2014-10-15 18:35:21Z shjzhang $"

/**
 * @file CycleSlipSimulation.cpp
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
//  2014/10/15      Simulate cycle slips given the sampling rate.
//
//============================================================================


#include "CycleSlipSimulation.hpp"

using namespace std;

namespace gpstk
{

      // Index initially assigned to this class
   int CycleSlipSimulation::classIndex = 8000000;


      // Returns an index identifying this object.
   int CycleSlipSimulation::getIndex(void) const
   { return index; }


      // Returns a string identifying this object.
   std::string CycleSlipSimulation::getClassName() const
   { return "CycleSlipSimulation"; }



      /* Sets sampling interval.
       *
       * @param sampleInterval      Sampling interval, in seconds.
       */
   CycleSlipSimulation& CycleSlipSimulation::setSampleInterval(const double sampleInterval)
   {

         // Make sure that sample interval is positive
      if( sampleInterval >= 0.0 )
      {
         sampling = sampleInterval;
      }

      return (*this);

   }  // End of method 'CycleSlipSimulation::setSampleInterval()'



      /* Sets tolerance, in seconds.
       *
       * @param tol                 Tolerance, in seconds.
       */
   CycleSlipSimulation& CycleSlipSimulation::setTolerance(const double tol)
   {

         // Make sure that tolerance is positive
      if( tol >= 0.0 )
      {
         tolerance = tol;
      }

      return (*this);

   }  // End of method 'CycleSlipSimulation::setTolerance()'



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
   satTypeValueMap& CycleSlipSimulation::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
      throw(ProcessingException)
   {
      try
      {

            // Set a threshold
         double lowerBound( std::abs(tolerance) );

            // Set a threshold
         double upperBound( std::abs(sampling - tolerance) );

            // Time offset relative to the intial epoch time
            // Warning: always positive, which is different with that
            // in Class 'Decimate'
         double timeoffset( std::abs(time - initialTime) );

            // According to the definition of the modulo(%) operator in C++,
            // for the positive integers, the remainder will also be positive
            // for example, 301 % 30 = 1, and 299 % 30 = 29. 
            //
            // So, given the tolerence, if the the remainder of the time  
            // is greater than (sampling - tolerance), or less than (tolerance), 
            // the time is near the integer sampling position. 
            //
            // Then will will insert cycle slips into the 'gnssRinex'
         if ( ( (int)(timeoffset) % (int)(sampling) < lowerBound) ||
              ( (int)(timeoffset) % (int)(sampling) > upperBound) )
         {
               // Loop through all the satellites
            satTypeValueMap::iterator it;
            for (it = gData.begin(); it != gData.end(); ++it)
            {
                  // insert the cycle slip flag into the structure. 
               (*it).second[TypeID::CSL1] = 1.0;

                  // Mark both flags.
               (*it).second[TypeID::CSL2] = 1.0;

            }
         }

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'CycleSlipSimulation::Process()'



}  // End of namespace gpstk
