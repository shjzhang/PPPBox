#pragma ident "$Id$"

/**
 * @file SolverPPPPredict.hpp
 * Class to compute the PPP Solution using Extended Kalman Filter.
 */

#ifndef GPSTK_SOLVERPPPPREDICT_HPP
#define GPSTK_SOLVERPPPPREDICT_HPP

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


#include <deque>
#include <list>
#include "CodeKalmanSolver.hpp"
#include "PPPExtendedKalmanFilter.hpp"

namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the Precise Point Positioning (PPP) solution
       *  using a Kalman solver that combines ionosphere-free code and phase
       *  measurements.
       *
       * This class may be used either in a Vector- and Matrix-oriented way,
       * or with GNSS data structure objects from "DataStructures" class (much
       * more simple to use it this way).
       *
       * A typical way to use this class with GNSS data structures follows:
       *
       * @code
       *      // INITIALIZATION PART
       *
       *      // EBRE station nominal position
       *   Position nominalPos(4833520.192, 41537.1043, 4147461.560);
       *   RinexObsStream rin("ebre0300.02o");  // Data stream
       *
       *      // Load all the SP3 ephemerides files
       *   SP3EphemerisStore SP3EphList;
       *   SP3EphList.loadFile("igs11512.sp3");
       *   SP3EphList.loadFile("igs11513.sp3");
       *   SP3EphList.loadFile("igs11514.sp3");
       *
       *   NeillTropModel neillTM( nominalPos.getAltitude(),
       *                           nominalPos.getGeodeticLatitude(),
       *                           30 );
       *
       *      // Objects to compute the tropospheric data
       *   BasicModel basicM(nominalPos, SP3EphList);
       *   ComputeTropModel computeTropo(neillTM);
       *
       *      // More declarations here: ComputeMOPSWeights, SimpleFilter,
       *      // LICSDetector, MWCSDetector, SolidTides, OceanLoading, 
       *      // PoleTides, CorrectObservables, ComputeWindUp, ComputeLinear,
       *      // LinearCombinations, etc.
       *
       *      // Declare a SolverPPP object
       *   SolverPPP pppSolver;
       *
       *     // PROCESSING PART
       *
       *   gnssRinex gRin;
       *
       *   while(rin >> gRin)
       *   {
       *      try
       *      {
       *         gRin  >> basicM
       *               >> correctObs
       *               >> compWindup
       *               >> computeTropo
       *               >> linear1      // Compute combinations
       *               >> pcFilter
       *               >> markCSLI2
       *               >> markCSMW
       *               >> markArc
       *               >> linear2      // Compute prefit residuals
       *               >> phaseAlign
       *               >> pppSolver;
       *      }
       *      catch(...)
       *      {
       *         cerr << "Unknown exception at epoch: " << time << endl;
       *         continue;
       *      }
       *
       *         // Print results
       *      cout << time.DOYsecond()      << "  "; // Output field #1
       *      cout << pppSolver.solution[1] << "  "; // dx: Output field #2
       *      cout << pppSolver.solution[2] << "  "; // dy: Output field #3
       *      cout << pppSolver.solution[3] << "  "; // dz: Output field #4
       *      cout << pppSolver.solution[0] << "  "; // wetTropo: Out field #5
       *      cout << endl;
       *   }
       * @endcode
       *
       * The "SolverPPP" object will extract all the data it needs from the
       * GNSS data structure that is "gRin" and will try to solve the PPP
       * system of equations using a Kalman filter. It will also push_back back
       * postfit residual data (both code and phase) into "gRin" if it
       * successfully solves the equation system.
       *
       * By default, it will build the geometry matrix from the values of
       * coefficients wetMap, dx, dy, dz and cdt, IN THAT ORDER. Please note
       * that the first field of the solution will be the estimation of the
       * zenital wet tropospheric component (or at least, the part that wasn't
       * modeled by the tropospheric model used).
       *
       * You may configure the solver to work with a NEU system in the class
       * constructor or using the "setNEU()" method.
       *
       * In any case, the "SolverPPP" object will also automatically add and
       * estimate the ionosphere-free phase ambiguities. The independent vector
       * is composed of the code and phase prefit residuals.
       *
       * This class expects some weights assigned to each satellite. That can
       * be achieved with objects from classes such as "ComputeIURAWeights",
       * "ComputeMOPSWeights", etc.
       *
       * If these weights are not assigned, then the "SolverPPP" object will
       * set a value of "1.0" to code measurements, and "weightFactor" to phase
       * measurements. The default value of "weightFactor" is "10000.0". This
       * implies that code sigma is 1 m, and phase sigma is 1 cm.
       *
       * By default, the stochastic models used for each type of variable are:
       *
       *    \li Coordinates are modeled as constants (StochasticModel).
       *    \li Zenital wet tropospheric component is modeled as a random walk
       *        (RandomWalkModel), with a qPrime value of 3e-8 m*m/s.
       *    \li Receiver clock is modeled as white noise (WhiteNoiseModel).
       *    \li Phase biases are modeled as white noise when cycle slips
       *        happen, and as constants between cycle slips
       *        (PhaseAmbiguityModel).
       *
       * You may change this assignment with methods "setCoordinatesModel()",
       * "setXCoordinatesModel()", "setYCoordinatesModel()",
       * "setZCoordinatesModel()", "setTroposphereModel()" and
       * "setReceiverClockModel()". However, you are not allowed to change the
       * phase biases stochastic model.
       *
       * For instance, in orden to use a 'full kinematic' mode we assign a white
       * noise model to all the coordinates:
       *
       * @code
       *      // Define a white noise model with 100 m of sigma
       *   WhiteNoiseModel wnM(100.0);
       *
       *      // Configure the solver to use this model for all coordinates
       *   pppSolver.setCoordinatesModel(&wnM);
       * @endcode
       *
       * Be aware, however, that you MUST NOT use this method to set a
       * state-aware stochastic model (like RandomWalkModel, for instance)
       * to ALL coordinates, because the results will certainly be erroneous.
       * Use this method ONLY with non-state-aware stochastic models like
       * 'StochasticModel' (constant coordinates) or 'WhiteNoiseModel'.
       *
       * In order to overcome the former limitation, this class provides methods
       * to set different, specific stochastic models for each coordinate, like:
       *
       * @code
       *      // Define a white noise model with 2 m of sigma for horizontal
       *      // coordinates (in this case, the solver is previously set to use
       *      // dLat, dLon and dH).
       *   WhiteNoiseModel wnHorizontalModel(2.0);
       *
       *      // Define a random walk model with 0.04 m*m/s of process spectral
       *      // density for vertical coordinates.
       *   RandomWalkModel rwVerticalModel(0.04);
       *
       *      // Configure the solver to use these models
       *   pppSolver.setXCoordinatesModel(&wnHorizontalModel);
       *   pppSolver.setYCoordinatesModel(&wnHorizontalModel);
       *   pppSolver.setZCoordinatesModel(&rwVerticalModel);
       * @endcode
       *
       *
       * \warning "SolverPPP" is based on a Kalman filter, and Kalman filters
       * are objets that store their internal state, so you MUST NOT use the
       * SAME object to process DIFFERENT data streams.
       *
       * @sa SolverBase.hpp, SolverLMS.hpp and CodeKalmanSolver.hpp for
       * base classes.
       *
       */
   class SolverPPPPredict : public CodeKalmanSolver
   {
    public:
         
            /// Common constructor.
      SolverPPPPredict(PPPExtendedKalmanFilter& pppEKF,
                       const double& interval);

            /// Common constructor.
      SolverPPPPredict(PPPExtendedKalmanFilter& pppEKF);

         /** Returns a reference to a gnnsSatTypeValue object after
          *  solving the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException, SVNumException);


         /** Returns a reference to a gnnsRinex object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException, SVNumException);

         /// Get stochastic model pointer for dx (or dLat) coordinate
      StochasticModel* getXCoordinatesModel() const
      { return pCoordXStoModel; };


         /** Set coordinates stochastic model for dx (or dLat) coordinate
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    dx (or dLat) coordinate.
          */
      SolverPPPPredict& setXCoordinatesModel(StochasticModel* pModel)
      { pCoordXStoModel = pModel; return (*this); };


         /// Get stochastic model pointer for dy (or dLon) coordinate
      StochasticModel* getYCoordinatesModel() const
      { return pCoordYStoModel; };


         /** Set coordinates stochastic model for dy (or dLon) coordinate
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    dy (or dLon) coordinate.
          */
      SolverPPPPredict& setYCoordinatesModel(StochasticModel* pModel)
      { pCoordYStoModel = pModel; return (*this); };


         /// Get stochastic model pointer for dz (or dH) coordinate
      StochasticModel* getZCoordinatesModel() const
      { return pCoordZStoModel; };


         /** Set coordinates stochastic model for dz (or dH) coordinate
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    dz (or dH) coordinate.
          */
      SolverPPPPredict& setZCoordinatesModel(StochasticModel* pModel)
      { pCoordZStoModel = pModel; return (*this); };


         /** Set a single coordinates stochastic model to ALL coordinates.
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
      virtual SolverPPPPredict& setCoordinatesModel(StochasticModel* pModel);


         /// Get wet troposphere stochastic model pointer
      virtual StochasticModel* getTroposphereModel(void) const
      { return pTropoStoModel; };


         /** Set zenital wet troposphere stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    zenital wet troposphere.
          *
          * \warning Be aware that some stochastic models store their internal
          * state (for instance, 'RandomWalkModel' and 'PhaseAmbiguityModel').
          * If that is your case, you MUST NOT use the SAME model in DIFFERENT
          * solver objects.
          */
      virtual SolverPPPPredict& setTroposphereModel(StochasticModel* pModel)
      { pTropoStoModel = pModel; return (*this); };


         /// Get receiver clock stochastic model pointer
      virtual StochasticModel* getReceiverClockModel(void) const
      { return pClockStoModel; };


         /** Set receiver clock stochastic model
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    receiver clock.
          *
          * \warning Be aware that some stochastic models store their internal
          * state (for instance, 'RandomWalkModel' and 'PhaseAmbiguityModel').
          * If that is your case, you MUST NOT use the SAME model in DIFFERENT
          * solver objects.
          */
      virtual SolverPPPPredict& setReceiverClockModel(StochasticModel* pModel)
      { pClockStoModel = pModel; return (*this); };


         /// Get phase biases stochastic model pointer
      virtual StochasticModel* getPhaseBiasesModel(void) const
      { return pBiasStoModel; };


         /** Set phase biases stochastic model.
          *
          * @param pModel      Pointer to StochasticModel associated with
          *                    phase biases.
          *
          * \warning Be aware that some stochastic models store their internal
          * state (for instance, 'RandomWalkModel' and 'PhaseAmbiguityModel').
          * If that is your case, you MUST NOT use the SAME model in DIFFERENT
          * solver objects.
          *
          * \warning This method should be used with caution, because model
          * must be of PhaseAmbiguityModel class in order to make sense.
          */
      virtual SolverPPPPredict& setPhaseBiasesModel(StochasticModel* pModel)
      { pBiasStoModel = pModel; return (*this); };

         /// Get the predicted state
      virtual Vector<double> getPredState(void) const
         throw(InvalidRequest);

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
      virtual SolverPPPPredict& setPhiMatrix(const Matrix<double> & pMatrix)
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
      virtual SolverPPPPredict& setQMatrix(const Matrix<double> & pMatrix)
      { qMatrix = pMatrix; return (*this); };

          /** Set the core variable list
           */
      virtual SolverPPPPredict& setCoreVarList(void);

          /** Set the positioning mode, kinematic or static.
           */
      virtual void setKinematic(void)
      { kinematicMode = true; };

          /// Set the acceleration sigma
      virtual void setAccSigma(const double& sigma)
      { AccSigma = sigma; };

         /// Set time interval 
      virtual void setInterval(const double& interval)
      { obsInterval = interval; };

         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverPPPPredict() {};


   private:

          /// Pointer to PPPExtendedKalmanFilter
      PPPExtendedKalmanFilter* pEKFStateStore;
          /// container to store variable
      std::list<TypeID> coreVarList;

          /// the receiver state(static or kinematic)
      bool kinematicMode;

          /// observable interval
      double obsInterval;

          /// In kinematicMode, treat acceleration as white noise
          /// 1.0m/s^2 as default
      double AccSigma;

         /// Number of variables
      int numVar;

         /// Number of unknowns
      int numUnknowns;

	     /// state transition matrix
	  Matrix<double> phiMatrix;

         /// noise covariance matrix
	  Matrix<double> qMatrix;

         /// Pointer to stochastic model for dx coordinate
      StochasticModel* pCoordXStoModel;

         /// Pointer to stochastic model for dy coordinate
      StochasticModel* pCoordYStoModel;

         /// Pointer to stochastic model for dz coordinate
      StochasticModel* pCoordZStoModel;


         /// Pointer to stochastic model for troposphere
      StochasticModel* pTropoStoModel;


         /// Pointer to stochastic model for receiver clock
      StochasticModel* pClockStoModel;


         /// Pointer to stochastic model for phase biases
      StochasticModel* pBiasStoModel;
          

         /// Boolean indicating if this filter was run at least once
      bool firstTime;


         /// Initializing method.
      void Init(void);


         /// Constant stochastic model
      StochasticModel constantModel;

         /// White noise stochastic model for position
      WhiteNoiseModel whitenoiseModelX;
      WhiteNoiseModel whitenoiseModelY;
      WhiteNoiseModel whitenoiseModelZ;


         /// Random Walk stochastic model
      RandomWalkModel rwalkModel;


         /// White noise stochastic model
      WhiteNoiseModel whitenoiseModel;


         /// Phase biases stochastic model (constant + white noise)
      PhaseAmbiguityModel biasModel;


         // Some methods that we want to hide
         /// Initial index assigned to this class.
      static int classIndex;

         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


   }; // End of class 'SolverPPPPredct'

      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SOLVERPPPPREDICT_HPP
