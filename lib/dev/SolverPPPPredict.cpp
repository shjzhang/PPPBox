#pragma ident "$Id$"

/**
 * @file SolverPPPPredict.cpp
 * Class to compute the PPP Solution using Extended Kalman Filter.
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


#include "SolverPPPPredict.hpp"
#include "MatrixFunctors.hpp"

using namespace std;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverPPPPredict::classIndex = 9300000;

      // Returns an index identifying this object.
   int SolverPPPPredict::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPPPPredict::getClassName() const
   { return "SolverPPPPredict"; }


      // Common constructor
   SolverPPPPredict::SolverPPPPredict(PPPExtendedKalmanFilter& pppEKF,
                                      const double& interval)
   {
        firstTime = true;
        kinematicMode = false;
        obsInterval = interval;
        AccSigma = 1.0;
		pEKFStateStore = &pppEKF;

            // preapre for variable
        setCoreVarList();
            // Call the initializing method 
        Init();
   }
      // Common constructor
   SolverPPPPredict::SolverPPPPredict(PPPExtendedKalmanFilter& pppEKF)
   {
        firstTime = true;
        kinematicMode = false;
        obsInterval = 30.0;  // 30s
        AccSigma = 1.0;
		pEKFStateStore = &pppEKF;
            // preapre for variable
        setCoreVarList();
            // Call the initializing method 
        Init();
   }
      // Initializing method.
   void SolverPPPPredict::Init(void)
   {

         // Set qdot value for default random walk stochastic model
      rwalkModel.setQprime(3e-8);

         // Pointer to default stochastic model for troposphere (random walk)
      pTropoStoModel = &rwalkModel;

         // Set default coordinates stochastic model (constant)
      setCoordinatesModel( &constantModel );

      whitenoiseModelX.setSigma(100.0);
      whitenoiseModelY.setSigma(100.0);
      whitenoiseModelZ.setSigma(100.0);

         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel = &whitenoiseModel;

         // Pointer to stochastic model for phase biases
      pBiasStoModel  = &biasModel;


   }  // End of method 'SolverPPPPredict::Init()'

      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPPPPredict::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverPPPPredict::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPPPPredict::Process(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

         // Get a set with all satellites present in this GDS
      SatIDSet currSatSet( gData.body.getSatID() );

         // Get the number of satellites currently visible
      int numCurrentSV( gData.numSats() );

      try
      {

            // Number of 'core' variables: Coordinates, RX clock, troposphere
         numVar = coreVarList.size();

            // Total number of unknowns is defined as variables + processed SVs
         numUnknowns = numVar + numCurrentSV;

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

            // the velocity of receiver 
            // if static mode, the process noises of velocity are zero
         phiMatrix(4,4) = phiMatrix(5,5) = phiMatrix(6,6) = 1.0;      

         if ( kinematicMode )
         {
              // phi realted to pos and vel
            phiMatrix(1,4) = phiMatrix(2,5) = phiMatrix(3,6) = obsInterval;

              // qMatrix of reveiver position
            qMatrix(1,1) = qMatrix(2,2) = qMatrix(3,3) 
                                        = std::pow(0.5*AccSigma*obsInterval*obsInterval,2);
              // qMatrix related to pos and vel
            qMatrix(1,4) = qMatrix(4,1) = std::pow(0.5*obsInterval*AccSigma,2)*obsInterval; 
            qMatrix(2,5) = qMatrix(5,2) = std::pow(0.5*obsInterval*AccSigma,2)*obsInterval; 
            qMatrix(3,6) = qMatrix(6,3) = std::pow(0.5*obsInterval*AccSigma,2)*obsInterval; 


              // qMatrix of velocity
            qMatrix(4,4) = qMatrix(5,5) = qMatrix(6,6)
                                        = std::pow(AccSigma*obsInterval,2);
         }
            // Then , the clock
         pClockStoModel->Prepare(dummySat, gData);
         phiMatrix(7,7) = pClockStoModel->getPhi();
         qMatrix(7,7)   = pClockStoModel->getQ();


            // Finally, the phase biases
         int count2(numVar);     // Note that for PPP, 'numVar' is always 5!!!
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {

               // Prepare stochastic model
            pBiasStoModel->Prepare( *itSat,
                                    gData );

               // Get values into phi and q matrices
            phiMatrix(count2,count2) = pBiasStoModel->getPhi();
            qMatrix(count2,count2)   = pBiasStoModel->getQ();

            ++count2;
         }

            // Feed the filter with the correct state and covariance matrix
         if(firstTime)
         {

            Vector<double> initialState(numUnknowns, 0.0);
            Matrix<double> initialErrorCovariance( numUnknowns,
                                                   numUnknowns,
                                                   0.0 );
               // The initial receiver position
            initialState[1] = pEKFStateStore->getRxPosition().getX(); // X
            initialState[2] = pEKFStateStore->getRxPosition().getY(); // Y
            initialState[3] = pEKFStateStore->getRxPosition().getZ(); // Z

               // Fill the initialErrorCovariance matrix
               // First, the zenital wet tropospheric delay
            initialErrorCovariance(0,0) = 0.25;          // (0.5 m)**2

               // Second, the coordinates
            for( int i=1; i<4; i++ )
            {
               initialErrorCovariance(i,i) = 10000.0;    // (100 m)**2
            }
            
               // the velocity of receiver 
            for( int i=4; i<7; i++ )
            {
               initialErrorCovariance(i,i) = 10000.0;    // (100 m/s)**2
            }
               // the receiver clock
            initialErrorCovariance(7,7) = 9.0e10;        // (300 km)**2
            
               // Finally, the phase biases
            for( int i=numVar; i<numUnknowns; i++ )
            {
               initialErrorCovariance(i,i) = 4.0e14;     // (20000 km)**2
            }
               // Reset Kalman filter
            pEKFStateStore->Reset( initialState, initialErrorCovariance );

               // No longer first time
            firstTime = false;

         }
         else
         {

               // Adapt the size to the current number of unknowns
            Vector<double> currentState(numUnknowns, 0.0);
            Matrix<double> currentErrorCov(numUnknowns, numUnknowns, 0.0);


               // Set first part of current state vector and covariance matrix
            for( int i=0; i<numVar; i++ )
            {
               currentState(i) = pEKFStateStore->xhat(i);

                  // This fills the upper left quadrant of covariance matrix
               for( int j=0; j<numVar; j++ )
               {
                  currentErrorCov(i,j) = pEKFStateStore->P(i,j);
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
               currentState(c1) = pEKFStateStore->ambiguityMap[*itSat];


               if( pEKFStateStore->ambCovMap.find( (*itSat) ) != pEKFStateStore->ambCovMap.end() )
               {

                     // Fill the diagonal element
                  currentErrorCov(c1,c1) = pEKFStateStore->ambCovMap[*itSat].aCovMap[*itSat];
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

                  currentErrorCov(c1,c2) = pEKFStateStore->ambCovMap[*itSat].aCovMap[*itSat2];
                  currentErrorCov(c2,c1) = pEKFStateStore->ambCovMap[*itSat].aCovMap[*itSat2];

                  ++c2;
               }

                  // Put variables X ambiguities covariances into
                  // covariance matrix. This fills the lower left and upper
                  // right quadrants of covariance matrix
               int c3(0);
			   std::list<TypeID>::const_iterator itType;
               for( itType  = coreVarList.begin();itType != coreVarList.end();++itType )
               {

                  currentErrorCov(c1,c3) = pEKFStateStore->ambCovMap[*itSat].vCovMap[*itType];
                  currentErrorCov(c3,c1) = pEKFStateStore->ambCovMap[*itSat].vCovMap[*itType];

                  ++c3;
               }

               ++c1;
            }

               // Reset Kalman filter to current state and covariance matrix
            pEKFStateStore->Reset( currentState, currentErrorCov );

         }  // End of 'if(firstTime)'

            // Predict  
         pEKFStateStore->TimeUpdate(phiMatrix,qMatrix);
            // Update the receiver poistion using predicted value
         pEKFStateStore->setRxPosition(pEKFStateStore->xhatminus[1],
                                       pEKFStateStore->xhatminus[2],
                                       pEKFStateStore->xhatminus[3]);

         return gData;

      }
      catch(Exception& u)
      {
            // Throw an exception if something unexpected happens
         ProcessingException e( getClassName() + ":"
                                + u.what() );

         GPSTK_THROW(e);

      }

   }  // End of method 'SolverPPPPredict::Process()'


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
   SolverPPPPredict& SolverPPPPredict::setCoordinatesModel( StochasticModel* pModel )
   {

         // All coordinates will have the same model
      pCoordXStoModel = pModel;
      pCoordYStoModel = pModel;
      pCoordZStoModel = pModel;

      return (*this);

   }  // End of method 'SolverPPPPredict::setCoordinatesModel()'

   SolverPPPPredict& SolverPPPPredict::setCoreVarList(void)
   {

      coreVarList.push_back(TypeID::wetMap); // #1    
      coreVarList.push_back(TypeID::recX);   // #2   
      coreVarList.push_back(TypeID::recY);   // #3    
      coreVarList.push_back(TypeID::recZ);   // #4    
      coreVarList.push_back(TypeID::recVX);  // #5    
      coreVarList.push_back(TypeID::recVY);  // #6    
      coreVarList.push_back(TypeID::recVZ);  // #7    
      coreVarList.push_back(TypeID::cdt);    // #8    

      return (*this);   
   }
   
   Vector<double> SolverPPPPredict::getPredState(void) const
      throw(InvalidRequest)
   {
      if (pEKFStateStore == NULL)
      {
          InvalidRequest e("cant't get the predicted state of kalman filter.");
          GPSTK_THROW(e);
      }
      
      return pEKFStateStore->xhatminus;
   }


}  // End of namespace gpstk
