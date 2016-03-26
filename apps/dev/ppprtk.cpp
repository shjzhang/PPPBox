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
//  Copywright
//
//  Shoujian Zhang, Wuhan University, shjzhang@sgg.whu.edu.cn, 2014
//
//============================================================================
//
//  Revision
//
//
//  2015/01/28      program to test the PPP-RTK with widelane observations
//
//============================================================================



// Basic input/output C++ classes
#include <iostream>
#include <iomanip>
#include <fstream>


   // Basic framework for programs in the GPSTk. 'process()' method MUST
   // be implemented
#include "BasicFramework.hpp"

   // Class for handling observation RINEX files
#include "RinexObsStream.hpp"

   // Class to store satellite precise navigation data
#include "SP3EphemerisStore.hpp"

   // Class to store a list of processing objects
#include "ProcessingList.hpp"

   // Class in charge of basic GNSS signal modelling
#include "BasicModel.hpp"

   // Class to model the tropospheric delays
#include "TropModel.hpp"

   // Class defining the GNSS data structures
#include "DataStructures.hpp"

   // Class to filter out satellites without required observables
#include "RequireObservables.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"

   // Class for easily changing reference base from ECEF to NEU
#include "XYZ2NEU.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetector.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to compute the effect of solid tides
#include "SolidTides.hpp"

   // Class to compute the effect of ocean loading
#include "OceanLoading.hpp"

   // Class to compute the effect of pole tides
#include "PoleTides.hpp"

   // Class to correct observables
#include "CorrectObservables.hpp"

   // Classes to deal with Antex antenna parameters
#include "Antenna.hpp"
#include "AntexReader.hpp"

   // Class to compute the effect of wind-up
#include "ComputeWindUp.hpp"

   // Class to compute the effect of satellite antenna phase center
#include "ComputeSatPCenter.hpp"

   // Class to compute the tropospheric data
#include "ComputeTropModel.hpp"

   // Class to compute linear combinations
#include "ComputeLinear.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

   // Class to compute Dilution Of Precision values
#include "ComputeDOP.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker2.hpp"

   // Class to compute gravitational delays
#include "GravitationalDelay.hpp"

   // Class to align phases with code measurements
#include "PhaseCodeAlignment.hpp"

   // Used to delete satellites in eclipse
#include "EclipsedSatFilter.hpp"

   // Used to decimate data. This is important because RINEX observation
   // data is provided with a 30 s sample rate, whereas SP3 files provide
   // satellite clock information with a 900 s sample rate.
#include "Decimate.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-only mode.
#include "SolverRTKLw.hpp"
#include "SolverRTKLc.hpp"

   // Class to compute the Precise Point Positioning (PPP) solution in
   // forwards-backwards mode.
//#include "SolverPPPXARFB.hpp"

   // Class to read configuration files.
#include "ConfDataReader.hpp"

   // Used to delete satellites in eclipse
#include "ComputeElevWeights.hpp"

   // Class to compute the ionospheric delays
#include "IonexModel.hpp"

   // Class to smooth the ambiguity
#include "AmbiguitySmoother.hpp"
//   // Class to correct satellite biases
//#include "CorrectUPDXs.hpp"

   // Class to correct satellite biases
#include "CorrectUPDs.hpp"

   // Class to correct satellite biases
#include "GDSUtils.hpp"

   // Class to interpolate the corrections
#include "InterpCorrection.hpp"

   // Class to compute the delta corrections
#include "ComputeDeltaCorrection.hpp"

   // Class to simulate and insert cycle slips 
#include "CycleSlipSimulation.hpp"


using namespace std;
using namespace gpstk;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class ppprtk : public BasicFramework
{
public:

      // Constructor declaration
   ppprtk(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();

      // Method that hold code to be run BEFORE processing
   virtual void spinUp();

      // Method that hold code to be run AFTER processing
   virtual void shutDown();

private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

      // If you want to share objects and variables among methods, you'd
      // better declare them here
      
      // Set this receiver as static reference station
   bool staticFlag;

      // Configuration file reader
   ConfDataReader confReader;


      // Declare our own methods to handle output
   gnssDataMap gdsMap;


      // Method to print solution values
   void printSolution( ofstream& outfile,
                       const  SolverRTKLc& solver,
                       const  CommonTime& time,
                       const  ComputeDOP& cDOP,
                       bool   useNEU,
                       int    numSats,
                       double dryTropo,
                       int    precision = 3 );


      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 4 );


}; // End of 'ppprtk' class declaration



   // Let's implement constructor details
ppprtk::ppprtk(char* arg0)
   :
   BasicFramework(  arg0,
"\nThis program reads GPS receiver data from a configuration file and\n"
"process such data applying a 'Precise Point Positioning' strategy.\n\n"
"Please consult the default configuration file, 'ppprtk.conf', for\n"
"further details.\n\n"
"The output file format is as follows:\n\n"
" 1) Year\n"
" 2) Day of year\n"
" 3) Seconds of day\n"
" 4) dx/dLat (m)\n"
" 5) dy/dLon (m)\n"
" 6) dz/dH (m)\n"
" 7) Zenital Tropospheric Delay - zpd (m)\n"
" 8) Covariance of dx/dLat (m*m)\n"
" 9) Covariance of dy/dLon (m*m)\n"
"10) Covariance of dz/dH (m*m)\n"
"11) Covariance of Zenital Tropospheric Delay (m*m)\n"
"12) Number of satellites\n"
"13) GDOP\n"
"14) PDOP\n"
"15) TDOP\n"
"16) HDOP\n"
"17) VDOP\n" ),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   " [-c|--conffile]    Name of configuration file ('ppprtk.conf' by default).",
             false )
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'ppprtk::ppprtk'



   // Method to print solution values
void ppprtk::printSolution( ofstream& outfile,
                              const SolverRTKLc& solver,
                              const CommonTime& time,
                              const ComputeDOP& cDOP,
                              bool  useNEU,
                              int   numSats,
                              double dryTropo,
                              int   precision )
{

      // Prepare for printing
   outfile << fixed << setprecision( precision );


      // Print results
   outfile << static_cast<YDSTime>(time).year        << "  ";    // Year           - #1
   outfile << static_cast<YDSTime>(time).doy         << "  ";    // DayOfYear      - #2
   outfile << static_cast<YDSTime>(time).sod   << "  ";    // SecondsOfDay   - #3

   if( useNEU )
   {

      outfile << solver.getSolution(TypeID::dLat) << "  ";       // dLat  - #4
      outfile << solver.getSolution(TypeID::dLon) << "  ";       // dLon  - #5
      outfile << solver.getSolution(TypeID::dH) << "  ";         // dH    - #6

         // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
         // nominal value of 0.1 m. Also to get the total we have to add the
         // dry tropospheric delay value
//    outfile << solver.getSolution(TypeID::wetMap) + 0.1 + dryTropo << "  ";
                                                                 // ztd - #7

      outfile << solver.getVariance(TypeID::dLat) << "  ";   // Cov dLat  - #8
      outfile << solver.getVariance(TypeID::dLon) << "  ";   // Cov dLon  - #9
      outfile << solver.getVariance(TypeID::dH) << "  ";     // Cov dH    - #10

//    outfile << solver.getVariance(TypeID::wetMap) << "  "; // Cov ztd   - #11

         // Now, Let's get the solution with the the fixed ambiguities
      outfile << solver.getFixedSolution(TypeID::dLat) << "  ";       // dLat  - #12
      outfile << solver.getFixedSolution(TypeID::dLon) << "  ";       // dLon  - #13
      outfile << solver.getFixedSolution(TypeID::dH) << "  ";         // dH    - #14
//    outfile << solver.getFixedSolution(TypeID::wetMap) + 0.1 + dryTropo << "  "; // ztd - #15

      outfile << solver.getCurrentSatNumber() << "  ";       // dLat  - #16
      outfile << solver.getAmbFixedNumL1() << "  ";       // dLat  - #17


   }
   else
   {

      outfile << solver.getSolution(TypeID::dx) << "  ";         // dx    - #4
      outfile << solver.getSolution(TypeID::dy) << "  ";         // dy    - #5
      outfile << solver.getSolution(TypeID::dz) << "  ";         // dz    - #6

         // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
         // nominal value of 0.1 m. Also to get the total we have to add the
         // dry tropospheric delay value
                                                                 // ztd - #7
      outfile << solver.getSolution(TypeID::wetMap) + 0.1 + dryTropo << "  ";

      outfile << solver.getVariance(TypeID::dx) << "  ";     // Cov dx    - #8
      outfile << solver.getVariance(TypeID::dy) << "  ";     // Cov dy    - #9
      outfile << solver.getVariance(TypeID::dz) << "  ";     // Cov dz    - #10
      outfile << solver.getVariance(TypeID::wetMap) << "  "; // Cov ztd   - #11

   }

   outfile << numSats << "  ";    // Number of satellites - #12

   outfile << cDOP.getGDOP()        << "  ";  // GDOP - #13
   outfile << cDOP.getPDOP()        << "  ";  // PDOP - #14
   outfile << cDOP.getTDOP()        << "  ";  // TDOP - #15
   outfile << cDOP.getHDOP()        << "  ";  // HDOP - #16
   outfile << cDOP.getVDOP()        << "  ";  // VDOP - #17

      // Add end-of-line
   outfile << endl;

   return;


}  // End of method 'ppprtk::printSolution()'



   // Method to print model values
void ppprtk::printModel( ofstream& modelfile,
                           const gnssRinex& gData,
                           int   precision )
{

      // Prepare for printing
   modelfile << fixed << setprecision( precision );

      // Get epoch out of GDS
   CommonTime time(gData.header.epoch);

      // Iterate through the GNSS Data Structure
   for ( satTypeValueMap::const_iterator it = gData.body.begin();
         it!= gData.body.end();
         it++ )
   {

         // Print epoch
      modelfile << static_cast<YDSTime>(time).year         << "  ";    // Year           #1
      modelfile << static_cast<YDSTime>(time).doy          << "  ";    // DayOfYear      #2
      modelfile << static_cast<YDSTime>(time).sod    << "  ";    // SecondsOfDay   #3

         // Print satellite information (Satellite system and ID number)
      modelfile << (*it).first << " ";             // System         #4
                                                   // ID number      #5

         // Print model values
      for( typeValueMap::const_iterator itObs  = (*it).second.begin();
           itObs != (*it).second.end();
           itObs++ )
      {
            // Print type names and values
         modelfile << (*itObs).first << " ";
         modelfile << (*itObs).second << " ";

      }  // End of 'for( typeValueMap::const_iterator itObs = ...'

      modelfile << endl;

   }  // End for (it = gData.body.begin(); ... )

}  // End of method 'ppprtk::printModel()'





   // Method that will be executed AFTER initialization but BEFORE processing
void ppprtk::spinUp()
{

      // Check if the user provided a configuration file name
   if ( confFile.getCount() > 0 )
   {

         // Enable exceptions
      confReader.exceptions(ios::failbit);

      try
      {

            // Try to open the provided configuration file
         confReader.open( confFile.getValue()[0] );

      }
      catch(...)
      {

         cerr << "Problem opening file "
              << confFile.getValue()[0]
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper "
              << "read permissions." << endl;

         exit (-1);

      }  // End of 'try-catch' block

   }
   else
   {

      try
      {
            // Try to open default configuration file
         confReader.open( "ppprtk.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'ppprtk.conf'"
              << endl;
         cerr << "Maybe it doesn't exist or you don't have proper read "
              << "permissions. Try providing a configuration file with "
              << "option '-c'."
              << endl;

         exit (-1);

      }  // End of 'try-catch' block

   }  // End of 'if ( confFile.getCount() > 0 )'


      // If a given variable is not found in the provided section, then
      // 'confReader' will look for it in the 'DEFAULT' section.
   confReader.setFallback2Default(true);


}  // End of method 'ppprtk::spinUp()'



   // Method that will really process information
void ppprtk::process()
{
      //
      // Before the real rtk positioning, you should load the reference stations' 
      // corrections data, which will be used to interpolate the corrections for 
      // the rover station
      //
      
      // Get the file name 
   string corrFileInput = confReader.getValue( "corrFileIn", "DEFAULT" );

      // Now, loading
   cout << "Now, loading the correction file ... ..." << endl;

   try
   {
         // Try to load the correction data stored with gnssDataMap format
      gdsMap = loadGnssDataMap( corrFileInput );

   }
   catch(...)
   {

      cerr << "Problem opening file "
           << corrFileInput
           << endl;
      cerr << "Maybe it doesn't exist or you don't have proper "
           << "read permissions." << endl;

      exit (-1);

   }  // End of 'try-catch' block


   cout << "Now, do the rtk station by station ... ..." << endl;

      // We will read each section name, which is equivalent to station name
      // Station names will be read in alphabetical order
   string station;
   while ( (station = confReader.getEachSection()) != "" )
   {

         // We will skip 'DEFAULT' section because we are waiting for
         // a specific section for each receiver. However, if data is
         // missing we will look for it in 'DEFAULT' (see how we use method
         // 'setFallback2Default()' of 'ConfDataReader' object in 'spinUp()'
      if( station == "DEFAULT" )
      {
         continue;
      }


         // Show a message indicating that we are starting with this station
      cout << "Starting processing for station: '" << station << "'." << endl;


         // Create input observation file stream
      RinexObsStream rin;

         // Enable exceptions
      rin.exceptions(ios::failbit);

         // Try to open Rinex observations file
      try
      {

            // Open Rinex observations file in read-only mode
         rin.open( confReader("rinexObsFile", station), std::ios::in );

      }
      catch(...)
      {

         cerr << "Problem opening file '"
              << confReader.getValue("rinexObsFile", station)
              << "'." << endl;

         cerr << "Maybe it doesn't exist or you don't have "
              << "proper read permissions."
              << endl;

         cerr << "Skipping receiver '" << station << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

         continue;

      }  // End of 'try-catch' block


         // Declare a "SP3EphemerisStore" object to handle precise ephemeris
      SP3EphemerisStore SP3EphList;

         // Set flags to reject satellites with bad or absent positional
         // values or clocks
      SP3EphList.rejectBadPositions(true);
      SP3EphList.rejectBadClocks(true);

         // Load all the SP3 ephemerides files from variable list
      string sp3File;
      while ( (sp3File = confReader.fetchListValue("SP3List",station) ) != "" )
      {

            // Try to load each ephemeris file
         try
         {

            SP3EphList.loadFile( sp3File );

         }
         catch (FileMissingException& e)
         {
               // If file doesn't exist, issue a warning
            cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
                 << "have permission to read it. Skipping it." << endl;

            continue;

         }

      }  // End of 'while ( (sp3File = confReader.fetchListValue( ... "


         // Read if we will use the cross-correlation receiver data,
      bool useRinexClock( confReader.getValueAsBoolean( "useRinexClock", station ) );

      if(useRinexClock)
      {
            // Load all the rinex clock files from variable list
         string rinexClockFile;
         while ( (rinexClockFile = confReader.fetchListValue("rinexClockList", station) ) != "" )
         {
            try
            {
               SP3EphList.loadRinexClockFile( rinexClockFile );
            }
            catch (...)
            {
                  // If file doesn't exist, issue a warning
               cerr << "rinex clock file '" << rinexClockFile << "' doesn't exist or you don't "
                    << "have permission to read it. Skipping it." << endl;

               exit(-1);
            }
         }  // End of 'while ( (rinexClockFile = confReader.fetchListValue( ... "
      }


//       // Declare a "RinexUPDXStore" object to handle satellite bias
//    RinexUPDXStore updxStore;

//       // Read if we should check for data gaps.
//    if ( confReader.getValueAsBoolean( "checkUPDXGapInterval", "DEFAULT" ) )
//    {
//       updxStore.setGapInterval(
//                     confReader.getValueAsDouble("UPDXGapInterval", "DEFAULT" ) );
//    }

//       // Read if we should check for too wide interpolation intervals
//    if ( confReader.getValueAsBoolean( "checkUPDXMaxInterval", "DEFAULT" ) )
//    {
//       updxStore.setMaxInterval(
//                     confReader.getValueAsDouble("UPDXMaxInterval", "DEFAULT" ) );
//    }


//       // Load all the rinex bias files from variable list
//    string updxFile;
//    while ( (updxFile = confReader.fetchListValue("updxFileList", station ) ) != "" )
//    {
//       try
//       {
//          updxStore.loadFile( updxFile );
//       }
//       catch (FFStreamError& e)
//       {
//          cerr << e << endl;
//       }
//       catch (...)
//       {
//             // If file doesn't exist, issue a warning
//          cerr << "rinex bias file '" << updxFile << "' doesn't exist or you don't "
//               << "have permission to read it. Skipping it." << endl;

//          exit(-1);
//       }

//    }  // End of 'while ( (updxFile = confReader.fetchListValue( ... "

      cout << "a" << endl;

         // Declare a "RinexUPDStore" object to handle satellite bias
      RinexUPDStore updStore;

         // Read if we should check for data gaps.
      if ( confReader.getValueAsBoolean( "checkUPDGapInterval", "DEFAULT" ) )
      {
         updStore.setGapInterval(
                       confReader.getValueAsDouble("UPDGapInterval", "DEFAULT" ) );
      }

         // Read if we should check for too wide interpolation intervals
      if ( confReader.getValueAsBoolean( "checkUPDMaxInterval", "DEFAULT" ) )
      {
         updStore.setMaxInterval(
                       confReader.getValueAsDouble("UPDMaxInterval", "DEFAULT" ) );
      }


         // Load all the rinex bias files from variable list
      string updFile;
      while ( (updFile = confReader.fetchListValue("updFileList", station ) ) != "" )
      {
         try
         {
            updStore.loadFile( updFile );
         }
         catch (FFStreamError& e)
         {
            cerr << e << endl;
         }
         catch (...)
         {
               // If file doesn't exist, issue a warning
            cerr << "rinex bias file '" << updFile << "' doesn't exist or you don't "
                 << "have permission to read it. Skipping it." << endl;

            exit(-1);
         }

      }  // End of 'while ( (updFile = confReader.fetchListValue( ... "



      //// ^^^^ Ephemeris handling ^^^^
      

         // Load station nominal position
      double xn(confReader.fetchListValueAsDouble("nominalPosition",station));
      double yn(confReader.fetchListValueAsDouble("nominalPosition",station));
      double zn(confReader.fetchListValueAsDouble("nominalPosition",station));
         // The former peculiar code is possible because each time we
         // call a 'fetchListValue' method, it takes out the first element
         // and deletes it from the given variable list.

      Position nominalPos( xn, yn, zn );


         // Create a 'ProcessingList' object where we'll store
         // the processing objects in order
      ProcessingList pList;


         // This object will check that all required observables are present
      RequireObservables requireObs;
      requireObs.addRequiredType(TypeID::P2);
      requireObs.addRequiredType(TypeID::L1);
      requireObs.addRequiredType(TypeID::L2);

         // This object will check that code observations are within
         // reasonable limits
      SimpleFilter pObsFilter;
      pObsFilter.setFilteredType(TypeID::P2);

         // Read if we should use C1 instead of P1
      bool usingC1( confReader.getValueAsBoolean( "useC1", station ) );
      if ( usingC1 )
      {
         requireObs.addRequiredType(TypeID::C1);
         pObsFilter.addFilteredType(TypeID::C1);
      }
      else
      {
         requireObs.addRequiredType(TypeID::P1);
         pObsFilter.addFilteredType(TypeID::P1);
      }

         // Add 'requireObs' to processing list (it is the first)
      pList.push_back(requireObs);

         // IMPORTANT NOTE:
         // It turns out that some receivers don't correct their clocks
         // from drift.
         // When this happens, their code observations may drift well beyond
         // what it is usually expected from a pseudorange. In turn, this
         // effect causes that "SimpleFilter" objects start to reject a lot of
         // satellites.
         // Thence, the "filterCode" option allows you to deactivate the
         // "SimpleFilter" object that filters out C1, P1 and P2, in case you
         // need to.
      bool filterCode( confReader.getValueAsBoolean( "filterCode", station ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterCode )
      {
         pList.push_back(pObsFilter);       // Add to processing list
      }


         // This object defines several handy linear combinations
      LinearCombinations comb;


         // Object to compute linear combinations for cycle slip detection
      ComputeLinear linear1;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         linear1.addLinear(comb.pdeltaCombWithC1);
         linear1.addLinear(comb.mwubbenaCombWithC1);
      }
      else
      {
         linear1.addLinear(comb.pdeltaCombination);
         linear1.addLinear(comb.mwubbenaCombination);
      }
      linear1.addLinear(comb.ldeltaCombination);
      linear1.addLinear(comb.liCombination);
      pList.push_back(linear1);       // Add to processing list


         // Declare a basic modeler
      BasicModel basic(nominalPos, SP3EphList);
         // Set the minimum elevation
      basic.setMinElev(confReader.getValueAsDouble("cutOffElevation",station));

         // If we are going to use P1 instead of C1, we must reconfigure 'basic'
      if ( !usingC1 )
      {
         basic.setDefaultObservable(TypeID::P1);
      }
         // Add to processing list
      pList.push_back(basic);


         // Object to correct the satellite biases.
      CorrectUPDs updCorr(updStore);
      pList.push_back(updCorr);


         // Objects to mark cycle slips
      LICSDetector markCSLI2;         // Checks LI cycle slips
      pList.push_back(markCSLI2);      // Add to processing list
      MWCSDetector markCSMW;          // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);       // Add to processing list

         // Get initial time for 'Decimate' class
      YDSTime yds( confReader.getValueAsInt("year", station),
                   confReader.getValueAsInt("dayOfYear", station),
                   confReader.getValueAsDouble("secOfDay", station), TimeSystem::GPS );
      CommonTime initialTime( yds.convertToCommonTime() );

        
         // Insert simulated cycle slips into gnssRinex
      CycleSlipSimulation insertCycleSlips; 
      insertCycleSlips.setInitialEpoch(initialTime);
         // If insert cycle slips, then insert this class into 'pList'
      if( confReader.getValueAsBoolean( "insertCycleSlips", station ) )
      {
         pList.push_back(insertCycleSlips);
      }



         // Object to keep track of satellite arcs
      SatArcMarker2 markArc;
      markArc.setDeleteUnstableSats(false);
      markArc.setUnstablePeriod(90.0); // 15 min
      pList.push_back(markArc);       // Add to processing list


         // Object to decimate data
      Decimate decimateData(
               confReader.getValueAsDouble( "decimationInterval", station ),
               confReader.getValueAsDouble( "decimationTolerance", station ),
               initialTime );
      pList.push_back(decimateData);       // Add to processing list


         // Object to compute weights based on elevation
      ComputeElevWeights elevWeights;
      pList.push_back(elevWeights);       // Add to processing list


         // Object to remove eclipsed satellites
      EclipsedSatFilter eclipsedSV;
      pList.push_back(eclipsedSV);       // Add to processing list


         // Object to compute gravitational delay effects
      GravitationalDelay grDelay(nominalPos);
      pList.push_back(grDelay);       // Add to processing list


         // Vector from monument to antenna ARP [UEN], in meters
      double uARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      double eARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      double nARP(confReader.fetchListValueAsDouble( "offsetARP", station ) );
      Triple offsetARP( uARP, eARP, nARP );


         // Declare some antenna-related variables
      Triple offsetL1( 0.0, 0.0, 0.0 ), offsetL2( 0.0, 0.0, 0.0 );
      AntexReader antexReader;
      Antenna receiverAntenna;

         // Check if we want to use Antex information
      bool useantex( confReader.getValueAsBoolean( "useAntex", station ) );
      string antennaModel;
      if( useantex )
      {
            // Feed Antex reader object with Antex file
         antexReader.open( confReader.getValue( "antexFile", station ) );

            // Antenna model 
         antennaModel = confReader.getValue( "antennaModel", station );

            // Get receiver antenna parameters
            // Warning: If no corrections are not found for one specific 
            //          radome, then the antenna with radome NONE are used.
         try
         {
            receiverAntenna = antexReader.getAntenna( antennaModel );
         }
         catch(ObjectNotFound& notFound)
         {
               // new antenna model
            antennaModel.replace(16,4,"NONE");
               // new receiver antenna with new antenna model
            receiverAntenna = antexReader.getAntenna( antennaModel );
         }

      }


         // Object to compute satellite antenna phase center effect
      ComputeSatPCenter svPcenter(nominalPos);
      if( useantex )
      {
            // Feed 'ComputeSatPCenter' object with 'AntexReader' object
         svPcenter.setAntexReader( antexReader );
      }

      pList.push_back(svPcenter);       // Add to processing list


         // Declare an object to correct observables to monument
      CorrectObservables corr(SP3EphList);
      corr.setNominalPosition(nominalPos);
      corr.setMonument( offsetARP );

         // Check if we want to use Antex patterns
      bool usepatterns(confReader.getValueAsBoolean("usePCPatterns", station ));
      if( useantex && usepatterns )
      {
         corr.setAntenna( receiverAntenna );

            // Should we use elevation/azimuth patterns or just elevation?
         corr.setUseAzimuth(confReader.getValueAsBoolean("useAzim", station));
      }
      else
      {
            // Fill vector from antenna ARP to L1 phase center [UEN], in meters
         offsetL1[0] = confReader.fetchListValueAsDouble("offsetL1", station);
         offsetL1[1] = confReader.fetchListValueAsDouble("offsetL1", station);
         offsetL1[2] = confReader.fetchListValueAsDouble("offsetL1", station);

            // Vector from antenna ARP to L2 phase center [UEN], in meters
         offsetL2[0] = confReader.fetchListValueAsDouble("offsetL2", station);
         offsetL2[1] = confReader.fetchListValueAsDouble("offsetL2", station);
         offsetL2[2] = confReader.fetchListValueAsDouble("offsetL2", station);

         corr.setL1pc( offsetL1 );
         corr.setL2pc( offsetL2 );

      }

      pList.push_back(corr);       // Add to processing list


         // Object to compute wind-up effect
      ComputeWindUp windup( SP3EphList,
                            nominalPos,
                            confReader.getValue( "satDataFile", station ) );
      pList.push_back(windup);       // Add to processing list


         // Declare a NeillTropModel object, setting its parameters
      NeillTropModel neillTM( nominalPos.getAltitude(),
                              nominalPos.getGeodeticLatitude(),
                              confReader.getValueAsInt("dayOfYear", station) );

         // We will need this value later for printing
      double drytropo( neillTM.dry_zenith_delay() );


         // Object to compute the tropospheric data
      ComputeTropModel computeTropo(neillTM);
      pList.push_back(computeTropo);       // Add to processing list


         /**
          * Now, Let's insert the zwd/ionoL1/updCorr from 'gdsMap'
          */

      cout << "b" << endl;
         // Object to compute the ionospheric delay and related parameters
      InterpCorrection interpCorr;
      interpCorr.setInitialRxPosition(nominalPos);
      interpCorr.setReferenceData(gdsMap);
      interpCorr.setInterpType(TypeID::corrPdelta);
      interpCorr.addInterpType(TypeID::corrLdelta);
      interpCorr.addInterpType(TypeID::corrPC);
      interpCorr.addInterpType(TypeID::corrLC);
      interpCorr.setUseOnlyStaticSta(false);
      pList.push_back(interpCorr);       // Add to processing list


      cout << "c" << endl;

         // Object to compute the difference of corrections between 
         // the interpolated values and the estimated values stored in 'gdsMap'
//    ComputeDeltaCorrection deltaCorr;
//    deltaCorr.setReferenceData(gdsMap);
//    deltaCorr.setInterpType(TypeID::corrPdelta);
//    pList.push_back(deltaCorr);       // Add to processing list

      cout << "d" << endl;


         // Object to compute code combination with minus ionospheric delays
         // for L1/L2 calibration
      ComputeLinear linear2;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         linear2.addLinear(comb.q1CombWithC1);
         linear2.addLinear(comb.q2CombWithC1);
      }
      else
      {
         linear2.addLinear(comb.q1Combination);
         linear2.addLinear(comb.q2Combination);
      }
      pList.push_back(linear2);       // Add to processing list


         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL1;
      phaseAlignL1.setCodeType(TypeID::Q1);
      phaseAlignL1.setPhaseType(TypeID::L1);
      phaseAlignL1.setPhaseWavelength(0.190293672798);

      pList.push_back(phaseAlignL1);       // Add to processing list

         // Object to align phase with code measurements
      PhaseCodeAlignment phaseAlignL2;
      phaseAlignL2.setCodeType(TypeID::Q2);
      phaseAlignL2.setPhaseType(TypeID::L2);
      phaseAlignL2.setPhaseWavelength(0.244210213425);
      pList.push_back(phaseAlignL2);       // Add to processing list


         // Object to compute combinations to be used in the PPP processing
      ComputeLinear linear3;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         linear3.addLinear(comb.pdeltaCombWithC1);
      }
      else
      {
         linear3.addLinear(comb.pdeltaCombination);
      }
      linear3.addLinear(comb.ldeltaCombination);
      pList.push_back(linear3);       // Add to processing list


         // Object to compute ionosphere-free combinations to be used
         // as observables in the PPP processing
      ComputeLinear linear4;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
            // WARNING: When using C1 instead of P1 to compute PC combination,
            //          be aware that instrumental errors will NOT cancel,
            //          introducing a bias that must be taken into account by
            //          other means. This won't be taken into account in this
            //          example.
         linear4.addLinear(comb.pcCombWithC1);
      }
      else
      {
         linear4.addLinear(comb.pcCombination);
      }
      linear4.addLinear(comb.lcCombination);
      pList.push_back(linear4);       // Add to processing list


         // Declare a simple filter object to screen PC
      SimpleFilter pcFilter;
      pcFilter.setFilteredType(TypeID::PC);

         // IMPORTANT NOTE:
         // Like in the "filterCode" case, the "filterPC" option allows you to
         // deactivate the "SimpleFilter" object that filters out PC, in case
         // you need to.
      bool filterPC( confReader.getValueAsBoolean( "filterPC", station ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterPC )
      {
         pList.push_back(pcFilter);       // Add to processing list
      }


         // Object to compute prefit-residuals
      ComputeLinear linear5;
      linear5.addLinear(comb.pcPrefit);
      linear5.addLinear(comb.lcPrefit);
      linear5.addLinear(comb.pdeltaPrefit);
      linear5.addLinear(comb.ldeltaPrefit);
      pList.push_back(linear5);       // Add to processing list


         // Declare a base-changing object: From ECEF to North-East-Up (NEU)
      XYZ2NEU baseChange(nominalPos);
         // We always need both ECEF and NEU data for 'ComputeDOP', so add this
      pList.push_back(baseChange);


         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list


         // Get if we want results in ECEF or NEU reference system
      bool isNEU( confReader.getValueAsBoolean( "USENEU", station ) );


         // Declare solver objects
      SolverRTKLw  lwSolver(isNEU);
      SolverRTKLc  lcSolver(isNEU);


         // Get if we want to process coordinates as white noise
      bool isWN( confReader.getValueAsBoolean( "coordinatesAsWhiteNoise",
                                               station ) );
         // White noise stochastic model
      WhiteNoiseModel wnM(100.0);      // 100 m of sigma

         // Check about coordinates as white noise
      if ( isWN )
      {
            // Reconfigure solver
         lwSolver.setCoordinatesModel(&wnM);
         lcSolver.setCoordinatesModel(&wnM);
      }

         // Add solver to processing list
      pList.push_back(lwSolver);
      pList.push_back(lcSolver);


         // Object to compute tidal effects
      SolidTides solid;

         // Configure ocean loading model
      OceanLoading ocean;
      ocean.setFilename( confReader.getValue( "oceanLoadingFile", station ) );


         // Declare a "EOPDataStore" object to handle earth rotation parameter file
      EOPDataStore eopStore;

         // Load all the EOP files from variable list
      string eopFile;
      while ( (eopFile = confReader.fetchListValue("EOPFileList", station ) ) != "" )
      {
         try
         {
            eopStore.loadIGSFile( eopFile );
         }
         catch (...)
         {
               // If file doesn't exist, issue a warning
            cerr << "EOP file '" << eopFile << "' doesn't exist or you don't "
                 << "have permission to read it. Skipping it." << endl;

            exit(-1);
         }

      }  // End of 'while ( (eopFile = confReader.fetchListValue( ... "

         // Object to model pole tides
      PoleTides pole(eopStore);


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;


         // Prepare for printing
      int precision( confReader.getValueAsInt( "precision", station ) );


         // Let's open the output file
      string outName(confReader.getValue( "outputFile", station ) );

      ofstream outfile;
      outfile.open( outName.c_str(), ios::out );

         // Let's check if we are going to print the model
      bool printmodel( confReader.getValueAsBoolean( "printModel", station ) );

      string modelName;
      ofstream modelfile;

         // Prepare for model printing
      if( printmodel )
      {
         modelName = confReader.getValue( "modelFile", station );
         modelfile.open( modelName.c_str(), ios::out );
      }

         //// *** Now comes the REAL forwards processing part *** ////

         // Loop over all data epochs
      while(rin >> gRin)
      {

            // Store current epoch
         CommonTime time(gRin.header.epoch);

            // Store the nominal position into 'SourceID'
         gRin.header.source.nominalPos = nominalPos;

            // Compute solid, oceanic and pole tides effects at this epoch
         Triple tides( solid.getSolidTide( time, nominalPos )  +
                       ocean.getOceanLoading( station, time )  +
                       pole.getPoleTide( time, nominalPos )    );

            // Update observable correction object with tides information
         corr.setExtraBiases(tides);

            // Set rover for interpCorr
         interpCorr.setRover(gRin.header.source);

         try
         {

               // Let's process data. Thanks to 'ProcessingList' this is
               // very simple and compact: Just one line of code!!!.
            gRin >> pList;

         }
         catch(DecimateEpoch& d)
         {
               // If we catch a DecimateEpoch exception, just continue.
            continue;
         }
         catch(SVNumException& s)
         {
               // If we catch a SVNumException, just continue.
            continue;
         }
         catch(Exception& e)
         {
            cerr << "Exception for receiver '" << station <<
                    "' at epoch: " << time << "; " << e << endl;
            continue;
         }
         catch(...)
         {
            cerr << "Unknown exception for receiver '" << station <<
                    " at epoch: " << time << endl;
            continue;
         }


            // Ask if we are going to print the model
         if ( printmodel )
         {
            printModel( modelfile,
                        gRin );

         }


            // This is a 'forwards-only' filter. Let's print to output
            // file the results of this epoch
         printSolution( outfile,
                        lcSolver,
                        time,
                        cDOP,
                        isNEU,
                        gRin.numSats(),
                        drytropo,
                        precision );

            //
            // Insert 'types' into 'gdsMap', which will used as corrections
            // for rtk positioning.
            //
            
            // The types you want to keep
         TypeIDSet types;
         types.insert(TypeID::postfitL);
         types.insert(TypeID::postfitLdelta);
         
            // Only keep the selected types in 'gRin'
         gRin.keepOnlyTypeID(types);

            // Store observation data
         gdsMap.addGnssRinex(gRin);

         // The given epoch hass been processed. Let's get the next one

      }  // End of 'while(rin >> gRin)'


         // Close current Rinex observation stream
      rin.close();


         // If we printed the model, we must close the file
      if ( printmodel )
      {
            // Close model file for this station
         modelfile.close();
      }


         // Clear content of SP3 ephemerides object
      SP3EphList.clear();

         // Close output file for this station
      outfile.close();

         // We are done with this station. Let's show a message
      cout << "Processing finished for station: '" << station
           << "'. Results in file: '" << outName << "'." << endl;

   }  // end of 'while ( (station = confReader.getEachSection()) != "" )'

   return;

}  // End of 'ppprtk::process()'


   // Method that will be executed AFTER the 'Process'
void ppprtk::shutDown()
{
   string corrFileOutput= confReader.getValue( "corrFileOut", "DEFAULT" );

      // Warning, the 'gdsMap' to a binary file 
   saveGnssDataMap( gdsMap, corrFileOutput);

      // Get the test file name 
   string testFile = confReader.getValue( "testFile", "DEFAULT" );

      // Now, write the 'correction' with format of gnssDataMap to text file?
   dumpGnssDataMap(gdsMap, testFile);

}


   // Main function
int main(int argc, char* argv[])
{

   try
   {

      ppprtk program(argv[0]);

         // We are disabling 'pretty print' feature to keep
         // our description format
      if ( !program.initialize(argc, argv, false) )
      {
         return 0;
      }

      if ( !program.run() )
      {
         return 1;
      }

      return 0;

   }
   catch(Exception& e)
   {

      cerr << "Problem: " << e << endl;

      return 1;

   }
   catch(...)
   {

      cerr << "Unknown error." << endl;

      return 1;

   }

   return 0;

}  // End of 'main()'
