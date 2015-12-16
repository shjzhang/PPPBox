#pragma ident "$Id: SolverRTKUC.cpp 2645 2012-11-05 03:23:24Z shjzhang $"

/**
 * @file SolverRTKUC.cpp
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
//  2014/03/01      created this program to process the extended equations
//                  for PPP, which are P1/P2/L1/L2/PW/LW. 
//
//  2014/06/01      created this program to process the extended equations
//                  for PPP, which are P3/L3/PW/LW. 
//
//  2014/07/03      Let's get and set the tropo/Iono constraints  
//                  
//
//============================================================================


#include "SolverRTKUC.hpp"
#include "MatrixFunctors.hpp"
#include "ARMLambda.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverRTKUC::classIndex = 9300000;


      // Returns an index identifying this object.
   int SolverRTKUC::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverRTKUC::getClassName() const
   { return "SolverRTKUC"; }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverRTKUC::SolverRTKUC(bool useNEU)
      : initialTropVar(2500.0), initialIonoVar(0.25), firstTime(true) 

   {

         // Set the equation system structure
      setNEU(useNEU);

         // Set the class index
      setIndex();

         // Call initializing method
      Init();

   }  // End of 'SolverRTKUC::SolverRTKUC()'



      // Initializing method.
   void SolverRTKUC::Init(void)
   {

         // Pointer to default stochastic model for troposphere (random walk)
      pTropoStoModel = &tropoModel;

         // Set default coordinates stochastic model (constant)
      setCoordinatesModel( &constantModel );

         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel   = &whitenoiseModel;
      pClockStoModelL1 = &whitenoiseModel;
      pClockStoModelL2 = &whitenoiseModel;

         // Pointer to stochastic model for phase biases
      pAmbiModelL1 = &ambiModelL1;
      pAmbiModelL2 = &ambiModelL2;

         // Pointer to stochastic model for ionospheric delays
      pIonoModel = &ionoModel;


   }  // End of method 'SolverRTKUC::Init()'




      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverRTKUC::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverRTKUC::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverRTKUC::Process(gnssRinex& gData)
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

   }  // End of method 'SolverRTKUC::Process()'


      /* Code to be executed before 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverRTKUC::preCompute( gnssRinex& gData )
      throw(ProcessingException, SVNumException)
   {

         // Continue ...
      try
      {
         cout << static_cast<YDSTime>(gData.header.epoch).year         << "  ";    // Year           #1
         cout << static_cast<YDSTime>(gData.header.epoch).doy          << "  ";    // DayOfYear      #2
         cout << static_cast<YDSTime>(gData.header.epoch).sod   << endl;    // SecondsOfDay   #3

         string line;
         CivilTime civTime(gData.header.epoch);
         line  = rightJustify(asString<short>(civTime.year), 6);
         line += rightJustify(asString<short>(civTime.month), 6);
         line += rightJustify(asString<short>(civTime.day), 6);
         line += rightJustify(asString<short>(civTime.hour), 6);
         line += rightJustify(asString<short>(civTime.minute), 6);
         line += rightJustify(asString(civTime.second, 7), 13);
         cout << "time:" << line << endl;


         double sod( (gData.header.epoch).getSecondOfDay() );

         if( sod < 43200.0 )
         {
             initialIonoVar = 0.001; // 0.01*0.01
         }
         else
         {
             initialIonoVar = 0.01;
         }

            // Get a set with all satellites present in this GDS
         currSatSet = gData.body.getSatID();

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

            // Check current SV number
         if( numCurrentSV < 5 )
         {
               // If epoch must be decimated, we issue an Exception
            SVNumException e("Satellite number is less than 4");
            
               // Throw exception
            GPSTK_THROW(e);

         }

            // Total measurement number
            // P1/P2/L1/L2/IonoL1/Trop
         numMeas = 4 * numCurrentSV + numCurrentSV + 1;

            // Number of 'core' variables i.e.  
            // 1 trospheric delay + 3 coordinates + 3 receiver clock 
         numVar = srcIndexedTypes.size();

            // Total number of unknowns is defined as :
            // numCurrentSV of ionospheric delays + 2*numCurrentSV of ambiguities
         numUnknowns = numVar + 3*numCurrentSV;


            /*
             * Now, fill the measVector
             */

            // Build the vector of measurements (Prefit-residuals): Code + phase
         measVector.resize(numMeas, 0.0);

            // Get the prefit residuals from 'gData'
         Vector<double> prefitC;
         satTypeValueMap dummyC1(gData.body.extractTypeID(TypeID::prefitC1));
         if ( dummyC1.numSats() == numCurrentSV )
         {
             prefitC = gData.getVectorOfTypeID(TypeID::prefitC1);
         }
         else
         {
             prefitC = gData.getVectorOfTypeID(TypeID::prefitP1);
         }
         Vector<double> prefitP2(gData.getVectorOfTypeID(TypeID::prefitP2));
         Vector<double> prefitL1(gData.getVectorOfTypeID(TypeID::prefitL1));
         Vector<double> prefitL2(gData.getVectorOfTypeID(TypeID::prefitL2));

         for( int i=0; i<numCurrentSV; i++ )
         {
            measVector( i                  ) = prefitC(i);
            measVector( i + 1*numCurrentSV ) = prefitP2(i);
            measVector( i + 2*numCurrentSV ) = prefitL1(i);
            measVector( i + 3*numCurrentSV ) = prefitL2(i);
         }

            // Get ionospheric delays from 'gData'
         Vector<double> initIono(gData.getVectorOfTypeID(TypeID::ionoL1));
         cout << "initIono" << initIono << endl;
         cout << "initialIonoVar" << initialIonoVar << endl;
         for( int i=0; i<numCurrentSV; i++ )
         {
            measVector( i + 4*numCurrentSV ) = initIono(i);
         }

            // Get tropospheric delays from 'gData.header.source'
         double initTropo( gData.header.source.zwdMap[TypeID::wetTropo] );
         cout << "initTropo" << initTropo << endl;
         cout << "initialTropVar" << initialTropVar << endl;
         measVector( 5*numCurrentSV ) = initTropo;

            //>Now, Fill the rMatrix

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
                   = 11.11*wVec(i); // 1.0/0.3 * 1.0/0.3
               rMatrix( i + 1*numCurrentSV, i + 1*numCurrentSV ) 
                   = 11.11*wVec(i); // 1.0/0.005 * 1.0/0.005
               rMatrix( i + 2*numCurrentSV, i + 2*numCurrentSV ) 
                   = 111111.11*wVec(i); // 1.0/0.25
               rMatrix( i + 3*numCurrentSV, i + 3*numCurrentSV ) 
                   = 2500*wVec(i); // 1.0/0.003
               rMatrix( i + 4*numCurrentSV, i + 4*numCurrentSV ) 
                   = 1.0/initialIonoVar*wVec(i); // 1.0/0.05

            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         }
         else
         {
               // Firstly, add the covariance matrix for P1/P2/L1/L2
            for( int i=0; i<numCurrentSV; i++ )
            {
               rMatrix( i,                  i                  ) 
                   = 11.11; // 1.0/0.3 * 1.0/0.3
               rMatrix( i + 1*numCurrentSV, i + 1*numCurrentSV ) 
                   = 11.11; // 1.0/0.003 * 1.0/0.003
               rMatrix( i + 2*numCurrentSV, i + 2*numCurrentSV ) 
                   = 111111.11; // 1.0/((c*c+d*d)*varCode)
               rMatrix( i + 3*numCurrentSV, i + 3*numCurrentSV ) 
                   = 111111.11; // 1.0/((e*e+f*f)*varPhase)
               rMatrix( i + 4*numCurrentSV, i + 4*numCurrentSV ) 
                   = 1.0/initialIonoVar; // 1.0/0.5
            }

         }

            // Weight for tropospheric delays
            // 1/0.02 
         rMatrix(5*numCurrentSV, 5*numCurrentSV) = 1.0/initialTropVar;

            //>Now, Fill the hMatrix

            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns, 0.0);

            // Get the values corresponding to 'core' variables
         Matrix<double> dMatrix(gData.body.getMatrixOfTypes(obsIndepTypes));

            // Let's fill 'hMatrix'
         for( int i=0; i<numCurrentSV; i++ )
         {
               // First, fill the coefficients related to tropo, coord 
            for( int j=0; j<4; j++ )
            {

               hMatrix( i                 , j ) = dMatrix(i,j); // C
               hMatrix( i +   numCurrentSV, j ) = dMatrix(i,j); // L
               hMatrix( i + 2*numCurrentSV, j ) = dMatrix(i,j); // PW
               hMatrix( i + 3*numCurrentSV, j ) = dMatrix(i,j); // LW
            }

               // Now, fill the reciever clock's coefficients for P1, P2, L1, L2
            hMatrix( i                 , 4 ) = 1.0; // P1
            hMatrix( i +   numCurrentSV, 4 ) = 1.0; // P2
            hMatrix( i + 2*numCurrentSV, 5 ) = 1.0; // L1
            hMatrix( i + 3*numCurrentSV, 6 ) = 1.0; // L2

         }  // End of 'for( int i=0; i<numCurrentSV; i++ )'


            // Now, fill the coefficients related to ionospheric delays (ionoL1)
         for( int i=0; i<numCurrentSV; i++ )
         {
            hMatrix( i                 , numVar + i ) = 1.0; // P1
            hMatrix( i +   numCurrentSV, numVar + i ) = 1.646944444; // P2
            hMatrix( i + 2*numCurrentSV, numVar + i ) =-1.0; // L1
            hMatrix( i + 3*numCurrentSV, numVar + i ) =-1.646944444; // L2
         }  

            // Now, fill the coefficients related to the ambiguities
         for( int i=0; i<numCurrentSV; i++ )
         {
               // ambL1 are listed after ( srcIndexedTypes + ionoL1 )
            hMatrix( i + 2*numCurrentSV, numVar +   numCurrentSV + i ) 
                = -0.190293672798;
               // ambL2 are listed after ( srcIndexedTypes + ionoL1 + ambL1 + ambL2)
            hMatrix( i + 3*numCurrentSV, numVar + 2*numCurrentSV + i ) 
                = -0.244210213425;
         }                                                    

            // Now, fill the coefficients for 'initIono'
         for(int i=0; i<numCurrentSV;i++)
         {
            hMatrix(i + 4*numCurrentSV, numVar + i ) = 1.0;
         }
            
            // Now, fill the coefficients for 'initTropo'
         hMatrix( 5*numCurrentSV, 0 ) = 1.0;
         

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
         pClockStoModelL1->Prepare( dummySat,
                                  gData );
         phiMatrix(5,5) = pClockStoModelL1->getPhi();
         qMatrix(5,5)   = pClockStoModelL1->getQ();

            // Five, the receiver clock
         pClockStoModelL2->Prepare( dummySat,
                                  gData );
         phiMatrix(6,6) = pClockStoModelL2->getPhi();
         qMatrix(6,6)   = pClockStoModelL2->getQ();


            // Now, fill the ionospheric delays
         int count2(numVar);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Prepare stochastic model
            pIonoModel->Prepare( *itSat,
                                 gData );

               // Get values into phi and q matrices
            phiMatrix(count2,count2) = pIonoModel->getPhi();
            qMatrix(count2,count2)   = pIonoModel->getQ();

            ++count2;
         }

            // Now, fill the ambiguity on L1
         count2 = (numVar + numCurrentSV);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Prepare stochastic model
            pAmbiModelL1->Prepare( *itSat,
                                   gData );

               // Get values into phi and q matrices
            phiMatrix(count2,count2) = pAmbiModelL1->getPhi();
            qMatrix(count2,count2)   = pAmbiModelL1->getQ();

            ++count2;
         }

            // Now, fill the ambiguity on L2 
         int numCS(0);
         count2 = (numVar + 2*numCurrentSV);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Prepare stochastic model
            pAmbiModelL2->Prepare( *itSat,
                                   gData );

               // Get values into phi and q matrices
            phiMatrix(count2,count2) = pAmbiModelL2->getPhi();
            qMatrix(count2,count2)   = pAmbiModelL2->getQ();

            numCS += pAmbiModelL2->getCS();

            ++count2;
         }

         cout << "numCS" << numCS << endl;

         if(numCS==numCurrentSV)
         {
             resetL2 = resetL1 = resetSol = true;
             startTime = sod ;
             startTimeVec.push_back(startTime);
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
            initialErrorCovariance(0,0) = 0.01;          // (0.1 m)**2

               // Second, the coordinates
            for( int i=1; i<4; i++ )
            {
               initialErrorCovariance(i,i) = 1.0;       // (1.0 m)**2
            }

            initialErrorCovariance(4,4) = 9.0e10;
            initialErrorCovariance(5,5) = 9.0e10;
            initialErrorCovariance(6,6) = 9.0e10;

               // Third, the ionospheric delays
            for( int i=numVar; i<numVar+numCurrentSV; i++ )
            {
               initialErrorCovariance(i,i) = 0.004;     // (0.02 m)**2
            }

               // Finally, the ambiguities 
            for( int i=numVar+numCurrentSV; i<numUnknowns; i++ )
            {
               initialErrorCovariance(i,i) = 4.0e14;     // (20000 km)**2
            }

               // Reset Kalman filter state and covariance matrix
            xhat = initialState; 
            P    = initialErrorCovariance;

               // No longer first time
            firstTime = false;

         }
         else
         {

               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);


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

   }  // End of method 'SolverRTKUC::preCompute()'



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
   gnssRinex& SolverRTKUC::Compute( gnssRinex& gData )
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

   }  // End of method 'SolverRTKUC::Compute()'


    

      // Predict the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverRTKUC::TimeUpdate( const Matrix<double>& phiMatrix,
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

   }  // End of method 'SolverRTKUC::TimeUpdate()'


   gnssRinex& SolverRTKUC::AmbiguityConstr( gnssRinex& gData )
      throw(ProcessingException)
   {
      try
      {

            // Firstly,  get the float ambiguities and corresponding covariance 
            // from 'predicted' state and covariance matrix.
         std::map<SatID, double> ambL1Map;
         std::map<SatID, double> ambL2Map;

            // Covariance Map
         std::map<SatID, std::map< SatID, double > > covAmbL1Map;
         std::map<SatID, std::map< SatID, double > > covAmbL2Map;

            // Now, get the ambiguity on LC
         int c1(0);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Ambiguity value
            ambL1Map[ (*itSat) ] = xhatminus( numVar + 1*numCurrentSV + c1 );
            ambL2Map[ (*itSat) ] = xhatminus( numVar + 2*numCurrentSV + c1 );

               // Ambiguity variance
            covAmbL1Map[ (*itSat) ][ (*itSat) ]
                = Pminus( numVar + 1*numCurrentSV + c1, numVar + 1*numCurrentSV + c1); 
            covAmbL2Map[ (*itSat) ][ (*itSat) ]
                = Pminus( numVar + 2*numCurrentSV + c1, numVar + 2*numCurrentSV + c1); 

               // Increment
            ++c1;

         }

            // Fixed ambiguities on L1
         std::map<SatID, double> ambL1FixedMap;
  
            // Warning: 
         datumL1.Reset( ambL1Map, covAmbL1Map);

            // Now, Let's 'find' the datum satellite 
         datumL1.Prepare( gData );

            // Get the datum
         ambL1FixedMap = datumL1.getAmbFixedMap();
  
            // Map holding the information regarding every satellite
         std::map<SatID, double> ambL2FixedMap;

            // Warning: 
         datumL2.Reset( ambL2Map, covAmbL2Map);

            // Now, Let's 'find' the datum satellite 
         datumL2.Prepare( gData );

            // Get the datum
         ambL2FixedMap = datumL2.getAmbFixedMap();

            // Now, Let's get the constraint equation

            // Number of Fixed widelane ambiguities
         int numBL1( ambL1FixedMap.size() );
        
            // Equation matrix
         Vector<double> mVecBL1(numBL1, 0.0);
         Matrix<double> hMatBL1(numBL1, numUnknowns, 0.0);
         Matrix<double> rMatBL1(numBL1, numBL1, 0.0);
          
            // Now, insert the fixed ambiguities to the equation system to form
            // the new measVector/hMatrix/rMatrix.
         int rowBL1(0);
         for( std::map<SatID, double>::iterator it = ambL1FixedMap.begin();
              it != ambL1FixedMap.end();
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
             int colBL1 = numVar + numCurrentSV + jsat;

                // Measurement
             mVecBL1( rowBL1 ) = (*it).second;

                // coefficient
             hMatBL1( rowBL1, colBL1) = 1.0;

                // weight
             rMatBL1( rowBL1, rowBL1) = 1.0E+10;

                // row increment
             rowBL1++;

         }

         int numBL2( ambL2FixedMap.size());

            // Equation matrix
         Vector<double> mVecBL2(numBL2, 0.0);
         Matrix<double> hMatBL2(numBL2, numUnknowns, 0.0);
         Matrix<double> rMatBL2(numBL2, numBL2, 0.0);

         int rowBL2(0);
         for( std::map<SatID, double>::iterator it = ambL2FixedMap.begin();
              it != ambL2FixedMap.end();
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
             int colBL2 = numVar + 2*numCurrentSV + jsat;

                // Measurement
             mVecBL2( rowBL2 ) = (*it).second;

                // coefficient
             hMatBL2( rowBL2, colBL2) = 1.0;

                // weight
             rMatBL2( rowBL2, rowBL2) = 1.0E+10;

                // row increment
             rowBL2++;
         }


            // The observation equation number is enlarged
         int numEqu( numMeas + numBL1 + numBL2 );

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
         for(int i=0; i<numBL1; i++)
         {
               // Prefit
            tempPrefit(rowStart+i) = mVecBL1(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++)
            {
               tempGeometry(rowStart+i,j) = hMatBL1(i,j); 
            }  
               // Weight
            tempWeight(rowStart+i,rowStart+i)  = rMatBL1(i,i); 
         }

            // Now, Let's store the mVecBL2/hMatBL2/rMatBL2
         rowStart = (numMeas + numBL1) ;
         for(int i=0; i<numBL2; i++)
         {
               // Prefit
            tempPrefit(rowStart+i) = mVecBL2(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++)
            {
               tempGeometry(rowStart+i,j) = hMatBL2(i,j); 
            }  
               // Weight
            tempWeight(rowStart+i,rowStart+i)  = rMatBL2(i,i); 
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

   }  // End of method 'SolverRTKUC::AmbiguityConstr()'



      // Now, fix all the ambiguities to integers 
   gnssRinex& SolverRTKUC::AmbiguityFixing( gnssRinex& gData )
      throw(ProcessingException)
   {

      try
      {
            // Firsly, store the solution/covMatrix into 'newState/newCov'
         newState = solution;
         newCov = covMatrix;

         double sod = (gData.header.epoch).getSecondOfDay();

         cout << "newState" << endl;
         cout <<  newState  << endl;

         cout << "newCov" << endl;
         cout <<  newCov  << endl;

         int numAmbs(2*numCurrentSV);

            // Ambiguities and corresponding covariance
         Vector<double> ambVec(numAmbs, 0.0);
         Matrix<double> ambCov(numAmbs, numAmbs, 0.0);

            // Ambiguity 
         int c1(0);
         for(int i=numVar+numCurrentSV; i<numUnknowns; i++)
         {
             ambVec(c1) = newState(i);
             c1++;
         }

            // Ambiguity covariance
         c1 = 0 ;
         for(int i=numVar+numCurrentSV; i<numUnknowns; i++)
         {
             int c2(0);
             for(int j=numVar+numCurrentSV;j<numUnknowns;j++)
             {
                 ambCov(c1,c2) = newCov(i,j);
                 c2++;
             }
             c1++;
         }

            // Used to store the index of the ambiguities to be fixed.
         std::set<int> indexSet;
         for(int i=0; i<2*numCurrentSV; i++)
         {
             indexSet.insert(i);
         }

            // 
         std::map<int, double> ambFixedMap;

            //
            // Now, Let's fix the ambiguities with 'LAMBDA' method
            //

            // Now, Let's fix the widelane ambiguities with LAMBDA
         ARMLambda mlambda;

            // ratio
         double ratio(0.0);

            // If the satellite number to be fixed is greater than 4.
         int iter(0);
         while( indexSet.size() > 2 )
         {
                // Ambiguity number
             int tempSize(indexSet.size());

                // Ambiguities and corresponding covariance
             Vector<double> tempAmb(tempSize, 0.0);
             Matrix<double> tempCov(tempSize, tempSize, 0.0);

                // Now, fill the 'tempAmb'
             int c1(0);
             for( std::set<int>::iterator itIndex = indexSet.begin();
                  itIndex != indexSet.end();
                  ++itIndex )
             {
                 tempAmb(c1) = ambVec( (*itIndex) );
                 c1++;
             }

                // Now, fill the covariance 
             std::set<int> tempIndexSet(indexSet);

             c1=0;
             for( std::set<int>::iterator itIndex = indexSet.begin();
                  itIndex != indexSet.end();
                  ++itIndex )
             {
                 int c2(0);
                 for( std::set<int>::iterator itIndex2=indexSet.begin();
                      itIndex2!=indexSet.end();
                      ++itIndex2 )
                 {
                     tempCov(c1,c2) = ambCov( (*itIndex), (*itIndex2) );
                     c2++;
                 }

                 c1++;
             }
             
             cout << "tempAmb:" << tempAmb << endl;
             cout << "tempCov:" << tempCov << endl;

                // Fill 'mlambda' with 'tempAmb/tempCov'
             mlambda.resolve(tempAmb, tempCov);
             ratio = mlambda.getRatio();

                // Get fixed ambiguity vector
             Vector<double> ambFixedVec = mlambda.getFixedAmbVec();

                // Fixing ratio
             if( (ratio>3.0) )
             {
                    // Store the fixed ambiguities into 'ambFixedMap'
                 int c1(0);
                 for( std::set<int>::iterator itIndex = indexSet.begin();
                      itIndex != indexSet.end();
                      ++itIndex )
                 {
                     ambFixedMap[(*itIndex)] = ambFixedVec(c1);
                     c1++;
                 }

                 break;

             }
             else
             {
                    // Satellite with minimum fixing decision
                 int tempIndex;

                    // Minimum decision for this iteration
                 double tempMaxVar(0.0);

                    // Temporary satellite
                 int c1(0);
                 for( std::set<int>::iterator itIndex=indexSet.begin();
                      itIndex !=indexSet.end();
                      ++itIndex )
                 {
                        // Variance 
                     double var = ambCov( (*itIndex), (*itIndex) );

                        // Get the minimum decision
                     if( var > tempMaxVar)
                     {
                         tempMaxVar = var;
                         tempIndex = (*itIndex);
                     }
                 }

                 cout << "Index to be removed:" << tempIndex << endl;

                    // remove this satellite from validSatSet 
                 indexSet.erase(tempIndex);

             }

               // Iter increment
             iter++;

         };  // End of 'while(validSatSet.size() >1)'


         cout << "numSV:" << indexSet.size() << endl;
         cout << "ratio: "<< ratio << endl;

            // Flags indicating whether this unknown variable has been fixed.
         Vector<double> stateFlag(numUnknowns, 0.0);

            // Update the solution/state with fixed widelane ambiguities
         if( ratio > 3.0 )
         {
               // Loop the indexSet 
            for( std::set<int>::iterator itIndex=indexSet.begin();
                 itIndex !=indexSet.end();
                 ++itIndex )
            {
                  // Fixed widelane ambiguity
               double ambFixed = ambFixedMap[(*itIndex)];

               int index = numVar + numCurrentSV + (*itIndex);

                  // Update the solution and covarinace  
               AmbiguityUpdate(newState, newCov, stateFlag, index, ambFixed );

                  // Not free at all
               stateFlag(index) = 1.0;
            }

         }

         numFixedBL1 =0;
         for(int i=numVar+numCurrentSV; i<numVar+2*numCurrentSV; i++)
         {
             numFixedBL1 += stateFlag(i);
         }

         numFixedBL2 =0;
         for(int i=numVar+2*numCurrentSV; i<numUnknowns; i++)
         {
             numFixedBL2 += stateFlag(i);
         }

         if(resetL1)
         {
             if(numFixedBL1>4)
             {
                double ttff = sod - startTime;
                cout << "startTime" << startTime << endl;
                cout << "NL ttff:" << ttff << endl;
                ttffL1.push_back(ttff);  
                resetL1 = false;
             }
         }

         if(resetL2)
         {
             if(numFixedBL2>4)
             {
                double ttff = sod - startTime;
                cout << "startTime" << startTime << endl;
                cout << "WL ttff:" << ttff << endl;
                ttffL2.push_back(ttff);  
                resetL2 = false;
             }
         }

         double dx = newState(1);
         double dy = newState(2);
         double dz = newState(3);
         double drou = std::sqrt(dx*dx+dy*dy+dz*dz);

         if(resetSol)
         {
             if(drou<0.10)
             {
                double dt = sod - startTime;
                cout << "startTime" << startTime << endl;
                cout << "ttsc:" << dt << endl;
                ttsc.push_back(dt);  
                resetSol = false;
             }
         }

            //
            // Now, Let's insert corrections into 'gnssRinex'
            //

            // Firstly, insert the wet tropospheric delays

            // Now, get the ionoL1 from 'currentState'
         double wetTropo(0.0);

            // Get ionospheric delays from solution
         wetTropo = newState(0);

            // Insert 'wetTropo' into 'gnssRinex'
         gData.header.source.zwdMap[TypeID::wetTropo] = wetTropo;

         cout << "estimated tropo:" << wetTropo << endl;

            // Secondly, insert the ionospheric delays into body of the 'gnssRinex'

            // Now, get the ionoL1 from 'currentState'
         Vector<double> ionoL1(numCurrentSV,0.0);

            // Get ionospheric delays from solution
         for( int i=0; i<numCurrentSV; i++ )
         {
             ionoL1(i) = newState( numVar + i );
         }

         cout << "ionoL1:" << ionoL1 << endl;

         gData.insertTypeIDVector(TypeID::ionoL1, ionoL1);

            // Thirdly, insert the 'corrections' into 'gnssRinex'

         Vector<double> ambL1Fixed(numCurrentSV, 0.0);
         Vector<double> ambL1Flag(numCurrentSV,0.0);

            // Now, get the widelane ambiguity 
         for(int i=0; i<numCurrentSV; i++)
         {
             ambL1Fixed(i) = newState(numVar+numCurrentSV+i);
             ambL1Flag(i)  = stateFlag(numVar+numCurrentSV+i);
         }

         Vector<double> ambL2Fixed(numCurrentSV, 0.0);
         Vector<double> ambL2Flag(numCurrentSV,0.0);

         for(int i=0; i<numCurrentSV; i++)
         {
             ambL2Fixed(i) = newState(numVar+2*numCurrentSV+i);
             ambL2Flag(i)  = stateFlag(numVar+2*numCurrentSV+i);
         }

         cout << "ambL1Fixed:" << ambL1Fixed << endl;
         cout << "ambL1Flag:" << ambL1Flag<< endl;
         cout << "ambL2Fixed:" << ambL2Fixed << endl;
         cout << "ambL2Flag:" << ambL2Flag<< endl;

            //
            // Compute TypeID::corrLdelta 
            //
            
         Vector<double> prefitC1(gData.getVectorOfTypeID(TypeID::prefitC1));
         Vector<double> prefitP1(gData.getVectorOfTypeID(TypeID::prefitP1));
         Vector<double> prefitP2(gData.getVectorOfTypeID(TypeID::prefitP2));
         Vector<double> prefitL1(gData.getVectorOfTypeID(TypeID::prefitL1));
         Vector<double> prefitL2(gData.getVectorOfTypeID(TypeID::prefitL2));

         Vector<double> corrC1(numCurrentSV, 0.0);
         Vector<double> corrP1(numCurrentSV, 0.0);
         Vector<double> corrP2(numCurrentSV, 0.0);
         Vector<double> corrL1(numCurrentSV, 0.0);
         Vector<double> corrL2(numCurrentSV, 0.0);

         corrC1 = prefitC1;
         corrP1 = prefitP1;
         corrP2 = prefitP2;
         for(int i=0; i<numCurrentSV; i++)
         {
            corrL1(i) = prefitL1(i) + 0.190293672798*ambL1Fixed(i);
            corrL2(i) = prefitL2(i) + 0.244210213425*ambL2Fixed(i);
         }

         cout << "corrLdelta" << endl;

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

         gData.insertTypeIDVector(TypeID::BL1, ambL1Fixed);
         gData.insertTypeIDVector(TypeID::BL2, ambL2Fixed);

            // Set to store the sats to be removed
         SatIDSet satRejectedSet;

         c1 = 0;
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // If BWL or BL1 is not fixed, then the satellite will be removed.
            if( (ambL1Flag(c1) != 1.0) || (ambL2Flag(c1) !=1.0 ) )
            {
                satRejectedSet.insert( (*itSat) );
            }
               // increment
            ++c1;
         }

            // Remove the satellite 
         gData.removeSatID(satRejectedSet);


//       solution = newState;
//       covMatrix = newCov;

  
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

   }  // End of method 'SolverRTKUC::AmbiguityFixing()'



      /** Update the free unknonws' solution and covariance after
       *  fixing the ambiguities
       */
   void SolverRTKUC::AmbiguityUpdate( Vector<double>& state,
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
            if( (fixedFlag(i) == 0.0) && (i != index) )
            {
               for(int j=0; j<numUnknowns; j++)
               {
                  if( (fixedFlag(j) == 0.0) && (j != index) )
                  {
                     covariance(i,j) = covariance(i,j) - Qba(i)*Qaa*Qba(j);
                  }
               }
            }
         }

            // Q(i, index), Q(index, j)
         for(int i=0; i<numUnknowns; i++)
         {
             if(i==index)
             {
                covariance(i,index) = eps;
             }
             else
             {
                covariance(i,index) = 
                    covariance(index,i) = 0.0;
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

   }  // End of method 'SolverRTKUC::AmbiguityUpdate()'



      // Correct the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverRTKUC::MeasUpdate( const Vector<double>& prefitResiduals,
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

   }  // End of method 'SolverRTKUC::MeasUpdate()'





      /* Code to be executed after 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverRTKUC::postCompute( gnssRinex& gData )
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

   }  // End of method 'SolverRTKUC::postCompute()'



      /* Sets if a NEU system will be used.
       *
       * @param useNEU  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverRTKUC& SolverRTKUC::setNEU( bool useNEU )
   {

         //>Firstly, fill the types that are source-indexed 

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
      srcIndexedTypes.insert(TypeID::cdtL1); // #6  
      srcIndexedTypes.insert(TypeID::cdtL2); // #7

         //>Firstly, fill the types that are common for all observables
      obsIndepTypes.insert(TypeID::wetMap);

      if (useNEU)
      {
         obsIndepTypes.insert(TypeID::dLat); // #2
         obsIndepTypes.insert(TypeID::dLon); // #3
         obsIndepTypes.insert(TypeID::dH);   // #4
      }
      else
      {
         obsIndepTypes.insert(TypeID::dx);   // #2
         obsIndepTypes.insert(TypeID::dy);   // #3
         obsIndepTypes.insert(TypeID::dz);   // #4
      }

         //>Then, fill the types that are satellite-indexed

         // The order of the elment is as follows:
      satIndexedTypes.insert(TypeID::ionoL1); // #2
      satIndexedTypes.insert(TypeID::BL1);    // #3
      satIndexedTypes.insert(TypeID::BL2);    // #4

         // Now, we build the basic equation definition
      defaultEqDef.header = TypeID::prefitC1;
      defaultEqDef.body = srcIndexedTypes;

      return (*this);

   }  // End of method 'SolverRTKUC::setNEU()'


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
   SolverRTKUC& SolverRTKUC::setCoordinatesModel( StochasticModel* pModel )
   {

         // All coordinates will have the same model
      pCoordXStoModel = pModel;
      pCoordYStoModel = pModel;
      pCoordZStoModel = pModel;

      return (*this);

   }  // End of method 'SolverRTKUC::setCoordinatesModel()'


      /** Set a random walk stochastic model to slant ionospheric delays.
       *
       * @param pModel      Pointer to StochasticModel associated with
       *                    ionospheric delays.
       */
   SolverRTKUC& SolverRTKUC::setIonosphericModel(StochasticModel* pModel)
   {
         // All coordinates will have the same model
      pIonoModel = pModel;
      return (*this);
   };


      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       */
   double SolverRTKUC::getSolution(const TypeID& type) const
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

      return solution(counter);

   }  // End of method 'SolverRTKUC::getSolution()'


      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       */
   double SolverRTKUC::getFixedSolution(const TypeID& type) const
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

   }  // End of method 'SolverRTKUC::getSolution()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       */
   double SolverRTKUC::getVariance(const TypeID& type) const
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

      return covMatrix(counter,counter);

   }  // End of method 'SolverRTKUC::getVariance()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       */
   double SolverRTKUC::getFixedVariance(const TypeID& type) const
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

   }  // End of method 'SolverRTKUC::getFixedVariance()'



      /** Return the CURRENT number of satellite.
       */
   int SolverRTKUC::getCurrentSatNumber() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numCurrentSV;
   }

      /** Return the CURRENT number of satellite.
       */
   int SolverRTKUC::getAmbFixedNumL1() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numFixedBL1;
   }

      /** Return the CURRENT number of satellite.
       */
   int SolverRTKUC::getAmbFixedNumL2() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numFixedBL2;
   }


}  // End of namespace gpstk
