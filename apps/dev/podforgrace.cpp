#pragma ident "$Id$"

//============================================================================
//
// POD
//
// This program shows how to use GNSS Data Structures (GDS) and other classes
// to build a reasonable complete application that computes "Precise Point
// Positioning" (PPP).
//
// For details on the PPP algorithm please consult:
//
//    Kouba, J. and P. Heroux. "Precise Point Positioning using IGS Orbit
//       and Clock Products". GPS Solutions, vol 5, pp 2-28. October, 2001.
//
// Copyright 
//
// Dagoberto Salazar - gAGE ( http://www.gage.es ). 2008, 2009
//
// Shoujian Zhang, Wuhan University, 2015
//
// Modifications
//
// 2010/10/01
//
// Create this program
//
// 2015/11/18
//
// if rinex header is not valid, then skip the rinex files, and then continue
// processing the other files.
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
// Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector2.hpp"

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

// Class to compute linear combinations
#include "ComputeLinear.hpp"

// This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

// Class to compute Dilution Of Precision values
#include "ComputeDOP.hpp"

// Class to keep track of satellite arcs
#include "SatArcMarker2.hpp"
// Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

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
#include "SolverPPP.hpp"

// Class to compute the Precise Point Positioning (PPP) solution in
// forwards-backwards mode.
#include "SolverPPPFB.hpp"


// Class to compute the Precise Oribt Determination (POD) solution in
// forwards-only mode.
#include "SolverPOD.hpp"

// Class to compute the Precise Oribt Determination (POD) solution in
// forwards-backwards mode.
#include "SolverPODFB.hpp"


// Class to filter the MW combination
#include "MWFilter.hpp"

// Class to read configuration files.
#include "ConfDataReader.hpp"

// Class to compute the elevation weights
#include "ComputeElevWeights.hpp"


   // Class to store satellite precise navigation data
#include "MSCStore.hpp"

//******//
// add hpp from pod.ar.cpp   for Professor sjzhang
// however there's something debug ,I try replace it with a simple one
//  "ReceiverAttStorew.hpp"

// Class to read and store the receiver position data
#include "ReceiverAttDataw.hpp"

// Class to correct Observables with reciver clock error
#include "CorrectObstime.hpp"

// Class to delete outliers based on the prefit residuals
#include "PrefitFilter.hpp"
// Class to delete outliers based on the prefit residuals
#include "PrefitCSDetector.hpp"
//// Class to calibrate the phase with its ambiguity
//#include "PhaseCalibration.hpp"
//// Class to correct satellite biases
//#include "CorrectPhaseBiases.hpp"
//// Class to compute the corrections for PPP-RTK and RTK positioning
//#include "ComputeCorrection.hpp"

//******//

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class pod : public gpstk::BasicFramework
{
public:

      // Constructor declaration
   pod(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();

      // Method that hold code to be run BEFORE processing
   virtual void spinUp();


private:


      // This field represents an option at command line interface (CLI)
   CommandOptionWithArg confFile;

      // Option for rinex file list
   CommandOptionWithAnyArg rnxFileListOpt;

      // Option for sp3 file list
   CommandOptionWithAnyArg sp3FileListOpt;

      // Option for clk file list
   CommandOptionWithAnyArg clkFileListOpt;

      // Option for eop file list
   CommandOptionWithAnyArg eopFileListOpt;

      // Option for monitor coordinate file
   CommandOptionWithAnyArg mscFileOpt;

      // Option for monitor coordinate file
   CommandOptionWithAnyArg outputFileListOpt;

      // If you want to share objects and variables among methods, you'd
      // better declare them here
   
   string rnxFileListName;
   string sp3FileListName;
   string clkFileListName;
   string eopFileListName;
   string mscFileName;
   string outputFileListName;

      // Configuration file reader
   ConfDataReader confReader;


      // Declare our own methods to handle output


      // Method to print solution values
   void printSolution( ofstream& outfile,
                       const  SolverLMS& solver,
                       const  CommonTime& time,
                       const  ComputeDOP& cDOP,
                       bool   useRAC,
                       int    numSats,
                       Triple& position,
//                       double dryTropo,
                       int    precision = 3 );


      // Method to print model values
   void printModel( ofstream& modelfile,
                    const gnssRinex& gData,
                    int   precision = 4 );


}; // End of 'pod' class declaration



   // Let's implement constructor details
pod::pod(char* arg0)
   :
   gpstk::BasicFramework(  arg0,
"\nThis program reads parameters from a configuration file, \n"
"reads GPS receiver data and ephemeris data from command line, \n"
"then process the data using the PPP strategy.\n\n"
"Please consult the default configuration file, 'ppp.conf', for \n"
"further details.\n\n"
"The output file format is as follows:\n"
"\n 1) Year"
"\n 2) doy"
"\n 3) Seconds of day"
"\n 4) dx/dR (m)"
"\n 5) dy/dA (m)"
"\n 6) dz/dC (m)"
"\n 7) cdt reciver time delay(m)"
"\n 8) Number of satellites"
"\n 9) GDOP"
"\n10) PDOP\n"),
      // Option initialization. "true" means a mandatory option
   confFile( CommandOption::stdType,
             'c',
             "conffile",
   "Name of configuration file ('ppp.conf' by default).",
             false),
      // Option for rinex file list reading
   rnxFileListOpt( 'r',
                   "rnxFileList",
   "file storing a list of rinex file name ",
                   true),
   sp3FileListOpt( 's',
                   "sp3FileList",
   "file storing a list of rinex SP3 file name ",
                   true),
   clkFileListOpt( 'k',
                   "clkFileList",
   "file storing a list of rinex clk file name ",
                   false),
   eopFileListOpt( 'e',
                   "eopFileList",
   "file storing a list of IGS erp file name ",
                   true),
   outputFileListOpt( 'o',
                   "outputFileList",
   "file storing the list of output file name ",
                   false),
   mscFileOpt( 'm',
               "mscFile",
   "file storing monitor station coordinates ",
               true)
{

      // This option may appear just once at CLI
   confFile.setMaxCount(1);

}  // End of 'pod::pod'


GPSEllipsoid ellipsoid;

   // Method to print solution values
void pod::printSolution( ofstream& outfile,
                              const SolverLMS& solver,
                              const CommonTime& time,
                              const ComputeDOP& cDOP,
                              bool  useRAC,
                              int   numSats,
                              Triple& position,
//                              double dryTropo,
                              int   precision )
{

      outfile << fixed << setprecision( precision );

      // Print results
   outfile << static_cast<YDSTime>(time).year        << "  ";    // Year           - #1
   outfile << setw(5) << static_cast<YDSTime>(time).doy         << "  ";    // DayOfYear      - #2
   outfile << setw(12)<< static_cast<YDSTime>(time).sod << "  ";    // SecondsOfDay   - #3

      // Prepare for printing

   if( useRAC )
   {

      outfile<< setw(8) << solver.getSolution(TypeID::dR) << "  ";       // dR  - #4
      outfile<< setw(8) << solver.getSolution(TypeID::dA) << "  ";       // dA  - #5
      outfile<< setw(8) << solver.getSolution(TypeID::dC) << "  ";         // dC    - #6

         // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
         // nominal value of 0.1 m. Also to get the total we have to add the
         // dry tropospheric delay value
                                                                 // ztd - #7
      //outfile<< setw(8) << solver.getSolution(TypeID::wetMap)  << "  ";
         outfile<< setw(8) << solver.getSolution(TypeID::cdt)/ellipsoid.c() << "  ";

   }
   else
   {

      outfile << solver.getSolution(TypeID::dx) << "  ";         // dx    - #4
      outfile << solver.getSolution(TypeID::dy) << "  ";         // dy    - #5
      outfile << solver.getSolution(TypeID::dz) << "  ";         // dz    - #6

         // We add 0.1 meters to 'wetMap' because 'NeillTropModel' sets a
         // nominal value of 0.1 m. Also to get the total we have to add the
         // dry tropospheric delay value
                                                                 // cdt - #7
      outfile<< setw(8) << solver.getSolution(TypeID::cdt)<< "  ";


   }
   outfile << position[0] << "  "<<position[1] << "  "<<position[2] << "  ";
   outfile << numSats << "  ";    // Number of satellites - #12
//   outfile << solver.getConverged() << "  ";

   outfile << cDOP.getGDOP()        << "  ";  // GDOP - #13
   outfile << cDOP.getPDOP()        << "  ";  // PDOP - #14

      // Add end-of-line
   outfile << endl;


   return;


}  // End of method 'pod::printSolution()'



   // Method to print model values
void pod::printModel( ofstream& modelfile,
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

}  // End of method 'pod::printModel()'




   // Method that will be executed AFTER initialization but BEFORE processing
void pod::spinUp()
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
         confReader.open( "ppp.conf" );
      }
      catch(...)
      {

         cerr << "Problem opening default configuration file 'ppp.conf'"
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

      // Now, Let's parse the command line
   if(rnxFileListOpt.getCount())
   {
      rnxFileListName = rnxFileListOpt.getValue()[0];
   }
   if(sp3FileListOpt.getCount())
   {
      sp3FileListName = sp3FileListOpt.getValue()[0];
   }
   if(clkFileListOpt.getCount())
   {
      clkFileListName = clkFileListOpt.getValue()[0];
   }
   if(eopFileListOpt.getCount())
   {
      eopFileListName = eopFileListOpt.getValue()[0];
   }
   if(outputFileListOpt.getCount())
   {
      outputFileListName = outputFileListOpt.getValue()[0];
   }
//   if(mscFileOpt.getCount())
//   {
//      mscFileName = mscFileOpt.getValue()[0];
//   }

}  // End of method 'pod::spinUp()'



   // Method that will really process information
void pod::process()
{
      //**********************************************
      // Now, Let's read SP3 Files
      //**********************************************

      // Declare a "SP3EphemerisStore" object to handle precise ephemeris
   SP3EphemerisStore SP3EphList;

      // Set flags to reject satellites with bad or absent positional
      // values or clocks
   SP3EphList.rejectBadPositions(true);
   SP3EphList.rejectBadClocks(true);

      // Now read sp3 files from 'sp3FileList'
   ifstream sp3FileListStream;

   sp3FileListStream.open(sp3FileListName.c_str(), ios::in);
   if(!sp3FileListStream)
   {
         // If file doesn't exist, issue a warning
      cerr << "SP3 file List Name'" << sp3FileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;

      exit(-1);
   }

   string sp3File;
   while( sp3FileListStream >> sp3File )
   {
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
   }
      // Close file
   sp3FileListStream.close();

      //***********************
      // Let's read clock files
      //***********************

      // If rinex clock file list is given, then use rinex clock
   if(clkFileListOpt.getCount())
   {
         // Now read clk files from 'clkFileList'
      ifstream clkFileListStream;

         // Open clkFileList File
      clkFileListStream.open(clkFileListName.c_str(), ios::in);
      if(!clkFileListStream)
      {
            // If file doesn't exist, issue a warning
         cerr << "clock file List Name'" << clkFileListName << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         exit(-1);
      }

      string clkFile;
      while( clkFileListStream >> clkFile )
      {
         try
         {
            SP3EphList.loadRinexClockFile( clkFile );
         }
         catch (FileMissingException& e)
         {
               // If file doesn't exist, issue a warning
            cerr << "rinex CLK file '" << clkFile << "' doesn't exist or you don't "
                 << "have permission to read it. Skipping it." << endl;
            continue;
         }
      }

         // Close file
      clkFileListStream.close();

   }  // End of 'if(...)'

      //***********************
      // Let's read eop files
      //***********************

      // Declare a "EOPDataStore" object to handle earth rotation parameter file
   EOPDataStore eopStore;

      // Now read eop files from 'eopFileList'
   ifstream eopFileListStream;

      // Open eopFileList File
   eopFileListStream.open(eopFileListName.c_str(), ios::in);
   if(!eopFileListStream)
   {
         // If file doesn't exist, issue a warning
      cerr << "erp file List Name'" << eopFileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;

      exit(-1);
   }

   string eopFile;
   while( eopFileListStream >> eopFile )
   {
      try
      {
         eopStore.loadIGSFile( eopFile );
      }
      catch (FileMissingException& e)
      {
            // If file doesn't exist, issue a warning
         cerr << "EOP file '" << eopFile << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;
         continue;
      }
   }
      // Close file
   eopFileListStream.close();

      //**********************************************
      // Now, Let's read MSC data
      //**********************************************
      
      // Declare a "MSCStore" object to handle msc file 
//   MSCStore mscStore;
//
//   try
//   {
//      mscStore.loadFile( mscFileName );
//   }
//   catch (FileMissingException& e)
//   {
//         // If file doesn't exist, issue a warning
//      cerr << "MSC file '" << mscFileName << "' doesn't exist or you don't "
//           << "have permission to read it. Skipping it." << endl;
//      exit(-1);
//   }

      
      //******//
      // add hpp from pod.ar.cpp   for Professor sjzhang
      
      // Load all the receiver position files
      char attfile1[100]="GO_CONS_SST_PRM_2__20091101T000000_20091131T235959_0101.GGT";
      char attfile2[100]="GO_CONS_EGG_NOM_2__20091101T000000_20091131T235959_0101.IAQ";
      // the attfile isn't read. as I didn't use correction of PCV
      char prdfile[100]="grace07_09a.col";
      //file time start month in J.D.day
      double tmonth0= 2.4551365E6;  // here is 2009.11.1.0
      double tatt0=1068800.0;     // here is second for 2009.11.1.0 at gpstime 1980.1.6.0 reduce 94*******
      double tprd0=941068800.0;   // here is second for 2009.11.1.0 at gpstime 1980.1.6.0
      
      // for grace data which start second time is different for goce
      tmonth0= 2.4558415E6;        // here is 2011.10.7.0
      tprd0=371217600.0;   // here is second for 2011.10.7.0 at gpstime 2000.1.1.12
     
      //const Triple offsetReciver(0.6899,-0.0046,-1.1755 );
      //      const Triple offsetReciver(0.0,0.0,0.0 );
      
      // give start time  and end time for compute to cut GOCE att and position file
      CivilTime StartOfKD(2011,10,7,0,0,0.0, TimeSystem::GPS);
      CivilTime   EndOfKD(2011,10,9,23,55,0.0, TimeSystem::GPS);
      
      //***********************
      //**********************************
      //compute position and offsetReciver
      // get Obsever time for intpoly by hwei;
      long wday,wsod;
      double wfsod;
      double timestatatt,timeendatt;
      double timestatprd,timeendprd;

      CommonTime epochsta=StartOfKD.convertToCommonTime();
      CommonTime epochend=EndOfKD.convertToCommonTime();
      
      epochsta.get(wday,wsod,wfsod);
      timestatatt=double(wday-tmonth0-0.5)*86400+wsod+wfsod+tatt0;
      timestatprd=double(wday-tmonth0-0.5)*86400+wsod+wfsod+tprd0;
      //cout<<"wday"<<wday<<"wsod"<<wsod<<endl;
      //cout<<timestatprd<<endl;
      
      epochend.get(wday,wsod,wfsod);
      timeendatt=double(wday-tmonth0-0.5)*86400+wsod+wfsod+tatt0;
      timeendprd=double(wday-tmonth0-0.5)*86400+wsod+wfsod+tprd0;
      
      ReceiverAttDataw rxAtt;
      // GOCE attfile class
      vector<ReceiverAttDataw::GOCEatt> vGOCEatts1,vGOCEatts2;
      
      ReceiverAttDataw::GOCEatt         vGOCEattag1, vGOCEattag2;
      
      //GOCE prd file class
      vector<ReceiverAttDataw::GOCEprdposition> vGOCEprdpositions;
      ReceiverAttDataw::GOCEprdposition vGOCEptag;
      
      // read GOCE attfile
      
      //      rxAtt.ReadGOCEatt(attfile1,vGOCEatts1);
      //      rxAtt.ReadGOCEatt(attfile2,vGOCEatts2);
//      rxAtt.ReadGOCEatt2(timestatatt,timeendatt,attfile1,vGOCEatts1);
//      rxAtt.ReadGOCEatt2(timestatatt,timeendatt,attfile2,vGOCEatts2);
      
      
      // read GOCE prdfile
      //      rxAtt.ReadGOCEposition(prdfile,vGOCEprdpositions);
      rxAtt.ReadGOCEposition2(timestatprd,timeendprd,prdfile,vGOCEprdpositions);
      
      //******//
      
      //**********************************************************
      // Now, Let's perform the POD for each rinex files
      //**********************************************************

   vector<string> rnxFileListVec;
      
      // Now read eop files from 'eopFileList'
   ifstream rnxFileListStream;

      // Open eopFileList File
   rnxFileListStream.open(rnxFileListName.c_str());
   if(!rnxFileListStream)
   {
         // If file doesn't exist, issue a warning
      cerr << "rinex file List Name'" << rnxFileListName << "' doesn't exist or you don't "
           << "have permission to read it. Skipping it." << endl;

      exit(-1);
   }

   string rnxFile;
   while( rnxFileListStream >> rnxFile )
   {
      rnxFileListVec.push_back(rnxFile);
   }
      // Close file
   rnxFileListStream.close();

   if(rnxFileListVec.size() == 0 )
   {
      cerr << rnxFileListName  << "rnxFileList is empty!! "
           << endl;
   }

      //////////////////////////////////////
      // read output file list name
      //////////////////////////////////////
      
   vector<string> outputFileListVec;
      
      // Now read eop files from 'eopFileList'
   ifstream outputFileListStream;

   if(outputFileListOpt.getCount())
   {
         // Open ouputFileList File
      outputFileListStream.open(outputFileListName);
      if( ! outputFileListStream.is_open() )
      {
            // If file doesn't exist, issue a warning
         cerr << "output file List Name'" << outputFileListName << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         exit(-1);
      }

      string outputFile;
      while( outputFileListStream >> outputFile )
      {
         outputFileListVec.push_back(outputFile);
      }
         // Close file
      outputFileListStream.close();

      if(outputFileListVec.size() == 0 )
      {
         cerr << outputFileListName  << "outputFileList is empty!! "
              << endl;
      }
   }

         // ===================
         // Let's read rinex file list !!!!
         // ===================

      // We will read each rinex file
   vector<string>::const_iterator rnxit = rnxFileListVec.begin();
   vector<string>::const_iterator outit = outputFileListVec.begin();

   while( rnxit != rnxFileListVec.end() )
   {
         // Read rinex file from the vector!
      string rnxFile = (*rnxit);

         // Create input observation file stream
      RinexObsStream rin;
      rin.exceptions(ios::failbit); // Enable exceptions

         // Try to open Rinex observations file
      try
      {
         rin.open( rnxFile, std::ios::in );
      }
      catch(...)
      {
         cerr << "Problem opening file '"
              << rnxFile
              << "'." << endl;
         cerr << "Maybe it doesn't exist or you don't have "
              << "proper read permissions."
              << endl;

         cerr << "Skipping receiver '" << rnxFile << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

         continue;

      }  // End of 'try-catch' block

         
         // ===================
         // Let's read the header firstly!!!!
         // ===================
      RinexObsHeader roh;
      try
      {
         rin >> roh;
      }
      catch(FFStreamError& e)
      {
         cerr << "Problem in reading file '"
              << rnxFile
              << "'." << endl;

         cerr << "Skipping receiver '" << rnxFile << "'."
              << endl;

            // Close current Rinex observation stream
         rin.close();

            // Index for rinex file iterator.
         ++rnxit;

         continue;
      }

         // Get the station name for current rinex file 
      string station = roh.markerName;

         // First time for this rinex file
      CommonTime initialTime( roh.firstObs ) ;

         // Show a message indicating that we are starting with this station
      cout << "Starting processing for station: '" << station << "'." << endl;

         // MSC data for this station
//     initialTime.setTimeSystem(TimeSystem::Unknown);
//      MSCData mscData( mscStore.findMSC( station, initialTime ) );
      initialTime.setTimeSystem(TimeSystem::GPS);

         // The former peculiar code is possible because each time we
         // call a 'fetchListValue' method, it takes out the first element
         // and deletes it from the given variable list.

//      Position nominalPos( mscData.coordinates );
      Position nominalPos;
         
         // Create a 'ProcessingList' object where we'll store
         // the processing objects in order
      ProcessingList pList,pListppp;
         
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
      bool usingC1( confReader.getValueAsBoolean( "useC1" ) );
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
      bool filterCode( confReader.getValueAsBoolean( "filterCode" ) );

         // Check if we are going to use this "SimpleFilter" object or not
      if( filterCode )
      {
         pList.push_back(pObsFilter);       // Add to processing list
      }


         // Declare a basic modeler
      BasicModel basic(nominalPos, SP3EphList);
         // Set the minimum elevation
      basic.setMinElev(confReader.getValueAsDouble("cutOffElevation"));
      basic.setDefaultObservable(TypeID::P1);
         // Add to processing list
      pList.push_back(basic);
         
         // Object to compute weights based on elevation
      ComputeElevWeights elevWeights(2.0);
      pList.push_back(elevWeights);       // Add to processing list
         
         
         // Object to remove eclipsed satellites
      EclipsedSatFilter eclipsedSV;
      pList.push_back(eclipsedSV);       // Add to processing list
         
         
         // Object to compute gravitational delay effects
      GravitationalDelay grDelay(nominalPos);
      pList.push_back(grDelay);       // Add to processing list
      
         // Declare some antenna-related variables
      Triple offsetL1( 0.0, 0.0, 0.0 ), offsetL2( 0.0, 0.0, 0.0 );
      AntexReader antexReader;
      Antenna receiverAntenna;
         
         // Check if we want to use Antex information
      bool useantex( confReader.getValueAsBoolean( "useAntex") );
      string antennaModel;
      if( useantex )
      {
            // Feed Antex reader object with Antex file
            antexReader.open( confReader.getValue( "antexFile" ) );
               
            // Antenna model
            antennaModel = roh.antType;
            
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
      //given as time for pod
      //      corr.setNominalPosition(nominalPos);
      //      corr.setMonument( offsetARP );
         
      // Check if we want to use Antex patterns
      bool usepatterns(confReader.getValueAsBoolean("usePCPatterns" ));
      if( useantex && usepatterns )
      {
            corr.setAntenna( receiverAntenna );
               
            // Should we use elevation/azimuth patterns or just elevation?
            corr.setUseAzimuth(confReader.getValueAsBoolean("useAzim" ));
      }
      else
      {
            // Fill vector from antenna ARP to L1 phase center [UEN], in meters
            offsetL1[0] = confReader.fetchListValueAsDouble("offsetL1");
            offsetL1[1] = confReader.fetchListValueAsDouble("offsetL1");
            offsetL1[2] = confReader.fetchListValueAsDouble("offsetL1");
               
            // Vector from antenna ARP to L2 phase center [UEN], in meters
            offsetL2[0] = confReader.fetchListValueAsDouble("offsetL2");
            offsetL2[1] = confReader.fetchListValueAsDouble("offsetL2");
            offsetL2[2] = confReader.fetchListValueAsDouble("offsetL2");
               
            corr.setL1pc( offsetL1 );
            corr.setL2pc( offsetL2 );
               
      }
         
 //     pList.push_back(corr);       // Add to processing list
       
      
         // This object defines several handy linear combinations
         // "LinearCombinations2.hpp"
         // as for pod ,there's none TypeID::tropoSlant corrt
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
         linear1.addLinear(comb.pcCombination);
         linear1.addLinear(comb.lcCombination);
         linear1.addLinear(comb.pcPrefit);
         linear1.addLinear(comb.lcPrefit);
         pList.push_back(linear1);       // Add to processing list
         
         
         // Objects to mark cycle slips
         //LICSDetector markCSLI;         // Checks LI cycle slips
         //pList.push_back(markCSLI);      // Add to processing list
         
         //PrefitCSDetector prefitCSDetector;
         //pList.push_back( prefitCSDetector );
         
         MWCSDetector2 markCSMW;          // Checks Merbourne-Wubbena cycle slips
         pList.push_back(markCSMW);       // Add to processing list
         
         
         // Object to keep track of satellite arcs
         SatArcMarker markArc;
         markArc.setDeleteUnstableSats(false);
         markArc.setUnstablePeriod(11.0);
         pList.push_back(markArc);       // Add to processing list
         
         // Object to compute wind-up effect
         // WARNING: Windup must be computed after the cycleslip has been detected
         //          and the satellite arc number be marked???
         
         // Object to compute wind-up effect
      ComputeWindUp windup( SP3EphList,
                            nominalPos);
      if( useantex )
      {
            // Feed 'ComputeSatPCenter' object with 'AntexReader' object
            windup.setAntexReader( antexReader );
      }
         
      pList.push_back(windup);       // Add to processing list
         

         
         
         
         
         // Object to compute code combination with minus ionospheric delays
         // for L1/L2 calibration
      ComputeLinear linearL;
      linearL.addLinear(comb.q1Combination);
      linearL.addLinear(comb.q2Combination);
      pList.push_back(linearL);       // Add to processing list
         
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
//
      ComputeLinear linear2;
      linear2.addLinear(comb.pcCombination);
      linear2.addLinear(comb.lcCombination);
      linear2.addLinear(comb.pcPrefit);
      linear2.addLinear(comb.lcPrefit);
      pList.push_back(linear2);       // Add to processing list
         
         
         // Declare a simple filter object to screen PC
      SimpleFilter pcFilter;
      pcFilter.setFilteredType(TypeID::PC);
         
         // IMPORTANT NOTE:
         // Like in the "filterCode" case, the "filterPC" option allows you to
         // deactivate the "SimpleFilter" object that filters out PC, in case
         // you need to.
      bool filterPC( confReader.getValueAsBoolean( "filterPC") );
         
         // Check if we are going to use this "SimpleFilter" object or not
      if( filterPC )
      {
            pList.push_back(pcFilter);       // Add to processing list
      }
         
      
         // Object to compute DOP values
      ComputeDOP cDOP;
      pList.push_back(cDOP);       // Add to processing list

      pListppp=pList;
         
         // Object to filter out PC outliers using the prefit-residuals
         // @author Shoujian Zhang
      //PrefitFilter prefitFilter;
      //pList.push_back(prefitFilter);
         
         ///////////////////////////////////////////////////
         //
         //  Now, the solver configuration !!!
         //
         ///////////////////////////////////////////////////
         
         
         // Get if we want results in ECEF or RAC reference system
      bool isRAC( confReader.getValueAsBoolean( "USERAC") );
         
         // Get if we want 'forwards-backwards' or 'forwards' processing only
      int cycles( confReader.getValueAsInt("filterCycles") );
         
         // Get if we want to process coordinates as white noise
      bool isWN( confReader.getValueAsBoolean( "coordAsWhiteNoise") );

         // White noise stochastic model
      WhiteNoiseModel wnM(100.0);      // 100 m of sigma
         
         // Declare solver objects
      SolverPPP pppSolver(isRAC);
      SolverPODFB fbpodSolver(isRAC);
         
         // Check about coordinates as white noise
      if ( isWN )
      {
            // Reconfigure solver
            pppSolver.setCoordinatesModel(&wnM);
            fbpodSolver.setCoordinatesModel(&wnM);
               
      }

//         // Declare a base-changing object: From ECEF to North-East-Up (NEU)
//         //XYZ2NEU baseChange(nominalPos);
//      XYZ2NEU baseChange;
//         // We always need both ECEF and NEU data for 'ComputeDOP', so add this
//      pList.push_back(baseChange);


         // This is the GNSS data structure that will hold all the
         // GNSS-related information
      gnssRinex gRin;


         // Prepare for printing
      int precision( confReader.getValueAsInt( "precision" ) );


      string outputFileName;

         // Let's open the output file
      if( outputFileListOpt.getCount() )
      {
        outputFileName = (*outit);
      }
      else
      {
        outputFileName = ( rnxFile + ".out" );
      }

      ofstream outfile;
      outfile.open( outputFileName.c_str(), ios::out );

         // print out the header
      outfile << "# col  1 -  3: year/doy/sod \n" 
              << "# col  4 -  7: dLat/dLon/dH/cdt \n"
              << "# col  8 - 11: TotalSatNumber/GDOP/PDOP \n"
              << "# END OF HEADER"<<endl
              <<fbpodSolver.getClassName()<< endl;

         // Let's check if we are going to print the model
      bool printmodel( confReader.getValueAsBoolean( "printModel" ) );

      string modelName;
      ofstream modelfile;

         // Prepare for model printing
      if( printmodel )
      {
         modelName = rnxFile + ".model";
         modelfile.open( modelName.c_str(), ios::out );
      }

         
         //*****************************
         // Vector from Center of mass to antenna ARP [SFR], in meters
         //Triple offsetmount(0.693,-0.0036,-1.094) ARP origin
         // or (-1.115,-0.0036,-1.094) for GOCE in SRF.
         //L1PhaseCenter(-0.071,0.0026,0.081) and
         //L2PhaseCenter(-0.067,0.00  ,0.084) for GOCE in ARF
         // given by ESA  referenced to
         //GOCE-LRR-GPS-positioning-Memo_3.1_[XGCE-GSEG-EOPG-TN-09-0007 v3.1].pdf
         
         //Attention:
         // 1.a 135 degrees counter-clockwise rotation on the Z axis
         // from SRF to ARF.
         //2. there is TWO antenna on GOCE
         //3.according to the reference .offsetmount[2]=1.094,However,it's 1.175
         //in Rinex file.  as.
//      Triple L1PhaseCenter(-0.071,0.0026,0.081);//in ARF
//      Triple L2PhaseCenter(-0.067,0.00  ,0.084);// in ARF
         // as there's  an offset of antenna helix center(0.0695,0.0,0.0) in ARF
         // 0.08cm was considered in Rinex o file. according the rotation between ARF&SRF
         
         Triple offsetReciver( roh.antennaDelta);
         Triple offsetRecivert;
         //// *** Now comes the REAL forwards processing part *** ////
         // Loop over all data epochs
      while(rin >> gRin)
      {
            
        //add by hwei
            
            // Store current epoch
            CommonTime time(gRin.header.epoch);
            double ttagatt,ttagprd,dtr;
            double lat,lon;
            time.get(wday,wsod,wfsod);
            //here epoch.get(wday) is +0.5  ,youneed to reduce it
            ttagatt=double(wday-tmonth0-0.5)*86400+wsod+wfsod+tatt0;
            ttagprd=double(wday-tmonth0-0.5)*86400+wsod+wfsod+tprd0;
  //          cout<<"ttagprd"<<ttagprd<<endl;
 //           rxAtt.GOCEroffsetvt(ttagatt,vGOCEatts1,vGOCEatts2,offsetReciver,offsetRecivert);
            offsetRecivert=offsetReciver;
            
            
            rxAtt.GetGOCEpostime(ttagprd,vGOCEprdpositions,vGOCEptag);
            // get GOCE position with time and give it to Position by hwei
            nominalPos=Position(vGOCEptag.x,vGOCEptag.y,vGOCEptag.z);
      //*******************************************************************
            // updata model or parameters according to nominalPos
            // Compute station latitude and longitude
            lat=nominalPos.geodeticLatitude();
            lon=nominalPos.longitude();
            
            // Vector from monument to antenna ARP [UEN], in meters
            // translate from ECEF to UEN
            offsetRecivert = (offsetRecivert.R3(lon)).R2(-lat);
            
            // Declare a basic modeler
            basic.rxPos = nominalPos;
            // Object to compute gravitational delay effects
            grDelay.setNominalPosition( nominalPos );
            
            svPcenter.setNominalPosition( nominalPos );
            // Declare an object to correct observables to monument
            corr.setNominalPosition(nominalPos);
            corr.setMonument( offsetRecivert );
            // Object to compute wind-up effect
            windup.setNominalPosition( nominalPos );
            
            CorrectObstime corrt;
            
      //*******************************************************************
            
            gnssRinex gRintmp;
            
            gRintmp=gRin;
            try
            {

            gRintmp >> pListppp
                    >>pppSolver;
                  //
                  // reviver cloclk bias from pppSlover,only need for pod GOCE
                  gRin.header.dtreciver=-pppSolver.getSolution(TypeID::cdt)/ellipsoid.c();
                  dtr=gRin.header.dtreciver;
                  //dtr=0.0E-3;
                  
                  // Store update current epoch
                  gRin.header.epoch +=dtr;
                  //update time for position and offsetRicivert
                  ttagatt +=dtr;
                  ttagprd +=dtr;
//                  rxAtt.GOCEroffsetvt(ttagatt,vGOCEatts1,vGOCEatts2,offsetReciver,offsetRecivert);
                  offsetRecivert=offsetReciver;
                  
                  
                  rxAtt.GetGOCEpostime(ttagprd,vGOCEprdpositions,vGOCEptag);
                  
                  // get GOCE position with time and give it to Position by hwei
                  nominalPos=Position(vGOCEptag.x,vGOCEptag.y,vGOCEptag.z);
                  gRin.header.antennaPosition=nominalPos;
                  
                  //cout<<wsod<<" "<<dtr<<"  "<<nominalPos<<endl;
                  //*******************************************************************
                  // updata model or parameters according to nominalPos
                  // Compute station latitude and longitude
                  lat=nominalPos.geodeticLatitude();
                  lon=nominalPos.longitude();
                  // Vector from monument to antenna ARP [UEN], in meters
                  // translate from ECEF to UEN
                  offsetRecivert = (offsetRecivert.R3(lon)).R2(-lat);
                  
                  //offsetRecivert=Triple(0.0,0.0,0.0);
                 // cout<<offsetRecivert<<endl;
                  // Declare a basic modeler
                  basic.rxPos = nominalPos;
                  // Object to compute gravitational delay effects
                  grDelay.setNominalPosition( nominalPos );
                  
                  svPcenter.setNominalPosition( nominalPos );
                  // Declare an object to correct observables to monument
                  corr.setNominalPosition(nominalPos);
                  corr.setMonument( offsetRecivert );
                  // Object to compute wind-up effect
                  windup.setNominalPosition( nominalPos );
                  
                  // correct Observablest with Reciver clock error only need for GOCE pod
                  corrt.setExtraDtr(dtr);
                  //*******************************************************************
                  
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
            
            
            try
            {
                  // Let's process data. Thanks to 'ProcessingList' this is
                  // very simple and compact: Just one line of code!!!.

                  gRin  >>corrt
                        >>pList
                        >>fbpodSolver;
                        // >>pppSolver;
                  
                  //cout time SatID satArc ,prefitLC
                  
//                  SatIDSet currSatSet= gRin.body.getSatID();
//                  Vector<double> prefitL(gRin.getVectorOfTypeID(TypeID::prefitL));
//                  
//                  cout<<wsod<<" ";
//                  int i=0;
//                  for( SatIDSet::const_iterator itSat = currSatSet.begin();
//                      itSat != currSatSet.end();
//                      ++itSat )
//                  {
//
//                        double satArc = gRin.body.getValue( (*itSat),TypeID::satArc );
//                        cout<<(*itSat)<<" "<<satArc<<" "<<prefitL(i);
//                        ++i;
//                  }
//                  cout<<endl;
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
                        gRin,
                        precision );

         }
            
            // Check what type of solver we are using
         if ( cycles < 1 )
         {
               // This is a 'forwards-only' filter. Let's print to output
               // file the results of this epoch
            printSolution( outfile,
                         //pppSolver,
                           fbpodSolver,
                           time,
                           cDOP,
                           isRAC,
                           gRin.numSats(),
                           gRin.header.antennaPosition,
  //                         drytropo,
                           precision );

         }  // End of 'if ( cycles < 1 )'



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

         //// *** Forwards processing part is over *** ////


         // Now decide what to do: If solver was a 'forwards-only' version,
         // then we are done and should continue with next station.
      if ( cycles < 1 )
      {
            // Now, Let's extract the Time-To-First-Fix information
            // that stored in 'ttfc/ttffL1' in the Solver.
         std::vector<double> ttfc;
         //ttfc = pppSolver.getTTFC();

            // Let's open the output file
         string ttfcName( outputFileName+ ".ttfc" );

         ofstream ttfcFile;
         ttfcFile.open( ttfcName.c_str(), ios::out );

         for(std::vector<double>::iterator it=ttfc.begin();
             it!=ttfc.end();
             ++it)
         {
            ttfcFile << (*it) << endl;
         }
         ttfcFile.close();

            // Close output file for this station
         outfile.close();

            // We are done with this station. Let's show a message
         cout << "Processing finished for station: '" << station
              << "'. Results in file: '" << outputFileName << "'." << endl;

            // WARNING:::::::::::::::::
            // Index for rinex file iterator.
            // before goto the next file
         ++rnxit;
         if(outputFileListOpt.getCount())
         {
            ++outit;
         }

            // Go process next station
         continue;

      }

      cout << "Backward processing ... ... " << endl;
         
         
         

         //// *** If we got here, it is a 'forwards-backwards' solver *** ////

         // Now, let's do 'forwards-backwards' cycles
      try
      {
            fbpodSolver.ReProcess(cycles);

      }
      catch(Exception& e)
      {

            // If problems arose, issue an message and skip receiver
         cerr << "Exception at reprocessing phase: " << e << endl;
         cerr << "Skipping receiver '" << station << "'." << endl;

            // Close output file for this station
         outfile.close();

            // Next file
         ++rnxit;

            // Go process next station
         continue;

      }
      catch(SVNumException& s)
      {
               // If we catch a SVNumException, just continue.
               continue;
      }
         // End of 'try-catch' block

      cout << "Last processing ... ... " << endl;

         // Reprocess is over. Let's finish with the last processing

         // Loop over all data epochs, again, and print results
      //while( fbpppSolver.LastProcess(gRin) )
         // The bool indicator used to swich off the while loop
      bool valid(false);
         
         // Loop over all data epochs, again, and print results
      do
      {
               // Last process
            try
            {
                  valid = fbpodSolver.LastProcess(gRin);
                  cDOP.Process(gRin);
            }
            catch(SVNumException& s)
            {
                  // Continue the loop
                  valid = true;
                  continue;
            }
            catch(...)
            {
                  valid = true;
                  continue;
            }
               
            // Rinex epoch
            CommonTime time=gRin.header.epoch;
            
            // The nominal position for current 'gRin'
            nominalPos=gRin.header.antennaPosition;
            
            double lat=nominalPos.geodeticLatitude();
            double lon=nominalPos.longitude();
            
            // Vector from monument to antenna ARP [UEN], in meters
            // translate from ECEF to UEN
            Triple offset(fbpodSolver.getSolution(TypeID::dx),
                          fbpodSolver.getSolution(TypeID::dy),
                          fbpodSolver.getSolution(TypeID::dz));
            
            offset = (offset.R3(lon)).R2(-lat);
            
//            
//            
//            // cout prefit of epoch obs.
//            time.get(wday,wsod,wfsod);
//            SatIDSet currSatSet= gRin.body.getSatID();
//            Vector<double> prefitL(gRin.getVectorOfTypeID(TypeID::prefitL));
//            
//
//            cout<<wsod<<" " << fixed << setprecision( precision );
//            int i=0;
//            for( SatIDSet::const_iterator itSat = currSatSet.begin();
//                itSat != currSatSet.end();
//                ++itSat )
//            {
//
//                  double satArc = gRin.body.getValue( (*itSat),TypeID::satArc );
//                  cout<<(*itSat)<<" "<<satArc<<" "<<prefitL(i);
//                  ++i;
//            }
//            cout<<endl;
            
            // Print out the solution
            printSolution( outfile,
                        fbpodSolver,
                        time,
                        cDOP,
                        isRAC,
                        gRin.numSats(),
                        offset,
                      //  gRin.header.antennaPosition,
                        precision );
               
      } while ( valid );

         
         // We are done. Close and go for next station
         
         // Now, Let's extract the Time-To-First-Fix information
         // that stored in 'ttfc/ttffL1' in the Solver.
         std::vector<double> ttfc;
      //   ttfc = pppSolver.getTTFC();
         
         // Let's open the output file
         string ttfcName( outputFileName+ ".ttfc" );
         
         ofstream ttfcFile;
         ttfcFile.open( ttfcName.c_str(), ios::out );
         
         for(std::vector<double>::iterator it=ttfc.begin();
             it!=ttfc.end();
             ++it)
         {
               ttfcFile << (*it) << endl;
         }
         ttfcFile.close();
         
         // Close output file for this station
         outfile.close();
         
         
         // We are done with this station. Let's show a message
         cout << "Processing finished for station: '" << station
         << "'. Results in file: '" << outputFileName << "'." << endl;
         
         // Index for rinex file iterator.
         ++rnxit;
         
         if(outputFileListOpt.getCount())
         {
               ++outit;
         }
         
   }  // end of 'while (...)'
      //***********************************************
      //
      // At last, Let's clear the content of SP3/EOP/MSC object
      //
      //***********************************************
   SP3EphList.clear();
   eopStore.clear();
//   mscStore.clear();


   return;

}  // End of 'pod::process()'



// Main function
int main(int argc, char* argv[])
{
      
      try
      {
            
            pod program(argv[0]);
            
            // We are disabling 'pretty print' feature to keep
            // our description format
            if ( !program.initialize(argc, argv, true) )
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
