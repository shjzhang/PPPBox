#pragma ident "$Id: SolverPPPAR.hpp 2645 2011-06-08 03:23:24Z shjzhang $"

/**
 * @file SolverPPPAR.hpp
 * Class to compute the PPP Solution using MW/LC observations.
 */

#ifndef GPSTK_SOLVERPPPAR_HPP
#define GPSTK_SOLVERPPPAR_HPP

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
//  Shoujian Zhang - Wuhan University, 2012
//
//============================================================================
//
//  Revision
//
//  2014/02/24  create to process mixed model for PPP
//  2014/04/07  solve the ppp solution with extended model by fixing the 
//              zero-difference ambiguities.
//  2015/11/03  plan.
//              (1) add function to set the ambiguity resolution method
//
//============================================================================


#include "CodeKalmanSolver.hpp"
#include "GNSSconstants.hpp"
#include "Variable.hpp"
#include "DatumPPP.hpp"

namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the Precise Point Positioning (PPP) solution
       *  using a Kalman solver that estimate the ionospheric delays with
       *  P1/P2/L1/L2/Pdelta/Ldelta combinations.
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
       *      // Declare a SolverPPPAR object
       *   SolverPPPAR pppSolver;
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
       * The "SolverPPPAR" object will extract all the data it needs from the
       * GNSS data structure that is "gRin" and will try to solve the PPP
       * system of equations using a Kalman filter. It will also insert back
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
       * In any case, the "SolverPPPAR" object will also automatically add and
       * estimate the B1/B2/BWL ambiguities. The independent vector
       * is composed of the code and phase prefit residuals.
       *
       * This class expects some weights assigned to each satellite. That can
       * be achieved with objects from classes such as "ComputeIURAWeights",
       * "ComputeMOPSWeights", etc.
       *
       * If these weights are not assigned, then the "SolverPPPAR" object will
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
       * \warning "SolverPPPAR" is based on a Kalman filter, and Kalman filters
       * are objets that store their internal state, so you MUST NOT use the
       * SAME object to process DIFFERENT data streams.
       *
       * @sa SolverBase.hpp, SolverLMS.hpp and CodeKalmanSolver.hpp for
       * base classes.
       *
       */
   class SolverPPPAR : public CodeKalmanSolver
   {
   public:

         /** Common constructor.
          *
          * @param useNEU   If true, will compute dLat, dLon, dH coordinates;
          *                 if false (the default), will compute dx, dy, dz.
          */
      SolverPPPAR(bool useNEU = false);


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


         /** Code to be executed before 'Compute()' method.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& preCompute( gnssRinex& gData )
         throw(ProcessingException, SVNumException);


         /** Code to be executed after 'Compute()' method.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& postCompute( gnssRinex& gData )
         throw(ProcessingException);


         /** Returns a reference to a gnssRinex object after computing .
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Compute( gnssRinex& gData )
         throw(InvalidSolver);


         /** Time Update of the kalman filter.
          *
          * @param gData    Data object holding the data.
          */
      virtual int TimeUpdate( const Matrix<double>& phiMatrix,
                              const Matrix<double>& processNoiseCovariance  )
         throw(InvalidSolver);


         /** Returns a reference to a gnnsRinex object after applying
          * ambiguity constraint.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& AmbiguityConstr(gnssRinex& gData)
         throw(ProcessingException);



         /** Measurement Update of the kalman filter.
          *
          * @param gData    Data object holding the data.
          */
      virtual int MeasUpdate( const Vector<double>& prefitResiduals,
                              const Matrix<double>& designMatrix,
                              const Matrix<double>& weightMatrix  )
         throw(InvalidSolver);



         /** Returns a reference to a gnnsRinex object after fixing 
          *  all the potential ambiguities  
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& AmbiguityFixing(gnssRinex& gData)
         throw(ProcessingException);


         /** Update the free unknonws' solution and covariance after
          *  fixing the ambiguities
          *
          * @param gData    Data object holding the data.
          */
      virtual void AmbiguityUpdate(Vector<double>& state,
                                  Matrix<double>& covariance,
                                  Vector<double>& fixedFlag,
                                  int& index,
                                  double& fixedValue)
         throw(ProcessingException);


         /** Sets if a NEU system will be used.
          *
          * @param useNEU  Boolean value indicating if a NEU system will
          *                be used
          *
          */
      virtual SolverPPPAR& setNEU( bool useNEU );


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
      virtual SolverPPPAR& setCoordinatesModel(StochasticModel* pModel);


         /** Set restart interval.
          *
          * @param interval    interval that restart the filter.
          */
      virtual SolverPPPAR& setRestartInterval(double interval)
      {
         reIntialInterv = interval;
         return (*this);
      };

         /** Set restart interval.
          *
          * @param interval    interval that restart the filter.
          */
      virtual SolverPPPAR& setReInitialize(double reInit)
      {
         reInitialize = reInit;
         return (*this);
      };


         /** Set Ambiguity resolution method.
          *
          * @param ARMethod  ambiguity resolution method
          */
      virtual SolverPPPAR& setARMethod(string ambMethod)
      {
         ARMethod = ambMethod;
         return (*this);
      };


         /** Returns the solution associated to a given TypeID.
          *
          * @param type    TypeID of the solution we are looking for.
          */
      virtual double getSolution(const TypeID& type) const
         throw(InvalidRequest);


         /** Returns the variance associated to a given TypeID.
          *
          * @param type    TypeID of the variance we are looking for.
          */
      virtual double getVariance(const TypeID& type) const
         throw(InvalidRequest);



         /** Return the CURRENT number of satellite.
          */
      virtual int getCurrentSatNumber() const
         throw(InvalidRequest);


         /** Return the CURRENT number of satellite.
          */
      virtual int getFixedAmbNumL1() const
         throw(InvalidRequest);


          /** Set buffer size for convergence statistics. 
           */
      virtual SolverPPPAR& setBufferSize(int size )
      { bufferSize = size; return(*this); };


         /** Return the converged flag
          */
      virtual bool getConverged() const
         throw(InvalidRequest);


         /** Return the CURRENT number of satellite.
          */
      virtual int getFixedAmbNumWL() const
         throw(InvalidRequest);


      virtual std::vector<double> getTTFFWL() const
      {
          return ttffWL;
      }

      virtual std::vector<double> getTTFFL1() const
      {
          return ttffL1;
      }

      virtual std::vector<double> getTTFFL2() const
      {
          return ttffL2;
      }

      virtual std::vector<double> getSTime() const
      { return startTimeVec; }



         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;

         /// Debug level
      static int debugLevel;

         /// Destructor.
      virtual ~SolverPPPAR() {};


   private:


         /// Ambiguity datum
      DatumPPP datumWL;

         /// Ambiguity datum
      DatumPPP datumNL;

         /// Whether turn on the 'reInitialize' or not
      bool reInitialize;

         /// Interval to restart the filter
      double reIntialInterv;

         /// Ambiguity resolution method
      string ARMethod;

         /// Boolean to indicate whether the solution is converged
      bool converged;

         /// Size for convergBuffer
      int bufferSize;

         /// Buffer to store the solution drou 
      std::deque<bool> convergBuffer;

         /// Variable to indicate the first epoch
      CommonTime firstEpoch;

         /// Number of variables
      int numVar;

         /// Number of unknowns
      int numUnknowns;

         /// Number of measurements
      int numMeas;

         /// Number of measurements
      int numCurrentSV;

         /// Number of fixed widelane ambiguities
      int numFixedBWL;

         /// Number of fixed widelane ambiguities
      int numFixedBL1;

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

         /// Source-indexed TypeID set
      TypeIDSet srcIndexedTypes;

         /// Satellite-indexed TypeID set
      TypeIDSet satIndexedTypes;

         /// Observable-independent TypeID set
      TypeIDSet TropAndCoord;

         /// Global set of unknowns
      VariableSet varUnknowns;

         /// Map holding state information for satellite related variables
      VariableDataMap stateMap;

         /// A structure used to store Kalman filter data.
      struct filterData
      {
            // Default constructor 
         filterData() {};

         std::map<TypeID, double> srcIndexedVarCov;  ///< source indexed variables' covariance values.
         std::map<Variable, double> satIndexedVarCov;  ///< satellite indexed variables' covariance values.

      };

         /// Map holding covariance information
      std::map<Variable, filterData> covarianceMap;


         /// Boolean indicating if this filter was run at least once
      bool firstTime;


      bool resetL1;
      bool resetL2;
      bool resetWL;

      double startTime;

      std::vector< double > startTimeVec;
      std::vector< double > ttffL1;
      std::vector< double > ttffL2;
      std::vector< double > ttffWL;


         // Predicted state
      Vector<double> xhatminus;


         // Predicted covariance.
      Matrix<double> Pminus;


         // A posteriori state
      Vector<double> xhat;


         // A posteriori covariance.
      Matrix<double> P;

         // The updated state 
      Vector<double> newState ;
      
     
         // The updated covariance
      Matrix<double> newCov;


         /// Set with all satellites being processed this epoch
      SatIDSet currSatSet;


         /// Set with all satellites being processed this epoch
      SatIDSet satSet;


         /// General Kalman filter object
      SimpleKalmanFilter kFilter;


         /// Initializing method.
      void Init(void);


         /// White noise stochastic model for position
      WhiteNoiseModel whitenoiseModelX;
      WhiteNoiseModel whitenoiseModelY;
      WhiteNoiseModel whitenoiseModelZ;


         /// Random Walk stochastic model
      TropoRandomWalkModel tropoModel;

         /// Constant stochastic model
      StochasticModel constantModel;

         /// White noise stochastic model
      WhiteNoiseModel whitenoiseModel;

         /// Phase Ambiguity models
      PhaseAmbiguityModel ambiModelLC;
      PhaseAmbiguityModel ambiModelWL;

         /// Pointer to stochastic model for troposphere
      StochasticModel* pTropoStoModel;

         /// Pointer to stochastic model for dx (or dLat) coordinate
      StochasticModel* pCoordXStoModel;
      StochasticModel* pCoordYStoModel;
      StochasticModel* pCoordZStoModel;

         /// Pointer to stochastic model for receiver clock
      StochasticModel* pClockStoModel;
      StochasticModel* pClockStoModelLC;
      StochasticModel* pBiasStoModelMW;

         /// Pointer to stochastic model for ambiguity on L1
      StochasticModel* pAmbiModelLC;
      StochasticModel* pAmbiModelWL; // for WL ambiguity


         // First, let's define a set with the typical code-based unknowns
      TypeIDSet defaultTypeSet;

         // Then, define a set with the typical for satellite-related unknowns
      TypeIDSet varTypeSet;


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;


         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


         // Some methods that we want to hide
      virtual int Compute( const Vector<double>& prefitResiduals,
                           const Matrix<double>& designMatrix )
         throw(InvalidSolver)
      { return 0; };


      virtual SolverPPPAR& setDefaultEqDefinition(
                             const gnssEquationDefinition& eqDef )
      { return (*this); };



   }; // End of class 'SolverPPPAR'

      //@}

}  // End of namespace gpstk
#endif   // GPSTK_SOLVERPPPAR_HPP
