#pragma ident "$Id: SolverPPPARFB.cpp 2346 2010-03-13 15:55:58Z yanweignss $"

/**
 * @file SolverPPPARFB.cpp
 * Class to compute the PPP solution in forwards-backwards mode.
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
//  Shjzhang, Wuhan University, 2015.
//
//============================================================================


#include "SolverPPPARFB.hpp"


namespace gpstk
{

      // Index initially assigned to this class
   int SolverPPPARFB::classIndex = 9400000;


      // Returns an index identifying this object.
   int SolverPPPARFB::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPPPARFB::getClassName() const
   { return "SolverPPPARFB"; }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverPPPARFB::SolverPPPARFB(bool useNEU)
      : SolverPPPAR(useNEU), firstIteration(true)
   {

         // Initialize the counter of processed measurements
      processedMeasurements = 0;

         // Initialize the counter of rejected measurements
      rejectedMeasurements = 0;

         // Set the class index
      setIndex();


         // Indicate the TypeID's that we want to keep

      keepTypeSet.insert(TypeID::wetMap);
      if (useNEU)
      {
         keepTypeSet.insert(TypeID::dLat);
         keepTypeSet.insert(TypeID::dLon);
         keepTypeSet.insert(TypeID::dH);
      }
      else
      {
         keepTypeSet.insert(TypeID::dx);
         keepTypeSet.insert(TypeID::dy);
         keepTypeSet.insert(TypeID::dz);
      }

      keepTypeSet.insert(TypeID::cdt);
      keepTypeSet.insert(TypeID::cdtLC);
      keepTypeSet.insert(TypeID::cdtMW);

      keepTypeSet.insert(TypeID::BLC);    // #3
      keepTypeSet.insert(TypeID::BWL);    // #4

         // Observables
      keepTypeSet.insert(TypeID::prefitC);
      keepTypeSet.insert(TypeID::prefitL);
      keepTypeSet.insert(TypeID::prefitMWubbena);
      keepTypeSet.insert(TypeID::weight);
      keepTypeSet.insert(TypeID::CSL1);
      keepTypeSet.insert(TypeID::satArc);
      keepTypeSet.insert(TypeID::elevation);

   }  // End of 'SolverPPPARFB::SolverPPPARFB()'



      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPPPARFB::Process(gnssSatTypeValue& gData)
      throw(ProcessingException, SVNumException)
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
      catch(SVNumException& e)
      {

            // Rethrow the exception
         GPSTK_RETHROW(e);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPARFB::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPPPARFB::Process(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {

         SolverPPPAR::Process(gData);

            // Before returning, store the results for a future iteration
         if(firstIteration)
         {

               // Create a new gnssRinex structure with just the data we need
            gnssRinex gBak(gData.extractTypeID(keepTypeSet));

               // Store observation data
            ObsData.push_back(gBak);

               // Update the number of processed measurements
            processedMeasurements += gData.numSats();

         }

         return gData;

      }
      catch(SVNumException& e)
      {

            // Rethrow the exception
         GPSTK_RETHROW(e);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPARFB::Process()'



      /* Reprocess the data stored during a previous 'Process()' call.
       *
       * @param cycles     Number of forward-backward cycles, 1 by default.
       *
       * \warning The minimum number of cycles allowed is "1". In fact, if
       * you introduce a smaller number, 'cycles' will be set to "1".
       */
   void SolverPPPARFB::ReProcess(int cycles)
      throw(ProcessingException, SVNumException)
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

         std::list<gnssRinex>::iterator pos;
         std::list<gnssRinex>::reverse_iterator rpos;

         if(debugLevel)
         {
            cout << "backward" << endl;
         }

            // Backwards iteration. We must do this at least once
         for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
         {
            try
            {
               SolverPPPAR::Process( (*rpos) );
            }
            catch(SVNumException& e)
            {
               continue;
            }

         }

            // If 'cycles > 1', let's do the other iterations
         for (int i=0; i<(cycles-1); i++)
         {

               // Forwards iteration
            for (pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
               try
               {
                  SolverPPPAR::Process( (*pos) );
               }
               catch(SVNumException& e)
               {
                  continue;
               }
            }

               // Backwards iteration.
            for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
               try
               {
                  SolverPPPAR::Process( (*rpos) );
               }
               catch(SVNumException& e)
               {
                  continue;
               }
            }

         }  // End of 'for (int i=0; i<(cycles-1), i++)'

         return;

      }
      catch(SVNumException& e)
      {

            // Rethrow the exception
         GPSTK_RETHROW(e);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPARFB::ReProcess()'



      /* Reprocess the data stored during a previous 'Process()' call.
       *
       * This method will reprocess data trimming satellites whose postfit
       * residual is bigger than the limits indicated by limitsCodeList and
       * limitsPhaseList.
       */
   void SolverPPPARFB::ReProcess( void )
      throw(ProcessingException, SVNumException)
   {

         // Let's use a copy of the lists
      std::list<double> codeList( limitsCodeList );
      std::list<double> phaseList( limitsPhaseList );

         // Get maximum size
      int maxSize( codeList.size() );
      if( maxSize < phaseList.size() ) maxSize = phaseList.size();

         // This will prevent further storage of input data when calling
         // method 'Process()'
      firstIteration = false;

      try
      {

         std::list<gnssRinex>::iterator pos;
         std::list<gnssRinex>::reverse_iterator rpos;

            // Backwards iteration. We must do this at least once
         for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
         {

            SolverPPPAR::Process( (*rpos) );

         }

            // If both sizes are '0', let's return
         if( maxSize == 0 )
         {
            return;
         }

            // We will store the limits here. By default we use very big values
         double codeLimit( 1000000.0 );
         double phaseLimit( 1000000.0 );

            // If 'maxSize > 0', let's do the other iterations
         for (int i = 0; i < maxSize; i++)
         {

               // Update current limits, if available
            if( codeList.size() > 0 )
            {
                  // Get the first element from the list
               codeLimit = codeList.front();

                  // Delete the first element from the list
               codeList.pop_front();
            }

            if( phaseList.size() > 0 )
            {
                  // Get the first element from the list
               phaseLimit = phaseList.front();

                  // Delete the first element from the list
               phaseList.pop_front();
            }


               // Forwards iteration
            for (pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
                  // Let's check limits
               checkLimits( (*pos), codeLimit, phaseLimit );

                  // Process data
               SolverPPPAR::Process( (*pos) );
            }

               // Backwards iteration.
            for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
                  // Let's check limits
               checkLimits( (*rpos), codeLimit, phaseLimit );

                  // Process data
               SolverPPPAR::Process( (*rpos) );
            }

         }  // End of 'for (int i=0; i<(cycles-1), i++)'

         return;

      }
      catch(SVNumException& e)
      {

            // Rethrow the exception
         GPSTK_RETHROW(e);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPARFB::ReProcess()'



      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverPPPARFB::LastProcess(gnssSatTypeValue& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {
         if(debugLevel)
         {
            cout << "LastProcess" << endl;
         }

            // Declare a gnssRinex object
         gnssRinex g1;

            // Call the 'LastProcess()' method and store the result
         bool result( LastProcess(g1) );

         if(result)
         {
               // Convert from 'gnssRinex' to 'gnnsSatTypeValue'
            gData.header = g1.header;
            gData.body   = g1.body;

         }

         return result;

      }
      catch(SVNumException& e)
      {

            // Rethrow the exception
         GPSTK_RETHROW(e);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPARFB::LastProcess()'



      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverPPPARFB::LastProcess(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {

            // Keep processing while 'ObsData' is not empty
         if( !(ObsData.empty()) )
         {

               // Get the first data epoch in 'ObsData' and process it. The
               // result will be stored in 'gData'
            gData = SolverPPPAR::Process( ObsData.front() );

               // Remove the first data epoch in 'ObsData', freeing some
               // memory and preparing for next epoch
            ObsData.pop_front();


               // Update some inherited fields
            solution = SolverPPPAR::solution;
            covMatrix = SolverPPPAR::covMatrix;
            postfitResiduals = SolverPPPAR::postfitResiduals;

               // If everything is fine so far, then results should be valid
            valid = true;

            return valid;

         }
         else
         {

               // There are no more data
            return false;

         }  // End of 'if( !(ObsData.empty()) )'

      }
      catch(SVNumException& e)
      {

            // Rethrow the exception
         GPSTK_RETHROW(e);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPARFB::LastProcess()'



      // This method checks the limits and modifies 'gData' accordingly.
   void SolverPPPARFB::checkLimits( gnssRinex& gData,
                                  double codeLimit,
                                  double phaseLimit )
   {

         // Set to store rejected satellites
      SatIDSet satRejectedSet;

         // Let's check limits
      for( satTypeValueMap::iterator it = gData.body.begin();
           it != gData.body.end();
           ++it )
      {

            // Check postfit values and mark satellites as rejected
         if( std::abs((*it).second( TypeID::postfitC )) > codeLimit )
         {
            satRejectedSet.insert( (*it).first );
         }

         if( std::abs((*it).second( TypeID::postfitL )) > phaseLimit )
         {
            satRejectedSet.insert( (*it).first );
         }

      }  // End of 'for( satTypeValueMap::iterator it = gds.body.begin();...'


         // Update the number of rejected measurements
      rejectedMeasurements += satRejectedSet.size();

         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return;

   }  // End of method 'SolverPPPARFB::checkLimits()'



      /* Sets if a NEU system will be used.
       *
       * @param useNEU  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverPPPARFB& SolverPPPARFB::setNEU( bool useNEU )
   {

         // Set the SolverPPP filter
      SolverPPPAR::setNEU(useNEU);


         // Clear current 'keepTypeSet' and indicate the TypeID's that
         // we want to keep
      keepTypeSet.clear();


         // Unknown parametes TypeID 

      keepTypeSet.insert(TypeID::wetMap);
      if (useNEU)
      {
         keepTypeSet.insert(TypeID::dLat);
         keepTypeSet.insert(TypeID::dLon);
         keepTypeSet.insert(TypeID::dH);
      }
      else
      {
         keepTypeSet.insert(TypeID::dx);
         keepTypeSet.insert(TypeID::dy);
         keepTypeSet.insert(TypeID::dz);
      }

         // Observables TypeID
      keepTypeSet.insert(TypeID::prefitC);
      keepTypeSet.insert(TypeID::prefitL);
      keepTypeSet.insert(TypeID::prefitMWubbena);
      keepTypeSet.insert(TypeID::weight);
      keepTypeSet.insert(TypeID::CSL1);
      keepTypeSet.insert(TypeID::satArc);
      keepTypeSet.insert(TypeID::elevation);

         // Return this object
      return (*this);

   }  // End of method 'SolverPPPARFB::setNEU()'


}  // End of namespace gpstk
