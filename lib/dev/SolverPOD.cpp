#pragma ident "$Id: SolverPOD.cpp 2645 2013-06-30 00:00:00Z shjzhang $"

/**
 * @file SolverPOD.cpp
 * Class to compute the POD Solution.
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
//  Shoujian Zhang, Wuhan University, 2013
//
//============================================================================
//
//  Modifications
//
//  2013/04/01   Add the 'SVNumException', which will throw exception if the
//               observed satellite number is less than 4.
//
//  2013/06/30   Make modifcations of 'SolverPOD' to 'SolverPOD' for precise
//               orbit determination.
//
//  2013/07/23   Add 'ambiguityData' to store the ambiguity and their 
//               covariance information, which will be used as the 
//               posterior constraints to improve the orbit precisions
//
//  2016/04/22   change 'SolverPOD' according to 'SolverPPP' used for pod
//============================================================================

#include "SolverPOD.hpp"
#include "MatrixFunctors.hpp"

using namespace std;
namespace gpstk
{

      // Index initially assigned to this class
   int SolverPOD::classIndex = 9310000;


      // Returns an index identifying this object.
   int SolverPOD::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   string SolverPOD::getClassName() const
   { return "SolverPOD"; }


      /* Common constructor.
       *
       * @param useRAC   If true, will compute (dR, dA, dC) coordinates;
       *                 if false (the default), will compute dx, dy, dz.
       */
   SolverPOD::SolverPOD(bool useRAC)
      : firstTime(true)
   {

         // Set the equation system structure
      setRAC(useRAC);

         // Set the class index
      setIndex();

         // Call initializing method
      Init();

   }  // End of 'SolverPOD::SolverPOD()'



      // Initializing method.
   void SolverPOD::Init(void)
   {

         // Set sigma for coordinate model (10m)
      coordinateModel.setSigma(10.0);

         // Set default coordinates stochastic model (white noise)
      setCoordinatesModel( &coordinateModel );

         // Set qdot value for default random walk stochastic model
//      rwalkModel.setQprime(0.06);
        
         // Pointer to default receiver clock stochastic model (white noise)
      pClockStoModel = &whitenoiseModel;
//      pClockStoModel = &rwalkModel;

         // Pointer to stochastic model for phase biases
      pBiasStoModel  = &biasModel;

         // Set default factor that multiplies phase weights
         // If code sigma is 1 m and phase sigma is 1 cm, the ratio is 100:1
      weightFactor = 10000.0;       // 100^2


   }  // End of method 'SolverPOD::Init()'

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

   int SolverPOD::Compute( const Vector<double>& prefitResiduals,
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

         // Call the more general SolverPPP::Compute() method
      return SolverPOD::Compute( prefitResiduals,
                                 designMatrix,
                                 wMatrix );

   }  // End of method 'SolverPOD::Compute()'



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
   int SolverPOD::Compute( const Vector<double>& prefitResiduals,
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
         
         
//         for(int i=0;i<numUnknowns;i++)
//         {
//               
//               cout<<"solution"<<i<<":"<<solution(i)<<"   "
//               <<"postfitResiduals:"<<postfitResiduals(numUnknowns-numVar+i)<<"  "<<endl;
//         }
//         
//         cout<<"prefitResiduals"<<endl<<prefitResiduals<<endl;
//         cout<<"designMatrix"<<endl<<designMatrix<<endl;
   
         
         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;
         

   }  // End of method 'SolverPOD::Compute()'
      
      
      
      
      
      
      
      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPOD::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverPOD::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPOD::Process(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

      try
      {

            // Prepare everything before computing
         preCompute( gData);

            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverPPP object with the appropriate
            // constructor.
         Compute( measVector,
                    hMatrix,
                    rMatrix );
            
            // Now, clear ambCovMap
         ambiguityMap.clear();
         ambCovMap.clear();
            

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

   }  // End of method 'SolverPOD::Process()'


      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPOD::preCompute(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

         /////////////////////////////////////////////////////////////////
         //
         // Please note that there are two different sets being defined:
         //
         // - "currSatSet" stores satellites currently in view, and it is
         //   related with the number of measurements.
         //
         // - "satSet" stores satellites being processed; this set is
         //   related with the number of unknowns.
         //
         /////////////////////////////////////////////////////////////////
         
            // Get a set with all satellites present in this GDS
         currSatSet= gData.body.getSatID();

            // Get the number of satellites currently visible
         numCurrentSV= gData.numSats();
         
            // check current SV numbers set 6 to aviod bad obs.
         if(numCurrentSV<5)
         {
               SVNumException e("Satellite number is less than 4");
               
               GPSTK_THROW(e);
               
         }

//            // Update set with satellites being processed so far
//         satSet.insert( currSatSet.begin(), currSatSet.end() );

//            // Get the number of satellites to be processed
//         int numSV( satSet.size() );
         
//            // cout Current currSatset SatID
//      for( SatIDSet::const_iterator itSat = currSatSet.begin();
//             itSat != currSatSet.end();
//             ++itSat )
//      {
//            cout<<(*itSat);
//      }
//      cout<<endl;
//
//      cout<<"ambiguityConstr:"<<endl;
//      for( AmbiguityDataMap::const_iterator itSat = ambiguityConstr.begin();
//             itSat != ambiguityConstr.end();
//             ++itSat )
//      {
//            cout<<(*itSat).first.getSatellite()<<":"
//                  <<(*itSat).first.getArcNumber()<<"  , "
//                  <<(*itSat).second.ambiguity<<" ; ";
//      }
//      cout<<endl;
         
         
      try
      {

            // Number of measurements is twice the number of visible satellites
            // as both "code + phase" set as measurements
         numMeas = 2 * numCurrentSV;

            // Number of 'core' variables: Coordinates, RX clock,for POD usual 4
         numVar = defaultEqDef.body.size();

            // Total number of unknowns is defined as variables + processed SVs'
            // ambiguities
         numUnknowns = numVar + numCurrentSV;

            /////////////////////////////////////////////////
            //
            // Now, Fill the  measVector/rMatrix/hMatrix
            //
            ////////////////////////////////////////////////

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

            for( int i=0; i<numCurrentSV; i++ )
            {

               rMatrix( i               , i         ) = weightsVector(i);
               rMatrix( i + numCurrentSV, i + numCurrentSV )
                                             = weightsVector(i) * weightFactor;

            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         }
         else
         {

               // If weights don't match, assign generic weights
            for( int i=0; i<numCurrentSV; i++ )
            {
               // set code sigma =1.0   ,power=1/(code.sigma^2);
               rMatrix( i               , i         ) = 1.0;

                  // Phases weights are bigger
               rMatrix( i + numCurrentSV, i + numCurrentSV )
                                                         = 1.0 * weightFactor;

            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         }  // End of 'if ( dummy.numSats() == numCurrentSV )'

            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns, 0.0);

            // Get the values corresponding to 'core' variables
         Matrix<double> dMatrix(gData.body.getMatrixOfTypes(defaultEqDef.body));

            // Let's fill 'hMatrix'
         for( int i=0; i<numCurrentSV; i++ )
         {

               // First, fill the coefficients related to coord and clock for pod
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


            ///////////////////////////////////////////////////////
            //
            // Here, fill the ambiguity constraint equations.
            //
            ///////////////////////////////////////////////////////
            
            // Firstly, get the ambiguity constraints' number
         int numConstr(0);
         for( SatIDSet::const_iterator itCurrSat = currSatSet.begin();
              itCurrSat != currSatSet.end();
              ++itCurrSat )
         {
               // Get the satellite arc for '(*itSat)'
            double satArc = gData.body.getValue( (*itCurrSat), 
                                                 TypeID::satArc );

               // Now, create the the ambiguity identity
            Ambiguity amb((*itCurrSat), satArc);

               // Iterator for ambiguity
            if(ambiguityConstr.find(amb) != ambiguityConstr.end())
            {
               numConstr++;
            }
         }
      
            // Now, Let's define the measVec/hMatrix/rMatrix for constraints
         Vector<double> measVector2(numConstr,0.0);
         Matrix<double> hMatrix2(numConstr,numUnknowns,0.0);
         Matrix<double> rMatrix2(numConstr, numConstr,0.0);

            // Now, Fill the measVector2/hMatrix2/rMatrix2
            // if there's none constrains, don't fill them.
       if(numConstr!=0)
       {
         int i(0);
         int j(0);
         for( SatIDSet::const_iterator itCurrSat = currSatSet.begin();
              itCurrSat != currSatSet.end();
              ++itCurrSat )
         {
               // Get the satellite arc for '(*itSat)'
            double satArc = gData.body.getValue( (*itCurrSat), 
                                                 TypeID::satArc );

               // Now, create the the ambiguity identity
            Ambiguity amb((*itCurrSat), satArc);

               // Iterator for ambiguity
            if(ambiguityConstr.find(amb) != ambiguityConstr.end())
            {
                  
                  // Fill the measurement
               measVector2( j ) = ambiguityConstr[amb].ambiguity;
                  
                  // Fill the coefficient matrix
               hMatrix2( j, i + numVar) = 1.0;
                  
                  // Fill the weight matrix
                  // Warning: you should compute the weight using the variance.
                  //          p = sigma_0^2/sigma_i^2 
               rMatrix2( j, j ) = 1.0/ambiguityConstr[amb].aCovMap[amb];
                  
//                  // avoid overweight power of ambiguity
//               if(rMatrix2( j, j )>1000000.0)
//               {
//                     rMatrix2( j, j )=1000000.0;
//               }
                // only find constrains ,++ ambiguity constraints' number
               j++;

            }
               // End of 'if(ambiguityConstr.find(amb) !='
               
               // increase as unknown ambiguity number in currSatSet.
            i++;

         } // End of 'for( SatIDSet::const_iterator itCurrSat ='
             
       } // End of '(numConstr!=0)'

       //        cout<<"hMatrix2"<<hMatrix2<<endl
       //        <<"rMatrix2"<<rMatrix2<<endl;
               
               
           // Now, erase the ambiguity from the 'ambiguityConstr', and
           // this will gurantee that each ambiguity only have one 'constraint' 
           // at the begining epoch of th continuous arc.
           // See the reference:
           // Kroes, Precise relative-positioning for GRACE.
         for( SatIDSet::const_iterator itCurrSat = currSatSet.begin();
              itCurrSat != currSatSet.end();
              ++itCurrSat )
         {
               // Get the satellite arc for '(*itSat)'
            double satArc = gData.body.getValue( (*itCurrSat), 
                                                 TypeID::satArc );

               // Now, create the the ambiguity identity
            Ambiguity amb((*itCurrSat), satArc);

               // Erase current ambiguity
            ambiguityConstr.erase(amb);

         } // End of ' for( SatIDSet::const_iterator itCurrSat ='


           // Now, Get the new measurement/geometry/weight matrix        
           

            // The observation equation number is enlarged
         int numEqu( numMeas + numConstr );

            // New observation prefit/geometry/weight matrix
         Vector<double> tempPrefit(numEqu, 0.0);
         Matrix<double> tempGeometry(numEqu, numUnknowns, 0.0);
         Matrix<double> tempWeight(numEqu, numEqu, 0.0);

            // Now, Let's store the tempPrefit1/tempGeometry1/tempWeight1
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

            // Now, insert constraint equation 
         for(int i=0; i<numConstr; i++)
         {
               // Prefit
            tempPrefit(numMeas+i) = measVector2(i); 

               // Geometry
            for(int j=0; j<numUnknowns; j++)
            {
               tempGeometry(numMeas+i,j) = hMatrix2(i,j); 
            }  

               // Weight
            tempWeight(numMeas+i,numMeas+i)  = rMatrix2(i,i); 

         }

            // Resize the measVector/hMatrix/rMatrix
         measVector.resize(numEqu,  0.0);
         hMatrix.resize(numEqu, numUnknowns, 0.0);
         rMatrix.resize(numEqu, numEqu, 0.0);

            // New Measurement vector update
         measVector = tempPrefit ;
         hMatrix = tempGeometry ;
         rMatrix = tempWeight ;

            //////////////////////////////////////////
            //
            // Now, Fill the phiMatrix and qMatrix 
            //
            //////////////////////////////////////////

            // State Transition Matrix (PhiMatrix)
         phiMatrix.resize(numUnknowns, numUnknowns, 0.0);

            // Noise covariance matrix (QMatrix)
         qMatrix.resize(numUnknowns, numUnknowns, 0.0);

            // Now, let's fill the Phi and Q matrices
         SatID  dummySat;

            // the coordinates, i.e. the satellite orbits
            // The radial component
         pCoordXStoModel->Prepare(dummySat, gData);
         phiMatrix(0,0) = pCoordXStoModel->getPhi();
         qMatrix(0,0)   = pCoordXStoModel->getQ();

            // The along-track component
         pCoordYStoModel->Prepare(dummySat, gData);
         phiMatrix(1,1) = pCoordYStoModel->getPhi();
         qMatrix(1,1)   = pCoordYStoModel->getQ();

            // The cross-track component
         pCoordZStoModel->Prepare(dummySat, gData);
         phiMatrix(2,2) = pCoordZStoModel->getPhi();
         qMatrix(2,2)   = pCoordZStoModel->getQ();


            // Fourth, the receiver clock
         pClockStoModel->Prepare( dummySat,gData );
         phiMatrix(3,3) = pClockStoModel->getPhi();
         qMatrix(3,3)   = pClockStoModel->getQ();



            // Finally, the phase biases
         int count2(numVar);     // Note that for POD, 'numVar' is always 4!!!
            
         for( SatIDSet::const_iterator itSat = currSatSet.begin();
              itSat != currSatSet.end();
              ++itSat )
         {

               // Prepare stochastic model
            pBiasStoModel->Prepare( *itSat,gData );

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

               // Fill the initialErrorCovariance matrix

               // First, the coordinates
            for( int i=0; i<3; i++ )
            {
               initialErrorCovariance(i,i) = 100.0;    // (10 m)**2
            }

               // Third, the receiver clock
            initialErrorCovariance(3,3) = 9.0e10;        // (300 km)**2

               // Finally, the phase biases
            for( int i=4; i<numUnknowns; i++ )
            {
               initialErrorCovariance(i,i) = 4.0e14;     // (20000 km)**2
            }

               // Reset Kalman filter state and covariance matrix
            kFilter.Reset( initialState, initialErrorCovariance );

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
                  // if *itSat exist in last epoch ,else set as:
                  //currentState=0.0;currentErrorCov=4.0e+14;
               if (ambiguityMap.find( (*itSat) ) != ambiguityMap.end())
               {
                   currentState(c1) = ambiguityMap[*itSat];
               }
                  
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
               SatIDSet::const_iterator itSat2;
               
               for( itSat2 = tempSatSet.begin(); itSat2 != tempSatSet.end(); ++itSat2 )
               {

                  // fill covariance if exist in last epoch ambCovMap
                  // else set it as 0.0
                  if(( ambCovMap.find( (*itSat) ) != ambCovMap.end() )&&
                     ( ambCovMap.find( (*itSat2) ) != ambCovMap.end() ))
                  {
                  currentErrorCov(c1,c2) = ambCovMap[*itSat].aCovMap[*itSat2];
                  currentErrorCov(c2,c1) = ambCovMap[*itSat].aCovMap[*itSat2];
                  }

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
                     // fill covariance if exist in last epoch ambCovMap
                     // else set it as 0.0
                  if (ambCovMap.find( (*itSat) ) != ambCovMap.end())
                  {
                  currentErrorCov(c1,c3) = ambCovMap[*itSat].vCovMap[*itType];
                  currentErrorCov(c3,c1) = ambCovMap[*itSat].vCovMap[*itType];
                  }

                  ++c3;
               }

               ++c1;
            }

               // Reset Kalman filter to current state and covariance matrix
            kFilter.Reset( currentState, currentErrorCov );
               

         }  // End of 'if(firstTime)'
      
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

   }  // End of method 'SolverPOD::Precompute()'


      /* Code to be executed after 'Compute()' method.
       *
       * @param gData    Data object holding the data.
       */
   gnssRinex& SolverPOD::postCompute( gnssRinex& gData )
      throw(ProcessingException)
   {

      try
      {

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
               
            SatIDSet::const_iterator itSat2;
            for( itSat2 = tempSatSet.begin(); itSat2 != tempSatSet.end(); ++itSat2 )
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
 
            ++c1;

         }  // End of 'for( itSat = satSet.begin(); ...'

           /**
            * Now, Let's store the ambiguity information into 'ambiguityData', 
            * which will maybe used as "ambiguityConstr"
            */


           /** 
            *  Try to pull out the satellite arc information
            */

            // Store those values of current state and covariance matrix
            // that depend on satellites currently in view
         int i(numVar);
            
         for( SatIDSet::const_iterator itCurrSat = currSatSet.begin();
              itCurrSat != currSatSet.end();
              ++itCurrSat )
         {

               // Get the satellite arc for '(*itSat)'
            double satArc = gData.body.getValue( (*itCurrSat), 
                                                 TypeID::satArc );

               // Now, create the the ambiguity identity
            Ambiguity amb((*itCurrSat), satArc);

               // Iterator for ambiguity
            AmbiguityDataMap::iterator itamb = ambiguityData.begin();

               // If found ,add windowSize in this satArc
            if(itamb == ambiguityData.end())
            {
               ambiguityData[amb].windowSize = 1;
            }
            else
            {
               ambiguityData[amb].windowSize += 1;
            }

               // Store ambiguities
            ambiguityData[amb].ambiguity = solution(i);

               // Store ambiguities covariance values
            int j(numVar);
            for( SatIDSet::const_iterator itCurrSat2 = currSatSet.begin(); 
                 itCurrSat2 != currSatSet.end(); 
                 ++itCurrSat2 )
            {
                  // Find in which position of 'satSet' is the current '(*itCurrSat)'
                  // Please note that 'currSatSet' is a subset of 'satSet'
               

                  // Get the satellite arc for '(*itCurrSat2)'
               double satArc = gData.body.getValue( (*itCurrSat2), 
                                                    TypeID::satArc );

                  // Now, create the the ambiguity identity
               Ambiguity amb2((*itCurrSat2), satArc);

                  // Covariance between amb and amb2
               ambiguityData[amb].aCovMap[amb2] = covMatrix(i,j);
                  
               ++j;
            }
            
            ++i;
         }


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
            
            
      // Now Let's insert the ambiguity parameters into 'gData'
            Vector<double> ambVec(numCurrentSV,0.0);
            for( int i=0; i<numCurrentSV; i++ )
            {
                  ambVec(i) = solution(numVar + i) + postfitPhase(i);
            }
            
            double dx = solution(0);
            double dy = solution(1);
            double dz = solution(2);
            double drou = std::sqrt(dx*dx+dy*dy+dz*dz);

//            // Update set of satellites to be used in next epoch
//         satSet = currSatSet;
            // Update set of satellites ambiguity to be used in next epoch
//         ambiguityConstr=ambiguityData;
         gData.insertTypeIDVector(TypeID::BLC, ambVec);

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

   }  // End of method 'SolverPOD::postCompute()'

      
      
      /* Sets if a NEU system will be used.
       *
       * @param useRAC  Boolean value indicating if a NEU system will
       *                be used
       *
       */
   SolverPOD& SolverPOD::setRAC( bool useRAC )
   {

         // First, let's define a set with the typical code-based unknowns
      TypeIDSet tempSet;

         // Watch out here: 'tempSet' is a 'set', and all sets order their
         // elements. According to 'TypeID' class, this is the proper order:

      if (useRAC)
      {
         tempSet.insert(TypeID::dR);   // #1 radial
         tempSet.insert(TypeID::dA);   // #2 along-track
         tempSet.insert(TypeID::dC);   // #3 cross-track
      }
      else
      {
         tempSet.insert(TypeID::dx);   // #1
         tempSet.insert(TypeID::dy);   // #2
         tempSet.insert(TypeID::dz);   // #3
      }
      tempSet.insert(TypeID::cdt);     // #4

         // Now, we build the basic equation definition
      defaultEqDef.header = TypeID::prefitC;
      defaultEqDef.body = tempSet;

      return (*this);

   }  // End of method 'SolverPOD::setRAC()'


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
   SolverPOD& SolverPOD::setCoordinatesModel( StochasticModel* pModel )
   {

         // All coordinates will have the same model
      pCoordXStoModel = pModel;
      pCoordYStoModel = pModel;
      pCoordZStoModel = pModel;

      return (*this);

   }  // End of method 'SolverPOD::setCoordinatesModel()'
   

}  // End of namespace gpstk
