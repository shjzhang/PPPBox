#pragma ident "$Id: SolverPODFB.cpp 2346 2010-03-13 15:55:58Z yanweignss $"

/**
 * @file SolverPODFB.cpp
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009
//
//============================================================================


#include "SolverPODFB.hpp"

using namespace std;
namespace gpstk
{

      // Index initially assigned to this class
   int SolverPODFB::classIndex = 9400000;


      // Returns an index identifying this object.
   int SolverPODFB::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPODFB::getClassName() const
   { return "SolverPODFB"; }


      /* Common constructor.
       *
       * @param useRAC   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverPODFB::SolverPODFB(bool useRAC)
      : firstIteration(true), minArcSize(30.0)
   {

         // Initialize the counter of processed measurements
      processedMeasurements = 0;

         // Initialize the counter of rejected measurements
      rejectedMeasurements = 0;

         // Set the equation system structure
      SolverPOD::setRAC(useRAC);

         // Set the class index
      setIndex();


         // Indicate the TypeID's that we want to keep
      if (useRAC)
      {
         keepTypeSet.insert(TypeID::dR);
         keepTypeSet.insert(TypeID::dA);
         keepTypeSet.insert(TypeID::dC);
      }
      else
      {
         keepTypeSet.insert(TypeID::dx);
         keepTypeSet.insert(TypeID::dy);
         keepTypeSet.insert(TypeID::dz);
      }
//      keepTypeSet.insert(TypeID::cdtL3);
      keepTypeSet.insert(TypeID::cdt);
      keepTypeSet.insert(TypeID::prefitC);
      keepTypeSet.insert(TypeID::prefitL);
//      keepTypeSet.insert(TypeID::prefitMWubbena);
//      keepTypeSet.insert(TypeID::prefitP3);
//      keepTypeSet.insert(TypeID::prefitL3);
      keepTypeSet.insert(TypeID::weight);
      keepTypeSet.insert(TypeID::CSL1);
      keepTypeSet.insert(TypeID::satArc);


   }  // End of 'SolverPODFB::SolverPODFB()'



      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPODFB::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverPODFB::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPODFB::Process(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {

         SolverPOD::Process(gData);


            // Before returning, store the results for a future iteration
         if(firstIteration)
         {

               // Create a new gnssRinex structure with just the data we need
            gnssRinex gBak(gData.extractTypeID(keepTypeSet));
               
            
               SatIDSet currSatSet= gBak.body.getSatID();
               Vector<double> prefitL(gBak.getVectorOfTypeID(TypeID::prefitL));
               
            
//                     int i=0;
//                     for( SatIDSet::const_iterator itSat = currSatSet.begin();
//                         itSat != currSatSet.end();
//                         ++itSat )
//                     {
//   
//                           double satArc = gBak.body.getValue( (*itSat),TypeID::satArc );
//                           cout<<(*itSat)<<" "<<satArc<<" "<<prefitL(i);
//                           ++i;
//                     }
//                     cout<<endl;

               
               
               

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

   }  // End of method 'SolverPODFB::Process()'

      /* Impose the ambiguity data as the constraint equation for
       * the 'SolverPOD', which will improve the orbits' precision.
       */
   void SolverPODFB::AmbiguityConstr( void )
   {

         // Firstly, take out the ambiguity data from 'SolverPOD'
      ambiguityData = SolverPOD::getAmbiguityData();

            // Now, Let's print out the ambiguity information
//    AmbiguityDataMap::const_iterator itAmb = ambiguityData.begin();
//
//       // If found 
//    while( itAmb!=ambiguityData.end() )
//    {
//       Ambiguity amb( (*itAmb).first );

//       cout << " B3:  " << amb
//            << " wSize: " << ambiguityData[amb].windowSize
//            << " xhat: "
//            << ambiguityData[amb].ambiguity
//            << " P: "
//            << ambiguityData[amb].aCovMap[amb]
//            << endl;
//       
//       ++itAmb;
//    }


         // Clear the ambiguity data in 'SolverPOD'
      SolverPOD::clearAmbiguityData();

         // Then, set the 'ambiguityData' as ambiguity constraints  
      SolverPOD::setAmbiguityConstr(ambiguityData);

         // Return 
      return;

   } // End of 'void SolverPODFB::AmbiguityConstr'



      /* Reprocess the data stored during a previous 'Process()' call.
       *
       * @param cycles     Number of forward-backward cycles, 1 by default.
       *
       * \warning The minimum number of cycles allowed is "1". In fact, if
       * you introduce a smaller number, 'cycles' will be set to "1".
       */
   void SolverPODFB::ReProcess(int cycles)
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

      //   cout<<"ReProcess"<<endl;
      try
      {

         std::list<gnssRinex>::iterator pos;
         std::list<gnssRinex>::reverse_iterator rpos;

            // Set the ambiguity constraints for 'SolverPOD'
         AmbiguityConstr();

            // Backwards iteration. We must do this at least once
         for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
         {
               // Let's check satellite arc size
            checkArcSize( (*rpos) );

            try
            {
               SolverPOD::Process( (*rpos) );
            }
            catch(SVNumException& s)
            {
               continue;
            }
            catch(...)
            {
               continue;
            }
         }

            // If 'cycles > 1', let's do the other iterations
         for (int i=0; i<(cycles-1); i++)
         {
               // Set the ambiguity constraints for 'SolverPOD'
            AmbiguityConstr();

               // Forwards iteration
            for (pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
                  // Let's check satellite arc size
               checkArcSize( (*pos) );

               try
               {
                  SolverPOD::Process( (*pos) );
               }
               catch(SVNumException& s)
               {
                  continue;
               }
            }

               // Set the ambiguity constraints for 'SolverPOD'
            AmbiguityConstr();

               // Backwards iteration.
            for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
                  // Let's check the satellite are size
               checkArcSize( (*rpos) );

               try
               {
                  SolverPOD::Process( (*rpos) );
               }
               catch(SVNumException& s)
               {
                  continue;
               }
            }

         }  // End of 'for (int i=0; i<(cycles-1), i++)'


            // Set the ambiguity constraints for the 'LastProcess'.
            // The 'LastProcess' will process only one epoch
            // data each time, so you should set the constraints
            // before that here.
         AmbiguityConstr();


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

   }  // End of method 'SolverPODFB::ReProcess()'



      /* Reprocess the data stored during a previous 'Process()' call.
       *
       * This method will reprocess data trimming satellites whose postfit
       * residual is bigger than the limits indicated by limitsCodeList and
       * limitsPhaseList.
       */
   void SolverPODFB::ReProcess( void )
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

            // Set the ambiguity constraints for 'SolverPOD'
         AmbiguityConstr();

            // Backwards iteration. We must do this at least once
         for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
         {

            SolverPOD::Process( (*rpos) );

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

               // Set the ambiguity constraints for 'SolverPOD'
            AmbiguityConstr();

               // Forwards iteration
            for (pos = ObsData.begin(); pos != ObsData.end(); ++pos)
            {
                  // Let's check limits
               checkLimits( (*pos), codeLimit, phaseLimit );

                  // Process data
               SolverPOD::Process( (*pos) );
            }

               // Set the ambiguity constraints for 'SolverPOD'
            AmbiguityConstr();

               // Backwards iteration.
            for (rpos = ObsData.rbegin(); rpos != ObsData.rend(); ++rpos)
            {
                  // Let's check limits
               checkLimits( (*rpos), codeLimit, phaseLimit );

                  // Process data
               SolverPOD::Process( (*rpos) );
            }


         }  // End of 'for (int i=0; i<(cycles-1), i++)'


            // Set the ambiguity constraints for the 'LastProcess'.
            // The 'LastProcess' will process only one epoch
            // data each time, so you should set the constraints
            // before that here.
         AmbiguityConstr();


         return;

      }
      catch(SVNumException& s)
      {
            // Rethrow the SVNumException
         GPSTK_RETHROW(s);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPODFB::ReProcess()'



      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverPODFB::LastProcess(gnssSatTypeValue& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {

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
      catch(SVNumException& s)
      {
            // Rethrow the SVNumException
         GPSTK_RETHROW(s);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPODFB::LastProcess()'



      /* Process the data stored during a previous 'ReProcess()' call, one
       * item at a time, and always in forward mode.
       *
       * @param gData      Data object that will hold the resulting data.
       *
       * @return FALSE when all data is processed, TRUE otherwise.
       */
   bool SolverPODFB::LastProcess(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {

            // Keep processing while 'ObsData' is not empty
         if( !(ObsData.empty()) )
         {

               // Get the first 'gnssRinex'
            gnssRinex gBak( ObsData.front() );

               // Remove the first data epoch in 'ObsData', freeing some
               // memory and preparing for next epoch
            ObsData.pop_front();

               // Let's check satellite arc size
            checkArcSize( gBak );

                // Get the first data epoch in 'ObsData' and process it. The
                // result will be stored in 'gData'
            gData = SolverPOD::Process( gBak );

               // Update some inherited fields
            solution = SolverPOD::solution;
            covMatrix = SolverPOD::covMatrix;
            postfitResiduals = SolverPOD::postfitResiduals;

               // If everything is fine so far, then results should be valid
            valid = true;

            return true;

         }
         else
         {

               // There are no more data
            return false;

         }  // End of 'if( !(ObsData.empty()) )'

      }
      catch(SVNumException& s)
      {
            // Rethrow the SVNumException
         GPSTK_RETHROW(s);

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPODFB::LastProcess()'



      // This method checks the limits and modifies 'gData' accordingly.
   void SolverPODFB::checkLimits( gnssRinex& gData,
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

   }  // End of method 'SolverPODFB::checkLimits()'


      // This method checks satellite arc size.
   void SolverPODFB::checkArcSize( gnssRinex& gData )
   {

         // Set to store rejected satellites
      SatIDSet satRejectedSet;

   // cout << "checkArcSize:" << gData.header.epoch << endl;

         // Let's check limits
      for( satTypeValueMap::iterator it = gData.body.begin();
           it != gData.body.end();
           ++it )
      {

            // Get the satellite arc number
         double satArc = (*it).second.getValue( TypeID::satArc );

            // Now, create the the ambiguity identity
         Ambiguity ambiguity( (*it).first, satArc);

            // Find this ambiguity in the 'ambiguityData'
         if( ambiguityData.find(ambiguity) != ambiguityData.end())
         {
            if(ambiguityData[ambiguity].windowSize <= minArcSize)
            {
//             cout << "checkArcSize:" << gData.header.epoch << endl;
//             cout << "shortArc:" << (*it).first<<"  "<<satArc;
//             cout << " windowSize:" << ambiguityData[ambiguity].windowSize<< endl;
//             //cout << " minArcSize" << minArcSize << endl;
               satRejectedSet.insert( (*it).first );
            }
         }

      }  // End of 'for( satTypeValueMap::iterator it = gds.body.begin();...'

         // Update the number of rejected measurements
      rejectedMeasurements += satRejectedSet.size();

         // Remove satellites with missing data
      gData.removeSatID(satRejectedSet);

      return;

   }  // End of method 'SolverPODFB::checkArcSize()'



      /* Sets if a RAC system will be used.
       *
       * @param useRAC  Boolean value indicating if a RAC system will
       *                be used
       *
       */
   SolverPODFB& SolverPODFB::setRAC( bool useRAC )
   {

         // Set the SolverPOD filter
      SolverPOD::setRAC(useRAC);


         // Clear current 'keepTypeSet' and indicate the TypeID's that
         // we want to keep
      keepTypeSet.clear();

      if (useRAC)
      {
         keepTypeSet.insert(TypeID::dR);
         keepTypeSet.insert(TypeID::dA);
         keepTypeSet.insert(TypeID::dC);
      }
      else
      {
         keepTypeSet.insert(TypeID::dx);
         keepTypeSet.insert(TypeID::dy);
         keepTypeSet.insert(TypeID::dz);
      }
//      keepTypeSet.insert(TypeID::cdtL3);

      keepTypeSet.insert(TypeID::cdt);
      keepTypeSet.insert(TypeID::prefitC);
      keepTypeSet.insert(TypeID::prefitL);
//      keepTypeSet.insert(TypeID::prefitMWubbena);
//      keepTypeSet.insert(TypeID::prefitP3);
//      keepTypeSet.insert(TypeID::prefitL3);
      keepTypeSet.insert(TypeID::weight);
      keepTypeSet.insert(TypeID::CSL1);
      keepTypeSet.insert(TypeID::satArc);


         // Return this object
      return (*this);

   }  // End of method 'SolverPODFB::setRAC()'



}  // End of namespace gpstk
