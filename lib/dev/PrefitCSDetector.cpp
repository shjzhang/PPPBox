#pragma ident "$Id: PrefitCSDetector.cpp 1325 2012-09-25 14:33:43Z shjzhang $"

/**
 * @file PrefitCSDetector.cpp
 * This is a class to detect cycle slips using prefit residuals.
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
//  Shoujian Zhang - Wuhan University. 2013 
//
//============================================================================
//
//  Modification
//
//  - Create this class, 2013/07/17, Shoujian Zhang
//
//============================================================================


#include "PrefitCSDetector.hpp"


namespace gpstk
{


      // Returns a string identifying this object.
   std::string PrefitCSDetector::getClassName() const
   { return "PrefitCSDetector"; }


      /* Common constructor
       *
       * @param mThr    Minimum threshold to declare cycle slip, in meters.
       * @param drift   LI combination limit drift, in meters/second.
       * @param dtMax   Maximum interval of time allowed between two
       *                successive epochs, in seconds.
       */
   PrefitCSDetector::PrefitCSDetector( const double& mThr,
                                       const bool& use )
      : obsType(TypeID::prefitL), 
        lliType1(TypeID::LLI1), lliType2(TypeID::LLI2),
        resultType1(TypeID::CSL1), resultType2(TypeID::CSL2), 
        useLLI(use)
   {
      setMinThreshold(mThr);
   }


      /* Returns a gnnsRinex object, adding the new data generated when
       * calling this object.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& PrefitCSDetector::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

         Process(gData.header.epoch, gData.body, gData.header.epochFlag);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'PrefitCSDetector::Process()'


      /* Returns a satTypeValueMap object, adding the new data generated
       *  when calling this object.
       *
       * @param epoch     Time of observations.
       * @param gData     Data object holding the data.
       * @param epochflag Epoch flag.
       */
   satTypeValueMap& PrefitCSDetector::Process( const CommonTime& epoch,
                                               satTypeValueMap& gData,
                                               const short& epochflag )
      throw(ProcessingException)
   {

      try
      {

         double value1(0.0);
         double value2(0.0);
         double lli1(0.0);
         double lli2(0.0);

         SatIDSet satRejectedSet;

            // satTypeValueMap data to store the difference data between consecutive epoch
         satTypeValueMap diffData;

            // Loop through all the satellites
         satTypeValueMap::iterator it;
         for (it = gData.begin(); it != gData.end(); ++it)
         {

               // Let's find if the same satellite is present in formerData
            satTypeValueMap::const_iterator itformer;
            itformer = formerData.find((*it).first);

               // If we found the satellite, let's proceed with the differences
            if (itformer != formerData.end())
            {
                
                  // Let's try to compute the difference
               value1 = gData((*it).first)(obsType);
               value2 = formerData((*it).first)(obsType);

                  // Get difference into data structure
               diffData[(*it).first][obsType] =  value1 - value2;

            }
            else
            {
                  // If we don't find the satellite in the previous 'satTypeValueData',
                  // it means this satellite is 'new', then set cycle slip for it.
               (*it).second[resultType1] = 1.0;

                  // We will mark both cycle slip flags
               (*it).second[resultType2] = (*it).second[resultType1];

               continue;
            }

         }

         double diffPrefit;
         double diffValue;

             // Compute the median value of the prefit residual differences between consecutive
             // epochs of all observed satellites.
         double medianDiff = median<double>(diffData.getVectorOfTypeID(obsType));

            // Loop through all the satellites
         satTypeValueMap::iterator jt;
         for (jt = diffData.begin(); jt != diffData.end(); ++jt)
         {
            try
            {
                  // Get the difference value of 'prefitL' for this satellite
               diffPrefit = (*jt).second(obsType);

                  // 'diffPrefit' minus the median value of 'diffPrefit' of all the satellites
               diffValue = std::abs(diffPrefit - medianDiff);

            }
            catch(...)
            {
                  // If some value is missing, then schedule this satellite
                  // for removal
               satRejectedSet.insert( (*jt).first );
               continue;
            }

            if (useLLI)
            {
               try
               {
                     // Try to get the LLI1 index
                  lli1  = gData((*jt).first)(lliType1);
               }
               catch(...)
               {
                     // If LLI #1 is not found, set it to zero
                     // You REALLY want to have BOTH LLI indexes properly set
                  lli1 = 0.0;
               }

               try
               {
                     // Try to get the LLI2 index
                  lli2  = gData((*jt).first)(lliType2);
               }
               catch(...)
               {
                     // If LLI #2 is not found, set it to zero
                     // You REALLY want to have BOTH LLI indexes properly set
                  lli2 = 0.0;
               }
            }

               // If everything is OK, then get the new values inside the
               // structure. This way of computing it allows concatenation of
               // several different cycle slip detectors
            gData[(*jt).first][resultType1] += getDetection( epochflag,
                                                             diffValue,
                                                             lli1,
                                                             lli2 );

               // Cycle slip flags
            if ( gData[(*jt).first][resultType1] > 1.0 )
            {
               gData[(*jt).first][resultType1] = 1.0;
            }

               // We will mark both cycle slip flags
            gData[(*jt).first][resultType2] = gData[(*jt).first][resultType1];
            
         }

            // Remove satellites with missing data
         gData.removeSatID(satRejectedSet);

            // Store current 'gData' 
         formerData = gData;

            // Return
         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'PrefitCSDetector::Process()'


      /* Method to set the minimum threshold for cycle slip detection, in
       * meters.
       *
       * @param mThr    Minimum threshold for cycle slip detection, in
       *                meters.
       */
   PrefitCSDetector& PrefitCSDetector::setMinThreshold(const double& mThr)
   {
         // Don't allow thresholds less than 0
      if (mThr < 0.0)
      {
         minThreshold = 0.05;
      }
      else
      {
         minThreshold = mThr;
      }

      return (*this);

   }  // End of method 'PrefitCSDetector::setMinThreshold()'


      /* Method that implements the LI cycle slip detection algorithm
       *
       * @param epoch     Time of observations.
       * @param sat       SatID.
       * @param tvMap     Data structure of TypeID and values.
       * @param epochflag Epoch flag.
       * @param diff      Current difference value of prefit residual between epochs.
       * @param lli1      LLI1 index.
       * @param lli2      LLI2 index.
       */
   double PrefitCSDetector::getDetection( const short& epochflag,
                                          const double& diffValue,
                                          const double& lli1,
                                          const double& lli2 )
   {

      bool reportCS(false);


      double delta(0.0);
      double tempLLI1(0.0);
      double tempLLI2(0.0);

         // Check if receiver already declared cycle slip or too much time
         // has elapsed
         // Note: If tvMap(lliType1) or tvMap(lliType2) don't exist, then 0
         // will be returned and those tests will pass
      if ( (lli1==1.0) ||
           (lli1==3.0) ||
           (lli1==5.0) ||
           (lli1==7.0) )
      {
         tempLLI1 = 1.0;
      }

      if ( (lli2==1.0) ||
           (lli2==3.0) ||
           (lli2==5.0) ||
           (lli2==7.0) )
      {
         tempLLI2 = 1.0;
      }

      if ( (epochflag==1)  ||
           (epochflag==6)  ||
           (tempLLI1==1.0) ||
           (tempLLI2==1.0) ||
           (diffValue > minThreshold) )
      {
         reportCS = true;
      }

      if (reportCS)
      {
         return 1.0;
      }
      else
      {
         return 0.0;
      }

   }  // End of method 'PrefitCSDetector::getDetection()'


}  // End of namespace gpstk
