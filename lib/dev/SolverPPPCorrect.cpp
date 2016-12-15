#pragma ident "$Id$"

/**
 * @file SolverPPPCorrect.cpp
 * Class to compute the PPP Solution using extended kalman filter.
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


#include "SolverPPPCorrect.hpp"
#include "MatrixFunctors.hpp"
#include <algorithm>

using namespace std;

namespace gpstk
{

      // Index initially assigned to this class
   int SolverPPPCorrect::classIndex = 9300000;

      // Returns an index identifying this object.
   int SolverPPPCorrect::getIndex() const
   { return index; }


      // Returns a string identifying this object.
   std::string SolverPPPCorrect::getClassName() const
   { return "SolverPPPCorrect"; }


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
   int SolverPPPCorrect::Compute( const Vector<double>& prefitResiduals,
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
         InvalidSolver e("Unable to compute measurements noise \
covariance matrix.");
         GPSTK_THROW(e);
      }

      try
      {
             // Call the Kalman filter object.
          pEKFStateStore->Correct(prefitResiduals,
                                  designMatrix,
                                  measNoiseMatrix);
      }
      catch(InvalidSolver& e)
      {
          GPSTK_RETHROW(e);
      }

         // Store the solution
      solution  = pEKFStateStore->xhat;

      pEKFStateStore->setRxPosition( pEKFStateStore->xhat[1],
                                     pEKFStateStore->xhat[2],
                                     pEKFStateStore->xhat[3] );

         // Store the covariance matrix of the solution
      covMatrix = pEKFStateStore->P;

         // Compute the postfit residuals Vector
      postfitResiduals = prefitResiduals - (designMatrix * solution);

         // If everything is fine so far, then the results should be valid
      valid = true;

      return 0;

   }  // End of method 'SolverPPPCorrect::MeasUpdate()'



      /* Returns a reference to a gnnsSatTypeValue object after
       * solving the previously defined equation system.
       *
       * @param gData    Data object holding the data.
       */
   gnssSatTypeValue& SolverPPPCorrect::Process(gnssSatTypeValue& gData)
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

   }  // End of method 'SolverPPPCorrect::Process()'



      /* Returns a reference to a gnnsRinex object after solving
       * the previously defined equation system.
       *
       * @param gData     Data object holding the data.
       */
   gnssRinex& SolverPPPCorrect::Process(gnssRinex& gData)
      throw(ProcessingException, SVNumException)
   {

         // Get a set with all satellites present in this GDS
      SatIDSet currSatSet( gData.body.getSatID() );

         // Get the number of satellites currently visible
      int numCurrentSV( gData.numSats() );

      try
      {
            // Number of measurements is twice the number of visible satellites
         numMeas = 2 * numCurrentSV;

            // Number of 'core' variables: Coordinates, RX clock, troposphere
         numVar = coreVarList.size();

            // Total number of unknowns is defined as variables + processed SVs
         numUnknowns = numVar + numCurrentSV;


            // Build the vector of measurements (Prefit-residuals): Code + phase
         measVector.resize(numMeas, 0.0);

         Vector<double> prefitC(gData.getVectorOfTypeID(TypeID::prefitC));
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
               rMatrix( i               , i         ) = 1.0;

                  // Phases weights are bigger
               rMatrix( i + numCurrentSV, i + numCurrentSV )
                                                      = 1.0 * weightFactor;

            }  // End of 'for( int i=0; i<numCurrentSV; i++ )'

         }  // End of 'if ( dummy.numSats() == numCurrentSV )'


            // Generate the corresponding geometry/design matrix
         hMatrix.resize(numMeas, numUnknowns, 0.0);

            // Get the coefficient of wetMap
         Vector<double> wetMap(gData.getVectorOfTypeID(TypeID::wetMap));
            // Coefficent of recX,recY,recZ (the same with dx,dy,dz)
         Vector<double> recX(gData.getVectorOfTypeID(TypeID::dx));
         Vector<double> recY(gData.getVectorOfTypeID(TypeID::dy));
         Vector<double> recZ(gData.getVectorOfTypeID(TypeID::dz));

            // Let's fill 'hMatrix'
         for( int i=0; i<numCurrentSV; i++ )
         {

               //fill the coefficients related to tropo
            hMatrix(i,0) = hMatrix(i + numCurrentSV,0) = wetMap(i);
               // coord
            hMatrix(i,1) = hMatrix(i+numCurrentSV,1)   = recX(i);
            hMatrix(i,2) = hMatrix(i+numCurrentSV,2)   = recY(i);
            hMatrix(i,3) = hMatrix(i+numCurrentSV,3)   = recZ(i);
               // velocity
            hMatrix(i,4) = hMatrix(i+numCurrentSV,4)   = 0.0;
            hMatrix(i,5) = hMatrix(i+numCurrentSV,5)   = 0.0;
            hMatrix(i,6) = hMatrix(i+numCurrentSV,6)   = 0.0;
               // clock
            hMatrix(i,7) = hMatrix(i+numCurrentSV,7)   = 1.0;

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

         
            // Call the Compute() method with the defined equation model.
            // This equation model MUST HAS BEEN previously set, usually when
            // creating the SolverPPPCorrect object with the appropriate
            // constructor.
         Compute( measVector,
                  hMatrix,
                  rMatrix );

            // Now, clear ambCovMap
         pEKFStateStore->ambiguityMap.clear();
         pEKFStateStore->ambCovMap.clear();


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
            pEKFStateStore->ambiguityMap[*itSat] = solution(c1);

               // Fill the diagonal element
            pEKFStateStore->ambCovMap[*itSat].aCovMap[*itSat] = covMatrix(c1,c1);


               // Store ambiguities covariance values
            int c2(c1+1);

               // Remove current satellite from 'tempSatSet'
            tempSatSet.erase( (*itSat) );

            for( SatIDSet::const_iterator itSat2 = tempSatSet.begin(); 
                 itSat2 != tempSatSet.end(); 
                 ++itSat2 )
            {

               pEKFStateStore->ambCovMap[*itSat].aCovMap[*itSat2] = covMatrix(c1,c2);

               ++c2;
            }

               // Store variables X ambiguities covariances
            int c3(0);
            TypeIDSet::const_iterator itType;
            for( itType  = defaultEqDef.body.begin();
                 itType != defaultEqDef.body.end();
                 ++itType )
            {

               pEKFStateStore->ambCovMap[*itSat].vCovMap[*itType] = covMatrix(c1,c3);

               ++c3;
            }

            ++c1;

         }  // End of 'for( itSat = currSatSet.begin(); ...'


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

   }  // End of method 'SolverPPPCorrect::Process()'

   SolverPPPCorrect& SolverPPPCorrect::setCoreVarList(void)
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
   
   double SolverPPPCorrect::getSolution(const TypeID& type) const
      throw(InvalidRequest)
   {

      std::list<TypeID>::const_iterator it =
                         find(coreVarList.begin(),coreVarList.end(),type);

      if (it == coreVarList.end())
      {
          InvalidRequest e("Type not found in solution vector.");
          GPSTK_THROW(e);
      }

      int counter(0);
      for(std::list<TypeID>::const_iterator it2 = coreVarList.begin();
          it2 != it;
          it2++)
      {
         ++counter;    
      }

      return solution(counter);
   }

}  // End of namespace gpstk
