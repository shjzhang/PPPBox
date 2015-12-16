#pragma ident "$Id: InterpCorrection.cpp 1325 2008-07-29 14:33:43Z architest $"

/**
 * @file InterpCorrection.cpp
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
//  Revision
//
//  2014/09/22  Don't delete the satellites in current 'GDS', even though the 
//              slant ionospheric delays are not found in corrections. 
//    
//  2014/12/31  Only insert correct ionospheric delays into 'GDS', don't insert
//              -9999.0 into data.
//
//============================================================================


#include "InterpCorrection.hpp"
#include "ENUUtil.hpp"
#include "TimeString.hpp"
#include "Epoch.hpp"


using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{


      // Index initially assigned to this class
   int InterpCorrection::classIndex = 3300000;



      // Returns an index identifying this object.
   int InterpCorrection::getIndex() const
   { return index; }



      // Returns a string identifying this object.
   std::string InterpCorrection::getClassName() const
   { return "InterpCorrection"; }


      /** Explicit constructor, taking as input reference station
       *  coordinates, ephemeris to be used and whether TGD will
       *  be computed or not.
       *
       * @param RxCoordinates Reference station coordinates.
       * @param gdsRef        Observable corrections for reference stations.
       *
       */
   InterpCorrection::InterpCorrection( const TypeID& type,
                                       const Position& RxCoordinates, 
                                       const gnssDataMap& referenceData,
                                       bool onlyStatic)
       : firstTime(true)
   {
         // Set the typeSet with 'type'
      setInterpType( type);

      cout << "size of typeset:" << interpTypeSet.size() << endl;

         // Set the initial coordinates
      setInitialRxPosition( RxCoordinates );

      cout << "rxCoord:" << RxCoordinates << endl;

         // Set the reference corrections
      setReferenceData( referenceData );


      cout << "onlyStatic" << onlyStatic << endl;
         // Set 'useOnlyStaticSta'
      setUseOnlyStaticSta( onlyStatic );

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
   InterpCorrection::InterpCorrection( const TypeIDSet& typeSet,
                                       const Position& RxCoordinates, 
                                       const gnssDataMap& refData,
                                       bool onlyStatic )
       : firstTime(true)
   {
         // Set the initial coordinates
      setInitialRxPosition( RxCoordinates );

         // Set the reference corrections
      setReferenceData( refData );

         // Set the corretion type to be interpolated
      setInterpType( typeSet );

         // Set 'useOnlyStaticSta'
      setUseOnlyStaticSta( onlyStatic );

         // Set the class index
      setIndex();

   }


      /* Method to set the initial (a priori) position of receiver.
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int InterpCorrection::setInitialRxPosition( const double& aRx,
                                               const double& bRx,
                                               const double& cRx,
                                               Position::CoordinateSystem s,
                                               EllipsoidModel *geoid )
   {

      try
      {
         Position rxpos(aRx, bRx, cRx, s, geoid);
         setInitialRxPosition(rxpos);
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'InterpCorrection::setInitialRxPosition()'



      // Method to set the initial (a priori) position of receiver.
   int InterpCorrection::setInitialRxPosition(const Position& RxCoordinates)
   {

      try
      {
         rxPos = RxCoordinates;
         return 0;
      }
      catch(GeometryException& e)
      {
         return -1;
      }

   }  // End of method 'InterpCorrection::setInitialRxPosition()'


      // Method to set the initial (a priori) position of receiver.
   int InterpCorrection::setReferenceData( const gnssDataMap& refData )
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

   }  // End of method 'InterpCorrection::setInitialRxPosition()'


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& InterpCorrection::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'InterpCorrection::Process()'


      /* Returns a reference to a gnssRinex object
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& InterpCorrection::Process( gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {
         Epoch dt;
         dt.setLocalTime();
         string dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
         cout << "local time 0: " << dat << endl;

         SatIDSet satRejectedSet;

            // Get time from the header of 'gData'
         CommonTime epoch( gData.header.epoch );

         string line;
         CivilTime civTime(gData.header.epoch);
         line  = rightJustify(asString<short>(civTime.year), 6);
         line += rightJustify(asString<short>(civTime.month), 6);
         line += rightJustify(asString<short>(civTime.day), 6);
         line += rightJustify(asString<short>(civTime.hour), 6);
         line += rightJustify(asString<short>(civTime.minute), 6);
         line += rightJustify(asString(civTime.second, 7), 13);

         cout << "InterpCorrection: time:" << line << endl;

         int numCurrentSV =  gData.numSats();
         cout << "InterpCorrection:numCurrentSV:" << numCurrentSV << endl;

            // 'gdsMap' to store the data for current epoch
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

         cout << "InterpCorrection:epoch:" << epoch 
              << "refEpoch:" << refDataMap.begin()->first << endl; 

         dt.setLocalTime();
         dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
         cout << "local time 1: " << dat << endl;
            /**
             * Now, Let's get the reference stations from 'refDataMap'
             */

            // Map to store the reference stations' coordinates
         SourceIDSet refSet;

            // Look for the sources in 'gdsRef' at current epoch
         for( gnssDataMap::iterator it = refDataMap.begin();
              it!=refDataMap.end();
              it++ )
         {
               // Get the reference source set 
            sourceDataMap& sourceMap(it->second);
            for(sourceDataMap::iterator itsrc = sourceMap.begin();
                itsrc != sourceMap.end();
                ++itsrc)
            {
                  // SourceID
               SourceID source(itsrc->first);

                  // Don't use the rover station
               if(source != rover)
               {
                  if(useOnlyStaticSta)
                  {
                     if(source.staticFlag)
                     {
                           // Reference station coordinate map
                        refSet.insert( source );
                     }
                  }
                  else
                  {
                     refSet.insert(source);
                  }
               }

            }  // loop in the sources

         }  // loop in the epoches

            // Number of reference stations
         double numRefs( refSet.size() );

         cout << "numRefs" << numRefs << endl;

         dt.setLocalTime();
         dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
         cout << "local time 2: " << dat<< endl;

            /**
             * Now, compute the interpolation coefficients
             */

            // Map to store the interpolation coefficient
         std::map<SourceID, double> coeffMap;

         cout << "now, getCoefficient " << endl;

            // Now, let's compute the interpolation coefficients 
         coeffMap = getCoefficient( refSet );

         cout << "now, interpolate..." << endl;

            /**
             * Then, Let's interpolate the corrections
             */
         cout << "size of typeset:" << interpTypeSet.size() << endl;

            // Check all the indicated TypeID's
         for (TypeIDSet::const_iterator itType = interpTypeSet.begin(); 
              itType != interpTypeSet.end(); 
              ++itType )
         {


            cout <<  "itType" << (*itType) << endl;

               // Interpolate the wet zenith tropospheric delay
            if( (*itType) == TypeID::wetTropo )
            {
                  // Value to store the correction computation
               double value(0.0);
               double result(0.0);

                  // Loop through all the reference station 
               for ( std::set<SourceID>::iterator itSource = refSet.begin(); 
                     itSource != refSet.end(); 
                     ++itSource ) 
               {
                      // Get the zwdMap from this 'Source'
                   std::map<TypeID, double> zwdMap = (*itSource).zwdMap;

                      // Define the iterator
                   std::map<TypeID, double>::iterator it=zwdMap.find(TypeID::wetTropo);

                   if(it!=zwdMap.end())
                   {
                       value = (*it).second;
                   }

                   cout << "wetTrop for source " << (*itSource) << " = " << value << endl;

                      // Add the contributions from this reference station
                   result = result + coeffMap[ (*itSource) ] * value;


               }  // End of "for ( std::set<SourceID>::iterator itSource = refSet.begin();"
                  // result of the 'zwd'
               cout << "result of zwdMap:" << result << endl;

                  // Insert 'zwd' into source of the header of gnssRinex
               gData.header.source.zwdMap[TypeID::wetTropo] = result;

               cout << gData.header.source.zwdMap[TypeID::wetTropo] << endl;

            }
            else
            {
                  // Loop throught all satellite
               for( satTypeValueMap::iterator it = gData.body.begin(); 
                     it != gData.body.end(); 
                     ++it )
               {

                     // Value to store the correction computation
                  double value(0.0);
                  double result(0.0);
      
                     // Loop through all the reference station 
                  for ( std::set<SourceID>::iterator itSource = refSet.begin(); 
                        itSource != refSet.end(); 
                        ++itSource ) 
                  {
                     try
                     {
                           // Try to extract the values from 'gdsRef'
                        value = refDataMap.getValue( (*itSource), (*it).first, (*itType) );
                        cout << "source:" << (*itSource) << "Sat:" << (*it).first
                              << "value" << value << endl;
      
                           // Add the contributions from this reference station
                        result = result + coeffMap[ (*itSource) ] * value ;
      
                     }
                     catch(...)
                     {
      
                        cout << "InterpCorrection satRejectedSet" << (*it).first << endl;
      
                           // If some value is missing, then assign a very negative value.
                        result = -9999.0;
                        satRejectedSet.insert( (*it).first );
                        break;

                     }
      
                  }  // End of "for(itSource = ... )"


                     // Insert result into gnssRinex
                  (*it).second[(*itType)] = result;

               }  // End of 'for( satTypeValueMap::iterator it = gData.body.begin(); '

            }  // End of 'if( (*itType) == TypeID::wetTropo )'

         }  // End of 'for (TypeIDSet::const_iterator itType = interpTypeSet.begin(); '

//       dt.setLocalTime();
//       dat = printTime( CommonTime(dt),"%02m/%02d/%04Y %02H:%02M:%02S");
//       cout << "local time 3: "<< dat << endl;

            // Don't remove satellite ids

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

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

   }  // End of method 'InterpCorrection::Process()'



      /** Method that determine the coefficients for the corrections 
       *  interpolation
       *
       * @param refCoordMap   Map to store the reference stations' postion.
       */
   std::map<SourceID, double> InterpCorrection::getCoefficient( SourceIDSet& refSet)
      throw(InvalidSolver)
   {

      int numRefs = refSet.size();

         // Map to store the distance between the current receiver 
         // and the references
      Vector<double> rhoVec(numRefs, 0.0);

         // Store the distance with Map 
      std::map<SourceID, double> rhoMap;

         // Now, compute the range between rover and reference stations
      int i(0); 
      for( SourceIDSet::iterator itSource = refSet.begin();
           itSource != refSet.end();
           ++itSource )
      {

         Position refPos = ((*itSource).nominalPos);

         cout << "(*itSource)" << (*itSource) << "refPos:" << refPos << endl;

            // Compute the distance 
         rhoVec(i) = RSS(rxPos.X() - refPos.X(), 
                         rxPos.Y() - refPos.Y(), 
                         rxPos.Z() - refPos.Z() ) ;

            // Compute and store the distance
         rhoMap[(*itSource)] = RSS(rxPos.X() - refPos.X(), 
                                   rxPos.Y() - refPos.Y(), 
                                   rxPos.Z() - refPos.Z() ) ;

         cout << "distance to:" << (*itSource) << "is:" << rhoVec(i) << endl; 

            // Increment 
         i++;
      }


         // Vector to store the 'pair'
      std::vector<PAIR> pairVec;

      for (std::map<SourceID,double>::iterator itMap = rhoMap.begin(); 
           itMap != rhoMap.end(); 
           ++itMap)
      {
          pairVec.push_back(make_pair(itMap->first, itMap->second));
      }

         // Sort the 'pair' element by 'distance'
      std::sort(pairVec.begin(), pairVec.end(), cmp);


      for(int i=0; i< pairVec.size(); i++)
      {
          cout << pairVec[i].first << ":" << pairVec[i].second << endl;
      }

         // Get the minimum range of the 'rhoVec'
      double minRange(9999999.0);
      int indexMin(0);

      for(int i=0; i<numRefs;i++)
      {
         if( rhoVec(i) < minRange)
         {
             minRange = rhoVec(i);
             indexMin = i;
         }
      }

      std::map<SourceID, double> coeffMap;
      if(minRange < 1.0)
      {
            // Now, the coefficient
         int c1(0); 
         for( std::set<SourceID>::iterator itSource = refSet.begin();
              itSource != refSet.end();
              ++itSource )
         {
             if(c1==indexMin)
             {
                coeffMap[(*itSource)] = 1.0;
             }
             else
             {
                coeffMap[(*itSource)] = 0.0;
             }

               // Index
             ++c1;
         }
      }
      else
      {
            // Get the numerica of the interpolation coefficients
         double sum(0.0);
         for(int i=0; i<numRefs; i++)
         {
            sum = sum + 1.0/rhoVec(i);
         }

         cout << "sum" << sum << endl;

            // Now, the coefficient
         int c1(0); 
         for( SourceIDSet::iterator itSource = refSet.begin();
              itSource != refSet.end();
              ++itSource )
         {
             cout << "range:" << rhoVec(c1) << endl;
             coeffMap[(*itSource)] = (1.0/rhoVec(c1))/sum;

             cout << "(*itSource)" << "coeff:" << coeffMap[(*itSource)] << endl;

               // Index
             ++c1;
         }
      }

         // return
      return  coeffMap;
  
   }  // End of 'std::map<SourceID,double> InterpCorrection::getCoefficient(...)'


}  // End of namespace gpstk
