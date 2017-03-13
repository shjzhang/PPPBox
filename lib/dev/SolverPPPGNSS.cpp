#pragma ident "$Id$"

/**
 * @file SolverPPPGNSS.cpp
 * Class to compute the PPP Solution.
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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//============================================================================
//  
//  Revision
//
//  2014/03/01   Change the state/covariance transferring. In the previous
//               version, the satSet will be inserted into currSatSet. Now, 
//               only the currSatSet is used.
//
//  2014/03/10   Add the 'SVNumException', which will throw exception if the
//               observed satellite number is less than 4.
//
//  2014/03/10   The 'SVNumException' and 'ProcessingException' should be
//               seperated. So, the 'SVNumException' throw should be thrown
//               independently.
//
//  2015/11/06   No Longer throw SVNumException
//
//============================================================================


#include "SolverPPPGNSS.hpp"
#include "MatrixFunctors.hpp"

using namespace std;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverPPPGNSS::classIndex = 9300000;

      // Returns an index identifying this object.
   int SolverPPPGNSS::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPPPGNSS::getClassName() const
   { return "SolverPPPGNSS"; }


      /* Common constructor.
       *
       * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       *
       */
   SolverPPPGNSS::SolverPPPGNSS(bool useNEU)
      : firstTime(true), converged(false), bufferSize(4)
   {

         // Set the equation system structure
      setNEU(useNEU);

         // Call initializing method
      Init();

   }  // End of 'SolverPPPGNSS::SolverPPP()'


      // Initializing method.
   void SolverPPPGNSS::Init(void)
   {

         // Set qdot value for default random walk stochastic model
      rwalkModel.setQprime(3e-8);
      rwalkModel2.setQprime(3e-4);
      rwalkModel3.setQprime(3e-4);

         // Pointer to default stochastic model for troposphere (random walk)
      pTropoStoModel = &rwalkModel;

         // Pointer to default stochastic model for BeiDou ISB and Galileo ISB
      pISBForBDSStoModel = &rwalkModel2;
      pISBForGALStoModel = &rwalkModel3;

         // Pointer to default stochastic model for Glonass ISB
      pISBForGLOStoModel = &GloISBModel;

         // Set default coordinates stochastic model (constant)
      setCoordinatesModel( &constantModel );


      whitenoiseModelX.setSigma(100.0);
      whitenoiseModelY.setSigma(100.0);
      whitenoiseModelZ.setSigma(100.0);

         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel = &whitenoiseModel;

         // Pointer to stochastic model for phase biases
      pBiasStoModel  = &biasModel;

         // for BeiDou phase biases
      BDSBiasModel.setCycleSlipFlag(TypeID::CSL2);
      pBDSBiasStoModel  = &BDSBiasModel;

         // Set default factor that multiplies phase weights
         // If code sigma is 1 m and phase sigma is 1 cm, the ratio is 100:1
      weightFactor = 10000.0;       // 100^2


   }  // End of method 'SolverPPPGNSS::Init()'



      /* Compute the solution of the given equations set.
       *
       * @param prefitResiduals   Vector of prefit residuals
       * @param designMatrix      Design matrix for the equation system
       * @param weightVector      Vector of weights assigned to each
       *                          satellite.
       *
       * \warning A typical Kalman filter works with the measurements noise
       * covariance matrix, instead of the vector of weights. Beware of this
       * detail, because this method uses the later.
       *
       * @return
       *  0 if OK
       *  -1 if problems arose
       */
   int SolverPPPGNSS::Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Vector<double>& weightVector )
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;

         // First, check that everyting has a proper size
      int wSize = static_cast<int>(weightVector.size());
      int pSize = static_cast<int>(prefitResiduals.size());
      if (!(wSize==pSize))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
of weightVector");
         GPSTK_THROW(e);
      }

      Matrix<double> wMatrix(wSize,wSize,0.0);  // Declare a weight matrix

         // Fill the weight matrix diagonal with the content of
         // the weights vector
      for( int i=0; i<wSize; i++ )
      {
         wMatrix(i,i) = weightVector(i);
      }

         // Call the more general SolverPPPGNSS::Compute() method
      return SolverPPPGNSS::Compute( prefitResiduals,
                                     designMatrix,
                                     wMatrix );

   }  // End of method 'SolverPPPGNSS::Compute()'



      // Compute the solution of the given equations set.
      //
      // @param prefitResiduals   Vector of prefit residuals
      // @param designMatrix      Design matrix for equation system
      // @param weightMatrix      Matrix of weights
      //
      // \warning A typical Kalman filter works with the measurements noise
      // covariance matrix, instead of the matrix of weights. Beware of this
      // detail, because this method uses the later.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int SolverPPPGNSS::Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix,
                           const Matrix<double>& weightMatrix )
      throw(InvalidSolver)
   {

         // By default, results are invalid
      valid = false;

      if (!(weightMatrix.isSquare()))
      {
         InvalidSolver e("Weight matrix is not square");
         GPSTK_THROW(e);
      }

      int wRow = static_cast<int>(weightMatrix.rows());
      int pRow = static_cast<int>(prefitResiduals.size());
      if (!(wRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension of \
weightMatrix");
         GPSTK_THROW(e);
      }

      int gRow = static_cast<int>(designMatrix.rows());
      if (!(gRow==pRow))
      {
         InvalidSolver e("prefitResiduals size does not match dimension \
of designMatrix");
         GPSTK_THROW(e);
      }

      if (!(phiMatrix.isSquare()))
      {
         InvalidSolver e("phiMatrix is not square");
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

         // After checking sizes, let's invert the matrix of weights in order
         // to get the measurements noise covariance matrix, which is what we
         // use in the "SimpleKalmanFilter" class
      Matrix<double> measNoiseMatrix;

      try
      {
         measNoiseMatrix = inverseChol(weightMatrix);
      }
      catch(...)
      {
         InvalidSolver e("Correct(): Unable to compute measurements noise \
covariance matrix.");
         GPSTK_THROW(e);
      }


      try
      {
            // Call the Kalman filter object.
         kFilter.Compute( phiMatrix,
                          qMatrix,
                          prefitResiduals,
                          designMatrix,
                          measNoiseMatrix );

      }
      catch(InvalidSolver& e)
      {
         GPSTK_RETHROW(e);
      }

         // Store the solution
      solution = kFilter.xhat;

         // Store the covariance matrix of the solution
      covMatrix = kFilter.P;

         // Compute the postfit residuals Vector
      postfitResiduals = prefitResiduals - (designMatrix * solution);

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverPPPGNSS::Compute()'



      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPPPGNSS::Process(gnssSatTypeValue& gData)
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
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPGNSS::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPPPGNSS::Process(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {
         // Get a set with all satellites present in this GDS
      SatIDSet currSatSet( gData.body.getSatID() );
         // Get a set with Glonass satellites present
      SatIDSet GloSatSet;
         // loop in currSatSet
      for ( SatIDSet::const_iterator it = currSatSet.begin();
            it != currSatSet.end();
            it++)
      { 
          if ((*it).system == SatID::systemGlonass)
          {
              GloSatSet.insert(*it);
          }
      }
         // the number of Glonass satellites
      int numGloSV (GloSatSet.size());
         // Get the number of satellites currently visible
      int numCurrentSV( gData.numSats() );
      try
      {
            // Number of measurements is twice the number of visible satellites
         numMeas = 2 * numCurrentSV;

            // Number of 'core' variables: Coordinates, RX clock, troposphere
         numVar = defaultEqDef.body.size();
         if ( useGPS && useGlonass)
         {
             // Total number of unknowns is defined as variables + processed SVs
             // + Glonass SVs(for ISB)
             numUnknowns = numVar + numCurrentSV + numGloSV;
         }
         else
         {
             numUnknowns = numVar + numCurrentSV;
         }
            // State Transition Matrix (PhiMatrix)
         phiMatrix.resize(numUnknowns, numUnknowns, 0.0);

            // Noise covariance matrix (QMatrix)
         qMatrix.resize(numUnknowns, numUnknowns, 0.0);


            // Build the vector of measurements (Prefit-residuals): Code + phase
         measVector.resize(numMeas, 0.0);

         Vector<double> prefitC(gData.getVectorOfTypeID(defaultEqDef.header));
         Vector<double> prefitL(gData.getVectorOfTypeID(TypeID::prefitL));

         for( int i=0; i<numCurrentSV; i++ )
         {
            measVector( i                ) = prefitC(i);
            measVector( numCurrentSV + i ) = prefitL(i);
         }

            // Weights matrix
         rMatrix.resize(numMeas, numMeas, 0.0);

            // Generate the appropriate weights matrix
            // Try to extract weights from GDS
         satTypeValueMap dummy(gData.body.extractTypeID(TypeID::weight));


            // Check if weights match
         if ( dummy.numSats() == numCurrentSV )
         {

               // If we have weights information, let's load it
            Vector<double>
               weightsVector(gData.getVectorOfTypeID(TypeID::weight));

            int i(0);
            for (SatIDSet::const_iterator it = currSatSet.begin();
                 it != currSatSet.end();
                 ++it)
            {
               if ((*it).id == 01 || (*it).id == 02 || (*it).id == 03 ||
                    (*it).id == 04 || (*it).id == 05)
               {
               rMatrix( i               , i         ) = 0.1*weightsVector(i);
               rMatrix( i + numCurrentSV, i + numCurrentSV )
                                            = 0.1*weightsVector(i) * weightFactor;
               }
               else
               {
                   
               rMatrix( i               , i         ) = weightsVector(i);
               rMatrix( i + numCurrentSV, i + numCurrentSV )
                                            = weightsVector(i) * weightFactor;
               }
                   
                   
               ++i;

            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         }
         else
         {

               // If weights don't match, assign generic weights
            int i(0);
            for (SatIDSet::const_iterator it = currSatSet.begin();
                 it != currSatSet.end();
                 ++it)
            {
               if ((*it).id == 01 || (*it).id == 02 || (*it).id == 03 ||
                    (*it).id == 04 || (*it).id == 05)
               {
               rMatrix( i               , i         ) = 0.2;
               rMatrix( i + numCurrentSV, i + numCurrentSV )
                                            = 0.2* weightFactor;
               }
               else
               {
                   
               rMatrix( i               , i         ) = 1.0;
               rMatrix( i + numCurrentSV, i + numCurrentSV )
                                            = 1.0 * weightFactor;
               }

               ++i;

            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'


          }
            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns, 0.0);
            // Get the values corresponding to 'core' variables
         Matrix<double> dMatrix(gData.body.getMatrixOfTypes(defaultEqDef.body));
            // Let's fill 'hMatrix'
         for( int i=0; i<numCurrentSV; i++ )
         {

               // First, fill the coefficients related to tropo, coord , clock
               // and ISB (if use BeiDou or Galileo)
            for( int j=0; j<numVar; j++ )
            {

               hMatrix( i               , j ) = dMatrix(i,j);
               hMatrix( i + numCurrentSV, j ) = dMatrix(i,j);

            }

         }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

            // Now, fill the coefficients related to phase biases
            // We must be careful because not all processed satellites
            // are currently visible
         int count1(0);
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Put coefficient in the right place
            hMatrix( count1 + numCurrentSV, count1 + numVar ) = 1.0;

            ++count1;

         }  // End of 'for( itSat = currSatSet.begin(); ... )'
            
            // Now ,fill the coefficients realted to glonass ISB
         if ( useGPS && useGlonass )
         {
           int count2(0);

           for (SatIDSet::const_iterator itGloSat = GloSatSet.begin();
                itGloSat != GloSatSet.end();
                itGloSat++)
           {
              size_t index = std::distance(currSatSet.begin(),currSatSet.find(*itGloSat));
                  // code
              hMatrix(index,count2+numVar+numCurrentSV) = 1.0;
                  // phase
              hMatrix(index+numCurrentSV,count2+numVar+numCurrentSV) = 1.0;
         
              count2++;
           }
             
         }
            // Now, let's fill the Phi and Q matrices
         SatID  dummySat;

            // First, the troposphere
         pTropoStoModel->Prepare( dummySat,
                                  gData );
         phiMatrix(0,0) = pTropoStoModel->getPhi();
         qMatrix(0,0)   = pTropoStoModel->getQ();
             // if no ISB, numVar = 5
         if (numVar == 5 )
         {
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
        
          } 
         
          else if( numVar == 6 )   // for BeiDou ISB
          {
       
            pISBForBDSStoModel->Prepare(dummySat, gData);
            phiMatrix(1,1) = pISBForBDSStoModel->getPhi();
            qMatrix(1,1)   = pISBForBDSStoModel->getQ();

               // Second, the coordinates
            pCoordXStoModel->Prepare(dummySat, gData);
            phiMatrix(2,2) = pCoordXStoModel->getPhi();
            qMatrix(2,2)   = pCoordXStoModel->getQ();

            pCoordYStoModel->Prepare(dummySat, gData);
            phiMatrix(3,3) = pCoordYStoModel->getPhi();
            qMatrix(3,3)   = pCoordYStoModel->getQ();

            pCoordZStoModel->Prepare(dummySat, gData);
            phiMatrix(4,4) = pCoordZStoModel->getPhi();
            qMatrix(4,4)   = pCoordZStoModel->getQ();

             // Third, the receiver clock
            pClockStoModel->Prepare( dummySat,
                                  gData );
            phiMatrix(5,5) = pClockStoModel->getPhi();
            qMatrix(5,5)   = pClockStoModel->getQ();
          }

          else if ( numVar == 7 )  // for BeiDou ISB and Galileo ISB
          {
             
            pISBForBDSStoModel->Prepare(dummySat, gData);
            phiMatrix(1,1) = pISBForBDSStoModel->getPhi();
            qMatrix(1,1)   = pISBForBDSStoModel->getQ();

            pISBForGALStoModel->Prepare(dummySat, gData);
            phiMatrix(2,2) = pISBForGALStoModel->getPhi();
            qMatrix(2,2)   = pISBForGALStoModel->getQ();
      

               // Second, the coordinates
            pCoordXStoModel->Prepare(dummySat, gData);
            phiMatrix(3,3) = pCoordXStoModel->getPhi();
            qMatrix(3,3)   = pCoordXStoModel->getQ();

            pCoordYStoModel->Prepare(dummySat, gData);
            phiMatrix(4,4) = pCoordYStoModel->getPhi();
            qMatrix(4,4)   = pCoordYStoModel->getQ();

            pCoordZStoModel->Prepare(dummySat, gData);
            phiMatrix(5,5) = pCoordZStoModel->getPhi();
            qMatrix(5,5)   = pCoordZStoModel->getQ();

              // Third, the receiver clock
            pClockStoModel->Prepare( dummySat,
                                  gData );
            phiMatrix(6,6) = pClockStoModel->getPhi();
            qMatrix(6,6)   = pClockStoModel->getQ();
        
          }

            // Finally, the phase biases
         int count2(numVar);     
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {
               // Prepare stochastic model
            if ((*itSat).system == SatID::systemBeiDou)
            {
                
                pBDSBiasStoModel->Prepare( *itSat,
                                            gData );

                  // Get values into phi and q matrices
                phiMatrix(count2,count2) = pBDSBiasStoModel->getPhi();
                qMatrix(count2,count2)   = pBDSBiasStoModel->getQ();
                
            }
            else
            {
                pBiasStoModel->Prepare( *itSat,
                                       gData );

                  // Get values into phi and q matrices
                phiMatrix(count2,count2) = pBiasStoModel->getPhi();
                qMatrix(count2,count2)   = pBiasStoModel->getQ();
            }

            ++count2;
         }
           // the Glonass ISB
         if ( useGPS && useGlonass )
         {
           int count3(numVar+numCurrentSV);
           for ( SatIDSet::const_iterator itGloSat = GloSatSet.begin();
                 itGloSat != GloSatSet.end();
                 itGloSat ++)
           {
                  // Prepare stochastic model
               pISBForGLOStoModel->Prepare( *itGloSat, gData );

                 // Get values into phi and q matrices
               phiMatrix(count3,count3) = pISBForGLOStoModel->getPhi();
               qMatrix(count3,count3) = pISBForGLOStoModel->getQ();
            
               ++count3;
           }

         }

         double sod( (gData.header.epoch).getSecondOfDay() );
            // Feed the filter with the correct state and covariance matrix
         if(firstTime )
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );


               // Fill the initialErrorCovariance matrix

               // First, the zenital wet tropospheric delay
            initialErrorCovariance(0,0) = 0.25;          // (0.5 m)**2
            if ( numVar == 5 )
            {
               // Second, the coordinates
             for( int i=1; i<4; i++ )
             {
               initialErrorCovariance(i,i) = 10000.0;    // (100 m)**2
             }

               // Third, the receiver clock
             initialErrorCovariance(4,4) = 9.0e10;        // (300 km)**2
            }
              // ISB
            else if ( numVar == 6 )
            {
              initialErrorCovariance(1,1) = 10000.0;    
                     // Second, the coordinates
              for( int i=2; i<5; i++ )
              {
                initialErrorCovariance(i,i) = 10000.0;    // (100 m)**2
              }

               // Third, the receiver clock
              initialErrorCovariance(5,5) = 9.0e10;        // (300 km)**2
           
            }

            else if ( numVar == 7 )
            {
               initialErrorCovariance(1,1) = 10000.0;        
               initialErrorCovariance(2,2) = 10000.0;        
                // Second, the coordinates
               for( int i=3; i<6; i++ )
               {
                 initialErrorCovariance(i,i) = 10000.0;    // (100 m)**2
               }

                // Third, the receiver clock
               initialErrorCovariance(6,6) = 9.0e10;        // (300 km)**2

            }
               // Finally, the phase biases
            for( int i=numVar; i<numVar+numCurrentSV; i++ )
            {
               initialErrorCovariance(i,i) = 4.0e14;     // (20000 km)**2
            }
               // the Glonass ISB
            if ( useGPS && useGlonass)
            { 
                for (int i = numVar+numCurrentSV;i<numUnknowns;i++)
                {
                  initialErrorCovariance(i,i) = 9.0e10;     
                } 
            }

               // Reset Kalman filter
            kFilter.Reset( initialState, initialErrorCovariance );

               // No longer first time
            firstTime = false;

               // Not converged!
            converged = false;

               // reset solution
            resetSol = true;

               // Start time to compute the convergence time
            startTime = sod;

         }
         else
         {
               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);


               // Set first part of current state vector and covariance matrix
            for( int i=0; i<numVar; i++ )
            {
               currentState(i) = solution(i);

                  // This fills the upper left quadrant of covariance matrix
               for( int j=0; j<numVar; j++ )
               {
                  currentErrorCov(i,j) =  covMatrix(i,j);
               }
            }
            

               // Temporary satellite set
            SatIDSet tempSatSet(currSatSet);

               // Fill in the rest of state vector and covariance matrix
               // These are values that depend on satellites being processed
            int c1(numVar);
            for( SatIDSet::const_iterator itSat = currSatSet.begin();
                 itSat != currSatSet.end();
                 ++itSat )
            {
                  // Put ambiguities into state vector
               currentState(c1) = ambiguityMap[*itSat];


               if( ambCovMap.find( (*itSat) ) != ambCovMap.end() )
               {

                     // Fill the diagonal element
                  currentErrorCov(c1,c1) = ambCovMap[*itSat].aCovMap[*itSat];
               }
               else
               {
                  currentErrorCov(c1,c1) = 4.0e+14;
               }

                  // Put ambiguities covariance values into covariance matrix
                  // This fills the lower right quadrant of covariance matrix
               int c2(c1+1);

                  // Remove current sat from 'tempSatSet'
               tempSatSet.erase( (*itSat) ); 

               for( SatIDSet::const_iterator itSat2 = tempSatSet.begin(); 
                    itSat2 != tempSatSet.end(); 
                    ++itSat2 )
               {

                  currentErrorCov(c1,c2) = ambCovMap[*itSat].aCovMap[*itSat2];
                  currentErrorCov(c2,c1) = ambCovMap[*itSat].aCovMap[*itSat2];

                  ++c2;
               }

                  // Put variables X ambiguities covariances into
                  // covariance matrix. This fills the lower left and upper
                  // right quadrants of covariance matrix
               int c3(0);
               TypeIDSet::const_iterator itType;
               for( itType  = defaultEqDef.body.begin();
                    itType != defaultEqDef.body.end();
                    ++itType )
               {

                  currentErrorCov(c1,c3) = ambCovMap[*itSat].vCovMap[*itType];
                  currentErrorCov(c3,c1) = ambCovMap[*itSat].vCovMap[*itType];

                  ++c3;
               }
  
               if ( useGPS && useGlonass)
               {
                  int c4(numVar+numCurrentSV);
                  
                  for ( SatIDSet::const_iterator itSat3 = GloSatSet.begin();
                        itSat3 != GloSatSet.end();
                        ++itSat3 )
                  {
                         
                     currentErrorCov(c1,c4) = ambCovMap[*itSat].ISBCovMap[*itSat3];
                     currentErrorCov(c4,c1) = ambCovMap[*itSat].ISBCovMap[*itSat3];
                      
                     ++c4;            
                  }

               }

               ++c1;
            }
               // for Glonass ISB
            if ( useGPS && useGlonass )
            {
               SatIDSet tempGloSat(GloSatSet);

               int c5(numVar+numCurrentSV);
               for ( SatIDSet::const_iterator itGloSat = GloSatSet.begin();
                    itGloSat != GloSatSet.end();
                    ++itGloSat )
               {
                   // put Glonass ISB into state vector
                  currentState(c5) = GlonassISBMap[*itGloSat];
                  
                  if (GloISBCovMap.find(*itGloSat) != GloISBCovMap.end())
                  {
                    currentErrorCov(c5,c5) = GloISBCovMap[*itGloSat].iCovMap[*itGloSat];     
                  }
                  else 
                  {
                    currentErrorCov(c5,c5) = 10000.0;    
                  }
                  int c6(c5+1);

                    // remove current sat for 'tempGloSat'
                  tempGloSat.erase(*itGloSat);
                   
                  for ( SatIDSet::const_iterator itGloSat2 = tempGloSat.begin();
                        itGloSat2 != tempGloSat.end();
                        ++itGloSat2)
                  {

                    currentErrorCov(c5,c6) = GloISBCovMap[*itGloSat].iCovMap[*itGloSat2];    
                    currentErrorCov(c6,c5) = GloISBCovMap[*itGloSat].iCovMap[*itGloSat2];    
                    
                    ++c6;
                  }
                  
                  int c7(0);
                  for ( TypeIDSet::const_iterator itType = defaultEqDef.body.begin();
                        itType != defaultEqDef.body.end();
                        ++itType )
                  {

                    currentErrorCov(c5,c7) = GloISBCovMap[*itGloSat].ivCovMap[*itType];        
                    currentErrorCov(c7,c5) = GloISBCovMap[*itGloSat].ivCovMap[*itType];        

                    ++c7;     
                  }
                 
                 ++c5;

               } // End of 'for (SatIDSet...)'

             } // End of 'if ( useGPS...)'

               // Reset Kalman filter to current state and covariance matrix
            kFilter.Reset( currentState, currentErrorCov );

         }  // End of 'if(firstTime)'

            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverPPPGNSS object with the appropriate
            // constructor.
         Compute( measVector,
                  hMatrix,
                  rMatrix );


            // Now, clear ambCovMap
         ambiguityMap.clear();
         ambCovMap.clear();

            // clear GloISBCovMap
         GlonassISBMap.clear();
         GloISBCovMap.clear();

            // Temporary satellite set
         SatIDSet tempSatSet(currSatSet);

            // Store those values of current state and covariance matrix
            // that depend on satellites currently in view
         int c1(numVar);
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {

               // Store ambiguities
            ambiguityMap[*itSat] = solution(c1);

               // Fill the diagonal element
            ambCovMap[*itSat].aCovMap[*itSat] = covMatrix(c1,c1);


               // Store ambiguities covariance values
            int c2(c1+1);

               // Remove current satellite from 'tempSatSet'
            tempSatSet.erase( (*itSat) );

            for( SatIDSet::const_iterator itSat2 = tempSatSet.begin(); 
                 itSat2 != tempSatSet.end(); 
                 ++itSat2 )
            {

               ambCovMap[*itSat].aCovMap[*itSat2] = covMatrix(c1,c2);

               ++c2;
            }

               // Store variables X ambiguities covariances
            int c3(0);
            TypeIDSet::const_iterator itType;
            for( itType  = defaultEqDef.body.begin();
                 itType != defaultEqDef.body.end();
                 ++itType )
            {

               ambCovMap[*itSat].vCovMap[*itType] = covMatrix(c1,c3);

               ++c3;
            }
              // Store Glonass ISB X ambiguities covariances
            if ( useGPS && useGlonass)
            {
               int c4(numVar+numCurrentSV);
               for ( SatIDSet::const_iterator itGloSat = GloSatSet.begin();
                     itGloSat != GloSatSet.end();
                     ++itGloSat )
               {
                  ambCovMap[*itSat].ISBCovMap[*itGloSat] = covMatrix(c1,c4);

                  ++c4;
               }        
                
            }

            ++c1;

         }  // End of 'for( itSat = currSatSet.begin(); ...'
           
            // for Glonass ISB Map
         if ( useGPS && useGlonass)
         {
            SatIDSet tempGloSat(GloSatSet);

            int c5(numVar+numCurrentSV);
            for ( SatIDSet::const_iterator itGloSat = GloSatSet.begin();
                  itGloSat != GloSatSet.end();
                  ++itGloSat )
            {
               
               GlonassISBMap[*itGloSat] = solution(c5);

               GloISBCovMap[*itGloSat].iCovMap[*itGloSat] = covMatrix(c5,c5);

               int c6(c5+1);
            
               tempGloSat.erase(*itGloSat);

               for ( SatIDSet::const_iterator itGloSat2 = tempGloSat.begin();
                     itGloSat2 != tempGloSat.end();
                     ++itGloSat2 )
               {
                 GloISBCovMap[*itGloSat].iCovMap[*itGloSat2] = covMatrix(c5,c6);   
                   
                 ++c6;
               }
                  // Store Glonass ISB X variables covariances     
               int c7(0);
               for ( TypeIDSet::const_iterator itType = defaultEqDef.body.begin();
                     itType != defaultEqDef.body.end();
                     ++itType )
               {

                 GloISBCovMap[*itGloSat].ivCovMap[*itType] = covMatrix(c5,c7);   

                 ++c7;     
               }
               
              ++c5;
            } // End of 'for (SatIDSet...)'

         }  // End of 'if  ( useGPS ...)'
            // Now we have to add the new values to the data structure
         Vector<double> postfitCode(numCurrentSV,0.0);
         Vector<double> postfitPhase(numCurrentSV,0.0);
         for( int i=0; i<numCurrentSV; i++ )
         {
            postfitCode(i)  = postfitResiduals( i                );
            postfitPhase(i) = postfitResiduals( i + numCurrentSV );
         }
    
         gData.insertTypeIDVector(TypeID::postfitC, postfitCode);
         gData.insertTypeIDVector(TypeID::postfitL, postfitPhase);

           // Now insert glonass ISB into gData
         for ( std::map<SatID, double>::const_iterator it = GlonassISBMap.begin();
               it != GlonassISBMap.end();
               ++it)
         {
            gData.body[(*it).first][TypeID::ISB_GLO] = (*it).second;         
         }

            // Now Let's insert the ambiguity parameters into 'gData'
         Vector<double> ambVec(numCurrentSV,0.0);
         for( int i=0; i<numCurrentSV; i++ )
         {
            ambVec(i) = solution(numVar + i) + postfitPhase(i);
         }
           // dx, dy, dz
         double dx(0.0);
         double dy(0.0);
         double dz(0.0);
         if (numVar == 5)
         {
            dx = solution(1);
            dy = solution(2);
            dz = solution(3);
         }
         else if (numVar == 6)
         {
            dx = solution(2);
            dy = solution(3);
            dz = solution(4);
         }
         else if (numVar == 7)
         {
           dx = solution(3);
           dy = solution(4);
           dz = solution(5);
         }

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

            // Only calculate the converge time if resetSol is set!!
         if(resetSol)
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
            else
            {
               converged = false;
            }

               // If already converged, then set the 'resetSol' as false.
            if(converged)
            {
               double ttfc = sod - startTime;
               ttfcVec.push_back(ttfc);
               resetSol = false;
            }
         }

         gData.insertTypeIDVector(TypeID::BLC, ambVec);

         return gData;

      }

      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPGNSS::Process()'



      /* Sets if a NEU system will be used.
       *
       * @param useNEU  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverPPPGNSS& SolverPPPGNSS::setNEU( bool useNEU )
   {

         // First, let's define a set with the typical code-based unknowns
      TypeIDSet tempSet;

         // Watch out here: 'tempSet' is a 'std::set', and all sets order their
         // elements. According to 'TypeID' class, this is the proper order:

      tempSet.insert(TypeID::wetMap);  // BEWARE: The first is wetMap!!!

      if (useNEU)
      {
         tempSet.insert(TypeID::dLat); // #2
         tempSet.insert(TypeID::dLon); // #3
         tempSet.insert(TypeID::dH);   // #4
      }
      else
      {
         tempSet.insert(TypeID::dx);   // #2
         tempSet.insert(TypeID::dy);   // #3
         tempSet.insert(TypeID::dz);   // #4
      }
      tempSet.insert(TypeID::cdt);     // #5
     
         // Now, we build the basic equation definition
      defaultEqDef.header = TypeID::prefitC;
      defaultEqDef.body = tempSet;

      return (*this);

   }  // End of method 'SolverPPPGNSS::setNEU()'
      
      /// Set SatSystem 
   SolverPPPGNSS& SolverPPPGNSS::setSatSystem( bool usingGPS,
                                               bool usingGLO,
                                               bool usingBDS,
                                               bool usingGAL )
   
   {
      useGPS     = usingGPS;
      useGlonass = usingGLO;
      useBeiDou  = usingBDS;
      useGalileo = usingGAL;

         // insert BeiDou ISB into defaultEqDef.body
      if ( useGPS && useBeiDou )
      {
        defaultEqDef.body.insert(TypeID::ISB_BDS);        
      }

        // insert Galileo ISB into defaultEqDef.body
      if ( useGPS && useGalileo )
      {
        defaultEqDef.body.insert(TypeID::ISB_GAL);        
      }
       
   }

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
   SolverPPPGNSS& SolverPPPGNSS::setCoordinatesModel( StochasticModel* pModel )
   {

         // All coordinates will have the same model
      pCoordXStoModel = pModel;
      pCoordYStoModel = pModel;
      pCoordZStoModel = pModel;

      return (*this);

   }  // End of method 'SolverPPPGNSS::setCoordinatesModel()'


       /** Set the positioning mode, kinematic or static.
        */
   SolverPPPGNSS& SolverPPPGNSS::setKinematic( bool kinematicMode,
                                       double sigmaX,
                                       double sigmaY,
                                       double sigmaZ )
   {
      if(kinematicMode)
      {
         whitenoiseModelX.setSigma(sigmaX);
         whitenoiseModelY.setSigma(sigmaY);
         whitenoiseModelZ.setSigma(sigmaZ);

         setXCoordinatesModel(&whitenoiseModelX);
         setYCoordinatesModel(&whitenoiseModelY);
         setZCoordinatesModel(&whitenoiseModelZ);
      }
      else
      {
         setCoordinatesModel(&constantModel);
      }

      return (*this);

   }  // End of method 'SolverPPPGNSS::setKinematic()'


      /** Return the converged flag.
       */
   bool SolverPPPGNSS::getConverged() const
      throw(InvalidRequest)
   {
         // Return current fixed satellite number
      return converged;
   }


}  // End of namespace gpstk
