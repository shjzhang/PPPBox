#pragma ident "$Id: ComputeDeltaCorrection.cpp 1325 2008-07-29 14:33:43Z architest $"

/**
 * @file ComputeDeltaCorrection.cpp
 * This is a class to interpolate the corrections for RTK positioning.
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
//  Copywright, Shoujian Zhang, Wuhan University, 2013, 2014
//
//============================================================================
//
//  Modification
//
//  - Created this subroutine, 2014/07/08, shjzhang
//
//============================================================================


#include "ComputeDeltaCorrection.hpp"
#include "ENUUtil.hpp"

using namespace std;

namespace gpstk
{


      // Index initially assigned to this class
   int ComputeDeltaCorrection::classIndex = 3300000;


      // Returns an index identifying this object.
   int ComputeDeltaCorrection::getIndex() const
   { return index; }



      // Returns a string identifying this object.
   std::string ComputeDeltaCorrection::getClassName() const
   { return "ComputeDeltaCorrection"; }


      /** Explicit constructor, taking as input reference station
       *  coordinates, ephemeris to be used and whether TGD will
       *  be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param gdsRef        Observable corrections for reference stations.
       *
       */
   ComputeDeltaCorrection::ComputeDeltaCorrection( const TypeID& type,
                                                   const gnssDataMap& referenceData )
       :firstTime(true) 
   {
         // Set the reference corrections
      setReferenceData( referenceData );

         // Set the corretion type to be interpolated
      setInterpType( type );

         // Set the class index
      setIndex();
   }


      /** Explicit constructor, taking as input reference station
       *  coordinates, ephemeris to be used and whether TGD will
       *  be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param gdsRef        Observable corrections for reference stations.
       *
       */
   ComputeDeltaCorrection::ComputeDeltaCorrection( const TypeIDSet& typeSet,
                                                   const gnssDataMap& refData )
       :firstTime(true) 
   {
         // Set the reference corrections
      setReferenceData( refData );

         // Set the corretion type to be interpolated
      setInterpType( typeSet );

         // Set the class index
      setIndex();

   }


      // Method to set the initial (a priori) position of receiver.
   int ComputeDeltaCorrection::setReferenceData( const gnssDataMap& refData )
   {

      try
      {
           // Now, get the reference gnss data
         gdsRef = refData; 

         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'ComputeDeltaCorrection::setInitialRxPosition()'


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& ComputeDeltaCorrection::Process(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {

            // Build a gnssRinex object and fill it with data
         gnssRinex g1;
         g1.header = gData.header;
         g1.body = gData.body;

            // Call the Process() method with the appropriate input object
         Process(g1);

            // Update the original gnssSatTypeValue object with the results
         gData.body = g1.body;

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'ComputeDeltaCorrection::Process()'


      /* Returns a reference to a gnssRinex object
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& ComputeDeltaCorrection::Process( gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         SatIDSet satRejectedSet;

            // Get time from the header of 'gData'
         CommonTime epoch( gData.header.epoch );

            // Get the rover from 'gData'
         SourceID rover( gData.header.source );

            /**
             * Now, Let's get the reference stations from 'refDataMap'
             */

            // Get the first epoch of the 'gdsRef'
         gnssDataMap refDataMap;


         if(firstTime)
         {
                // Get the data of the first epoch
             refDataMap = gdsRef.frontEpoch();
             gdsRef.pop_front_epoch();

                // Get the first epoch time
             CommonTime refEpoch = refDataMap.begin()->first;

                // If the 'epoch' is less than the first epoch of 'gdsRef', 
                // it means that there will be no augmentation data for this 'epoch'
             if( refEpoch > epoch )
             {
                 ProcessingException e("there are no reference data for this epoch");
                 GPSTK_THROW(e);
             }

                // If the first time of 'gdsMap' is less than the 
                // observation time 'epoch', then contine 'poping', until
                // they equals.
             while (refEpoch < epoch)
             {
                 refDataMap = gdsRef.frontEpoch();
                 gdsRef.pop_front_epoch();
                 refEpoch = refDataMap.begin()->first;
                 cout << "refEpoch" << refEpoch << endl;
             }

                // No long the first time
             firstTime = false;
         }
         else
         {
                // Only pop 'ONE' epoch data given that
                // the data sampling rate equals with the rate of the
                // correction data.
             refDataMap = gdsRef.frontEpoch();
             gdsRef.pop_front_epoch();
         }

            // Look for the sources in 'gdsRef' at current epoch
         gnssRinex gRef = refDataMap.getGnssRinex( rover );

            // Let's define the reference satellite to compute
            // the single difference values for ionospheric delays

            // Reference ionospheric delays

            // The first elemement of the body of 'gData'
         satTypeValueMap::iterator stv = gData.body.begin();

            // The min elevation
         double maxElev(0.0);

            // The datum satellite
         SatID satDatum;

            // Now, Let's find the satellite with highest elevation
         for( satTypeValueMap::iterator it = gData.body.begin();
              it != gData.body.end();
              ++it )
         {
                // current elev
             double elev = gData.getValue((*it).first, TypeID::elevation);

                // Update the elevation
             if(elev > maxElev)
             {
                satDatum = (*it).first ;
                maxElev = elev;
             }
         }

         cout << "delta datum:" << satDatum << endl;

         double ionoDatum(0.0);

         try
         {
               // Get the ionospheric delays
            double iono = gData.getValue( satDatum, TypeID::ionoL1 );

               // Get the estimated value from 'gRef' for the datum satellite
            double ionoRef = gRef.getValue( satDatum, TypeID::ionoL1 );

               // Now, the datum values for the difference of ionoL1
            ionoDatum = iono - ionoRef;

         }
         catch(...)  // If no satellite is found, then just return. 
         {
             return gData;
         }

            // Warning:
            // Loop the satellite in 'gData'
         for( satTypeValueMap::iterator it = gData.body.begin();
              it != gData.body.end();
              ++it )
         {
               // Store the result
            double result(0.0);

               // Check all the indicated TypeID's
            for (TypeIDSet::iterator itType = interpTypeSet.begin(); 
                 itType != interpTypeSet.end(); 
                 ++itType )
            {

               try
               {
                     // Interpolate the wet zenith tropospheric delay
                  if( (*itType) == TypeID::wetTropo )
                  {
                         // Temporary variable
                      double trop, tropRef, diffTrop;

                         // Get the interpolated trop value from gData
                      trop = gData.header.source.zwdMap[TypeID::wetTropo];

                         // Get the estimated trop value from 'gRef'
                      tropRef = gRef.header.source.zwdMap[TypeID::wetTropo];

                         // Result
                      result = trop - tropRef;
                  }
                  else if ( (*itType) == TypeID::ionoL1)
                  {
                         // Temporary variable
                      double value, valueRef, diffValue;

                         // Get the interpolated value from gData
                      value = gData.getValue((*it).first, (*itType) );

                         // Get the estimated value from 'gRef', according
                         // the SatID and TypeID 
                      valueRef = gRef.getValue( (*it).first, (*itType) );

                         // Result
                      result = value - valueRef;

                         // Minus the datum value
                      result = result - ionoDatum;

                  }  // End of 'if( (*itType) == TypeID::wetTropo )'
                  else
                  {

                         // Temporary variable
                      double value, valueRef, diffValue;

                         // Get the interpolated value from gData
                      value = gData.getValue((*it).first, (*itType) );

                         // Get the estimated value from 'gRef', according
                         // the SatID and TypeID 
                      valueRef = gRef.getValue( (*it).first, (*itType) );

                         // Result
                      result = value - valueRef;

                  }
               }
               catch(...)
               {
                   satRejectedSet.insert( (*it).first );
                   continue;
               }

                  // Now, insert the result into 'gData'
               if( (*itType) == TypeID::wetTropo )
               {
                   (*it).second[TypeID::deltaTropo] = result;
               }
               else if( (*itType) == TypeID::ionoL1)
               {
                   (*it).second[TypeID::deltaIono] = result;
               }

            } // End of ' for (TypeIDSet::const_iterator itType = interpTypeSet.begin();'

         } // End of ' for( satTypeValueMap::const_iterator stvmIter ='

            // Return
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

   }  // End of method 'ComputeDeltaCorrection::Process()'


}  // End of namespace gpstk
