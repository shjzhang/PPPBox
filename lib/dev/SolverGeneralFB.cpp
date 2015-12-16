#pragma ident "$Id: SolverGeneralFB.cpp $"

/**
 * @file SolverGeneralFB.cpp
 * Class to solve the general equations in forwards-backwards mode.
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
//  Revision
//  --------
//  2014/02/17      Modify this program from the 'SolverPPPFB' 
//
//  Author
//  ------
//  Shoujian Zhang, Wuhan University, 2014, email:shjzhang@sgg.whu.edu.cn
//
//============================================================================

#include "SolverGeneralFB.hpp"

using namespace std;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverGeneralFB::classIndex = 9500000;


      // Returns an index identifying this object.
   int SolverGeneralFB::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverGeneralFB::getClassName() const
   { return "SolverGeneralFB"; }


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverGeneralFB::Process(gnssSatTypeValue& gData)
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
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGeneral::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverGeneralFB::Process(gnssRinex& gData)
      throw(ProcessingException)
   {

      try
      {

            // Build a gnssDataMap object and fill it with data
         gnssDataMap gdsMap;
         SourceID source( gData.header.source );
         gdsMap.addGnssRinex( gData );

            // Call the Process() method with the appropriate input object,
            // and update the original gnssRinex object with the results
         Process(gdsMap);
         gData = gdsMap.getGnssRinex( source );

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

   }  // End of method 'SolverGeneral::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssDataMap& SolverGeneralFB::Process(gnssDataMap& gData)
      throw(ProcessingException)
   {

      try
      {
            // Processing the input gnssRinex Data
         SolverGeneral::Process(gData);

            // Before returning, store the results for a future iteration
         if( firstIteration)
         {
                // Store observation data, which will be used later
             ObsData.addGnssDataMap( gData );
         }

            // return
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

   }  // End of method 'SolverGeneralFB::Process()'


      /* Reprocess the data stored during a previous 'Process()' call.
       *
       * @param cycles     Number of forward-backward cycles, 1 by default.
       *
       * \warning The minimum number of cycles allowed is "1". In fact, if
       * you introduce a smaller number, 'cycles' will be set to "1".
       */
   void SolverGeneralFB::ReProcess(int cycles)
      throw(ProcessingException)
   {

         // Check number of cycles. The minimum allowed is "1".
      if (cycles < 1)
      {
         cycles = 1;
      }

         // This will prevent further storage of input data when calling
         // method 'Process()'
      firstIteration = false;

      try
      {
            // Temporary data map
         gnssDataMap dataMap;

            // Get data from 'ObsData'
         dataMap.addGnssDataMap(ObsData);

            // Backward filter
         while( !dataMap.empty() )
         {
               // Get the first epoch data out of GDS
            gnssDataMap gds(dataMap.backEpoch());

               // Process the gnss data
            SolverGeneral::Process( gds );

               // Remove the last epoch gnss data
            dataMap.pop_back_epoch();  
         }

            // If 'cycles > 1', let's do the other iterations
         for (int i=0; i<(cycles-1); i++)
         {
               // Firstly, The forward filter
               
               // Get data from 'ObsData'
            dataMap.addGnssDataMap(ObsData);

               // forward filter
            while( !dataMap.empty() )
            {
                  // Get the first epoch data out of GDS
               gnssDataMap gds(dataMap.frontEpoch());

                  // Process the gnss data
               SolverGeneral::Process( gds );

                  // Remove the last epoch gnss data
               dataMap.pop_front_epoch();  

            } // End of the forward filter

               // Now, The backward filter

               // Get data from 'ObsData'
            dataMap.addGnssDataMap(ObsData);

               // Loop the 'dataMap'
            while( !dataMap.empty() )
            {
                  // Get the first epoch data out of GDS
               gnssDataMap gds(dataMap.backEpoch());

                  // Process the gnss data
               SolverGeneral::Process( gds );

                  // Remove the last epoch gnss data
               dataMap.pop_back_epoch();  

            }  // End of the backward filter

         }  // End of 'for (int i=0; i<(cycles-1), i++)'

         return;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGeneralFB::ReProcess()'



      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverGeneralFB::LastProcess(gnssSatTypeValue& gData)
      throw(ProcessingException)
   {

      try
      {
            // Declare a gnssRinex object
         gnssRinex g1;

            // Call the 'LastProcess()' method and store the result
         bool result( LastProcess(g1) );

            // Convert from 'gnssRinex' to 'gnnsSatTypeValue'
         if(result)
         {
            gData.header = g1.header;
            gData.body   = g1.body;
         }

         return result;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGeneralFB::LastProcess()'



      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverGeneralFB::LastProcess(gnssRinex& gData)
      throw(ProcessingException)
   {
      try
      {
            // Build a gnssDataMap object and fill it with data
         gnssDataMap gdsMap;

            // Call the Process() method with the appropriate input object,
            // and update the original gnssRinex object with the results
         bool result(LastProcess(gdsMap));

            // Convert from 'gnssRinex' to 'gnnsSatTypeValue'
         if(result)
         {
            SourceID source( gData.header.source );
            gData = gdsMap.getGnssRinex( source );
         }

         return result;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGeneralFB::LastProcess()'


      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverGeneralFB::LastProcess(gnssDataMap& gData)
      throw(ProcessingException)
   {

      try
      {
            // Keep processing while 'ObsData' is not empty
         if( !(ObsData.empty()) )
         {

               // Build a gnssDataMap object and fill it with data
            gnssDataMap gdsMap(ObsData.frontEpoch());

               // Remove the first data epoch in 'ObsData', freeing some
               // memory and preparing for next epoch
            ObsData.pop_front_epoch();

               // Get the first data epoch in 'ObsData' and process it. The
               // result will be stored in 'gData'
            gData = SolverGeneral::Process( gdsMap );

               // If everything is fine so far, then results should be valid
            valid = true;

               // Return valid
            return valid;

         }
         else
         {
               // There are no more data
            return false;

         }  // End of 'if( !(ObsData.empty()) )'

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverGeneralFB::LastProcess()'


}  // End of namespace gpstk
