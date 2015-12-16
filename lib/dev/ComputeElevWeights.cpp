#pragma ident "$Id: $"

/**
 * @file ComputeElevWeights.hpp
 * This class computes satellites weights simply based on the elevation .
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
//  Modifications
//  -------------
//
//  2012.06.07  Create this program
//
//  Copyright
//  ---------
//
//  Shoujian Zhang, Wuhan University, 2012
//
//============================================================================


#include "geometry.hpp"
#include "ComputeElevWeights.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int ComputeElevWeights::classIndex = 6300000;


      // Returns an index identifying this object.
   int ComputeElevWeights::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string ComputeElevWeights::getClassName() const
   { return "ComputeElevWeights"; }



      /* Returns a satTypeValueMap object, adding the new data
       * generated when calling this object.
       *
       * @param time      Epoch corresponding to the data.
       * @param gData     Data object holding the data.
       */
   satTypeValueMap& ComputeElevWeights::Process( const CommonTime& time,
                                                 satTypeValueMap& gData )
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;


            // Loop through all the satellites
         for( satTypeValueMap::iterator it = gData.begin();
              it != gData.end();
              ++it )
         {

            double elev( 0.0 );

            try
            {
               elev = gData.getValue( (*it).first, TypeID::elevation );
            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*it).first );
               continue;
            }

            double weight;


               // Compute the weight according to elevation
            if( elev > 30.0 )
            {
               weight = 1.0;
            }
            else
            {
                  // Transform the unit of elevation from degree to radian
               elev = elev*DEG_TO_RAD;

                  // Compute the elevation weight
               weight = std::pow( 2.0*std::sin(elev), expotential) ;

            }

            typeValueMap::iterator ittvm = (*it).second.find(TypeID::weight);
            if( ittvm != (*it).second.end() )
            {
               double prevWeight = (*it).second[TypeID::weight];
               (*it).second[TypeID::weight] = weight*prevWeight;
            }
            else
            {
               (*it).second[TypeID::weight] = weight;
            }

         }

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

   }  // End of method 'ComputeElevWeightsWeights::Process()'



      /* Returns a gnnsSatTypeValue object, adding the new data
       * generated when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& ComputeElevWeights::Process(gnssSatTypeValue& gData)
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
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeElevWeightsWeights::Process()'



      /* Returns a gnnsRinex object, adding the new data generated
       * when calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& ComputeElevWeights::Process(gnssRinex& gData)
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
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeElevWeightsWeights::Process()'


}  // End of namespace gpstk
