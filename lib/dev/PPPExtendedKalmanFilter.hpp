#pragma ident "$Id$"

/**
 * @file PPPExtendedKalmanFilter.hpp
 * Class to store state and covariance of Extended Kalman filter for PPP.
 */

#ifndef PPPEXTENDEDKALMANFILTER_HPP
#define PPPEXTENDEDKALMANFILTER_HPP

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

#include "TypeID.hpp"
#include "SimpleKalmanFilter.hpp"
#include "Position.hpp"
#include <map>

namespace gpstk
{

   class PPPExtendedKalmanFilter : public SimpleKalmanFilter
   {
   public:

         /// Common constructor.
      PPPExtendedKalmanFilter():recPos(0.0,0.0,0.0) {};

      PPPExtendedKalmanFilter(const Position& pos):
                              recPos(pos[0],pos[1],pos[2]) {};
 
        /// Get receiver position
      virtual Position getRxPosition(void) const
      { return recPos; }

         /// Set receiver position
      virtual void setRxPosition(const Position& pos)
      { recPos = pos; };

      virtual void setRxPosition(const double& X,
                                 const double& Y,
                                 const double& Z)
      { recPos[0] = X; recPos[1] = Y; recPos[2] = Z;};


         /// Destructor.
      virtual ~PPPExtendedKalmanFilter() {};
      
         /// the receiver postion
      Position recPos;
         // Map to store the value of the ambiguity
      std::map<SatID, double> ambiguityMap;

         /// A struct used to store Kalman Filter data
      struct filterData
      {
          filterData() {};

          std::map<TypeID, double> vCovMap;///< Variables covariance values

          std::map<SatID,  double> aCovMap;///< Ambiguities covariance values
          
      };

         /// Map holding the information regarding every satellite
      std::map<SatID,filterData> ambCovMap;

          /** Corrects (or "measurement updates") the a posteriori estimate
          *  of the system state vector, as well as the a posteriori estimate
          *  error covariance matrix, using as input the predicted a priori
          *  state vector and error covariance matrix, plus measurements and
          *  associated matrices.
          *
          * @param measurements      Measurements vector.
          * @param measurementsMatrix    Measurements matrix. Called geometry
          *                              matrix in GNSS.
          * @param measurementsNoiseCovariance   Measurements noise covariance
          *                                      matrix.
          *
          * @return
          *  0 if OK
          *  -1 if problems arose
          */
      virtual int Correct( const Vector<double>& measurements,
                           const Matrix<double>& measurementsMatrix,
                           const Matrix<double>& measurementsNoiseCovariance )
         throw(InvalidSolver);
   };

}  // End of namespace gpstk

#endif 
<<<<<<< HEAD

=======
>>>>>>> 902525c9b09a2f59b3ed34fa9311d56ffd119a55
