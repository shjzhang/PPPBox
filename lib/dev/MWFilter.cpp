#pragma ident "$Id$"

/**
 * @file MWFilter.cpp
 * This is a class to detect cycle slips using the Melbourne-Wubbena
 * combination.
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2011
//
//============================================================================


#include "MWFilter.hpp"


namespace gpstk
{

   using namespace std;

      // Returns a string identifying this object.
   std::string MWFilter::getClassName() const
   { return "MWFilter"; }



      /* Returns a satTypeValueMap object, adding the new data generated
       * when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       * @param epochflag Epoch flag.
       */
   satTypeValueMap& MWFilter::Process( const CommonTime& epoch,
                                       satTypeValueMap& gData)
      throw(ProcessingException)
   {

      try
      {

         double mw(0.0);

         SatIDSet satRejectedSet;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {
               // SatID
            SatID sat( (*it).first );

            try
            {
                  // Try to extract the values
               mw = (*it).second(obsType);
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( sat );
               continue;
            }

               // First check if this satellite has previous arc information
            if ( satArcMap.find( sat ) == satArcMap.end() )
            {
                  // If it doesn't have an entry, insert one
               satArcMap[ sat ] = 0.0;
            }

               //>>> Start to filter the mw data

               // Then, check both if there is arc information, and if current
               // arc number is different from arc number in storage (which
               // means a cycle slip happened), reset the buffer for smoothing data.
            if ( (*it).second.find(TypeID::satArc) != (*it).second.end() &&
                 (*it).second(TypeID::satArc) != satArcMap[ sat ] )
            {
                  // If different, update satellite arc in storage
               satArcMap[ sat ] = (*it).second(TypeID::satArc);

                  // We reset the filter with this
               MWData[ sat ].windowSize = 1;

                  // Now, the mean value equals with the current 'ambValue'
               MWData[ sat ].meanMW = mw;

                  // Now, the mean value equals with the current 'ambValue'
               MWData[ sat ].varMW = 0.25*0.25;


            }
            else
            {
                  // Increment window size
               ++MWData[sat].windowSize;

                  // MW bias from the mean value
               double mwBias(0.0);
               double size(0.0);

               mwBias = ( mw - MWData[sat].meanMW );
               size = ( static_cast<double>(MWData[sat].windowSize) );

                  // Compute average
                  // meanMW(i)= meanMW(i-1) + ( mwBias ) / size;
               MWData[sat].meanMW += mwBias / size;

                  // Compute variance 
                  // Var(i) = Var(i-1) + [ ( mw(i) - meanMW)^2 - Var(i-1) ]/(i);
               MWData[sat].varMW  += ( mwBias*mwBias - MWData[sat].varMW ) / size;

            }  // End of smoothing data


               // Insert the mean value into 'gdsMap'
            (*it).second[ resultType1 ] = MWData[sat].meanMW; 

         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'MWFilter::Process()'



      /* Returns a gnnsRinex object, adding the new data generated when
       * calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& MWFilter::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'MWFilter::Process()'


}  // End of namespace gpstk
