#pragma ident "$Id: AmbiguitySmoother.cpp 1325 2012-09-25 14:33:43Z shjzhang $"

/**
 * @file AmbiguitySmoother.cpp
 * This class smoothes melboune-wubbena observable.
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


#include "AmbiguitySmoother.hpp"

using namespace std;

namespace gpstk
{

      // Index initially assigned to this class
   int AmbiguitySmoother::classIndex = 2450000;


      // Returns an index identifying this object.
   int AmbiguitySmoother::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string AmbiguitySmoother::getClassName() const
   { return "AmbiguitySmoother"; }



      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& AmbiguitySmoother::Process( const CommonTime& epoch,
                                                satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         double ambValue(0.0);
         int currentSize;

         SatIDSet satRejectedSet;

            // Loop through all satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {

            try
            {
                  // Try to extract the values
               ambValue = (*it).second(smoothType);
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }

               // First check if this satellite has previous arc information
            if ( satArcMap.find( (*it).first ) == satArcMap.end() )
            {
                  // If it doesn't have an entry, insert one
               satArcMap[ (*it).first ] = 0.0;
            }

               //>>> Start to smooth the ambiguity data

               // Then, check both if there is arc information, and if current
               // arc number is different from arc number in storage (which
               // means a cycle slip happened), reset the buffer for smoothing data.
            if ( (*it).second.find(TypeID::satArc) != (*it).second.end() &&
                 (*it).second(TypeID::satArc) != satArcMap[ (*it).first ] )
            {
                  // If different, update satellite arc in storage
               satArcMap[ (*it).first ] = (*it).second(TypeID::satArc);

                  // Reset the buffer 
               smoothingData[ (*it).first ].Buffer.clear();
               smoothingData[ (*it).first ].Buffer.push_back(ambValue);

                  // Now, the mean value equals with the current 'ambValue'
               smoothingData[ (*it).first ].mean = ambValue ;

                  // Set the currentSize 
               currentSize = 1;

            }
            else
            {
                  // Insert current value of smoothType to the end of buffer
               smoothingData[ (*it).first ].Buffer.push_back( ambValue );

                  // Get current window size
               currentSize = smoothingData[ (*it).first ].Buffer.size();

                  // Check if we have exceeded maximum window size
               if(currentSize > windowSize)
               {
                     // Get rid of oldest data, which is at the beginning of deque
                  smoothingData[ (*it).first ].Buffer.pop_front();
               }

                  // Update current window size
               currentSize = smoothingData[ (*it).first ].Buffer.size();

                  // Now, Let's compute the mean values and variance for the smoothingData
               PowerSum errorStats;

                  // Now, Let's insert all the data into errorStats.
               for(int i = 0; i < currentSize; i++ )
               {
                  errorStats.add( smoothingData[ (*it).first ].Buffer[i] );
               }

                  // Now, Let's compute the average value
               smoothingData[ (*it).first ].mean = errorStats.average();
               smoothingData[ (*it).first ].variance = errorStats.variance();

            }  // End of smoothing data


               // Insert the mean value into 'gdsMap'
               // only change the values, when the 'mean' operator is realy done.
            if( windowSize > 1 )
            {
               (*it).second[ smoothType ] = smoothingData[(*it).first].mean; 
            }

         }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'AmbiguitySmoother::Process()'



      /* Method to set the size of filter window, in samples.
       *
       * @param wSize       Windows size of filter window, in samples.
       */
   AmbiguitySmoother& AmbiguitySmoother::setWindowSize(const int& wSize)
   {

         // Don't allow window sizes less than 1
      if (wSize > 1)
      {
         windowSize = wSize;
      }
      else
      {
         windowSize = 1;
      }

      return (*this);

   }  // End of method 'AmbiguitySmoother::setWindowSize()'


      /* Method to set the variance threshold , in meter^2.
       *
       * @param varT    varinace thresold, in m^2.
       */
   AmbiguitySmoother& AmbiguitySmoother::setVarThreshold(const double& varT)
   {

      varThreshold = varT;

      return (*this);

   }  // End of method 'AmbiguitySmoother::setVarThreshold()'

}  // End of namespace gpstk
