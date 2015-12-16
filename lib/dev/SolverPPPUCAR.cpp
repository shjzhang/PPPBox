#pragma ident "$Id: SolverPPPUCAR.cpp 2645 2012-11-05 03:23:24Z shjzhang $"

/**
 * @file SolverPPPUCAR.cpp
 * Class to compute the PPP Solution by fixing the ambiguities.
 */

//============================================================================
//
//  This file is a part of ROCKET software
//
//  The ROCKET software is based on GPSTK, RTKLIB and some other open source
//  GPS data processing software. The aim of ROCKET software is to compute
//  real-time GNSS orbit, clock and positions using GNSS data. 
//
//  The ROCKET software is developed at School of Geodesy and Geomatics, 
//  Wuhan University. 
//
//  Shoujian Zhang - Wuhan University, 2012
//
//============================================================================
//
//  Revision
//
//  2014/02/24  
//  create to process mixed model for PPP
//
//  2014/04/07  
//  solve the ppp solution with extended model by fixing the 
//  zero-difference ambiguities.
//
//  2015/12/07  
//  re-design the whole equation system. The L1&L2 ambiguity will be estimated
//  in the filter, and the WL/L1 ambiguities are fixed in sequence.
//
//  2015/12/09
//  The unknown ionospheric delay in the equation is 'slantIonoL1'
//
//============================================================================


#include "SolverPPPUCAR.hpp"
#include "MatrixFunctors.hpp"
#include "ARMLambda.hpp"
#include "PowerSum.hpp"

using namespace std;
using namespace gpstk::StringUtils;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverPPPUCAR::classIndex = 9300000;

      // Debug level for print out information
   int SolverPPPUCAR::debugLevel = 0;

      // Returns an index identifying this object.
   int SolverPPPUCAR::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPPPUCAR::getClassName() const
   { return "SolverPPPUCAR"; }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverPPPUCAR::SolverPPPUCAR(bool useNEU)
      : firstTime(true), converged(false), bufferSize(4),
        aprioriIonoVar(100.0), aprioriTropoVar(1.0),
        reInitialize(false), reInitialInterval(864000000.0)
   {

         // Set the equation system structure
      setNEU(useNEU);

         // Set the class index
      setIndex();

         // Call initializing method
      Init();

   }  // End of 'SolverPPPUCAR::SolverPPPUCAR()'


      // Initializing method.
   void SolverPPPUCAR::Init(void)
   {

         // Pointer to default stochastic model for troposphere (random walk)
      pTropoStoModel = &tropoModel;

         // Set default coordinates stochastic model (constant)
      setCoordinatesModel( &constantModel );

         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel   = &whitenoiseModel;
      pClockStoModelP2 = &whitenoiseModel;
      pClockStoModelL1 = &whitenoiseModel;
      pClockStoModelL2 = &whitenoiseModel;

         // Pointer to stochastic model for phase biases
      pAmbiModelL1 = &ambiModelL1;
      pAmbiModelL2 = &ambiModelL2;

         // Pointer to stochastic model for ionospheric delays
      pIonoModel = &ionoModel;


   }  // End of method 'SolverPPPUCAR::Init()'


      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPPPUCAR::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverPPPUCAR::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPPPUCAR::Process(gnssRinex& gData)
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

   }  // End of method 'SolverPPPUCAR::Process()'


      /* Code to be executed before 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverPPPUCAR::preCompute( gnssRinex& gData )
      throw(ProcessingException, SVNumException)
   {

      try
      {
         cout << static_cast<YDSTime>(gData.header.epoch).year  << "  ";     
         cout << static_cast<YDSTime>(gData.header.epoch).doy   << "  ";  
         cout << static_cast<YDSTime>(gData.header.epoch).sod   << endl; 

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
         ionoUnks.clear();

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

                    // If the type is for ionoL1, then keep it in 'ionoUnks' 
                 if( (*itType) == TypeID::ionoL1)
                 {
                    ionoUnks.insert(var);
                 }
             }

         }


         for( VariableSet::const_iterator itVar = varUnknowns.begin();
              itVar != varUnknowns.end();
              ++itVar )
         {
             cout << asString(*itVar) << endl;
         }


            // Get the number of satellites currently visible
         numCurrentSV =  gData.numSats();

            // Total measurement number
            // P1/P2/L1/L2/IonoL1/Trop
         numMeas = 4 * numCurrentSV + numCurrentSV + 1;

            // Number of 'src-indexed' variables i.e.  
            // 1 trospheric delay + 3 coordinates + 4 receiver clock 
         numVar = srcIndexedTypes.size();

            // Total number of unknowns is defined as :
            // numVar + 
            // numCurrentSV of ionospheric delays + 
            // numCurrentSV of L1 ambiguities
            // numCurrentSV of L2 ambiguities
         numUnknowns = numVar + 3*numCurrentSV;

            //=============================================
            //
            // Now, fill the measVector
            //
            //=============================================

            // Build the vector of measurements (Prefit-residuals): Code + phase
         measVector.resize(numMeas, 0.0);

  
         satTypeValueMap dummyC1(gData.body.extractTypeID(TypeID::prefitC1));
         satTypeValueMap dummyP1(gData.body.extractTypeID(TypeID::prefitP1));

            // Firstly, let's check the satellite number of C1 and P1 observables
         if( dummyC1.numSats() == 0 && dummyP1.numSats() ==0 )
         {
            GPSTK_THROW(ValueNotFound("Both C1 and P1 is not found in gRin!"));
         }

            // Get the prefit residuals from 'gData'
         Vector<double> prefitC;

            // If C1 is not empty, then use C1
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
            
         ////////////// ///////////////////////////////////////////////////
         //            
         //  Warning:
         //  The input ionospheric delays constraints should be match!
         //
         //  slant_iono_L1    = 1.0*slant_iono_type;
         //  vertical_iono_L1 = 1.0/iono_map*slant_iono_type;
         //
         //  slant_iono_L1    = 1.0*iono_map*vertical_iono_type;
         //  vertical_iono_L1 = 1.0*vertical_iono_type; 
         //
         ////////////// ///////////////////////////////////////////////////

            // Get apriori initial ionospheric delays from 'gData'
         Vector<double> aprioriIono(gData.getVectorOfTypeID(TypeID::ionoL1));

            // Warning: if there are no iono data, 'ZERO' data will be assigned
            //          to 'measVector'
         for( int i=0; i<numCurrentSV; i++ )
         {
            measVector( i + 4*numCurrentSV ) = aprioriIono(i);
         }
         cout << "aprioriIono" << aprioriIono << endl;

            // Get tropospheric delays from 'gData.header.source'
         double aprioriTropo( gData.header.source.zwdMap[TypeID::wetTropo] );

            // Warning: if there are no 'wetTropo' data, 'ZERO' data will be assigned
            //          to 'measVector'
         measVector( 5*numCurrentSV ) = aprioriTropo;

         cout << "aprioriTropo" << aprioriTropo << endl;

            //>Now, Fill the rMatrix

            // Weights matrix
         rMatrix.resize(numMeas, numMeas, 0.0);

            // Now, get the weight from 'gData'
         satTypeValueMap dummy(gData.body.extractTypeID(TypeID::weight));

            // 
            // Weight for observations
            //
         if ( dummy.numSats() == numCurrentSV )
         {

               // If we have weights information, let's load it
            Vector<double> wVec(gData.getVectorOfTypeID(TypeID::weight));

            for( int i=0; i<numCurrentSV; i++ )
            {
               rMatrix( i,                  i                  ) 
                   = 11.11*wVec(i); // P1, sig = 0.3; var=1.0/0.3 * 1.0/0.3
               rMatrix( i + 1*numCurrentSV, i + 1*numCurrentSV ) 
                   = 11.11*wVec(i); // P2, sig = 0.3; var=3.3*3.3                     
               rMatrix( i + 2*numCurrentSV, i + 2*numCurrentSV ) 
                   = 111111.11*wVec(i); // L1, sig=0.003; var=111111.11
               rMatrix( i + 3*numCurrentSV, i + 3*numCurrentSV ) 
                   = 111111.11*wVec(i); // L2, sig=0.003; var=111111.11

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

            }
         }

            // 
            // Weight for ionospheric delays
            //
         satTypeValueMap dummyIonoWeight(gData.body.extractTypeID(TypeID::ionoL1Weight));
         if ( dummyIonoWeight.numSats() == numCurrentSV )
         {
               // If we have weights information, let's load it
            Vector<double> wVec(gData.getVectorOfTypeID(TypeID::ionoL1Weight));

            for( int i=0; i<numCurrentSV; i++ )
            {
               rMatrix( i + 4*numCurrentSV, i + 4*numCurrentSV ) = wVec(i); 
            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'
         }
         else
         {
            for( int i=0; i<numCurrentSV; i++ )
            {
               rMatrix( i + 4*numCurrentSV, i + 4*numCurrentSV ) = 1.0/aprioriIonoVar; 
            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'
         }

         double tropoWeight(gData.header.source.zwdMap[TypeID::tropoWeight]);

           // If initial trop weight is given
         if ( tropoWeight != 0.0 )
         {
            rMatrix(5*numCurrentSV, 5*numCurrentSV) = tropoWeight ;
         }
         else
         {
            rMatrix(5*numCurrentSV, 5*numCurrentSV) = 1.0/aprioriTropoVar ;
         }

            /////////////////////////////////
            // Now, Fill the hMatrix
            /////////////////////////////////

            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns, 0.0);

            // Get the values corresponding to 'core' variables
         Matrix<double> dMatrix(gData.body.getMatrixOfTypes(commonUnkTypes));

            // Warning:
            // The common unknow type number is 4!
         if(commonUnkTypes.size() != 4)
         {
            InvalidSolver e("The common unknown type size is not equal with 4!");
            GPSTK_THROW(e);
         }

            // Let's fill 'hMatrix'
         for( int i=0; i<numCurrentSV; i++ )
         {
               // Common types include:
               // tropo, dx, dy, dz
            for( int j=0; j<4; j++ )
            {
               hMatrix( i                 , j ) = dMatrix(i,j); // C1 or P1
               hMatrix( i +   numCurrentSV, j ) = dMatrix(i,j); // P2
               hMatrix( i + 2*numCurrentSV, j ) = dMatrix(i,j); // L1
               hMatrix( i + 3*numCurrentSV, j ) = dMatrix(i,j); // L2
            }

               // Now, fill the reciever clock's coefficients for P1, P2, L1, L2
               // Warning: receiver clock for P1 and P2 should be the same ( ??? )
            hMatrix( i                 , 4 ) = 1.0; // P1
            hMatrix( i +   numCurrentSV, 5 ) = 1.0; // P2
               // Warning: receiver clock for L1 and L2 are different from 
               //          the common clock on Pc
            hMatrix( i + 2*numCurrentSV, 6 ) = 1.0; // L1
            hMatrix( i + 3*numCurrentSV, 7 ) = 1.0; // L2

         }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         ////////////// ///////////////////////////////////////////////////
         //            
         //             Furture plan
         //
         //  The mapping function for ionospheric delays should be
         //  applied to convert the slant ionospheric delays 
         //  to vertical ionospheric delays.
         //            
         ////////////// ///////////////////////////////////////////////////

            // Now, fill the coefficients related to ionospheric delays (ionoL1)
         for( int i=0; i<numCurrentSV; i++ )
         {
            hMatrix( i                 , numVar + i ) =  1.0; // P1
            hMatrix( i +   numCurrentSV, numVar + i ) =  GAMMA_GPS; // P2 
            hMatrix( i + 2*numCurrentSV, numVar + i ) = -1.0; // L1
            hMatrix( i + 3*numCurrentSV, numVar + i ) = -GAMMA_GPS; // L2 
         }  

            // Now, fill the coefficients related to the ambiguities
         for( int i=0; i<numCurrentSV; i++ )
         {
               // ambL1 are listed after ( numVar + ionoL1 )
            hMatrix( i + 2*numCurrentSV, numVar +   numCurrentSV + i ) 
                = -0.190293672798;
               // ambL2 are listed after ( numVar + ionoL1 + ambL1 )
            hMatrix( i + 3*numCurrentSV, numVar + 2*numCurrentSV + i ) 
                = -0.244210213425;
         }                                                    

            // Now, fill the coefficients for 'aprioriIono'
         for(int i=0; i<numCurrentSV;i++)
         {
            hMatrix(i + 4*numCurrentSV, numVar + i ) = 1.0;
         }
            
            // Now, fill the coefficients for 'aprioriTropo'
         hMatrix( 5*numCurrentSV, 0 ) = 1.0;

            ////////////////////////////////////////////////
            //          
            // Now, Fill the phiMatrix and qMatrix
            //          
            ////////////////////////////////////////////////

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

            // Third, the PC receiver clock
         pClockStoModel->Prepare( dummySat,
                                  gData );
         phiMatrix(4,4) = pClockStoModel->getPhi();
         qMatrix(4,4)   = pClockStoModel->getQ();

            // Fourth, the P2 receiver clock 
         pClockStoModelP2->Prepare( dummySat,
                                  gData );
         phiMatrix(5,5) = pClockStoModelP2->getPhi();
         qMatrix(5,5)   = pClockStoModelP2->getQ();

            // Fifth, the L1 receiver clock 
         pClockStoModelL1->Prepare( dummySat,
                                  gData );
         phiMatrix(6,6) = pClockStoModelL1->getPhi();
         qMatrix(6,6)   = pClockStoModelL1->getQ();

            // Sixth, the receiver clock on L2
         pClockStoModelL2->Prepare( dummySat,
                                  gData );
         phiMatrix(7,7) = pClockStoModelL2->getPhi();
         qMatrix(7,7)   = pClockStoModelL2->getQ();


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
            initialErrorCovariance(0,0) = 0.04;          // (0.2 m)**2

               // Second, the coordinates
            for( int i=1; i<4; i++ )
            {
               initialErrorCovariance(i,i) = 1.0;       // (1.0 m)**2
            }

            initialErrorCovariance(4,4) = 9.0e10;
            initialErrorCovariance(5,5) = 9.0e10;
            initialErrorCovariance(6,6) = 9.0e10;
            initialErrorCovariance(7,7) = 9.0e10;

               // Third, the ionospheric delays
            for( int i=numVar; i<numVar+numCurrentSV; i++ )
            {
               initialErrorCovariance(i,i) = 0.25;     // (0.1 m)**2
            }

               // Finally, the ambiguities 
            for( int i=numVar+numCurrentSV; i<numUnknowns; i++ )
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
            resetL1 = resetWL = resetSol = true;
            startTime = sod;
            startTimeVec.push_back(startTime);

               // No longer first time
            firstTime = false;

         }
         else
         {

               // Current Epoch
            CommonTime currEpoch(gData.header.epoch);

            cout << "currEpoch" << currEpoch << endl;
            cout << "firstEpoch" << firstEpoch << endl;

               // Offset
            double timeOffset( currEpoch - firstEpoch );

            cout << "timeOffset" << timeOffset << endl;

            double sod( (gData.header.epoch).getSecondOfDay() );

            double tolerance(0.5);
            double lowerBound( std::abs(tolerance) );
            double upperBound( std::abs(reInitialInterval - tolerance) );

               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);

               // Reset all the state/covariance matrix
            if( ( (int)(timeOffset) % (int)(reInitialInterval) < lowerBound ) || 
                ( (int)(timeOffset) % (int)(reInitialInterval) > upperBound ) )
            {
                  // set the flags for ttff/ttfs statistics
               resetL1 = resetWL = resetSol = true;
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

                  // Then, Fill the state and covariance matrix for 
                  // ionospheric delay variables

               int c1(numVar);      // Set an index
               for( VariableSet::const_iterator itVar = ionoUnks.begin();
                    itVar != ionoUnks.end();
                    ++itVar )
               {
                  currentState(c1) = stateMap[ (*itVar) ];
                  ++c1;
               }

                  // Fill the covariance matrix
                  
                  // We need a copy of 'ionoUnks'
               VariableSet tempSet( ionoUnks);

               c1 = numVar;         // Reset 'c1' index

               for( VariableSet::const_iterator itVar1 = ionoUnks.begin();
                    itVar1 != ionoUnks.end();
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
                     // ionospheric delay variables

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

               }  // End of for( VariableSet::const_iterator itVar1 = ionoUnks...'


                  // Then, reset the ambiguity, which is equivalent
                  // to introducing cycle slips for all satellites.
               for( int i=numVar+numCurrentSV; i<numUnknowns; i++ )
               {
                  currentErrorCov(i,i) = 4.0e14;     // (20000 km)**2
               }
                  
            }
            else // Update normally !
            {
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

   }  // End of method 'SolverPPPUCAR::preCompute()'



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
   gnssRinex& SolverPPPUCAR::Compute( gnssRinex& gData )
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

   }  // End of method 'SolverPPPUCAR::Compute()'


    

      // Predict the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverPPPUCAR::TimeUpdate( const Matrix<double>& phiMatrix,
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

   }  // End of method 'SolverPPPUCAR::TimeUpdate()'


   gnssRinex& SolverPPPUCAR::AmbiguityConstr( gnssRinex& gData )
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

            // Now, get the ambiguity on L1/L2
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
  
            // Set the ambiguity estimates and covariance for 'datumL1'
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

   }  // End of method 'SolverPPPUCAR::AmbiguityConstr()'



      // Now, fix all the ambiguities to integers 
   gnssRinex& SolverPPPUCAR::AmbiguityFixing( gnssRinex& gData )
      throw(ProcessingException)
   {

      try
      {

         Vector<double> ambWLFixed(numCurrentSV, 0.0);
         Vector<double> ambWLFlag(numCurrentSV,0.0);

         Vector<double> ambL1Fixed(numCurrentSV, 0.0);
         Vector<double> ambL1Flag(numCurrentSV,0.0);

         Vector<double> ambL2Fixed(numCurrentSV, 0.0);
         Vector<double> ambL2Flag(numCurrentSV,0.0);

            // Flags indicating whether this unknown variable has been fixed.
         Vector<double> stateFlag(numUnknowns, 0.0);

            /////////////////////////////////////////////
            //
            //  Reference:
            //  de Jonge & Tiberius, LGR, 1996
            //  D.N. Dong, Y. Bock, Journal of Geophysical Research, 1989.
            //
            //  Firstly convert [X BL1 BL2] to [X BL1 BWL]
            //
            //  [X BL1 BL2] => [X  BL1 BWL]
            //
            //  Then, update the Y = [ X BL1 ] by fixing the BWL. 
            //
            //  v   ^
            //  Y = Y - QywQww(BWL - NWL)
            //
            //  Afterwards, update the X by fixing the BL1. 
            //  _   v
            //  X = X - Qx1Q11(BL1 - NL1)
            //
            /////////////////////////////////////////////

            ////////////////////////////////////////////////////////
            //
            // Now, Let's define the mapping matrix for converting
            //
            //       | I            ||  X  |
            //       |   1          || b11 |
            //       |     1        || b12 |
            //       !      ...     !! ... |
            //       |              !!     !
            // Map = |   1    -1    || b21 |
            //       |     1    -1  || b22 |
            //       !      ...  ...!! ... |
            //
            // Xprime = M*X
            // XprimCov = M*Qxx*M^T
            //
            ////////////////////////////////////////////////////////

         Matrix<double> mapMatrix(numUnknowns,numUnknowns,0.0);
           
            // For trop,coord,rcv clk,iono,and BL1
            // don't change
         for(int i=0; i<numVar + 2*numCurrentSV;i++)
         {
            mapMatrix(i,i) = 1.0;
         }

            // For BWL, BWL(isat) = BL1(isat) - BL2(isat)
         for(int i=numVar+2*numCurrentSV; i<numUnknowns;i++)
         {
               // col of BL2
            mapMatrix(i, i) = -1.0;
               
               // col of BL1
            mapMatrix(i, (i-numCurrentSV) ) = 1.0;
         }

         Matrix<double> mT  = transpose(mapMatrix) ;

            // Warning: 
            // now, the ambiguities will be arranged 
            // as [x b1 bw] 
         newState = mapMatrix * solution;
         newCov = mapMatrix * covMatrix * mT;

         std::map<SatID, double> ambL1Map;
         std::map<SatID, double> ambL2Map;
         std::map<SatID, std::map< SatID, double > > covAmbL1Map;
         std::map<SatID, std::map< SatID, double > > covAmbL2Map;
         std::map<SatID, double> ambL1FixedMap;
         std::map<SatID, double> ambL2FixedMap;
           
            //
            // Now, Let's fix the WL firstly, then the L1 or L2 ambiguity
            //
         std::map<SatID, double> ambWLMap;
         std::map<SatID, std::map< SatID, double > > covAmbWLMap;
         std::map<SatID, double> ambWLFixedMap;

            // Store values of ambiguity values
         int c1(0);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
            ambWLMap[ (*itSat) ] = newState( numVar + 2*numCurrentSV + c1 );
            c1++;
         }

            /////////////////////////////////////
            // Store values of covariance matrix
            /////////////////////////////////////
            
            // we need a copy of 'currSatSet'
         SatIDSet tempSatSet(currSatSet);
            
         c1 = 0;     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
            covAmbWLMap[(*itSat)][(*itSat)] 
                = newCov(numVar+2*numCurrentSV+c1, numVar+2*numCurrentSV+c1);

            int c2 = (c1+1);

               // Erase current satellite from 'tempSatSet'
            tempSatSet.erase(*itSat);

            for( SatIDSet::const_iterator itSat2 = tempSatSet.begin();
                 itSat2 != tempSatSet.end();
                 ++itSat2 )
            {
                covAmbWLMap[(*itSat)][(*itSat2)] 
                    = newCov(numVar+2*numCurrentSV+c1, numVar+2*numCurrentSV+c2);
                c2++;
            }
               // Increment
            c1++;
         }

            // Get widelane ambiguity information from 'newState/newCov'
         SatIDSet validSatSet(currSatSet);

            // Now, Let's fix the widelane ambiguities using LAMBDA
         ARMLambda mlambda;

         double ratioWL(0.0);

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

                    // Erase current satellite
                 int c2(c1+1);
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
             
             cout << "tempAmb:" << tempAmb << endl;
             cout << "tempCov:" << tempCov << endl;

                // Fill 'mlambda' with 'tempAmb/tempCov'
             mlambda.resolve(tempAmb, tempCov);
             ratioWL = mlambda.getRatio();

                // Get fixed ambiguity vector
             Vector<double> ambFixedVec = mlambda.getFixedAmbVec();

             cout << "iter:" << iter << endl;
             cout << "numSV:" << numSV << endl;
             cout << "ratioWL:" << ratioWL << endl;

                // Fixing ratio
             if( (ratioWL>3.0) )
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
                 double tempMaxVar(0.0);

                    // Temporary satellite
                 int c1(0);
                 for( SatIDSet::iterator itSat=validSatSet.begin();
                      itSat !=validSatSet.end();
                      ++itSat )
                 {
                     double var = covAmbWLMap[(*itSat)][(*itSat)];
                     if( var > tempMaxVar )
                     {
                         tempMaxVar = var;
                         tempSat = (*itSat);
                     }
                 }

                    // remove this satellite from validSatSet 
                 validSatSet.erase(tempSat);

             }

               // Iter increment
             iter++;

         };  // End of 'while(validSatSet.size() >1)'


            // Update the solution/state with fixed widelane ambiguities
            // Warning: 
            // There are possibilites that the ratioWL is less than 3.0
            // and the program runs to this place. 
            // for example that the number is less than 2 ( change according to
            // your setting )
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

               int index = numVar+2*numCurrentSV+jsat;

                  // Update the solution and covarinace  
               AmbiguityUpdate(newState, newCov, stateFlag, index, bwFixed );

                  // Not free at all
               stateFlag(index) = 1.0;
            }

            cout << "update solution/state" << endl;

            cout << "stateFlag:" << stateFlag << endl;
            cout << "newState:" << newState << endl;
            cout << "newCov:" << newCov << endl;

               // Update the number of the fixed widelane ambiguities
            numFixedBWL = validSatSet.size();

         }

            ///////////////////////////////////////////
            // Now, comes to the L1 ambiguity fixing.
            ///////////////////////////////////////////

         c1 = 0;     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
            ambL1Map[ (*itSat) ] = newState( numVar + 1*numCurrentSV + c1 );
            c1++;
         }
            // Store values of covariance matrix

            // we need a copy of 'currSatSet'
         tempSatSet = currSatSet;
            
         c1 = 0;     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
            covAmbL1Map[(*itSat)][(*itSat)] 
                = newCov(numVar+numCurrentSV+c1, numVar+numCurrentSV+c1);

            int c2 = (c1+1);

               // Erase current satellite from 'tempSatSet'
            tempSatSet.erase(*itSat);
            for( SatIDSet::const_iterator itSat2 = tempSatSet.begin();
                 itSat2 != tempSatSet.end();
                 ++itSat2 )
            {
                covAmbL1Map[(*itSat)][(*itSat2)] 
                    = newCov(numVar+numCurrentSV+c1, numVar+numCurrentSV+c2);
                c2++;
            }
            c1++;
         }

            // Now, if the widelane ambiguities can be fixed 
         if( ratioWL>3.0 )
         {
               // min decisioin
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
                Vector<double> ambL1Fixed = mlambda.getFixedAmbVec();


                cout << "iter:" << iter << endl;
                cout << "numSV:" << numSV << endl;
                cout << "ratioL1:" << ratioL1 << endl;
                cout << "ambL1Fixed:" << ambL1Fixed << endl;

                   // Fixing ratio
                   // Two different control parameter!
                if( (ratioL1>3.0) )
                {
                       // Store the fixed ambiguities into 'ambL1FixedMap'
                    int c1(0);
                    for( SatIDSet::iterator itSat=validSatSet.begin();
                         itSat !=validSatSet.end();
                         ++itSat )
                    {
                           // Fixed L1 ambiguities
                        ambL1FixedMap[(*itSat)] = ambL1Fixed(c1);

                        cout << "(*itSat)"  << (*itSat)
                             << "L1:" << ambL1FixedMap[(*itSat)] 
                             << "WL:" << ambWLFixedMap[(*itSat)]
                             << endl;
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
                        double var = covAmbL1Map[(*itSat)][(*itSat)] ;
                        if( var > tempMaxVar )
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

            };

            
                // Update the solution/state with fixed LC ambiguities
            if(ratioL1>3.0)
            {
                  // Loop the validSatSet
               for( SatIDSet::iterator itSat=validSatSet.begin();
                    itSat !=validSatSet.end();
                    ++itSat )
               {
                     // Fixed L1 ambiguity
                  double b1Fixed = ambL1FixedMap[(*itSat)];

                     // Find the position of current satellite.
                  int jsat(0);
                  SatIDSet::const_iterator itSat2( currSatSet.begin() );
                  while( (*itSat2) != (*itSat) )
                  {
                      ++jsat;
                      ++itSat2;
                  }

                  int index = numVar+numCurrentSV+jsat;

                     // Update the solution and covarinace  
                  AmbiguityUpdate(newState, newCov, stateFlag, index, b1Fixed );

                     // Not free at all
                  stateFlag(index) = 1.0;
               }

               cout << "update solution/state" << endl;

               cout << "stateFlag:" << stateFlag << endl;
               cout << "newState:" << newState << endl;
               cout << "newCov:" << newCov << endl;

                  // Update the number of the fixed widelane ambiguities
               numFixedBL1 = validSatSet.size();

            }  // End of 'if(ratioL1 > 3.0)'

         }  // End of 'if(ratioWL > 3.0)'

         double sod( (gData.header.epoch).getSecondOfDay() );

            // Now, Let's update the ttff information
         if(resetWL)
         {
             if(numFixedBWL>4)
             {
                double ttff = sod - startTime;
                cout << "startTime" << startTime << endl;
                cout << "WL ttff:" << ttff << endl;
                ttffWL.push_back(ttff);  
                resetWL = false;
             }
         }

         cout << "resetWL" << resetWL << "resetL1" << resetL1 << endl;
         cout << "numFixedBL1" << numFixedBL1 << endl;
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

         for(int i=0; i<numCurrentSV; i++)
         {
             ambWLFixed(i) = newState(numVar+2*numCurrentSV+i);
             ambWLFlag(i)  = stateFlag(numVar+2*numCurrentSV+i);
         }

            // Now, get the widelane ambiguity 
         for(int i=0; i<numCurrentSV; i++)
         {
             ambL1Fixed(i) = newState(numVar+numCurrentSV+i);
             ambL1Flag(i)  = stateFlag(numVar+numCurrentSV+i);
         }

         for(int i=0; i<numCurrentSV; i++)
         {
             ambL2Fixed(i) = ambL1Fixed(i) - ambWLFixed(i);
             ambL2Flag(i)  = ambL1Flag(i);
         }

            //
            // Now, convert the transformed newState/newCov to the
            // raw values.
            //
        
         Vector<double> tempState ;
         Matrix<double> tempCov;

         Matrix<double> mInv  = inverseChol(mapMatrix) ;
         Matrix<double> mInvT = transpose(mInv) ;

            // Transform newState/newCov back to the L1/L2 ambiguity related ones
         tempState  = mInv * newState;
         tempCov = mInv * newCov * mInvT;

         newState = tempState;
         newCov = tempCov;

            //
            // Now, Let's compute the postfit-residual                  
            //

            // Compute the postfit residuals Vector
         postfitResiduals = measVector - (hMatrix* newState);

            // Now, add the pseudorange postfit residuals 
         Vector<double> postfitC(numCurrentSV,0.0);
         Vector<double> postfitP2(numCurrentSV,0.0);

            // Now we have to add the new values to the data structure
         Vector<double> postfitL1(numCurrentSV,0.0);
         Vector<double> postfitL2(numCurrentSV,0.0);

         for( int i=0; i<numCurrentSV; i++ )
         {
            postfitC(i)  = postfitResiduals( i );
            postfitP2(i) = postfitResiduals( i + 1*numCurrentSV );
            postfitL1(i) = postfitResiduals( i + 2*numCurrentSV );
            postfitL2(i) = postfitResiduals( i + 3*numCurrentSV );
         }

            ////////////////////////////////////////////////////////
            //
            // Postfit residual testing is to be applied in the future!
            //
            ////////////////////////////////////////////////////////

            //*************************************************
            //
            // Now, Let's get the wetTropo/iono 
            //
            //*************************************************

            // Firstly, insert the wet tropospheric delays
            
         double wetTropo(0.0);
         wetTropo = newState(0);
         gData.header.source.zwdMap[TypeID::wetTropo] = wetTropo;

            // Secondly, insert the ionospheric delays into body of the 'gnssRinex'

            // Now, get the ionoL1 from 'currentState'
         Vector<double> ionoL1(numCurrentSV,0.0);

            // Get ionospheric delays from solution
         for( int i=0; i<numCurrentSV; i++ )
         {
             ionoL1(i) = newState( numVar + i );
         }
         gData.insertTypeIDVector(TypeID::ionoL1, ionoL1);

            // Insert the fixed ambiguity flags for RTX positioning
         gData.insertTypeIDVector(TypeID::BL1Flag, ambL1Flag);

         //*******************************************************
         //                                                       
         //  Only the float solution/covMatrix are transferred    
         //                                                       
         //*******************************************************
  
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

   }  // End of method 'SolverPPPUCAR::AmbiguityFixing()'



      /** Update the free unknonws' solution and covariance after
       *  fixing the ambiguities
       */
   void SolverPPPUCAR::AmbiguityUpdate( Vector<double>& state,
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

   }  // End of method 'SolverPPPUCAR::AmbiguityUpdate()'



      // Correct the state vector and covariance matrix
      //
      // @param gData    Data object holding the data.
      //
   int SolverPPPUCAR::MeasUpdate( const Vector<double>& prefitResiduals,
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

   }  // End of method 'SolverPPPUCAR::MeasUpdate()'





      /* Code to be executed after 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverPPPUCAR::postCompute( gnssRinex& gData )
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

   }  // End of method 'SolverPPPUCAR::postCompute()'


      /* Sets if a NEU system will be used.
       *
       * @param useNEU  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverPPPUCAR& SolverPPPUCAR::setNEU( bool useNEU )
   {
         //
         //  Firstly, let's clear all the set that storing the variable types.
         //  or else, SolverPPPARFB will throw error!
         //  if you don't clear these set, the number of this sets used in 
         //  equation generation will be wrong!
         //

      srcIndexedTypes.clear();
      satIndexedTypes.clear();
      commonUnkTypes.clear();

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
      srcIndexedTypes.insert(TypeID::cdtP2); // #5
      srcIndexedTypes.insert(TypeID::cdtL1); // #6  
      srcIndexedTypes.insert(TypeID::cdtL2); // #7

         //>Then, fill the types that are satellite-indexed

         // The order of the elment is as follows:
      satIndexedTypes.insert(TypeID::ionoL1); // #2
      satIndexedTypes.insert(TypeID::BL1);    // #3
      satIndexedTypes.insert(TypeID::BL2);    // #4


         //>Firstly, fill the types that are common for all observables
      commonUnkTypes.insert(TypeID::wetMap);

      if (useNEU)
      {
         commonUnkTypes.insert(TypeID::dLat); // #2
         commonUnkTypes.insert(TypeID::dLon); // #3
         commonUnkTypes.insert(TypeID::dH);   // #4
      }
      else
      {
         commonUnkTypes.insert(TypeID::dx);   // #2
         commonUnkTypes.insert(TypeID::dy);   // #3
         commonUnkTypes.insert(TypeID::dz);   // #4
      }

         // Now, we build the basic equation definition
      defaultEqDef.header = TypeID::prefitC1;
      defaultEqDef.body = srcIndexedTypes;

      return (*this);

   }  // End of method 'SolverPPPUCAR::setNEU()'


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
   SolverPPPUCAR& SolverPPPUCAR::setCoordinatesModel( StochasticModel* pModel )
   {

         // All coordinates will have the same model
      pCoordXStoModel = pModel;
      pCoordYStoModel = pModel;
      pCoordZStoModel = pModel;

      return (*this);

   }  // End of method 'SolverPPPUCAR::setCoordinatesModel()'

      /** Set a random walk stochastic model to slant ionospheric delays.
       *
       * @param pModel      Pointer to StochasticModel associated with
       *                    ionospheric delays.
       */
   SolverPPPUCAR& SolverPPPUCAR::setIonosphericModel(StochasticModel* pModel)
   {
         // All coordinates will have the same model
      pIonoModel = pModel;
      return (*this);
   };



      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       */
   double SolverPPPUCAR::getSolution(const TypeID& type) const
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

   }  // End of method 'SolverPPPUCAR::getSolution()'


      /* Returns the solution associated to a given TypeID.
       *
       * @param type    TypeID of the solution we are looking for.
       */
   double SolverPPPUCAR::getFixedSolution(const TypeID& type) const
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

   }  // End of method 'SolverPPPUCAR::getSolution()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       */
   double SolverPPPUCAR::getVariance(const TypeID& type) const
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

   }  // End of method 'SolverPPPUCAR::getVariance()'



      /* Returns the variance associated to a given TypeID.
       *
       * @param type    TypeID of the variance we are looking for.
       */
   double SolverPPPUCAR::getFixedVariance(const TypeID& type) const
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

   }  // End of method 'SolverPPPUCAR::getFixedVariance()'



      /** Return the CURRENT number of satellite.
       */
   int SolverPPPUCAR::getCurrentSatNumber() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numCurrentSV;
   }

      /** Return the CURRENT number of satellite.
       */
   int SolverPPPUCAR::getAmbFixedNumWL() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numFixedBWL;
   }

      /** Return the CURRENT number of satellite.
       */
   int SolverPPPUCAR::getAmbFixedNumL1() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return numFixedBL1;
   }


}  // End of namespace gpstk
