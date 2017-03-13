#pragma ident "$Id$"

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
//  Copyright Dagoberto Salazar - gAGE ( http://www.gage.es ). 2007, 2008, 2012
//
//============================================================================

// Example program Nro 6 for GPSTk
// This program shows a minimalist way to process GPS data
// using GNSS Data Structures (DataStructures.hpp).


#include <iostream>
#include <iomanip>

   // Class for handling satellite observation parameters RINEX files
#include "Rinex3ObsStream.hpp"
#include "Rinex3NavStream.hpp"
#include "Rinex3NavHeader.hpp"
#include "Rinex3NavData.hpp"

   // Class in charge of the GPS signal modelling
#include "ModelObs.hpp"


   // Class to model the tropospheric delays
#include "TropModel.hpp"

  // Class to get IPP position and ionosphere mapping function
#include "IonexModel.hpp"

#include "SP3EphemerisStore.hpp"

   // Class to solve the equation system using Least Mean Squares
#include "SolverLMS.hpp"
#include "ComputeElevWeights.hpp"
   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class in charge of basic GNSS signal modelling
#include "BasicModel.hpp"
   // Class to compute the tropospheric data
#include "ComputeTropModel.hpp"

#include "Rinex3EphemerisStore.hpp"

   // Class to filter out satellites without required observables
#include "RequireObservables.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"

  // YDS Time-class
#include "YDSTime.hpp"

   // Class to compute linear combinations
#include "ComputeLinear.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

using namespace std;
using namespace gpstk;

int main(void)
{

      //////// Initialization phase ////////

   cout << fixed << setprecision(8);   // Set a proper output format

 // Declare a "SP3EphemerisStore" object to handle precise ephemeris
   SP3EphemerisStore SP3EphList;

      // Set flags to reject satellites with bad or absent positional
      // values or clocks
   SP3EphList.rejectBadPositions(true);
   SP3EphList.rejectBadClocks(true);
      // Set clock max interval ( 5min )
   SP3EphList.setClockMaxInterval(300.0);

   
   SP3EphList.loadFile( "gbm18825.sp3");
   SP3EphList.loadFile( "gbm18826.sp3");
   SP3EphList.loadFile( "gbm18830.sp3");


    // GMSD station nominal position
   Position nominalPos(-3607665.367,4147867.957,3223717.038);
                                       
   // Object to get IPP position and mapping function
   IonexStore IonexMapList;
   IonexMapList.loadFile("codg0370.16i");
   IonexModel ionex(nominalPos, IonexMapList);  // Declare a Ionospheric Model object

      // Create the input observation file stream
   Rinex3ObsStream rin("gmsd0370.16o");
   
   Rinex3NavData rNavData;             // Object to store Rinex navigation data
   Rinex3EphemerisStore bceStore;         // Object to store satellites ephemeris
   Rinex3NavHeader rNavHeader;         // Object to read the header of Rinex
                                                                   
         // Create the input navigation file stream
   Rinex3NavStream rnavin("brdm0370.16p");

   rnavin >> rNavHeader;

   while ( rnavin >> rNavData)
   {
      bceStore.addEphemeris(rNavData);  
   }
      
   RequireObservables requireObs;
   requireObs.addRequiredType(TypeID::C1);
   requireObs.addRequiredType(TypeID::P2);

   requireObs.addBDSRequiredType(TypeID::C2);
   requireObs.addBDSRequiredType(TypeID::C7);
   
   // Declare a basic modeler
   BasicModel basic(nominalPos, SP3EphList);
  // BasicModel basic(nominalPos, bceStore);

   // Declare a NeillTropModel object, setting its parameters
   NeillTropModel neillTM( nominalPos.getLongitude(),
                           nominalPos.getGeodeticLatitude(), 037 );

         // Object to compute the tropospheric data
   ComputeTropModel computeTropo(neillTM);


     // This object defines several handy linear combinations
   LinearCombinations comb;
         // Object to compute linear combinations for cycle slip detection
   ComputeLinear linear1;
   linear1.addLinear(comb.pcCombWithC1);
   linear1.addBeiDouLinear(comb.pcCombForBeiDou);
   ComputeLinear linear2;
   linear2.addBeiDouLinear(comb.pcPrefit);
   linear2.addLinear(comb.pcPrefit);

   ComputeElevWeights elevWeights;

      // Declare SolverLMS object
   SolverLMS solver;

      // Declare a simple filter object. By default, it filters C1
   //SimpleFilter myFilter;
   //myFilter.setFilteredType(TypeID::C2);
      // This is the GNSS data structure that will hold all the
      // GNSS-related information
   gnssRinex gRin;

   TypeIDSet unknownsSet;
   unknownsSet.insert(TypeID::dx);
   unknownsSet.insert(TypeID::dy);
   unknownsSet.insert(TypeID::dz);
   unknownsSet.insert(TypeID::cdt);

      // Create a new equation definition
      // newEq(independent value, set of unknowns)
   gnssEquationDefinition newEq(TypeID::prefitC, unknownsSet);

      // Reconfigure solver
   solver.setDefaultEqDefinition(newEq);

      //////// Processing phase ////////

      // Loop over all data epochs
   while(rin >> gRin)
   {

      try
      {

            // This is the line that will process all the GPS data
         gRin.keepOnlySatSystem(SatID::systemGPS);
         gRin >> requireObs ;
         gRin >> basic >> elevWeights;
         gRin >> ionex ;
         gRin >> computeTropo;
         gRin >> linear1 >> linear2;
         gRin >> solver;
            // First: Wipe off all data that we will not use (may be skipped)
            // Second: Filter out observables out of bounds (may be skipped)
            // Third: The resulting data structure will feed the modeler object
            // Fourth: The resulting structure from third step will feed the 
            // solver object

      }
      catch(...)
      {
         cerr << "Exception at epoch: " << gRin.header.epoch << endl;
      }

      cout << gRin.header.epoch.getSecondOfDay()<<" "
           <<solver.solution[0]<<" "
           <<solver.solution[1]<<" "
           <<solver.solution[2]<<endl;
       
      

   }

   exit(0);

}  // End of 'main()'
