#pragma ident "$Id$"

/**
 * @file SolverGeneralFB.hpp
 * Class to compute the solution for general equaions in 
 * forwards-backwards mode.
 */

#ifndef GPSTK_SOLVERPPPFB_HPP
#define GPSTK_SOLVERPPPFB_HPP

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
//  Revision
//  --------
//  2014/02/17      Modify this program from the 'SolverPPPFB' 
//
//  Author
//  ------
//  Shoujian Zhang, Wuhan University, 2014, email:shjzhang@sgg.whu.edu.cn
//
//============================================================================


#include "SolverGeneral.hpp"
#include <list>
#include <set>


namespace gpstk
{

      /** @addtogroup GPSsolutions */
      /// @ingroup math

      //@{

      /** This class computes the 'general' equations using a Kalman solver 
       *  in the 'forwards-backwards' approach, where the provided data set 
       *  is processed from 'past to future' and from 'future to past' several 
       *  times.
       *
       * Just as the 'SolverPPPFB' is the 'forwards-backwards' version of
       * 'SolverPPP', so is the 'SolverGeneralFB' to 'SolverGeneral'. 
       *
       * This approach improves the final solution because it takes advantage
       * of a better phase ambiguity resolution. On the other hand, it is only
       * applicable in post-process mode (of course).
       *
       * Actually, This approach can't improve the precision of the kinematic
       * reciever's solution, for that the phase ambiguities are not 
       * continuous parameters, and the variance and covarince information 
       * from last cycle is not stored as a-priori values for this new cycle.
       *
       * In reality, "SolverGeneralFB.hpp" objects are "SolverGeneral.hpp"
       * objects at their core, with some wrapper code that takes 'normal'
       * forwards input data, stores them, and feeds the internal "SolverGeneral"
       * object with a continuous data stream formed by several instances of
       * forwards input data, "mirrored" input data (oldest is newest and
       * viceversa), forwards input data again, and so on.
       *
       * In order to achieve this, SolverGeneralFB.hpp" objects work in three
       * distinct phases:
       *
       *    \li Initial "Process()" phase: This does a "normal" forward
       *        processing, like the "SolverGeneral.hpp" objects. Data feed 
       *        during this phase are internally stored for later phases.
       *    \li "ReProcess()" phase: In this phase the filter will process the
       *        stored data in "backwards-forwards" fashion as many times as
       *        set by the provided "cycles" parameter (by default, "cycles" is
       *        equal to 1). This phase always ends in backwards processing.
       *    \li "LastProcess()" phase: This is the last phase and it is always
       *        done in forwards mode. During this phase you will get your
       *        final results.
       *
       * Take due note that the "SolverGeneralFB.hpp" class is designed to be 
       * used ONLY with GNSS data structure objects from "DataStructures" class.
       *
       *
       * A typical way to use this class follows:
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
       *      // You can set up the equation system, which will
       *      // be insert in the 'SolverGeneralFB'
       *
       *      // SETTING THE RULES: DEFINE VARIABLES
       *
       *      // Declare stochastic models to be used
       *   StochasticModel coordinatesModel;
       *   TropoRandomWalkModel tropoModel;
       *   PhaseAmbiguityModel ambiModel;
       *
       *      // These variables are, by default, SourceID-indexed
       *   Variable dx( TypeID::dx, &coordinatesModel, true, false, 100.0 );
       *
       *   Variable dy( TypeID::dy, &coordinatesModel );
       *   dy.setInitialVariance( 100.0 );     // Equivalent to 'dx' setup
       *
       *   Variable dz( TypeID::dz, &coordinatesModel );
       *   dz.setInitialVariance( 100.0 );
       *
       *   Variable cdt( TypeID::cdt );
       *   cdt.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *   Variable tropo( TypeID::wetMap, &tropoModel );
       *   tropo.setInitialVariance( 25.0 );
       *
       *
       *      // The following variable is, SourceID and SatID-indexed
       *   Variable ambi( TypeID::BLC, &ambiModel, true, true );
       *   ambi.setDefaultForced(true);   // Force default coefficient (1.0)
       *
       *      // This will be the independent term for code equation
       *   Variable prefitC( TypeID::prefitC );
       *
       *      // This will be the independent term for phase equation
       *   Variable prefitL( TypeID::prefitL );
       *
       *
       *      // SETTING THE RULES: DESCRIBE EQUATIONS
       *
       *      // Define Equation object for code equations, and add variables
       *   Equation equPC( prefitC );
       *
       *   equPC.addVariable(dx);
       *   equPC.addVariable(dy);
       *   equPC.addVariable(dz);
       *   equPC.addVariable(cdt);
       *   equPC.addVariable(tropo);
       *
       *      // Define Equation object for phase equations, and add variables
       *   Equation equLC( prefitL );
       *
       *   equLC.addVariable(dx);
       *   equLC.addVariable(dy);
       *   equLC.addVariable(dz);
       *   equLC.addVariable(cdt);
       *   equLC.addVariable(tropo);
       *   equLC.addVariable(ambi);       // This variable is for phase only
       *
       *      // Phase equations should have higher relative weight
       *   equLC.setWeight(10000.0);     // 100.0 * 100.0
       *
       *
       *      // SETTING THE RULES: SETUP EQUATION SYSTEM
       *
       *      // Create 'EquationSystem' object
       *   EquationSystem eqSystem;
       *
       *      // Add equation descriptions
       *   eqSystem.addEquation(equPC);
       *   eqSystem.addEquation(equLC);
       *
       *      // Declare a SolverGeneralFB object
       *   SolverGeneralFB fbSolver(eqSystem);
       *
       *     // PROCESSING PART
       *
       *   gnssRinex gRin;
       *
       *      // --->>> Process() phase <<<--- //
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
       *               >> fbSolver;
       *      }
       *      catch(...)
       *      {
       *         cerr << "Unknown exception at epoch: " << time << endl;
       *         continue;
       *      }
       *
       *   }   // End of 'while(rin >> gRin)'
       *
       *
       *      // --->>> ReProcess() phase <<<--- //
       *
       *   try
       *   {
       *
       *         // Now, let's do the forward-backward cycles (4)
       *      fbSolver.ReProcess(4);
       *
       *   }
       *   catch(Exception& e)
       *   {
       *      cerr << "Exception: " << e << endl;
       *   }
       *
       *
       *      // --->>> LastProcess() phase <<<--- //
       *
       *      // Loop over all data epochs, again
       *   while( fbSolver.LastProcess(gRin) )  // True while there are data
       *   {
       *
       *         // In this case, gRin is an output from 'LastProcess()'
       *      DayTime time(gRin.header.epoch);
       *
       *         // Print results
       *      cout << time.DOYsecond() << "  ";     // Epoch - Output field #1
       *
       *      cout << fbSolver.getSolution(TypeID::dLat) << "  "; // dLat - #2
       *      cout << fbSolver.getSolution(TypeID::dLon) << "  "; // dLon - #3
       *      cout << fbSolver.getSolution(TypeID::dH) << "  ";   // dH   - #4
       *      cout << fbSolver.getSolution(TypeID::wetMap) << "  "; // Tropo-#5
       *
       *   }
       * @endcode
       *
       * The "SolverGeneralFB" object will also insert back postfit residual data
       * (both code and phase) into "gRin" if it successfully solves the
       * equation system.
       *
       * It should be noted that this class don't support the "checkLimits()", 
       * that in 'SolverPPPFB', because this class will process the 'general'
       * equations, and can't distinguish the 'code' and 'phase' type. But you
       * can set the limits value for the special TypeID you want.
       * 
       * A way to apply this feature follows:
       *
       * @code
       *      // INITIALIZATION PART
       *
       *      // INITIALIZATION CODE HERE...
       *
       *      // Declare a SolverGeneralFB object
       *   SolverGeneralFB fbSolver;
       *
       *      // PROCESSING PART CODE HERE...
       *
       *
       *      // --->>> ReProcess() phase <<<--- //
       *
       *   try
       *   {
       *
       *         // Now, let's do the forward-backward cycles.
       *         // Please note that this method needs no parameters.
       *         // Three forwards-backwards cycles will be made because
       *         // maximum limits list size is 3.
       *      fbSolver.ReProcess();
       *
       *   }
       *   catch(Exception& e)
       *   {
       *      cerr << "Exception: " << e << endl;
       *   }
       *
       *
       *      // LastProcess() PHASE CODE HERE ...
       *
       * @endcode
       *
       * \warning "SolverGeneralFB" is based on a Kalman filter, and Kalman filters
       * are objets that store their internal state, so you MUST NOT use the
       * SAME object to process DIFFERENT data streams.
       *
       * @sa SolverBase.hpp, SolverLMS.hpp, CodeKalmanSolver.hpp and
       * SolverPPP.hpp for base classes.
       *
       */
   class SolverGeneralFB : public SolverGeneral
   {
   public:

         /** Common constructor.
          *
          * @param equation      Object describing the equations to be solved.
          *
          */
      SolverGeneralFB(const Equation& equation)
          : SolverGeneral(equation), firstIteration(true)
      {};


         /** Explicit constructor.
          * 
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          **/
      SolverGeneralFB( const std::list<Equation>& equationList )
          : SolverGeneral(equationList), firstIteration(true)
      {};

      
         /** Explicit constructor.
          *
          * @param equationSys         Object describing an equation system to
          *                            be solved.
          **/
      SolverGeneralFB( const EquationSystem& equationSys )
         : SolverGeneral(equationSys), firstIteration(true)
      {};


         /** Returns a reference to a gnnsSatTypeValue object after
          *  solving the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssSatTypeValue& Process(gnssSatTypeValue& gData)
         throw(ProcessingException);



         /** Returns a reference to a gnnsRinex object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssRinex& Process(gnssRinex& gData)
         throw(ProcessingException);


         /** Returns a reference to a gnssDataMap object after solving
          *  the previously defined equation system.
          *
          * @param gData    Data object holding the data.
          */
      virtual gnssDataMap& Process(gnssDataMap& gData )
         throw(ProcessingException);


         /** Reprocess the data stored during a previous 'Process()' call.
          *
          * @param cycles     Number of forward-backward cycles (1 by default).
          *
          * \warning The minimum number of cycles allowed is "1". In fact, if
          * you introduce a smaller number, 'cycles' will be set to "1".
          */
      virtual void ReProcess( int cycles )
         throw(ProcessingException);


         /** Process the data stored during a previous 'ReProcess()' call, one
          *  item at a time, and always in forward mode.
          *
          * @param gData      Data object that will hold the resulting data.
          *
          * @return FALSE when all data is processed, TRUE otherwise.
          */
      virtual bool LastProcess(gnssSatTypeValue& gData)
         throw(ProcessingException);


         /** Process the data stored during a previous 'ReProcess()' call, one
          *  item at a time, and always in forward mode.
          *
          * @param gData      Data object that will hold the resulting data.
          *
          * @return FALSE when all data is processed, TRUE otherwise.
          */
      virtual bool LastProcess(gnssRinex& gData)
         throw(ProcessingException);


         /** Process the data stored during a previous 'ReProcess()' call, one
          *  item at a time, and always in forward mode.
          *
          * @param gData      Data object that will hold the resulting data.
          *
          * @return FALSE when all data is processed, TRUE otherwise.
          */
      virtual bool LastProcess(gnssDataMap& gData)
         throw(ProcessingException);


         /// Returns the number of processed measurements.
      virtual int getProcessedMeasurements(void) const
      { return processedMeasurements; };


         /// Returns the number of measurements rejected because they were
         /// off limits.
      virtual int getRejectedMeasurements(void) const
      { return rejectedMeasurements; };


         /// Returns an index identifying this object.
      virtual int getIndex(void) const;


         /// Returns a string identifying this object.
      virtual std::string getClassName(void) const;


         /// Destructor.
      virtual ~SolverGeneralFB() {};


   private:


         /// Boolean indicating if this is the first iteration of this filter.
      bool firstIteration;


         /// Data Map that holding all the gnss data.
      gnssDataMap ObsData;
      

         /// Set storing the TypeID's that we want to keep.
      TypeIDSet keepTypeSet;


         /// Number of processed measurements.
      int processedMeasurements;


         /// Number of measurements rejected because they were off limits.
      int rejectedMeasurements;


         /// Initial index assigned to this class.
      static int classIndex;


         /// Index belonging to this object.
      int index;

         /// Sets the index and increment classIndex.
      void setIndex(void)
      { index = classIndex++; };


         /// Do not allow the use of the default constructor.
      SolverGeneralFB();


   }; // End of class 'SolverGeneralFB'


      //@}

}  // End of namespace gpstk

#endif   // GPSTK_SOLVERPPPFB_HPP
