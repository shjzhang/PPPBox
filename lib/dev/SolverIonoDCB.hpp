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
 * @file SolverIonoDCB.hpp
 * Class to model the ionosphere and estimate DCBs of satellite and receivers
 * together using spherical harmonic expansion.
 *
 * created by Wei Wang, Wuhan University, 2016/03/01
 */

#ifndef GPSTK_SolverIonoDCB_HPP
#define GPSTK_SolverIonoDCB_HPP

#include "CodeKalmanSolver.hpp"
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
       * \warning "SolverIonoDCB" is based on a Kalman filter, and Kalman filters
       * are objets that store their internal state, so you MUST NOT use the
       * SAME object to process DIFFERENT data streams.
       *
       * @sa SolverBase.hpp, SolverLMS.hpp and CodeKalmanSolver.hpp for
       * base classes.
       *
       */
   class SolverIonoDCB : public CodeKalmanSolver
   {
   public:

      SolverIonoDCB(int SHOrder = 2);

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
                           const Matrix<double>& designMatrix,
                           const Matrix<double>& weightMatrix )
        throw(InvalidSolver);


         /** Returns a reference to a gnssDataMap object after
          *  solving the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(CommonTime& epoch, gnssDataMap& gData)
         throw(ProcessingException);


         /** Resets the PPP internal Kalman filter.
          *
          * @param newState         System state vector
          * @param newErrorCov      Error covariance matrix
          *
          * \warning Take care of dimensions: In this case newState must be 6x1
          * and newErrorCov must be 6x6.
          *
          */
      virtual SolverIonoDCB& Reset( const Vector<double>& newState,
                                const Matrix<double>& newErrorCov )
      { kFilter.Reset( newState, newErrorCov ); return (*this); };

         /** Get the weight factor multiplying the phase measurements sigmas.
          *  This factor is the code_sigma/phase_sigma ratio.
          */
    
         /// Get the State Transition Matrix (phiMatrix)
      virtual Matrix<double> getPhiMatrix(void) const
      { return phiMatrix; };


         /** Set the State Transition Matrix (phiMatrix)
          *
          * @param pMatrix     State Transition matrix.
          *
          * \warning Process() methods set phiMatrix and qMatrix according to
          * the stochastic models already defined. Therefore, you must use
          * the Compute() methods directly if you use this method.
          *
          */
      virtual SolverIonoDCB& setPhiMatrix(const Matrix<double> & pMatrix)
      { phiMatrix = pMatrix; return (*this); };


         /// Get the Noise covariance matrix (QMatrix)
      virtual Matrix<double> getQMatrix(void) const
      { return qMatrix; };


         /** Set the Noise Covariance Matrix (QMatrix)
          *
          * @param pMatrix     Noise Covariance matrix.
          *
          * \warning Process() methods set phiMatrix and qMatrix according to
          * the stochastic models already defined. Therefore, you must use
          * the Compute() methods directly if you use this method.
          *
          */
      virtual SolverIonoDCB& setQMatrix(const Matrix<double> & pMatrix)
      { qMatrix = pMatrix; return (*this); };

         /// prepare before the filter
      SolverIonoDCB& prepare(void);

      SolverIonoDCB& getSolution(void);
        /// Norm Factor
      double norm(int n, int m);
        /// Legendre Polynomial
      double legendrePoly(int n, int m, double u);

         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverIonoDCB() {};


   private:
     
          // the max order of spherical harmonic(SH), dedault 2;
      int order;
         /// Number of SH coefficents
      int numIonoCoef;
         /// Number of unknowns
      int numUnknowns;
         /// Number of measurements
      int numMeas;
         /// the geographic latitude of geomagnetic north pole
      static const double NGPLat = 80.27;
         /// the geographic longitude of geomagnetic north pole
      static const double NGPLon = -72.58;
        /// Source-indexed(receiver-indexed) TypeID set
      TypeIDSet recIndexedTypes;
         /// Satellite-indexed TypeID set
      TypeIDSet satIndexedTypes;
         /// Global set of unknowns
      VariableSet recUnknowns;
         /// Global set of unknowns
      VariableSet satUnknowns;
         /// Map holding state information for receiver related variables
      VariableDataMap recState;
         /// The value of  ionospheric coefficients 
      VariableDataMap satState;
         /// The value of  ionospheric coefficients 
		 /// the sequence is A00,A10,A11,B11,A20,A21,B21,...
	  Vector<double> IonoCoef;

         /// A structure used to store Kalman filter data.
      struct filterData
      {
            // Default constructor 
         filterData() {};

         std::map<Variable, double> recIndexedVarCov;  ///< source indexed variables' covariance values.
         std::map<Variable, double> satIndexedVarCov;  ///< satellite indexed variables' covariance values.
         Vector<double> ionoCoefVarCov;     ///< ionospheric coefficents covariance values 
      };

         /// Map holding covariance information
      std::map<Variable, filterData> covarianceMap;
         /// Pointer to stochastic model for DCBs of satellite
      StochasticModel* pSatDCBStoModel;
         /// Pointer to stochastic model for DCBs of receiver
      StochasticModel* pRecDCBStoModel;

         /// Pointer to stochastic model for SH coefficients
      StochasticModel* pCoefStoModel;
         /// State Transition Matrix (PhiMatrix)
      Matrix<double> phiMatrix;


         /// Noise covariance matrix (QMatrix)
      Matrix<double> qMatrix;


         /// Geometry matrix
      Matrix<double> hMatrix;


         /// Weights matrix
      Matrix<double> rMatrix;


         /// Measurements vector (Prefit-residuals)
      Vector<double> measVector;

      /// Set with all satellites being processed this epoch
      SatIDSet currSatSet;
         /// Set with all satellites being processed this epoch
      SatIDSet satSet;
     /// Set with all satellites which are not in view 
      SatIDSet satNotInView;
	  /// Set with all receivers being processed with epoch
	  SourceIDSet recSet;
         /// Boolean indicating if this filter was run at least once
      bool firstTime;
   
         /// General Kalman filter object
      SimpleKalmanFilter kFilter;

         /// Initializing method.
      void Init(void);

         /// Constant stochastic model for dcbs
      StochasticModel constantModel;
	  RecBiasRandomWalkModel recDCBModel;

	  SatBiasRandomWalkModel satDCBModel;

         /// random walk model for SH coefficents
      RandomWalkModel CoefModel;
         /// Some methods that we want to hide
         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


      virtual int Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix )
         throw(InvalidSolver)
      { return 0; };


   }; // End of class 'SolverIonoDCB'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SolverIonoDCB_HPP
