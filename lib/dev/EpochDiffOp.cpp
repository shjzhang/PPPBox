#pragma ident "$Id: EpochDiffOp.cpp 2619 2012-04-11 03:36:15Z shjzhang $"

/**
 * @file EpochDiffOp.cpp
 * This is a class to apply the time difference Delta operator 
 * to GNSS data structures.
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
//  General Modifications
//  ---------------------
//
//  - If the satellite in current epoch, is not found in the previous epoch, 
//    just jump and do nothing. ( Shoujian Zhang, 2012.04.19)
//
//  
//  Copyright
//  ---------
//  Shoujian Zhang - Wuhan University. 2011, 2012
//
//============================================================================


#include "EpochDiffOp.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int EpochDiffOp::classIndex = 7100000;


      // Returns an index identifying this object.
   int EpochDiffOp::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string EpochDiffOp::getClassName() const
   { return "EpochDiffOp"; }



      /* Method to add a set of data value types to be differenced.
       *
       * @param diffSet       TypeIDSet of data values to be added to the
       *                      ones being differenced.
       */
   EpochDiffOp& EpochDiffOp::addDiffTypeSet(const TypeIDSet& diffSet)
   {

         // Iterate over 'diffSet' and add its components to 'diffTypes'
      TypeIDSet::const_iterator pos;
      for (pos = diffSet.begin(); pos != diffSet.end(); ++pos)
      {
         diffTypes.insert(*pos);
      }

      return (*this);

   }  // End of method 'EpochDiffOp::addDiffTypeSet()'


      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous 
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
   gnssSatTypeValue& EpochDiffOp::Process(gnssSatTypeValue& gData)
      throw(ProcessingException, TypeIDNotFound)
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
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );
         GPSTK_THROW(e);
      }

   }

      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous 
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
   gnssRinex& EpochDiffOp::Process(gnssRinex& gData)
      throw(ProcessingException, TypeIDNotFound)
   {

      try
      {
         Process(gData.body);

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
   }


      /* Returns a reference to a satTypeValueMap object after differencing
       * data type values given in 'diffTypes' field with the previous 
       * corresponding type.
       *
       * @param gData      Data object holding the data.
       */
   satTypeValueMap& EpochDiffOp::Process(satTypeValueMap& gData)
      throw(ProcessingException, TypeIDNotFound)
   {

      try
      {
         SatIDSet satRejectedSet;

           // Store the raw gData
         satTypeValueMap gRaw(gData);

            // Loop through all the satellites in the station data set
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {

               // Let's find if the same satellite is present in gPrev
            satTypeValueMap::const_iterator itformer;

            itformer = gPrev.find((*it).first);

               // If we found the satellite, let's proceed with the differences
            if (itformer != gPrev.end())
            {
                  // Now, Let's check the satArc firstly. If the sat arc 
                  // is not equal, remove the satellite directly.
               double satArc = gData((*it).first)(TypeID::satArc);
               double satArcPrev = gPrev((*it).first)(TypeID::satArc);

               if( satArc != satArcPrev )
               {
                     // If cycle slip happened, then schedule this
                     // satellite for removal
                  satRejectedSet.insert( (*it).first );

                     // Skip this value if problems arise
                  continue;
               }

               
                  // We must compute the difference for all the types in
                  // 'diffTypes' set
               TypeIDSet::const_iterator itType;
               for( itType  = diffTypes.begin();
                    itType != diffTypes.end();
                    ++itType )
               {
                  double value1(0.0);
                  double value2(0.0);

                  try
                  {
                        // Let's try to compute the difference
                     value1 = gData((*it).first)(*itType);
                     value2 = gPrev((*it).first)(*itType);

                        // result type
                     TypeID resultType;

                        // Get difference into data structure
                     if ( (*itType) == (TypeID::prefitC)) 
                     { resultType = TypeID::diffPrefitC; }
                     else if( (*itType) == (TypeID::prefitL))
                     { resultType = TypeID::diffPrefitL; }
                     else if( (*itType) == (TypeID::wetMap))
                     { resultType = TypeID::diffWetMap; }
                     else if( (*itType) == (TypeID::prefitC1))
                     { resultType = TypeID::diffPrefitC1; }
                     else if( (*itType) == (TypeID::prefitP1))
                     { resultType = TypeID::diffPrefitP1; }
                     else if( (*itType) == (TypeID::prefitP2))
                     { resultType = TypeID::diffPrefitP2; }
                     else if( (*itType) == (TypeID::prefitL1))
                     { resultType = TypeID::diffPrefitL1; }
                     else if( (*itType) == (TypeID::prefitL2))
                     { resultType = TypeID::diffPrefitL2; }
                     else
                     {
                        GPSTK_THROW(TypeIDNotFound("TypeID not found in diffTypes"));
                     }

                       // Insert the result type into gData
                     (*it).second[resultType] = value1 - value2;

                  }
                  catch(...)
                  {
                        // If some value is missing, then schedule this
                        // satellite for removal
                     satRejectedSet.insert( (*it).first );

                        // Skip this value if problems arise
                     continue;
                  }

               }  // End of 'for( itType = diffTypes.begin(); ...'

            }
            else
            {
                  // If the current satellite is not found in the previous epoch,
                  // then remove the satellite from current 'gData'
               satRejectedSet.insert( (*it).first );

                  // Skip this value if problems arise
               continue;

            }  // End of 'if (itformer != gPrev.end())'

         }  // End of 'for (it = gData.begin(); it != gData.end(); ++it)'

         gData.removeSatID(satRejectedSet);

            // Store the current gData 
            // Warning: !!!!!!!!!
            // store the raw gData to 'gPrev'
         gPrev = gRaw;

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

   }  // End of method 'EpochDiffOp::Process()'


}  // End of namespace gpstk
