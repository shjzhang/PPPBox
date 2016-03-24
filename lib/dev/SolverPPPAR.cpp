#pragma ident "$Id: SolverPPPAR.cpp 2645 2012-11-05 03:23:24Z shjzhang $"

/**
 * @file SolverPPPAR.cpp
 * Class to compute the PPP Solution by fixing the ambiguities.
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
//  
//  Revision
//
//  2014/03/01      
//  created this program to process the extended equations
//  for PPP, which are P1/P2/L1/L2/PW/LW. 
//
//  2014/06/01      
//  created this program to process the extended equations
//  for PPP, which are P3/L3/PW/LW. 
//
//  2014/07/03      
//  Let's get and set the tropo/Iono constraints  
//
//  2014/12/10      
//  Now, Let's Solver LC/MW together
//
//  2015/10/16      
//  Now, Let's fix the ambiguity using LAMBDA method.
//
//  2015/09/20      
//  print out the ttff.
//
//  2015/11/04      
//  Now, stable version!!! (shjzhang)
//
//  2015/11/06      
//  No longer throw out SVNumException!!!  Because that, even satellite 
//  is less than 4, the filter still can continues. 
//
//  2015/12/07      
//  Don't remove satellite if the ambiguity is not fixed, instead, insert
//  the flag into the gRin.
//                  
//============================================================================


#include "SolverPPPAR.hpp"
#include "MatrixFunctors.hpp"
#include "ARMLambda.hpp"
#include "TimeString.hpp"
#include "Epoch.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverPPPAR::classIndex = 9300000;

      // Debug level for print out information
   int SolverPPPAR::debugLevel = 0;

      // Returns an index identifying this object.
   int SolverPPPAR::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPPPAR::getClassName() const
   { return "SolverPPPAR"; }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverPPPAR::SolverPPPAR(bool useNEU )
      : firstTime(true), converged(false), bufferSize(4),
        reInitialize(false), reIntialInterv(864000000.0)
   {

         // Set the equation system structure
      setNEU(useNEU);

         // Set the class index
      setIndex();

         // Call initializing method
      Init();

   }  // End of 'SolverPPPAR::SolverPPPAR()'



      // Initializing method.
   void SolverPPPAR::Init(void)
   {

         // Pointer to default stochastic model for troposphere (random walk)
      pTropoStoModel = &tropoModel;

         // Set default coordinates stochastic model (constant)
      setCoordinatesModel( &constantModel );

         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel   = &whitenoiseModel;
      pClockStoModelLC = &whitenoiseModel;
      pBiasStoModelMW  = &whitenoiseModel;

         // Pointer to stochastic model for phase biases
      pAmbiModelLC = &ambiModelLC;
      pAmbiModelWL = &ambiModelWL;


   }  // End of method 'SolverPPPAR::Init()'




      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPPPAR::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverPPPAR::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPPPAR::Process(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {

            // Prepare everything before computing
         preCompute( gData);


            // Call the Compute() method with the defined equation model.
         Compute( gData );


            // Store data after computing
         postCompute( gData);


            // return
         return gData;

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

   }  // End of method 'SolverPPPAR::Process()'


      /* Code to be executed before 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverPPPAR::preCompute( gnssRinex& gData )
      throw(ProcessingException, SVNumException)
   {

         // Continue ...
      try
      {
            // Time
         if(debugLevel)
         {
            cout << static_cast<YDSTime>(gData.header.epoch).year  << " "
                 << static_cast<YDSTime>(gData.header.epoch).doy   << " "
                 << static_cast<YDSTime>(gData.header.epoch).sod   << " "
                 << endl;
         }

            // Get a set with all satellites present in this GDS
         currSatSet = gData.body.getSatID();

         if(debugLevel)
         {
            cout <<  currSatSet.size() << endl;
         }

            //>Let's create the variables for PPP 

            // Firstly, clear the 'varUnknowns'
         varUnknowns.clear();

            // Now, Let's create the satellite-related unknowns
         for(TypeIDSet::const_iterator itType = satIndexedTypes.begin();
             itType != satIndexedTypes.end();
             ++itType)
         {
                // We will work with a copy of current TypeID
             Variable var( (*itType) );

                // Loop the current satellites
             for( SatIDSet::const_iterator itSat = currSatSet.begin();
                  itSat != currSatSet.end();
                  ++itSat )
             {
                    // Set satellite
                 var.setSatellite( (*itSat) );

                    // Insert this variable in 'varUnknowns'
                 varUnknowns.insert(var);
             }

         }


            // Get the number of satellites currently visible
         numCurrentSV =  gData.numSats();

///////////////////// /////////////////////
//
//          // Check current SV number
//       if( numCurrentSV < 4 )
//       {
//             // If epoch must be decimated, we issue an Exception
//          SVNumException e("Satellite number is less than 4");
//          
//             // Throw exception
//          GPSTK_THROW(e);
//
//       }
//
///////////////////// /////////////////////

            // Number of measurement
         numMeas = 3 * numCurrentSV ;

            // Number of 'core' variables i.e.  
            // 1 trospheric delay + 3 coordinates + 3 receiver clock 
         numVar = srcIndexedTypes.size();

            // Total number of unknowns is defined as :
            // 2*numCurrentSV of ambiguities
         numUnknowns = numVar + 2*numCurrentSV;


            ///////////////////
            // The measVector
            ///////////////////
            

            // Build the vector of measurements (Prefit-residuals): Code + phase
         measVector.resize(numMeas, 0.0);

            // Get the prefit residuals from 'gData'
         Vector<double> prefitC(gData.getVectorOfTypeID(TypeID::prefitC));
         Vector<double> prefitL(gData.getVectorOfTypeID(TypeID::prefitL));
         Vector<double> prefitMW(gData.getVectorOfTypeID(TypeID::prefitMWubbena));

         for( int i=0; i<numCurrentSV; i++ )
         {
            measVector( i                  ) = prefitC(i);
            measVector( i + 1*numCurrentSV ) = prefitL(i);
            measVector( i + 2*numCurrentSV ) = prefitMW(i);
         }

         if(debugLevel)
         {
            cout << "measVector" << endl;
            cout <<  measVector  << endl;
         }

            ///////////////////
            // Fill the rMatrix
            ///////////////////

            // Weights matrix
         rMatrix.resize(numMeas, numMeas, 0.0);

            // Now, get the weight from 'gData'
         satTypeValueMap dummy(gData.body.extractTypeID(TypeID::weight));

            // Check if weights match
         if ( dummy.numSats() == numCurrentSV )
         {
               // If we have weights information, let's load it
            Vector<double> wVec(gData.getVectorOfTypeID(TypeID::weight));

            for( int i=0; i<numCurrentSV; i++ )
            {
               rMatrix( i,                  i                  ) 
                   = 1.0*wVec(i); // 1.0/0.5 * 1.0/0.5
               rMatrix( i + 1*numCurrentSV, i + 1*numCurrentSV ) 
                   = 10000.0*wVec(i); // 1.0/0.01 * 1.0/0.01
               rMatrix( i + 2*numCurrentSV, i + 2*numCurrentSV ) 
                   = 16*wVec(i); // 1.0/0.25

            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         }
         else
         {
            for( int i=0; i<numCurrentSV; i++ )
            {
               rMatrix( i,                  i                  ) 
                   = 1.0; // 1.0/0.3 * 1.0/0.3
               rMatrix( i + 1*numCurrentSV, i + 1*numCurrentSV ) 
                   = 10000.0; // 1.0/0.003 * 1.0/0.003
               rMatrix( i + 2*numCurrentSV, i + 2*numCurrentSV ) 
                   = 16; // 1.0/((c*c+d*d)*varCode)
            }
         }

            //////////////// //////////////
            //
            // Fill the hMatrix
            //
            //////////////// //////////////

            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns, 0.0);

            // Get the values corresponding to 'core' variables
         Matrix<double> dMatrix(gData.body.getMatrixOfTypes(TropAndCoord));

            // Let's fill 'hMatrix'
         for( int i=0; i<numCurrentSV; i++ )
         {
               // First, fill the coefficients related to tropo, coord 
            for( int j=0; j<4; j++ )
            {

                  // PC
               hMatrix( i                 , j ) = dMatrix(i,j); // C
                
                  // LC
               hMatrix( i +   numCurrentSV, j ) = dMatrix(i,j); // L
            }

               // Now, fill the reciever clock's coefficients 
            hMatrix( i                 , 4 ) = 1.0; // C
            hMatrix( i +   numCurrentSV, 5 ) = 1.0; // L

               // Now, fill the receiver clock for MW
               // MW = br - bs - lambda*BWL
            hMatrix( i + 2*numCurrentSV, 6 ) = 1.0; // MW


         }  // End of 'for( int i=0; i<numCurrentSV; i++ )'


         int count1(0);

            // Now, fill the coefficients related to ambiguities
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // ambLC are listed after ( srcIndexedTypes )
            hMatrix( count1 +   numCurrentSV, numVar + count1 ) 
                = -0.106953378142;
               // ambWL are listed after ( srcIndexedTypes + ambLC )
            hMatrix( count1 + 2*numCurrentSV, numVar + numCurrentSV + count1 ) 
                = -0.861918400322;

            ++count1;

         }  // End of 'for( itSat = currSatSet.begin(); ... )'


            // Now, Fill the phiMatrix and qMatrix

            // State Transition Matrix (PhiMatrix)
         phiMatrix.resize(numUnknowns, numUnknowns, 0.0);

            // Noise covariance matrix (QMatrix)
         qMatrix.resize(numUnknowns, numUnknowns, 0.0);


            // Now, let's fill the Phi and Q matrices
         SatID  dummySat;

            // First, the troposphere
         pTropoStoModel->Prepare( dummySat,
                                  gData );
         phiMatrix(0,0) = pTropoStoModel->getPhi();
         qMatrix(0,0)   = pTropoStoModel->getQ();

            // Second, the coordinates
         pCoordXStoModel->Prepare(dummySat, gData);
         phiMatrix(1,1) = pCoordXStoModel->getPhi();
         qMatrix(1,1)   = pCoordXStoModel->getQ();

         pCoordYStoModel->Prepare(dummySat, gData);
         phiMatrix(2,2) = pCoordYStoModel->getPhi();
         qMatrix(2,2)   = pCoordYStoModel->getQ();

         pCoordZStoModel->Prepare(dummySat, gData);
         phiMatrix(3,3) = pCoordZStoModel->getPhi();
         qMatrix(3,3)   = pCoordZStoModel->getQ();

            // Third, the receiver clock
         pClockStoModel->Prepare( dummySat,
                                  gData );
         phiMatrix(4,4) = pClockStoModel->getPhi();
         qMatrix(4,4)   = pClockStoModel->getQ();

            // Four, the receiver clock
         pClockStoModelLC->Prepare( dummySat,
                                  gData );
         phiMatrix(5,5) = pClockStoModelLC->getPhi();
         qMatrix(5,5)   = pClockStoModelLC->getQ();

            // Five, the receiver clock
         pBiasStoModelMW->Prepare( dummySat,
                                  gData );
         phiMatrix(6,6) = pBiasStoModelMW->getPhi();
         qMatrix(6,6)   = pBiasStoModelMW->getQ();


            // Now, fill the ionospheric delays
         int count2(numVar);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Prepare stochastic model
            pAmbiModelLC->Prepare( *itSat,
                                   gData );

               // Get values into phi and q matrices
            phiMatrix(count2,count2) = pAmbiModelLC->getPhi();
            qMatrix(count2,count2)   = pAmbiModelLC->getQ();

            ++count2;
         }

            // Now, fill the ambiguity on WL
         count2 = (numVar + numCurrentSV);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Prepare stochastic model
            pAmbiModelWL->Prepare( *itSat,
                                   gData );

               // Get values into phi and q matrices
            phiMatrix(count2,count2) = pAmbiModelWL->getPhi();
            qMatrix(count2,count2)   = pAmbiModelWL->getQ();

            ++count2;
         }

            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );

               // Fill the initialErrorCovariance matrix

               // First, the zenital wet tropospheric delay
            initialErrorCovariance(0,0) = 0.25;          // (0.5 m)**2

               // Second, the coordinates
            for( int i=1; i<4; i++ )
            {
               initialErrorCovariance(i,i) = 100.0;       // (1.0 m)**2
            }

            initialErrorCovariance(4,4) = 9.0e6;
            initialErrorCovariance(5,5) = 9.0e6;
            initialErrorCovariance(6,6) = 9.0e6;

               // Finally, the ambiguities 
            for( int i=numVar; i<numUnknowns; i++ )
            {
               initialErrorCovariance(i,i) = 4.0e14;     // (20000 km)**2
            }

               // Reset Kalman filter state and covariance matrix
            xhat = initialState; 
            P    = initialErrorCovariance;

               // record the first epoch 
            firstEpoch = gData.header.epoch;

            double sod( (gData.header.epoch).getSecondOfDay() );

               // set the flags for ttff/ttfs statistics
            resetL1 = resetL2 = resetWL = true;
            converged = false;
            startTime = sod;
            startTimeVec.push_back(startTime);

               // No longer first time
            firstTime = false;

         }
         else
         {
               // Current Epoch
            CommonTime currEpoch(gData.header.epoch);

               // Offset
            double timeOffset( currEpoch - firstEpoch );

            double sod( (gData.header.epoch).getSecondOfDay() );

            double tolerance(0.5);
            double lowerBound( std::abs(tolerance) );
            double upperBound( std::abs(reIntialInterv - tolerance) );

            
               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);

            if( reInitialize &&
                ( ( (int)(timeOffset) % (int)(reIntialInterv) < lowerBound ) || 
                  ( (int)(timeOffset) % (int)(reIntialInterv) > upperBound ) ) )
            {
                  // set the flags for ttff/ttfs statistics
               resetL1 = resetL2 = resetWL = true;
               converged = false;
               startTime = sod;
               startTimeVec.push_back(startTime);

                  // Firstly, fill the state and covariance matrix for 
                  // source-indexed variables
               for( int i=0; i<numVar; i++ )
               {
                  currentState(i) = solution(i);

                     // This fills the upper left quadrant of covariance matrix
                  for( int j=0; j<numVar; j++ )
                  {
                     currentErrorCov(i,j) =  covMatrix(i,j);
                  }
               }

                  // Then, reset the ambiguity, which is equivalent
                  // to introducing cycle slips for all satellites.
               for( int i=numVar; i<numUnknowns; i++ )
               {
                  currentErrorCov(i,i) = 4.0e14;     // (20000 km)**2
               }
                  
            }
            else  // update as common
            {

                  // Firstly, fill the state and covariance matrix for 
                  // source-indexed variables
               for( int i=0; i<numVar; i++ )
               {
                  currentState(i) = solution(i);

                     // Now, fills covariance matrix
                  for( int j=0; j<numVar; j++ )
                  {
                     currentErrorCov(i,j) = covMatrix(i,j);
                  }
               }

                  // Then, Fill the state and covariance matrix for 
                  // satellite-indexed variables

               int c1(numVar);      // Set an index

               for( VariableSet::const_iterator itVar = varUnknowns.begin();
                    itVar != varUnknowns.end();
                    ++itVar )
               {
                  currentState(c1) = stateMap[ (*itVar) ];
                  ++c1;
               }

                  // Fill the covariance matrix

                  // We need a copy of 'varUnknowns'
               VariableSet tempSet( varUnknowns );

               c1 = numVar;         // Reset 'c1' index

               for( VariableSet::const_iterator itVar1 = varUnknowns.begin();
                    itVar1 != varUnknowns.end();
                    ++itVar1 )
               {
                        // Check if '(*itVar2)' belongs to 'covarianceMap'
                  if( covarianceMap.find( (*itVar1) ) != covarianceMap.end()  )
                  {
                        // Fill the diagonal element
                     currentErrorCov(c1, c1) 
                         = covarianceMap[ (*itVar1) ].satIndexedVarCov[ (*itVar1) ];
                  }
                  else
                  {
                     currentErrorCov(c1, c1) 
                         = (*itVar1).getInitialVariance();
                  }

                  int c2(c1+1);      // Set 'j' index

                     // Remove current Variable from 'tempSet'
                  tempSet.erase( (*itVar1) );

                  for( VariableSet::const_iterator itVar2 = tempSet.begin();
                       itVar2 != tempSet.end();
                       ++itVar2 )
                  {

                           // If it belongs, get element from 'covarianceMap'
                        currentErrorCov(c1, c2) =
                           currentErrorCov(c2, c1) =
                              covarianceMap[ (*itVar1) ].satIndexedVarCov[ (*itVar2) ];

                     ++c2;
                  }

                     // Now, Fill the covariance of source-indexed variables and
                     // satellite-indexed variables

                     // covariance matrix. This fills the lower left and upper
                     // right quadrants of covariance matrix
                  int c3(0);
                  for( TypeIDSet::const_iterator itType  = srcIndexedTypes.begin();
                       itType != srcIndexedTypes.end();
                       ++itType )
                  {
                        currentErrorCov(c1,c3) =  
                            currentErrorCov(c3,c1) = 
                               covarianceMap[ (*itVar1) ].srcIndexedVarCov[ (*itType) ];

                     ++c3;
                  }

                  ++c1;

               }  // End of for( VariableSet::const_iterator itVar1 = varUnknowns...'
            }


               // Reset Kalman filter to current state and covariance matrix
            xhat = currentState;
            P    = currentErrorCov;

         }  // End of 'if(firstTime)'

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

      return gData;

   }  // End of method 'SolverPPPAR::preCompute()'



      // Compute the solution of the given equations set.
      //
      // @param gData    Data object holding the data.
      //
      // \warning A typical kalman filter will be devided into 
      // two different phase, the prediction and the correction.
      // for some special application, for example, the 'integer'
      // satellite and receiver clock/bias estimation, the ambiguity
      // constraints are generated using the 'predicted' ambiguity 
      // values and its variances. 
      //
   gnssRinex& SolverPPPAR::Compute( gnssRinex& gData )
      throw(InvalidSolver)
   {

         // Call the TimeUpdate() of the kalman filter, which will predict the 
         // state vector and their covariance matrix
      TimeUpdate( phiMatrix, qMatrix );

         // Now, according to the float ambiguity, compute the single-difference
         // ambiguity and fix them to integers, and then apply the single-difference
         // integer ambiguity as contraints to the measurement equations
      AmbiguityConstr( gData );

         // Call the MeasUpdate() of the kalman filter, which will update the 
         // state vector and their covariance using new measurements.
      MeasUpdate( measVector, hMatrix, rMatrix );

         // Now, according to the float ambiguity, compute the single-difference
         // ambiguity and fix them to integers, and then apply the single-difference
         // integer ambiguity as contraints to the measurement equations
      AmbiguityFixing( gData );


         // Return  
      return gData;

   }  // End of method 'SolverPPPAR::Compute()'


    

      // Predict the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverPPPAR::TimeUpdate( const Matrix<double>& phiMatrix,
                                 const Matrix<double>& processNoiseCovariance )
      throw(InvalidSolver)
   {

         // Check whether the phiMatrix is square
      if (!(phiMatrix.isSquare()))
      {
         InvalidSolver e("phiMatrix is not square");
         GPSTK_THROW(e);
      }


      int stateSize(xhat.size());
      if(!(stateSize==numUnknowns))
      {
         InvalidSolver e("TimeUpdate(): Size of a posteriori state estimation vector \
do not match the number of unknowns");
         GPSTK_THROW(e);
      }


      int phiRow = static_cast<int>(phiMatrix.rows());
      if (!(phiRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of phiMatrix");
         GPSTK_THROW(e);
      }

      if (!(qMatrix.isSquare()))
      {
         InvalidSolver e("qMatrix is not square");
         GPSTK_THROW(e);
      }

      int qRow = static_cast<int>(qMatrix.rows());
      if (!(qRow==numUnknowns))
      {
         InvalidSolver e("Number of unknowns does not match dimension \
of qMatrix");
         GPSTK_THROW(e);
      }

      try
      {
            // Compute the a priori state vector
         xhatminus = phiMatrix*xhat;

         Matrix<double> phiT(transpose(phiMatrix));

            // Compute the a priori estimate error covariance matrix
         Pminus = phiMatrix*P*phiT + processNoiseCovariance;

      }
      catch(...)
      {
         InvalidSolver e("TimeUpdate(): Unable to predict next state.");
         GPSTK_THROW(e);
         return -1;
      }
        
      return 0;

   }  // End of method 'SolverPPPAR::TimeUpdate()'


   gnssRinex& SolverPPPAR::AmbiguityConstr( gnssRinex& gData )
      throw(ProcessingException)
   {
      try
      {

            // Firstly,  get the float ambiguities and corresponding covariance 
            // from 'predicted' state and covariance matrix.
         std::map<SatID, double> ambLCMap;
         std::map<SatID, double> ambWLMap;

            // Covariance Map
         std::map<SatID, std::map< SatID, double > > covAmbLCMap;
         std::map<SatID, std::map< SatID, double > > covAmbWLMap;
            
            // Now, get the ambiguity on LC
         int c1(0);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Ambiguity value
            ambLCMap[ (*itSat) ] = xhatminus( numVar + c1 );
            ambWLMap[ (*itSat) ] = xhatminus( numVar + numCurrentSV + c1 );

               // Ambiguity variance
            covAmbLCMap[ (*itSat) ][ (*itSat) ]
                = Pminus( numVar + c1, numVar + c1); 
            covAmbWLMap[ (*itSat) ][ (*itSat) ]
                = Pminus( numVar + numCurrentSV + c1, numVar + numCurrentSV + c1); 

               // Increment
            ++c1;

         }

            /**
             *  Ambiguity Datum Selection.
             */

         double f1 = 1575.42e6;
         double f2 = 1227.60e6;
         double cw = f2/(f1-f2);

            // Map holding the information regarding every satellite
         std::map<SatID, double> ambWLFixedMap;

            //
            // Firstly, Let's get the fixed widelane ambiguities
            //

            // Warning: 
         datumWL.Reset( ambWLMap, covAmbWLMap);

            // Now, Let's 'find' the datum satellite 
         datumWL.Prepare( gData );

            // Get the datum
         ambWLFixedMap = datumWL.getAmbFixedMap();

            /**
             *  Get all the widelane that can be fixed, which will
             *  be used as candidates for narrow-lane ambiguity fixing
             */

            // Get a set with all satellites present in this GDS
         currSatSet = gData.body.getSatID();

            // Cutting decision to fix the ambiguities
         double cutDec(1000.0);

            // Ambiguity Fixing method
         ARRound ambRes(1000,0.3,0.3);

            // Ambiguity and covariance for L1 
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Ambiguity value
            double bw = ambWLMap[(*itSat)];

               // Sigma 
            double bwSig = std::sqrt( covAmbWLMap[(*itSat)][(*itSat)] );

               // Ambiguity fixing decision
            double decision = ambRes.getDecision(bw, bwSig);

               // Look for the largest fixing decision
            if( decision > cutDec )
            {
               ambWLFixedMap[(*itSat)] =  std::floor( bw + 0.5 );
            }

         }

            //
            // Secondly, Let's get the fixed narrowlane ambiguities
            //
            
            // Compute the BLC with B1/BWL/NWL.
         std::map<SatID, double> ambL1Map;
         std::map<SatID, std::map<SatID, double> > covAmbL1Map;

            // Ambiguity and covariance for L1 
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {

               // If the widelane of this satellite is fixed already.
            if( ambWLFixedMap.find( (*itSat) ) != ambWLFixedMap.end()  )
            {
                   // ambiguity
                ambL1Map[(*itSat)] 
                    = ambLCMap[(*itSat)] - cw*ambWLFixedMap[(*itSat)];

                   // Covariance
                covAmbL1Map[(*itSat)][(*itSat)]
                    = covAmbLCMap[(*itSat)][(*itSat)];

            }
         }


            // Fixed ambiguities on L1
         std::map<SatID, double> ambL1FixedMap;
  
            // Warning: 
         datumNL.Reset( ambL1Map, covAmbL1Map);

            // Now, Let's 'find' the datum satellite 
         datumNL.Prepare( gData );

            // Get the datum
         ambL1FixedMap = datumNL.getAmbFixedMap();
  
            //
            // Now, get the reference satellite
            //
            
            // Let's get the first satellite
         std::map<SatID, double>::iterator itMap = ambL1FixedMap.begin();

            // The reference satellite
         SatID refSat = (*itMap).first;

            // Widelane ambiguity datum
         std::map<SatID, double> datumWLMap;
         datumWLMap[refSat] = ambWLFixedMap[refSat] ;
  
            // LC ambiguity datum
         std::map<SatID, double> datumLCMap;
         datumLCMap[refSat] = ambL1FixedMap[refSat] + cw*ambWLFixedMap[refSat];


           /**
            *  Ambiguity Constraint Equation
            */

            // Number of Fixed widelane ambiguities
         int numBWL( datumWLMap.size() );
        
            // Equation matrix
         Vector<double> mVecBWL(numBWL, 0.0);
         Matrix<double> hMatBWL(numBWL, numUnknowns, 0.0);
         Matrix<double> rMatBWL(numBWL, numBWL, 0.0);
          
            // Now, insert the fixed ambiguities to the equation system to form
            // the new measVector/hMatrix/rMatrix.
         int rowBWL(0);
         for( std::map<SatID, double>::iterator it = datumWLMap.begin();
              it != datumWLMap.end();
              ++it)
         {
                // Find the position of current satellite.
             int jsat(0);
             SatIDSet::const_iterator itSat2( currSatSet.begin() );
             while( (*itSat2) != (*it).first )
             {
                 ++jsat;
                 ++itSat2;
             }

                // Position of this satellite 
             int colBWL = numVar + numCurrentSV + jsat;

                // Measurement
             mVecBWL( rowBWL ) = (*it).second;

                // coefficient
             hMatBWL( rowBWL, colBWL) = 1.0;

                // weight
             rMatBWL( rowBWL, rowBWL) = 1.0E+10;

                // row increment
             rowBWL++;

         }

            
         int numBLC( datumLCMap.size());

            // Equation matrix
         Vector<double> mVecBLC(numBLC, 0.0);
         Matrix<double> hMatBLC(numBLC, numUnknowns, 0.0);
         Matrix<double> rMatBLC(numBLC, numBLC, 0.0);

         int rowBLC(0);
         for( std::map<SatID, double>::iterator it = datumLCMap.begin();
              it != datumLCMap.end();
              ++it)
         {
                // Find the position of current satellite.
             int jsat(0);
             SatIDSet::const_iterator itSat2( currSatSet.begin() );
             while( (*itSat2) != (*it).first )
             {
                 ++jsat;
                 ++itSat2;
             }

                // Position of this satellite 
             int colBLC = numVar + jsat;

                // Measurement
             mVecBLC( rowBLC ) = (*it).second;

                // coefficient
             hMatBLC( rowBLC, colBLC) = 1.0;

                // weight
             rMatBLC( rowBLC, rowBLC) = 1.0E+10;

                // row increment
             rowBLC++;
         }


            // The observation equation number is enlarged
         int numEqu( numMeas + numBWL + numBLC );

            // New observation prefit/geometry/weight matrix
         Vector<double> tempPrefit(numEqu, 0.0);
         Matrix<double> tempGeometry(numEqu, numUnknowns, 0.0);
         Matrix<double> tempWeight(numEqu, numEqu, 0.0);

            /**
             * Firstly, copy the measVector/hMatrix/rMatrix to temporary store
             */

            // Now, Let's store the measVector/hMatrix/rMatrix
         for(int i=0; i<numMeas; i++)
         {
               // Prefit
            tempPrefit(i) = measVector(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++) 
            { 
               tempGeometry(i,j) = hMatrix(i,j); 
            }

               // Weight
            tempWeight(i,i) = rMatrix(i,i); 

         }


            // Now, Let's store the bw 
         int rowStart(numMeas) ;
         for(int i=0; i<numBWL; i++)
         {
               // Prefit
            tempPrefit(rowStart+i) = mVecBWL(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++)
            {
               tempGeometry(rowStart+i,j) = hMatBWL(i,j); 
            }  
               // Weight
            tempWeight(rowStart+i,rowStart+i)  = rMatBWL(i,i); 
         }

            // Now, Let's store the mVecBLC/hMatBLC/rMatBLC
         rowStart = (numMeas + numBWL) ;
         for(int i=0; i<numBLC; i++)
         {
               // Prefit
            tempPrefit(rowStart+i) = mVecBLC(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++)
            {
               tempGeometry(rowStart+i,j) = hMatBLC(i,j); 
            }  
               // Weight
            tempWeight(rowStart+i,rowStart+i)  = rMatBLC(i,i); 
         }

            // Resize the measVector/hMatrix/rMatrix
         measVector.resize(numEqu,  0.0);
         hMatrix.resize(numEqu, numUnknowns, 0.0);
         rMatrix.resize(numEqu, numEqu, 0.0);

            // New Measurement vector update
         measVector = tempPrefit ;
         hMatrix = tempGeometry ;
         rMatrix = tempWeight ;

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

   }  // End of method 'SolverPPPAR::AmbiguityConstr()'



      // Now, fix all the ambiguities to integers 
   gnssRinex& SolverPPPAR::AmbiguityFixing( gnssRinex& gData )
      throw(ProcessingException)
   {

      try
      {
            // Firsly, store the solution/covMatrix into 'newState/newCov'
         newState = solution;
         newCov = covMatrix;

         double sod( (gData.header.epoch).getSecondOfDay() );

            // Flags indicating whether this unknown variable has been fixed.
         Vector<double> stateFlag(numUnknowns, 0.0);

            // Get the elevation from 'gData'
         Vector<double> elev(gData.getVectorOfTypeID(TypeID::elevation));

            // Get a set with all satellites present in this GDS
         currSatSet = gData.body.getSatID();

         double f1 = 1575.42e6;
         double f2 = 1227.60e6;
         double cw = f2/(f1-f2);

            //
            // 'BootStrapping' method
            //
         if(ARMethod == "BootStrapping" )
         {
               // Cutting decision to fix the ambiguities
            double cutDec(1000.0);

               // Ambiguity Fixing method
            ARRound ambRes(1000,0.2,0.2);

               // Loop to fix a ambiguity
            numFixedBWL = 0;

               // Still have ambiguity to be fixed? 
            bool done(false); 

               // Now, fix the widelane ambiguities
            while( !done )
            {
                  // Indicator variable
               int index(-1);
               double maxDec(-100.0);

                  // Only fix the widelane ambiguities
               for( int i=(numVar+numCurrentSV); i<numUnknowns; i++)
               {
                     // If still free 
                  if( stateFlag(i) == 0.0 )
                  {
                        // Ambiguity value
                     double bw = newState(i);

                        // Sigma 
                     double bwSig = std::sqrt( newCov(i,i) );

                        // Ambiguity fixing decision
                     double decision = ambRes.getDecision(bw, bwSig);

                        // Look for the largest fixing decision
                     if( decision > maxDec )
                     {
                        index = i;
                        maxDec = decision;
                     }

                  }  // End of 'if( ambWLFlag(i) == 0.0 ) '

               }   // End of 'for(int i=0; ... )'

                  // If found 
               if(index != -1)
               {
                     // If can be fixed
                  if( maxDec > cutDec )
                  {
                        // Ambiguity value
                     double bw = newState(index);

                        // Fixed value of bw
                     double bwFixed = std::floor( bw + 0.5 );

                        // Update the solution and covarinace  
                     AmbiguityUpdate(newState, newCov, stateFlag, index, bwFixed );

                        // Not free at all
                     stateFlag(index) = 1.0;

                        // Store the fixed ambiguity value into 'state'
                     newState(index) = bwFixed;

                        // Update the fixed number
                     numFixedBWL = numFixedBWL + 1;

                  }
                  else
                  {
                       // If ambiguity can't be fixed
                     done = true;
                  } 

               }
               else
               {
                    // if no ambiguity is found
                  done = true;
               }
                   
            }  // End of 'while(done)'

               /**
                * Then, fix the BLC ambiguities
                */
            ARRound ambRes2(1000,0.25,0.25);

               // Initialize
            numFixedBL1 = 0;

               // Still have ambiguity to be fixed?
            done = false;
            while( !done )
            {
                  // Indicator variable
               int index(-1);
               double maxDec(-100.0);

                  // Only fix the narrowlane ambiguities
               for( int i=(numVar); i<(numVar+numCurrentSV); i++)
               {
                     // If the widelane ambiguity is fixed and the narrowlane 
                     // ambiguity is still free.
                  if( (stateFlag(i+numCurrentSV) == 1.0) && 
                      (stateFlag(i) == 0.0) )
                  {
                        // Widelane ambiguity
                     double bw = newState(i+numCurrentSV) ;

                        // LC ambiguity
                     double bc = newState(i);

                        // Narrowlane ambiguity
                     double b1 = (bc - cw*bw);

                        // Sigma of narrowlane ambiguity
                     double b1Sig = std::sqrt( newCov(i,i) );

                        // Ambiguity fixing decision
                     double decision = ambRes2.getDecision(b1, b1Sig);

                        // Look for the largest fixing decision
                     if( decision > maxDec )
                     {
                        index = i;
                        maxDec = decision;
                     }

                  }  // End of 'if( ambNLFlag(i) == 0.0 ) '

               }   // End of 'for(int i=numVar; ... )'

                  // If found 
               if(index != -1)
               {
                     // If can be fixed
                  if( maxDec > cutDec )
                  {
                        // Widelane ambiguity
                     double bw = newState(index+numCurrentSV) ;

                        // LC ambiguity
                     double bc = newState(index);

                        // Narrowlane ambiguity
                     double b1 = (bc - cw*bw);

                        // Fixed value of bw
                     double b1Fixed = std::floor( b1 + 0.5 );

                        // Compute the fixed LC ambiguity 
                     double bcFixed = b1Fixed + cw*bw;

                        // Update the solution and covarinace  
                     AmbiguityUpdate(newState, newCov, stateFlag, index, bcFixed);

                        // Not free at all
                     stateFlag(index) = 1.0;

                        // Store the fixed ambiguity value into 'newState'
                     newState(index) = bcFixed;

                        // update
                     numFixedBL1 = numFixedBL1 + 1;

                  }
                  else
                  {
                       // If ambiguity can't be fixed
                     done = true;
                  } 

               }
               else
               {
                    // if no ambiguity is found
                  done = true;
               }
                   
            }  // End of 'while(done)'

         }
         else if (ARMethod == "LAMBDA" ) // 'LAMBDA' method
         {
               // Initialize
            numFixedBWL = 0; 
            numFixedBL1 = 0; 

               // Firstly, Let's get the estimate and covariance 
               // from the state
            std::map<SatID, double> ambWLMap;
            std::map<SatID, std::map< SatID, double > > covAmbWLMap;
            std::map<SatID, double> ambWLFixedMap;

            std::map<SatID, double> ambL1Map;
            std::map<SatID, std::map< SatID, double > > covAmbL1Map;
            std::map<SatID, double> ambL1FixedMap;

            std::map<SatID, double> ambLCMap;
            std::map<SatID, std::map< SatID, double > > covAmbLCMap;
            std::map<SatID, double> ambLCFixedMap;

               // Widelane ambiguity 
            int c1(0);     
            for( SatIDSet::const_iterator itSat = currSatSet.begin();
                 itSat != currSatSet.end();
                 ++itSat )
            {
               ambWLMap[ (*itSat) ] = newState( numVar + numCurrentSV + c1 );
               c1++;
            }

               // WL ambiguity covariance
               // we need a copy of 'currSatSet'
            SatIDSet tempSatSet(currSatSet);
            c1 = 0;     
            for( SatIDSet::const_iterator itSat = currSatSet.begin();
                 itSat != currSatSet.end();
                 ++itSat )
            {
               covAmbWLMap[(*itSat)][(*itSat)] 
                   = newCov(numVar+numCurrentSV+c1, numVar+numCurrentSV+c1);

               int c2 = (c1+1);

                  // Erase current satellite from 'tempSatSet'
               tempSatSet.erase(*itSat);

               for( SatIDSet::const_iterator itSat2 = tempSatSet.begin();
                    itSat2 != tempSatSet.end();
                    ++itSat2 )
               {
                   covAmbWLMap[(*itSat)][(*itSat2)] 
                       = newCov(numVar+numCurrentSV+c1, numVar+numCurrentSV+c2);
                   c2++;
               }
                
                  // Increment
               c1++;

            }

               // Sat set to indicate the valid satellite
            SatIDSet validSatSet(currSatSet);

               // MLAMBDA
            ARMLambda mlambda;

               // Ratio for WL ambiguity fixing
            double ratioWL(0.0);

               // Single ambiguity fixing method
            ARRound ambRes(0.0, 0.3, 0.4);

               // min decisioin
            double lastMinDec(9.0E+10);

               // If the satellite number to be fixed is greater than 4.
            int iter(0);
            while( validSatSet.size()>1 )
            {
                   // Number of valid satellite
                int numSV = validSatSet.size();

                Vector<double> tempAmb(numSV, 0.0);
                Matrix<double> tempCov(numSV, numSV, 0.0);

                   // Now, fill the 'tempAmb'
                int c1(0);
                for( SatIDSet::iterator itSat=validSatSet.begin();
                     itSat !=validSatSet.end();
                     ++itSat )
                {
                    tempAmb(c1) = ambWLMap[(*itSat)] ;
                    c1++;
                }

                   // Now, fill the covariance 
                SatIDSet tempSatSet(validSatSet);

                c1=0;
                for( SatIDSet::iterator itSat=validSatSet.begin();
                     itSat !=validSatSet.end();
                     ++itSat )
                {
                        // The diagonal element
                    tempCov(c1,c1) = covAmbWLMap[(*itSat)][(*itSat)] ;

                    int c2(c1+1);

                       // Erase current satellite
                    tempSatSet.erase(*itSat);
                    for( SatIDSet::iterator itSat2=tempSatSet.begin();
                         itSat2 !=tempSatSet.end();
                         ++itSat2 )
                    {
                        tempCov(c1,c2) =
                            tempCov(c2,c1) =
                                covAmbWLMap[(*itSat)][(*itSat2)];
                        c2++;
                    }
                    c1++;
                }
                
                   // Fill 'mlambda' with 'tempAmb/tempCov'
                mlambda.resolve(tempAmb, tempCov);
                ratioWL = mlambda.getRatio();

                   // Get fixed ambiguity vector
                Vector<double> ambFixedVec = mlambda.getFixedAmbVec();

                   // Fixing ratio
                if( (ratioWL>3.0) && (lastMinDec > 0.0))
                {
                       // Store the fixed ambiguities into 'ambWLFixedMap'
                    int c1(0);
                    for( SatIDSet::iterator itSat=validSatSet.begin();
                         itSat !=validSatSet.end();
                         ++itSat )
                    {
                        ambWLFixedMap[(*itSat)] = ambFixedVec(c1);
                        c1++;
                    }

                    break;
                }
                else
                {
                       // Satellite with minimum fixing decision
                    SatID tempSat;

                       // Minimum decision for this iteration
                    double tempMinDec(9.0E+10);

                       // Temporary satellite
                    int c1(0);
                    for( SatIDSet::iterator itSat=validSatSet.begin();
                         itSat !=validSatSet.end();
                         ++itSat )
                    {
                           // Value
                        double bw = ambWLMap[(*itSat)];

                           // Sigma
                        double sig = std::sqrt( covAmbWLMap[(*itSat)][(*itSat)] );

                           // Get the fixing decision
                        double decision = ambRes.getDecision(bw, sig);

                           // Get the minimum decision
                        if( decision < tempMinDec )
                        {
                            tempMinDec = decision;
                            tempSat = (*itSat);
                        }
                    }

                    lastMinDec = tempMinDec;

                       // remove this satellite from validSatSet 
                    validSatSet.erase(tempSat);

                }

                  // Iter increment
                iter++;

            };  // End of 'while(validSatSet.size() >1)'

               // LC ambiguity estimate
            c1 = 0;     
            for( SatIDSet::const_iterator itSat = currSatSet.begin();
                 itSat != currSatSet.end();
                 ++itSat )
            {
               ambLCMap[ (*itSat) ] = newState( numVar + c1 );
               c1++;

            }
               // LC ambiguity covariance
               //
               // we need a copy of 'currSatSet'
            tempSatSet = currSatSet;
               
            c1 = 0;     
            for( SatIDSet::const_iterator itSat = currSatSet.begin();
                 itSat != currSatSet.end();
                 ++itSat )
            {
               covAmbLCMap[(*itSat)][(*itSat)] = newCov(numVar+c1, numVar+c1);

               int c2 = (c1+1);

                  // Erase current satellite from 'tempSatSet'
               tempSatSet.erase(*itSat);

               for( SatIDSet::const_iterator itSat2 = tempSatSet.begin();
                    itSat2 != tempSatSet.end();
                    ++itSat2 )
               {
                   covAmbLCMap[(*itSat)][(*itSat2)] 
                      = newCov(numVar+c1, numVar+c2);
               
                   c2++;
               }
                  // Increment
               c1++;
            }

               // Update the solution/state with fixed widelane ambiguities
            if( ratioWL > 3.0 )
            {
                  // Loop the validSatSet
               for( SatIDSet::iterator itSat=validSatSet.begin();
                    itSat !=validSatSet.end();
                    ++itSat )
               {
                     // Fixed widelane ambiguity
                  double bwFixed = ambWLFixedMap[(*itSat)];

                     // Find the position of current satellite.
                  int jsat(0);
                  SatIDSet::const_iterator itSat2( currSatSet.begin() );
                  while( (*itSat2) != (*itSat) )
                  {
                      ++jsat;
                      ++itSat2;
                  }

                     // Index of the WL ambiguity
                  int index = numVar+numCurrentSV+jsat;

                     // Update the solution and covarinace  
                  AmbiguityUpdate(newState, newCov, stateFlag, index, bwFixed );

                     // Not free at all
                  stateFlag(index) = 1.0;

               }

                  // Update the number of the fixed widelane ambiguities
               numFixedBWL = validSatSet.size();

            }

               // Now, if the widelane ambiguities can be fixed 
            if( ratioWL>3.0 )
            {

                  // Now, fill the 'ambL1Map/covAmbL1Map'
               int c1(0);
               for( SatIDSet::iterator itSat=validSatSet.begin();
                    itSat !=validSatSet.end();
                    ++itSat )
               {
                   ambL1Map[(*itSat)] 
                       = ambLCMap[(*itSat)] - cw*ambWLFixedMap[(*itSat)];

                   c1++;
               }

                  // Now, fill the covariance 
               SatIDSet tempSatSet(validSatSet);

               c1=0;
               for( SatIDSet::iterator itSat=validSatSet.begin();
                    itSat !=validSatSet.end();
                    ++itSat )
               {
                       // The diagonal element
                   covAmbL1Map[(*itSat)][(*itSat)] 
                       = covAmbLCMap[(*itSat)][(*itSat)] ;

                   int c2(c1+1);

                      // Erase current satellite
                   tempSatSet.erase(*itSat);
                   for( SatIDSet::iterator itSat2=tempSatSet.begin();
                        itSat2 !=tempSatSet.end();
                        ++itSat2 )
                   {
                       covAmbL1Map[(*itSat)][(*itSat2)]
                           = covAmbLCMap[(*itSat)][(*itSat2)];

                       c2++;
                   }

                   c1++;

               }  // End of 'for( SatIDSet::iterator itSat=validSatSet.begin(); )'


                  // min decisioin
               double lastMinDec(9.0E+10);
               double ratioL1(0.0);

               int iter(0);
               while( validSatSet.size()>1 )
               {
                      // Number of valid satellite
                   int numSV = validSatSet.size();

                   Vector<double> tempAmb(numSV, 0.0);
                   Matrix<double> tempCov(numSV, numSV, 0.0);

                      // Now, fill the 'tempAmb'
                   int c1(0);
                   for( SatIDSet::iterator itSat=validSatSet.begin();
                        itSat !=validSatSet.end();
                        ++itSat )
                   {
                       tempAmb(c1) = ambL1Map[(*itSat)] ;

                       c1++;
                   }

                      // Now, fill the covariance 
                   SatIDSet tempSatSet(validSatSet);

                   c1=0;
                   for( SatIDSet::iterator itSat=validSatSet.begin();
                        itSat !=validSatSet.end();
                        ++itSat )
                   {
                           // The diagonal element
                       tempCov(c1,c1) = covAmbL1Map[(*itSat)][(*itSat)] ;

                       int c2(c1+1);

                          // Erase current satellite
                       tempSatSet.erase(*itSat);
                       for( SatIDSet::iterator itSat2=tempSatSet.begin();
                            itSat2 !=tempSatSet.end();
                            ++itSat2 )
                       {
                           tempCov(c1,c2) =
                               tempCov(c2,c1) =
                                   covAmbL1Map[(*itSat)][(*itSat2)];

                           c2++;
                       }

                       c1++;
                   }
                   
                      // Fill 'mlambda' with 'tempAmb/tempCov'
                   mlambda.resolve(tempAmb, tempCov);
                   ratioL1 = mlambda.getRatio();

                      // Get fixed ambiguity vector
                   Vector<double> ambL1Fixed = mlambda.getFixedAmbVec();

                      // Fixing ratio
                   if( (ratioL1>3.0) && (lastMinDec > 0.0))
                   {
                          // Store the fixed ambiguities into 'ambWLFixedMap'
                       int c1(0);
                       for( SatIDSet::iterator itSat=validSatSet.begin();
                            itSat !=validSatSet.end();
                            ++itSat )
                       {
                              // Fixed L1 ambiguities
                           ambL1FixedMap[(*itSat)] = ambL1Fixed(c1);

                              // Fixed LC ambiguities
                           ambLCFixedMap[(*itSat)]
                               = ambL1FixedMap[(*itSat)] + cw*ambWLFixedMap[(*itSat)];

                           c1++;
                       }

                       break;
                   }
                   else
                   {
                          // Satellite with minimum fixing decision
                       SatID tempSat;

                          // Minimum decision for this iteration
                       double tempMinDec(9.0E+10);

                          // Temporary satellite
                       int c1(0);
                       for( SatIDSet::iterator itSat=validSatSet.begin();
                            itSat !=validSatSet.end();
                            ++itSat )
                       {
                              // Value
                           double b1 = ambL1Map[(*itSat)];

                              // Sigma
                           double sig = std::sqrt( covAmbL1Map[(*itSat)][(*itSat)] );

                              // Get the fixing decision
                           double decision = ambRes.getDecision(b1, sig);

                              // Get the minimum decision
                           if( decision < tempMinDec )
                           {
                               tempMinDec = decision;
                               tempSat = (*itSat);
                           }
                       }

                       lastMinDec = tempMinDec;

                          // remove this satellite from validSatSet 
                       validSatSet.erase(tempSat);
                   }

                     // Iter increment
                   iter++;

               };

                   // Update the solution/state with fixed LC ambiguities
               if(ratioL1>3.0)
               {
                     // Loop the validSatSet
                  for( SatIDSet::iterator itSat=validSatSet.begin();
                       itSat !=validSatSet.end();
                       ++itSat )
                  {

                        // Fixed LC ambiguity
                     double bcFixed = ambLCFixedMap[(*itSat)];

                        // Find the position of current satellite.
                     int jsat(0);
                     SatIDSet::const_iterator itSat2( currSatSet.begin() );
                     while( (*itSat2) != (*itSat) )
                     {
                         ++jsat;
                         ++itSat2;
                     }

                        // Index for LC ambiguity
                     int index = numVar+jsat;

                        // Update the solution and covarinace  
                     AmbiguityUpdate(newState, newCov, stateFlag, index, bcFixed );

                        // Not free at all
                     stateFlag(index) = 1.0;
                  }

                     // Update the number of the fixed widelane ambiguities
                  numFixedBL1 = validSatSet.size();

               }  // End of 'if(ratioL1 > 3.0)'

            }  // End of 'if(ratioWL > 3.0)'

         }

///////////////////////////////////////////////////////
//
//       PartialAR par;       
//
//       par(x,CovX,varUnknowns,partialVar);
//
//       par.setSolution();
//       par.setCovariance();
//       par.setTotalVars();
//       par.setPartialVars();
//
//       PartialAR(x, CovX, varUnknowns, partialVar );
//
///////////////////////////////////////////////////////
            
             //
             // Insert ambiguity fixed flags
             //
         Vector<double> ambWLFixed(numCurrentSV, 0.0);
         Vector<double> ambWLFlag(numCurrentSV,0.0);

             // Now, get the WL ambiguity 
         for(int i=0; i<numCurrentSV; i++)
         {
            ambWLFixed(i) = newState(numVar+numCurrentSV+i);
            ambWLFlag(i)  = stateFlag(numVar+numCurrentSV+i);;
         }
         
         Vector<double> ambLCFixed(numCurrentSV, 0.0);
         Vector<double> ambLCFlag(numCurrentSV,0.0);

             // Now, get the LC ambiguity 
         for(int i=0; i<numCurrentSV; i++)
         {
            ambLCFixed(i) = newState(numVar+i);
            ambLCFlag(i)  = stateFlag(numVar+i);;
         }

         Vector<double> ambL1Fixed(numCurrentSV, 0.0);
         Vector<double> ambL1Flag(numCurrentSV,0.0);

            // Now, get the L1 ambiguity 
         for(int i=0; i<numCurrentSV; i++)
         {
            ambL1Fixed(i) = ambLCFixed(i) - cw*ambWLFixed(i);
            ambL1Flag(i)  = ambLCFlag(i);
         }

         Vector<double> ambL2Fixed(numCurrentSV, 0.0);
         Vector<double> ambL2Flag(numCurrentSV,0.0);

            // Now, get the L1 ambiguity 
         for(int i=0; i<numCurrentSV; i++)
         {
            ambL2Fixed(i) = ambL1Fixed(i) - ambWLFixed(i);
            ambL2Flag(i)  = ambL1Flag(i);
         }

         if(debugLevel)
         {
            cout << "ambWLFlag" << endl;
            cout <<  ambWLFlag  << endl;
            cout << "ambWLFixed" << endl;
            cout <<  ambWLFixed  << endl;

            cout << "ambL1Flag" << endl;
            cout <<  ambL1Flag  << endl;
            cout << "ambL1Fixed" << endl;
            cout <<  ambL1Fixed  << endl;
         }


            //
            // Compute the corrections for RTK positioning
            //
            
         Vector<double> prefitC(gData.getVectorOfTypeID(TypeID::prefitC));
         Vector<double> prefitPdelta(gData.getVectorOfTypeID(TypeID::prefitPdelta));
         Vector<double> prefitL(gData.getVectorOfTypeID(TypeID::prefitL));
         Vector<double> prefitLdelta(gData.getVectorOfTypeID(TypeID::prefitLdelta));

         Vector<double> prefitC1(gData.getVectorOfTypeID(TypeID::prefitC1));
         Vector<double> prefitP1(gData.getVectorOfTypeID(TypeID::prefitP1));
         Vector<double> prefitP2(gData.getVectorOfTypeID(TypeID::prefitP2));
         Vector<double> prefitL1(gData.getVectorOfTypeID(TypeID::prefitL1));
         Vector<double> prefitL2(gData.getVectorOfTypeID(TypeID::prefitL2));


         Vector<double> corrPC(numCurrentSV, 0.0);
         Vector<double> corrPdelta(numCurrentSV, 0.0);
         Vector<double> corrLC(numCurrentSV, 0.0);
         Vector<double> corrLdelta(numCurrentSV, 0.0);

         Vector<double> corrC1(numCurrentSV, 0.0);
         Vector<double> corrP1(numCurrentSV, 0.0);
         Vector<double> corrP2(numCurrentSV, 0.0);
         Vector<double> corrL1(numCurrentSV, 0.0);
         Vector<double> corrL2(numCurrentSV, 0.0);

            // Correct ambiguities for 'prefitLdelta'
         corrPC        = prefitC ;
         corrPdelta    = prefitPdelta ;
         for(int i=0; i<numCurrentSV; i++)
         {
            corrLC(i)     = prefitL(i) + 0.106953378142*ambLCFixed(i);
            corrLdelta(i) = prefitLdelta(i) + 0.861918400322*ambWLFixed(i);
         }

         corrC1 = prefitC1;
         corrP1 = prefitP1;
         corrP2 = prefitP2;
         for(int i=0; i<numCurrentSV; i++)
         {
            corrL1(i) = prefitL1(i) + 0.190293672798*ambL1Fixed(i);
            corrL2(i) = prefitL2(i) + 0.244210213425*ambL2Fixed(i);
         }

            // Warning: insert TypeID::prefitPdelta directly without any change
         gData.insertTypeIDVector(TypeID::corrPC, corrPC);
         gData.insertTypeIDVector(TypeID::corrPdelta, corrPdelta);
         gData.insertTypeIDVector(TypeID::corrLC, corrLC);
         gData.insertTypeIDVector(TypeID::corrLdelta, corrLdelta);

            // Now, get the prefit of C1 from 'gData'
         satTypeValueMap dummy(gData.body.extractTypeID(TypeID::prefitC1));
            // Check whether the C1 is used 
         if ( dummy.numSats() == numCurrentSV )
         {
            gData.insertTypeIDVector(TypeID::corrC1, corrC1);
         }
         else
         {
            gData.insertTypeIDVector(TypeID::corrP1, corrP1);
         }
         gData.insertTypeIDVector(TypeID::corrP2, corrP2);
         gData.insertTypeIDVector(TypeID::corrL1, corrL1);
         gData.insertTypeIDVector(TypeID::corrL2, corrL2);

         gData.insertTypeIDVector(TypeID::BWL, ambWLFixed);
         gData.insertTypeIDVector(TypeID::BL1, ambL1Fixed);
         gData.insertTypeIDVector(TypeID::BL2, ambL2Fixed);

            // Insert amb flags
         gData.insertTypeIDVector(TypeID::BWLFlag, ambWLFlag);
         gData.insertTypeIDVector(TypeID::BL1Flag, ambL1Flag);
         gData.insertTypeIDVector(TypeID::BL2Flag, ambL2Flag);

            ////////////////////////////////////////////////////
            //
            //  Get the statistics 
            //
            ////////////////////////////////////////////////////

         double dx = newState(1);
         double dy = newState(2);
         double dz = newState(3);
         double drou = std::sqrt(dx*dx+dy*dy+dz*dz);

         if(drou < 0.10)
         {
            convergBuffer.push_back(true);
         }
         else
         {
            convergBuffer.push_back(false);
         }

         double size = convergBuffer.size();

            // If the size is greater than the given value
         if(size > bufferSize)
         {
               // Get rid of oldest data, which is at the beginning of deque
            convergBuffer.pop_front();
         }

            // Update the size
         size = convergBuffer.size();

            // WL ambiguities are reset
         if(resetWL)
         {
            if( numFixedBWL>4 )
            {
               double ttff = sod - startTime;
               ttffWL.push_back(ttff);
               resetWL = false;
            }
         }

         if(resetL1)
         {
               // Now, Let's judge whehter the solution has converged
            if(size == bufferSize)
            {
               converged = convergBuffer[0]; 
               for(int i=1;i<size;i++)
               {
                  converged = converged && convergBuffer[i];
               }
            }

               // If already converged, then set the 'resetSol' as false.
            if( numFixedBL1>4 && converged)
            {
               double ttff = sod - startTime;
               ttffL1.push_back(ttff);
               resetL1 = false;
            }
         }


// ///////////// ///////////// ///////////// /////////////
//
//           // Set to store the sats to be removed
//       SatIDSet satRejectedSet;
//
//       int c1(0);
//       for( SatIDSet::const_iterator itSat = currSatSet.begin();
//            itSat != currSatSet.end();
//            ++itSat )
//       {
//             // If BWL or BL1 is not fixed, then the satellite will be removed.
//          if( (ambWLFlag(c1) != 1.0) || (ambL1Flag(c1) !=1.0 ) )
//          {
//              satRejectedSet.insert( (*itSat) );
//          }
//
//             // increment
//          ++c1;
//       }
//
//          // Remove the satellite 
//       gData.removeSatID(satRejectedSet);
//
//
// ///////////// ///////////// ///////////// /////////////
            
         ///////////// ///////////// ///////////// /////////////
         //
         // The postfit-residual check and test is to be done !!!!
         //
         ///////////// ///////////// ///////////// /////////////
  
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

   }  // End of method 'SolverPPPAR::AmbiguityFixing()'



      /** Update the free unknonws' solution and covariance after
       *  fixing the ambiguities
       */
   void SolverPPPAR::AmbiguityUpdate( Vector<double>& state,
                                        Matrix<double>& covariance,
                                        Vector<double>& fixedFlag,
                                        int& index,
                                        double& fixedValue )
      throw(ProcessingException)
   {

      try
      {
            // Variance epsilon to avoid numerical exception
         const double eps(1.0e-9);

            // Covariance related to the fixed ambiguity
         Vector<double> Qba(numUnknowns, 0.0);

            // Get the covariance
         for( int i=0; i<numUnknowns; i++)
         {
            Qba(i) = covariance(i,index);
         }

            // Qii
         double Qaa;
         Qaa = Qba(index) + eps;
         Qaa = 1.0/Qaa;

            // Free value
         double freeValue = state(index);

            // Solution udpate
         for(int i=0; i<numUnknowns; i++)
         {
            if( fixedFlag(i) == 0.0 )
            {
               state(i) = state(i) + Qba(i)*Qaa*(fixedValue - freeValue); 
            }
         }

            // Covariance update
         for(int i=0; i<numUnknowns; i++)
         {
            if( fixedFlag(i) == 0.0 )
            {
               for(int j=0; j<numUnknowns; j++)
               {
                  if( fixedFlag(j) == 0.0 )
                  {
                     covariance(i,j) = covariance(i,j) - Qba(i)*Qaa*Qba(j);
                  }
               }
            }
         }

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPAR::AmbiguityUpdate()'



      // Correct the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverPPPAR::MeasUpdate( const Vector<double>& prefitResiduals,
                                  const Matrix<double>& designMatrix,
                                  const Matrix<double>& weightMatrix )
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;

      if (!(weightMatrix.isSquare()))
      {
         InvalidSolver e("MeasUpdate(): Weight matrix is not square");
         GPSTK_THROW(e);
      }


      int wRow = static_cast<int>(weightMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(wRow==pRow))
      {
         InvalidSolver e("MeasUpdate(): prefitResiduals size does not match dimension of \
weightMatrix");
         GPSTK_THROW(e);
      }


      int gRow = static_cast<int>(designMatrix.rows());
      if (!(gRow==pRow))
      {
         InvalidSolver e("MeasUpdate(): prefitResiduals size does not match \
dimension of designMatrix");
         GPSTK_THROW(e);
      }


      int gCol = static_cast<int>(designMatrix.cols());
      int aprioriStateRow = static_cast<int>(xhatminus.size());
      if (!(gCol==aprioriStateRow))
      {
         InvalidSolver e("MeasUpdate(): designMatrix size and a priori state \
estimation vector do not match.");
         GPSTK_THROW(e);
      }


      int pMCol = static_cast<int>(Pminus.cols());
      if ( pMCol != aprioriStateRow )
      {
         InvalidSolver e("MeasUpdate(): Sizes of a priori error covariance \
matrix and a priori state estimation vector do not match.");
         GPSTK_THROW(e);
      }


         // After checking sizes, let's do the real correction work
      Matrix<double> invPMinus;
      Matrix<double> designMatrixT( transpose(designMatrix) );

      try
      {
         invPMinus = inverseChol(Pminus);
      }
      catch(...)
      {
         InvalidSolver e("MeasUpdate(): Unable to compute invPMinus matrix.");
         GPSTK_THROW(e);
         return -1;
      }

      try
      {

         Matrix<double> invTemp( designMatrixT * weightMatrix * designMatrix +
                                 invPMinus );

            // Compute the a posteriori error covariance matrix
         P = inverseChol( invTemp );

      }
      catch(...)
      {
         InvalidSolver e("MeasUpdate(): Unable to compute P matrix.");
         GPSTK_THROW(e);
         return -1;
      }

      try
      {

            // Compute the a posteriori state estimation
         xhat = P * ( (designMatrixT * weightMatrix * prefitResiduals) + 
                      (invPMinus * xhatminus) );

      }
      catch(Exception e)
      {
         InvalidSolver eis("MeasUpdate(): Unable to compute xhat.");
         GPSTK_THROW(eis);
         return -1;
      }

      solution = xhat;
      covMatrix = P;

         // Compute the postfit residuals Vector
      postfitResiduals = prefitResiduals - (designMatrix * solution);

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverPPPAR::MeasUpdate()'





      /* Code to be executed after 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverPPPAR::postCompute( gnssRinex& gData )
      throw(ProcessingException)
   {

      try
      {
            // Clean up values in 'stateMap' and 'covarianceMap'
         stateMap.clear();
         covarianceMap.clear();

            // Store values of current state

         int i(numVar);      // Set an index

         for( VariableSet::const_iterator itVar = varUnknowns.begin();
              itVar != varUnknowns.end();
              ++itVar )
         {

            stateMap[ (*itVar) ] = solution(i);
            ++i;
         }

            // Store values of covariance matrix

            // We need a copy of 'varUnknowns'
         VariableSet tempSet( varUnknowns );

         i = numVar;         // Reset 'i' index

         for( VariableSet::const_iterator itVar1 = varUnknowns.begin();
              itVar1 != varUnknowns.end();
              ++itVar1 )
         {

               // Fill the diagonal element
            covarianceMap[ (*itVar1) ].satIndexedVarCov[ (*itVar1) ] 
                                                        = covMatrix(i, i);

            int j(i+1);      // Set 'j' index

               // Remove current Variable from 'tempSet'
            tempSet.erase( (*itVar1) );

            for( VariableSet::const_iterator itVar2 = tempSet.begin();
                 itVar2 != tempSet.end();
                 ++itVar2 )
            {

               covarianceMap[ (*itVar1) ].satIndexedVarCov[ (*itVar2) ] 
                                                        = covMatrix(i, j);

               ++j;
            }

               // Store variables X ambiguities covariances
            int k(0);
            for( TypeIDSet::const_iterator itType = srcIndexedTypes.begin();
                 itType != srcIndexedTypes.end();
                 ++itType )
            {

               covarianceMap[(*itVar1)].srcIndexedVarCov[*itType] 
                                                        = covMatrix(i,k);

               ++k;
            }

            ++i;

         }  // End of for( VariableSet::const_iterator itVar1 = unkSet...'



      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + StringUtils::asString( getIndex() ) + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

      return gData;

   }  // End of method 'SolverPPPAR::postCompute()'



      /* Sets if a NEU system will be used.
       *
       * @param useNEU  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverPPPAR& SolverPPPAR::setNEU( bool useNEU )
   {

         //> Firstly, let's clear all the set that storing the variable types.
         //> or else, SolverPPPARFB will throw error!

      srcIndexedTypes.clear();
      TropAndCoord.clear();

         // Watch out here: 'srcIndexedTypes' is a 'std::set', and all sets order their
         // elements. According to 'TypeID' class, this is the proper order:
      srcIndexedTypes.insert(TypeID::wetMap);  // BEWARE: The first is wetMap!!!

      if (useNEU)
      {
         srcIndexedTypes.insert(TypeID::dLat); // #2
         srcIndexedTypes.insert(TypeID::dLon); // #3
         srcIndexedTypes.insert(TypeID::dH);   // #4
      }
      else
      {
         srcIndexedTypes.insert(TypeID::dx);   // #2
         srcIndexedTypes.insert(TypeID::dy);   // #3
         srcIndexedTypes.insert(TypeID::dz);   // #4
      }

      srcIndexedTypes.insert(TypeID::cdt); // #5
      srcIndexedTypes.insert(TypeID::cdtLC); // #6  
      srcIndexedTypes.insert(TypeID::cdtMW); // #7


         //>Fill the types that are satellite-indexed

         // The order of the elment is as follows:
      satIndexedTypes.insert(TypeID::BLC);    // #3
      satIndexedTypes.insert(TypeID::BWL);    // #4


         //>Fill the types that are common for all observables
      TropAndCoord.insert(TypeID::wetMap);

      if (useNEU)
      {
         TropAndCoord.insert(TypeID::dLat); // #2
         TropAndCoord.insert(TypeID::dLon); // #3
         TropAndCoord.insert(TypeID::dH);   // #4
      }
      else
      {
         TropAndCoord.insert(TypeID::dx);   // #2
         TropAndCoord.insert(TypeID::dy);   // #3
         TropAndCoord.insert(TypeID::dz);   // #4
      }

         // Now, we build the basic equation definition
      defaultEqDef.header = TypeID::prefitC;
      defaultEqDef.body = srcIndexedTypes;

      return (*this);

   }  // End of method 'SolverPPPAR::setNEU()'


      /* Set a single coordinates stochastic model to ALL coordinates.
       *
       * @param pModel      Pointer to StochasticModel associated with
       *                    coordinates.
       *
       * @warning Do NOT use this method to set the SAME state-aware
       * stochastic model (like RandomWalkModel, for instance) to ALL
       * coordinates, because the results will certainly be erroneous. Use
       * this method only with non-state-aware stochastic models like
       * 'StochasticModel' (constant coordinates) or 'WhiteNoiseModel'.
       */
   SolverPPPAR& SolverPPPAR::setCoordinatesModel( StochasticModel* pModel )
   {

         // All coordinates will have the same model
      pCoordXStoModel = pModel;
      pCoordYStoModel = pModel;
      pCoordZStoModel = pModel;

      return (*this);

   }  // End of method 'SolverPPPAR::setCoordinatesModel()'


      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       */
   double SolverPPPAR::getSolution(const TypeID& type) const
      throw(InvalidRequest)
   {

         // Define iterator
      TypeIDSet::const_iterator it;

         // Check if the provided type exists in the solution. If not,
         // an InvalidSolver exception will be issued.
      it = defaultEqDef.body.find(type);
      if( it == defaultEqDef.body.end() )
      {
         InvalidRequest e("Type not found in solution vector.");
         GPSTK_THROW(e);
      }


         // Define counter
      int counter(0);

         // Define a new iterator and count where the given type is
      TypeIDSet::const_iterator it2;
      for (it2 = defaultEqDef.body.begin(); it2!= it; it2++)
      {
         ++counter;
      }

      return newState(counter);

   }  // End of method 'SolverPPPAR::getSolution()'


      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       */
   double SolverPPPAR::getVariance(const TypeID& type) const
      throw(InvalidRequest)
   {

         // Define iterator
      TypeIDSet::const_iterator it;


         // Check if the provided type exists in the covariance matrix. If not,
         // an InvalidSolver exception will be issued.
      it = defaultEqDef.body.find(type);
      if( it == defaultEqDef.body.end() )
      {
         InvalidRequest e("Type not found in covariance matrix.");
         GPSTK_THROW(e);
      }


         // Define counter
      int counter(0);

         // Define a new iterator and count where the given type is
      TypeIDSet::const_iterator it2;
      for (it2 = defaultEqDef.body.begin(); it2!= it; it2++)
      {
         ++counter;
      }

      return newCov(counter,counter);

   }  // End of method 'SolverPPPAR::getVariance()'


      /** Return the CURRENT number of satellite.
       */
   int SolverPPPAR::getCurrentSatNumber() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numCurrentSV;
   }



      /** Return the CURRENT number of satellite.
       */
   int SolverPPPAR::getFixedAmbNumL1() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numFixedBL1;
   }

      /** Return the CURRENT number of satellite.
       */
   int SolverPPPAR::getFixedAmbNumWL() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numFixedBWL;
   }

      /** Return the CURRENT number of satellite.
       */
   bool SolverPPPAR::getConverged() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return converged;
   }



}  // End of namespace gpstk
