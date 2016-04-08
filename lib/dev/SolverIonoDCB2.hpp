//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
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
//  Copyright 2004, The University of Texas at Austin
//  Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009, 2011
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file SolverIonoDCB2.hpp
 * Class to model the ionosphere and estimate DCBs of satellite and receivers
 * together using spherical harmonic expansion.
 *
 * created by Wei Wang, Wuhan University, 2016/03/01
 */

#ifndef GPSTK_SolverIonoDCB2_HPP
#define GPSTK_SolverIonoDCB2_HPP

#include "SolverBase.hpp"
#include "Variable.hpp"
#include <vector>
namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the Precise Point Positioning (PPP) solution
       *  using a Kalman solver that combines ionosphere-free code and phase
       *  measurements.
       *
       * \warning "SolverIonoDCB2" is based on a Kalman filter, and Kalman filters
       * are objets that store their internal state, so you MUST NOT use the
       * SAME object to process DIFFERENT data streams.
       *
       * @sa SolverBase.hpp, SolverLMS.hpp and CodeKalmanSolver.hpp for
       * base classes.
       *
       */
   class SolverIonoDCB2 :  public SolverBase
   {
   public:

      SolverIonoDCB2(int SHOrder = 4);

         /** Compute the PPP Solution of the given equations set.
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
      virtual int Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix)
         throw(InvalidSolver);


         /** Returns a reference to a gnssDataMap object after
          *  solving the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(gnssDataMap& gData, satValueMap& satMap);


      SolverIonoDCB2&  prepare(void);
        /// Norm Factor
      double norm(int n, int m);
        /// Legendre Polynomial
      double legendrePoly(int n, int m, double u);

         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverIonoDCB2() {};


   private:
     
          // the max order of spherical harmonic(SH), dedault 4;
      int order;
         /// Number of SH coefficents
      int numIonoCoef;
         /// Number of unknowns
      int numUnknowns;
         /// Number of measurements
      int numMeas;

        /// Source-indexed(receiver-indexed) TypeID set
      TypeIDSet recIndexedTypes;
         /// Satellite-indexed TypeID set
      TypeIDSet satIndexedTypes;
         /// Global set of unknowns
      VariableSet recUnknowns;
         /// Global set of unknowns
      VariableSet satUnknowns;
         /// Map holding the state information for reciver related varialbes
	  VariableDataMap  recState;
         /// Map holding state information for satellite related variables
      VariableDataMap satState;
         /// The value of  ionospheric coefficients 
		 /// the sequence is A00,A10,A11,B11,A20,A21,B21,...
	  Vector<double> IonoCoef;

      Matrix<double> hMatrix;
         /// Measurements vector (Prefit-residuals)
      Vector<double> measVector;
         /// Constraint vector of satellite DCBs
      Matrix<double> consMatrix;
	     /// Constrain Vector 
	  Vector<double> consVector;

      /// Set with all satellites being processed this epoch
      SatIDSet currSatSet;
         /// Set with all satellites which are not in view 
      SatIDSet satNotInView;
	  /// Set with all receivers being processed with epoch
	  SourceIDSet recSet;
         /// Boolean indicating if this filter was run at least once

         /// Initializing method.
      void Init(void);

         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


     

   }; // End of class 'SolverIonoDCB2'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SolverIonoDCB2_HPP
