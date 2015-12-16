#pragma ident "$Id: SolverRTKLc.cpp 2645 2012-11-05 03:23:24Z shjzhang $"

/**
 * @file SolverRTKLc.cpp
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


#include "SolverRTKLc.hpp"
#include "MatrixFunctors.hpp"
#include "ARMLambda.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverRTKLc::classIndex = 9300000;


      // Returns an index identifying this object.
   int SolverRTKLc::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverRTKLc::getClassName() const
   { return "SolverRTKLc"; }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverRTKLc::SolverRTKLc(bool useNEU)
      : firstTime(true), fixWithLambda(true)

   {

         // Set the equation system structure
      setNEU(useNEU);

         // Set the class index
      setIndex();

         // Call initializing method
      Init();

   }  // End of 'SolverRTKLc::SolverRTKLc()'



      // Initializing method.
   void SolverRTKLc::Init(void)
   {

         // Set qdot value for default random walk stochastic model
      rwalkModel.setQprime(3e-8);

         // Pointer to default stochastic model for troposphere (random walk)
      pTropoStoModel = &rwalkModel;

         // Set default coordinates stochastic model (constant)
      setCoordinatesModel( &constantModel );

         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel   = &whitenoiseModel;
      pClockStoModelLC = &whitenoiseModel;

         // Pointer to stochastic model for phase biases
      pAmbiModel = &ambiModel;


   }  // End of method 'SolverRTKLc::Init()'




      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverRTKLc::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverRTKLc::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverRTKLc::Process(gnssRinex& gData)
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

   }  // End of method 'SolverRTKLc::Process()'


      /* Code to be executed before 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverRTKLc::preCompute( gnssRinex& gData )
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
         if( numCurrentSV < 4 )
         {
               // If epoch must be decimated, we issue an Exception
            SVNumException e("Satellite number is less than 4");
            
               // Throw exception
            GPSTK_THROW(e);

         }

            // Total measurement number
         numMeas = 2 * numCurrentSV;

            // Number of 'core' variables i.e.  
            // 3 coordinates + 2 receiver clock 
         numVar = srcIndexedTypes.size();

            // Total number of unknowns is defined as :
            // 1*numCurrentSV of ambiguities
         numUnknowns = numVar + 1*numCurrentSV;

            /*
             * Now, fill the measVector
             */

            // Build the vector of measurements (Prefit-residuals): Code + phase
         measVector.resize(numMeas, 0.0);

            // Get the prefit residuals from 'gData'
         Vector<double> prefitLdelta(gData.getVectorOfTypeID(TypeID::prefitLdelta));
         Vector<double> prefitL(gData.getVectorOfTypeID(TypeID::prefitL));

         for( int i=0; i<numCurrentSV; i++ )
         {
            measVector( i                ) = prefitLdelta(i);
            measVector( i + numCurrentSV ) = prefitL(i);
         }

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
                   = 1600*wVec(i); // 1.0/0.05 * 1.0/0.05
               rMatrix( i + 1*numCurrentSV, i + 1*numCurrentSV ) 
                   = 10000.0*wVec(i); // 1.0/0.01 * 1.0/0.05
            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         }
         else
         {
               // Firstly, add the covariance matrix for P1/P2/L1/L2
            for( int i=0; i<numCurrentSV; i++ )
            {
               rMatrix( i,                  i                  ) 
                   = 2500.0; // 1.0/0.3 * 1.0/0.3
               rMatrix( i + 1*numCurrentSV, i + 1*numCurrentSV ) 
                   = 10000.0; // 1.0/0.003 * 1.0/0.003
            }

         }

            //>Now, Fill the hMatrix

            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns, 0.0);

            // Get the values corresponding to 'core' variables
         Matrix<double> dMatrix(gData.body.getMatrixOfTypes(obsIndepTypes));

            // Let's fill 'hMatrix'
         for( int i=0; i<numCurrentSV; i++ )
         {
               // First, fill the coefficients related to coord, and cdtWL 
            for( int j=0; j<4; j++ )
            {

               hMatrix( i               , j ) = dMatrix(i,j); // C
               hMatrix( i + numCurrentSV, j ) = dMatrix(i,j); // L
            }

            hMatrix( i               , 4 ) = 1.0; // C
            hMatrix( i + numCurrentSV, 5 ) = 1.0; // L


         }  // End of 'for( int i=0; i<numCurrentSV; i++ )'


            // Now, fill the coefficients related to the ambiguities
         int count1(0);
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // ambL1 are listed after ( srcIndexedTypes + ambL1)
            hMatrix( count1 + numCurrentSV, numVar + count1 ) = -0.106953378;
            ++count1;

         }  // End of 'for( itSat = currSatSet.begin(); ... )'

         cout << "hMatrix" << endl;
         cout <<  hMatrix  << endl;

            // Now, Fill the phiMatrix and qMatrix

            // State Transition Matrix (PhiMatrix)
         phiMatrix.resize(numUnknowns, numUnknowns, 0.0);

            // Noise covariance matrix (QMatrix)
         qMatrix.resize(numUnknowns, numUnknowns, 0.0);


            // Now, let's fill the Phi and Q matrices
         SatID  dummySat;

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

            // Third, the receiver clock
         pClockStoModelLC->Prepare( dummySat,
                                  gData );
         phiMatrix(5,5) = pClockStoModelLC->getPhi();
         qMatrix(5,5)   = pClockStoModelLC->getQ();

//          // Second, the coordinates
//       pCoordXStoModel->Prepare(dummySat, gData);
//       phiMatrix(0,0) = pCoordXStoModel->getPhi();
//       qMatrix(0,0)   = pCoordXStoModel->getQ();

//       pCoordYStoModel->Prepare(dummySat, gData);
//       phiMatrix(1,1) = pCoordYStoModel->getPhi();
//       qMatrix(1,1)   = pCoordYStoModel->getQ();

//       pCoordZStoModel->Prepare(dummySat, gData);
//       phiMatrix(2,2) = pCoordZStoModel->getPhi();
//       qMatrix(2,2)   = pCoordZStoModel->getQ();


//          // Third, the receiver clock
//       pClockStoModel->Prepare( dummySat,
//                                gData );
//       phiMatrix(3,3) = pClockStoModel->getPhi();
//       qMatrix(3,3)   = pClockStoModel->getQ();


//          // Third, the receiver clock
//       pClockStoModelLC->Prepare( dummySat,
//                                gData );
//       phiMatrix(4,4) = pClockStoModelLC->getPhi();
//       qMatrix(4,4)   = pClockStoModelLC->getQ();


            // Now, fill the ambiguity on WL
         int count2(numVar);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Prepare stochastic model
            pAmbiModel->Prepare( *itSat,
                                   gData );

               // Get values into phi and q matrices
            phiMatrix(count2,count2) = pAmbiModel->getPhi();
            qMatrix(count2,count2)   = pAmbiModel->getQ();

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
            initialErrorCovariance(0,0) = 0.01;          // (0.1 m)**2

               // First, the coordinates
            for( int i=1; i<4; i++ )
            {
               initialErrorCovariance(i,i) = 1.0;       // (1.0 m)**2
            }

            initialErrorCovariance(4,4) = 9.0e10;
            initialErrorCovariance(5,5) = 9.0e10;

               // Then, the ambiguities 
            for( int i=numVar; i<numUnknowns; i++ )
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

   }  // End of method 'SolverRTKLc::preCompute()'



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
   gnssRinex& SolverRTKLc::Compute( gnssRinex& gData )
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

   }  // End of method 'SolverRTKLc::Compute()'


    

      // Predict the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverRTKLc::TimeUpdate( const Matrix<double>& phiMatrix,
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

   }  // End of method 'SolverRTKLc::TimeUpdate()'


   gnssRinex& SolverRTKLc::AmbiguityConstr( gnssRinex& gData )
      throw(ProcessingException)
   {
      try
      {

            // Firstly,  get the float ambiguities and corresponding covariance 
            // from 'predicted' state and covariance matrix.
         std::map<SatID, double> ambL1Map;

            // Covariance Map
         std::map<SatID, std::map< SatID, double > > covAmbL1Map;

            // Now, get the ambiguity on widelane
         int c1(0);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Ambiguity value
            ambL1Map[ (*itSat) ] = xhatminus( numVar + c1 );

               // Ambiguity variance
            covAmbL1Map[ (*itSat) ][ (*itSat) ]
                = Pminus( numVar + c1, numVar + c1); 

               // Increment
            ++c1;

         }

            // Map holding the information regarding every satellite
         std::map<SatID, double> ambL1FixedMap;

         datumL1.Reset( ambL1Map, covAmbL1Map);
         datumL1.Prepare( gData );

            // Get the datum
         ambL1FixedMap = datumL1.getAmbFixedMap();

            // Now, get the reference satellite
            
            // Let's get the first satellite
         std::map<SatID, double>::iterator itMap = ambL1FixedMap.begin();

            // The reference satellite
         SatID refSat = (*itMap).first;

            // Widelane ambiguity datum
         std::map<SatID, double> datumL1Map;
         datumL1Map[refSat] = ambL1FixedMap[refSat] ;
  

            // Now, Let's get the constraint equation

            // Number of Fixed widelane ambiguities
         int numBL1( datumL1Map.size() );
        
            // Equation matrix
         Vector<double> mVecBL1(numBL1, 0.0);
         Matrix<double> hMatBL1(numBL1, numUnknowns, 0.0);
         Matrix<double> rMatBL1(numBL1, numBL1, 0.0);
          
            // Now, insert the fixed ambiguities to the equation system to form
            // the new measVector/hMatrix/rMatrix.
         int rowBL1(0);
         for( std::map<SatID, double>::iterator it = datumL1Map.begin();
              it != datumL1Map.end();
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
             int colBL1 = numVar + jsat;

                // Measurement
             mVecBL1( rowBL1 ) = (*it).second;

                // coefficient
             hMatBL1( rowBL1, colBL1) = 1.0;

                // weight
             rMatBL1( rowBL1, rowBL1) = 1.0E+9;

                // row increment
             rowBL1++;

         }

            // The observation equation number is enlarged
         int numEqu( numMeas + numBL1);

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

   }  // End of method 'SolverRTKLc::AmbiguityConstr()'



      // Now, fix all the ambiguities to integers 
   gnssRinex& SolverRTKLc::AmbiguityFixing( gnssRinex& gData )
      throw(ProcessingException)
   {

      try
      {
            // Firsly, store the solution/covMatrix into 'newState/newCov'
         newState = solution;
         newCov = covMatrix;

         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
             cout << (*itSat) << " ";
         }
         cout << " " << endl;


         cout << "newState before ambiguity fixing" << endl;
         cout << newState << endl;
         cout << newCov << endl;

         double f1 = 1575.42e6;
         double f2 = 1227.60e6;
         double cw = f2/(f1-f2);

         std::map<SatID, double> ambL1Map;
         std::map<SatID, std::map< SatID, double > > covAmbL1Map;

         std::map<SatID, double> ambL1FixedMap;

            // Store values of ambiguity values

         int c1(0);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Ambiguity value
            ambL1Map[ (*itSat) ] = newState( numVar + c1 );

               // Increment
            c1++;

         }
            // Store values of covariance matrix

            // we need a copy of 'currSatSet'
         SatIDSet tempSatSet(currSatSet);
            
         c1 = 0;     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
            covAmbL1Map[(*itSat)][(*itSat)] = newCov(numVar+c1, numVar+c1);

            int c2 = (c1+1);

               // Erase current satellite from 'tempSatSet'
            tempSatSet.erase(*itSat);

            for( SatIDSet::const_iterator itSat2 = tempSatSet.begin();
                 itSat2 != tempSatSet.end();
                 ++itSat2 )
            {
                covAmbL1Map[(*itSat)][(*itSat2)] = newCov(numVar+c1, numVar+c2);
                c2++;
            }
             
               // Increment
            c1++;

         }

            // Flags indicating whether this unknown variable has been fixed.
         Vector<double> stateFlag(numUnknowns, 0.0);

            // Get widelane ambiguity information from 'newState/newCov'
            //
         SatIDSet validSatSet(currSatSet);

            // Now, Let's fix the widelane ambiguities with LAMBDA
         ARMLambda mlambda;

         double ratioL1(0.0);

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
             
             cout << "tempAmb:" << tempAmb << endl;
             cout << "tempCov:" << tempCov << endl;

                // Fill 'mlambda' with 'tempAmb/tempCov'
             mlambda.resolve(tempAmb, tempCov);
             ratioL1 = mlambda.getRatio();

                // Get fixed ambiguity vector
             Vector<double> ambFixedVec = mlambda.getFixedAmbVec();

             cout << "iter:" << iter << endl;
             cout << "numSV:" << numSV << endl;
             cout << "ratioL1:" << ratioL1 << endl;

                // Fixing ratio
             if( (ratioL1>3.0) )
             {
                    // Store the fixed ambiguities into 'ambL1FixedMap'
                 int c1(0);
                 for( SatIDSet::iterator itSat=validSatSet.begin();
                      itSat !=validSatSet.end();
                      ++itSat )
                 {
                     ambL1FixedMap[(*itSat)] = ambFixedVec(c1);
                     c1++;
                 }

                 break;
             }
             else
             {
                    // Satellite with minimum fixing decision
                 SatID tempSat;

                    // Minimum decision for this iteration
                 double tempMaxVar(0.0);

                    // Temporary satellite
                 int c1(0);
                 for( SatIDSet::iterator itSat=validSatSet.begin();
                      itSat !=validSatSet.end();
                      ++itSat )
                 {

                     double var = covAmbL1Map[(*itSat)][(*itSat)];

                     if( var > tempMaxVar)
                     {
                         tempMaxVar = var;
                         tempSat = (*itSat);
                     }
                 }

                 cout << "sat to be removed:" << tempSat << endl;

                    // remove this satellite from validSatSet 
                 validSatSet.erase(tempSat);

             }

               // Iter increment
             iter++;

         };  // End of 'while(validSatSet.size() >1)'


         cout << "numSV:" << validSatSet.size() << endl;
         cout << "ratioL1: "<< ratioL1 << endl;

            // Update the solution/state with fixed widelane ambiguities
         if( ratioL1 > 3.0 )
         {
               // Loop the validSatSet
            for( SatIDSet::iterator itSat=validSatSet.begin();
                 itSat !=validSatSet.end();
                 ++itSat )
            {
                  // Fixed widelane ambiguity
               double b1Fixed = ambL1FixedMap[(*itSat)];

                  // Find the position of current satellite.
               int jsat(0);
               SatIDSet::const_iterator itSat2( currSatSet.begin() );
               while( (*itSat2) != (*itSat) )
               {
                   ++jsat;
                   ++itSat2;
               }

               int index = numVar+jsat;

                  // Update the solution and covarinace  
               AmbiguityUpdate(newState, newCov, stateFlag, index, b1Fixed );

                  // Not free at all
               stateFlag(index) = 1.0;
            }

               // Update the number of the fixed widelane ambiguities
            numFixedBL1 = validSatSet.size();

         }

            //
            // Now, Let's insert corrections into 'gnssRinex'
            //

         Vector<double> ambL1Fixed(numCurrentSV, 0.0);
         Vector<double> ambL1Flag(numCurrentSV,0.0);

            // Now, get the widelane ambiguity 
         for(int i=0; i<numCurrentSV; i++)
         {
             ambL1Fixed(i) = newState(numVar+i);
             ambL1Flag(i)  = stateFlag(numVar+i);;
         }

            // Set to store the sats to be removed
         SatIDSet satRejectedSet;

         c1 = 0;
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // If BL1 or BL1 is not fixed, then the satellite will be removed.
            if( (ambL1Flag(c1) != 1.0) )
            {
                satRejectedSet.insert( (*itSat) );
            }

               // increment
            ++c1;
         }

            // Remove the satellite 
         gData.removeSatID(satRejectedSet);

         cout << "newState" << endl;
         cout <<  newState  << endl;

         cout << "newCov" << endl;
         cout <<  newCov << endl;

         cout << "ambL1Fixed" << ambL1Fixed << endl;
         cout << "ambL1Flag" << ambL1Flag << endl;

            // Ambiguity solution/covariance udpate

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

   }  // End of method 'SolverRTKLc::AmbiguityFixing()'



      /** Update the free unknonws' solution and covariance after
       *  fixing the ambiguities
       */
   void SolverRTKLc::AmbiguityUpdate( Vector<double>& state,
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

   }  // End of method 'SolverRTKLc::AmbiguityUpdate()'



      // Correct the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverRTKLc::MeasUpdate( const Vector<double>& prefitResiduals,
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

   }  // End of method 'SolverRTKLc::MeasUpdate()'





      /* Code to be executed after 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverRTKLc::postCompute( gnssRinex& gData )
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

   }  // End of method 'SolverRTKLc::postCompute()'



      /* Sets if a NEU system will be used.
       *
       * @param useNEU  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverRTKLc& SolverRTKLc::setNEU( bool useNEU )
   {

         //>Firstly, fill the types that are source-indexed 
      
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
      srcIndexedTypes.insert(TypeID::cdtLC); // #5

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
      satIndexedTypes.insert(TypeID::BL1);    // #4

         // Now, we build the basic equation definition
      defaultEqDef.header = TypeID::prefitLdelta;
      defaultEqDef.body = srcIndexedTypes;

      return (*this);

   }  // End of method 'SolverRTKLc::setNEU()'


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
   SolverRTKLc& SolverRTKLc::setCoordinatesModel( StochasticModel* pModel )
   {

         // All coordinates will have the same model
      pCoordXStoModel = pModel;
      pCoordYStoModel = pModel;
      pCoordZStoModel = pModel;

      return (*this);

   }  // End of method 'SolverRTKLc::setCoordinatesModel()'


      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       */
   double SolverRTKLc::getSolution(const TypeID& type) const
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

   }  // End of method 'SolverRTKLc::getSolution()'


      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       */
   double SolverRTKLc::getFixedSolution(const TypeID& type) const
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

   }  // End of method 'SolverRTKLc::getSolution()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       */
   double SolverRTKLc::getVariance(const TypeID& type) const
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

   }  // End of method 'SolverRTKLc::getVariance()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       */
   double SolverRTKLc::getFixedVariance(const TypeID& type) const
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

   }  // End of method 'SolverRTKLc::getFixedVariance()'



      /** Return the CURRENT number of satellite.
       */
   int SolverRTKLc::getCurrentSatNumber() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numCurrentSV;
   }


      /** Return the CURRENT number of satellite.
       */
   int SolverRTKLc::getAmbFixedNumL1() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numFixedBL1;
   }



}  // End of namespace gpstk
