#pragma ident "$Id$"

/**
 * @file PPPExtendedKalmanFilter.cpp
 * Class to compute the solution using a Kalman filter.
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
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008
//
//============================================================================



#include "PPPExtendedKalmanFilter.hpp"
#include "MatrixFunctors.hpp"


namespace gpstk
{


      // Corrects (or "measurement updates") the a posteriori estimate of
      // the system state vector, as well as the a posteriori estimate error
      // covariance matrix, using as input the predicted a priori state vector
      // and error covariance matrix, plus measurements and associated
      // matrices.
      //
      // @param measurements                  Measurements vector.
      // @param measurementsMatrix            Measurements matrix. Called geometry
      //                                      matrix in GNSS.
      // @param measurementsNoiseCovariance   Measurements noise covariance
      //                                      matrix.
      //
      // @return
      //  0 if OK
      //  -1 if problems arose
      //
   int PPPExtendedKalmanFilter::Correct( const Vector<double>& measurements,
                                         const Matrix<double>& measurementsMatrix,
                                         const Matrix<double>& measurementsNoiseCovariance )
   throw(InvalidSolver)
   {
      // Let's check sizes before start
      int measRow(measurements.size());
      int aprioriStateRow(xhatminus.size());

      int mMRow(measurementsMatrix.rows());

      int mNCCol(measurementsNoiseCovariance.cols());
      int mNCRow(measurementsNoiseCovariance.rows());

      int pMCol(Pminus.cols());
      int pMRow(Pminus.rows());

      if ( ( mNCCol != mNCRow ) || 
           ( pMCol != pMRow )      )
      {
         InvalidSolver e("Correct(): Either Pminus or measurement covariance \
matrices are not square, and therefore not invertible.");
         GPSTK_THROW(e);
      }

      if ( mMRow != mNCRow )
      {
         InvalidSolver e("Correct(): Sizes of measurements matrix and \
measurements noise covariance matrix do not match.");
         GPSTK_THROW(e);
      }

      if ( mNCCol != measRow )
      {
         InvalidSolver e("Correct(): Sizes of measurements matrix and \
measurements vector do not match.");
         GPSTK_THROW(e);
      }

      if ( pMCol != aprioriStateRow )
      {
         InvalidSolver e("Correct(): Sizes of a priori error covariance \
matrix and a priori state estimation vector do not match.");
         GPSTK_THROW(e);
      }

         // After checking sizes, let's do the real correction work
      Matrix<double> GainMatrix;

      Vector<double> predResiduals;

      Matrix<double> idenityMatrix(pMCol,pMCol,0.0);
      for (int i =0;i<pMCol;i++)
      {
          idenityMatrix(i,i) = 1.0;
      }

      Matrix<double> measMatrixT( transpose(measurementsMatrix) );

      try
      {

          Matrix<double> temp(measurementsMatrix * Pminus* measMatrixT 
                                      + measurementsNoiseCovariance);

          GainMatrix = Pminus * measMatrixT * inverse(temp);
         
      }
      catch(...)
      {
         InvalidSolver e("Correct(): Unable to compute Gain matrix.");
         GPSTK_THROW(e);
         return -1;
      }


      try
      {

            // Compute the a posteriori state estimation
          predResiduals = measurements - measurementsMatrix * xhatminus;

          xhat = xhatminus + GainMatrix * predResiduals;

          Matrix<double> tempMatrix(idenityMatrix - GainMatrix * measurementsMatrix);
           
          P = tempMatrix * Pminus * transpose(tempMatrix) 
              + GainMatrix * measurementsNoiseCovariance * transpose(GainMatrix);

      }
      catch(Exception e)
      {
         InvalidSolver eis("Correct(): Unable to compute xhat.");
         GPSTK_THROW(eis);
         return -1;
      }

      xhatminus = xhat;
      Pminus = P;

      return 0;

   }  // End of method 'PPPExtendedKalmanFilter::Correct()'

}
