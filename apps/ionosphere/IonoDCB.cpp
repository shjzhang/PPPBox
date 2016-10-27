//============================================================================
//
// This program shows how to use GNSS Data Structures (GDS) and other classes
// to model regional ionosphere together with estimating DCBs of satellite 
// and receivers.
//
// Wei Wang , Wuhan University, 2016/03/09
//
//============================================================================
// 
//
//
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

  // Class to smooth PI using LI combination
#include "PISmoother.hpp"

   // Class to get IPP position and ionosphere mapping function
#include "GetIPP.hpp"

   // Class to filter out observables grossly out of limits
#include "SimpleFilter.hpp"

   // Class to detect cycle slips using LI combination
#include "LICSDetector.hpp"

   // Class to detect cycle slips using the Melbourne-Wubbena combination
#include "MWCSDetector.hpp"

   // Class to correct observables
#include "CorrectObservables.hpp"

   // This class pre-defines several handy linear combinations
#include "LinearCombinations.hpp"

 // Class to compute linear combinations
#include "ComputeLinear.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker.hpp"

   // Class to keep track of satellite arcs
#include "SatArcMarker2.hpp"

   // Used to delete satellites in eclipse
#include "EclipsedSatFilter.hpp"

   // Used to decimate data. This is important because RINEX observation
   // data is provided with a 30 s sample rate, whereas SP3 files provide
   // satellite clock information with a 900 s sample rate.
#include "Decimate.hpp"

   // Class to estimate the DCBs and ionosphere models using a Spherical
   // Harmonic expansion
#include "SolverIonoDCB2.hpp"
   // Class to compute the elevation weights
#include "ComputeElevWeights.hpp"

   // Class to convert the CC to NONCC.
#include "CC2NONCC.hpp"

#include "RecTypeDataReader.hpp"

   // Class to read and store the receiver type.
#include "RecTypeDataReader.hpp"

#include "CommandOption.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;


   // A new class is declared that will handle program behaviour
   // This class inherits from BasicFramework
class IonoDCB : public gpstk::BasicFramework
{
public:

      // Constructor declaration
   IonoDCB(char* arg0);


protected:


      // Method that will take care of processing
   virtual void process();

      // Method that hold code to be run BEFORE processing
   virtual void spinUp();


private:

            // Option for rinex file list
   CommandOptionWithAnyArg rnxFileListOpt;

      // Option for sp3 file list
   CommandOptionWithAnyArg sp3FileListOpt;
     
     // Option for clk file list
   CommandOptionWithAnyArg clkFileListOpt;

     // Option for dcb file list
   CommandOptionWithAnyArg dcbFileListOpt;

      // Option for output file
   CommandOptionWithAnyArg outputFileListOpt;

     // Option for the max order of SH
   CommandOptionWithAnyArg maxOrderOpt;

     // Option for the interval(unit: h)
   CommandOptionWithAnyArg intervalOpt;

      // If you want to share objects and variables among methods, you'd
      // better declare them here
   string rnxFileListName;
   string sp3FileListName;
   string clkFileListName;
   string dcbFileListName;
   string outputFileListName;
   int maxOrder;
   int interval;

   void printSolution(  ofstream& outfile,
                        int interval,
                        int order,
                        SolverIonoDCB2& solver,
                        const CommonTime& time1,
                        const CommonTime& time2,
                        const gnssDataMap& gData );

     
}; // End of 'IonoDCB' class declaration



   // Let's implement constructor details
IonoDCB::IonoDCB(char* arg0)
   :
   gpstk::BasicFramework(  arg0,
"\nThis program models the ionosphere together with estimating DCBs\n"
"for satellite and receiver using a spherical harmonic expansion\n"),
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
   maxOrderOpt ( 'O',
                 "maxOrder",
   "max order of the spherical harmonic expansion",
                   true),
   intervalOpt ( 'I',
                 "interval",
   "the interval of estimating DCBs,usually 6 hours above",
                   false),
   dcbFileListOpt( 'D',
                   "dcbFileList",
   "file storing a list of P1-C1 DCB file name ",
                   false),
   outputFileListOpt( 'o',
                   "outputFileList",
   "file storing the list of output file name ",
                   false)
  
{ 

   
}  // End of 'IonoDCB::IonoDCB'

   // Method that will be executed AFTER initialization but BEFORE processing
void IonoDCB::spinUp()
{
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
   if(maxOrderOpt.getCount())
   {
      maxOrder = asInt(maxOrderOpt.getValue()[0]);
   }
   if(intervalOpt.getCount())
   {
      interval = asInt(intervalOpt.getValue()[0]);
   }
   if(dcbFileListOpt.getCount())
   {
      dcbFileListName = dcbFileListOpt.getValue()[0];
   }
   if(outputFileListOpt.getCount())
   {
      outputFileListName = outputFileListOpt.getValue()[0];
   }
   

}  // End of method 'IonoDCB::spinUp()'

  // Method to print solution values
void IonoDCB::printSolution(  ofstream& outfile,
                              int interval,
                              int order,
                              SolverIonoDCB2& solver,
                              const CommonTime& time1,
                              const CommonTime& time2,
                              const gnssDataMap& gData )
{

      // Prepare for printing
   outfile << fixed << setprecision(3);
     
     // Print results
   CivilTime civtime1(time1);
   CivilTime civtime2(time2);

   SourceIDSet recSet = gData.getSourceIDSet();
   SatIDSet satSet = gData.getSatIDSet();

   outfile<<"Start time : "<<civtime1<<endl
	  <<"End   time : "<<civtime2<<endl;
   outfile<<"DCBs for satellite and receiver (unit : ns)"<<endl
          <<"Satellite  P1-P2     P1-C1"<<endl;
   for (SatIDSet::iterator it = satSet.begin();
        it != satSet.end(); it++)
   {  
      outfile<<asString(*it)<<setw(10)<<solver.getSatDCB(*it)
             <<setw(10)<<solver.getSatP1C1DCB(*it)<<endl;
   }
   outfile<<endl<<"Station    P1-P2"<<endl;    
   for (SourceIDSet::iterator it2 = recSet.begin();
        it2 != recSet.end(); it2++)
   {
      if (asString(*it2).substr(0,1) == "G")  // GPS receiver
      {
        outfile<<asString(*it2).substr(4,4)<<setw(12)<<solver.getRecDCB(*it2)<<endl;
      }
      
      else if (asString(*it2).substr(0,1) == "M")  // Mixed receiver
      {
        outfile<<asString(*it2).substr(6,4)<<setw(12)<<solver.getRecDCB(*it2)<<endl;
      }
   }
   outfile<<endl;
   Vector<double> ionoCoef = solver.getIonoCoef();
   int sum = ionoCoef.size();
   int count = interval/2 + 1;
   int i = 0;
   for (int index = 1;index <= count;index++)
   {
      CommonTime epoch(time1+(index-1)*7200.0);
      CivilTime time(epoch);

      outfile<<index<<endl;
      outfile<<"Time : "<<time<<endl
             <<"Coefficients of ionosphere (unit : TECU)"<<endl 
             <<"Order    Degree    Value"<<endl;
     for (int n = 0;n <=order;n++)
     {
       for (int m = 0; m <=n;m++)
       {
      
         if (m == 0)
         { 
             outfile<<setw(3)<<n<<setw(9)<<m<<setw(12)<<ionoCoef(i)<<endl;
         }
      
         else
         {
             outfile<<setw(3)<<n<<setw(9)<<m<<setw(12)<<ionoCoef(i)<<endl;
             i++;
             outfile<<setw(3)<<n<<setw(9)<<-m<<setw(12)<<ionoCoef(i)<<endl;
         }
         i++;
       }
     
     }

  }  // End for (int index ...)
     
    return;


}  // End of method 'IonoDCB::printSolution()'



   // Method that will really process information
void IonoDCB::process()
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


      // Read and store dcb data
   DCBDataReader dcbStore;

   if(dcbFileListOpt.getCount() )
   {
         // Now read dcb file from 'dcbFileName'
      ifstream dcbFileListStream;

         // Open dcbFileList File
      dcbFileListStream.open(dcbFileListName.c_str(), ios::in);
      if(!dcbFileListStream)
      {
            // If file doesn't exist, issue a warning
         cerr << "dcb file List Name '" << dcbFileListName << "' doesn't exist or you don't "
              << "have permission to read it." << endl;
         exit(-1);
      }

      string dcbFile;

         // Here is just a dcb file, we only read one month's dcb data.
      while(dcbFileListStream >> dcbFile)
      {
         try
         {
            dcbStore.open(dcbFile);
         }
         catch(FileMissingException e)
         {
            cerr << "Warning! The DCB file '"<< dcbFile <<"' does not exist!" 
                 << endl;
            exit(-1);
         }
      };

      dcbFileListStream.close();
   }  
      //**********************************************************
      // Now, Let's perform the IonoDCB for each rinex files
      //**********************************************************

   vector<string> rnxFileListVec;
      
      // Now read rinex files from 'rnxFileList'
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
      
      // Now read out files from 'outFileList'
   ifstream outputFileListStream;

   if(outputFileListOpt.getCount())
   {
         // Open ouputFileList File
      outputFileListStream.open(outputFileListName.c_str());
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
      // store the information of plural stations
   gnssDataMap gdsMap;

     // store the receiver
   SourceIDSet P1P2RecSet;
   SourceIDSet C1P2RecSet;

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

      bool usingC1(false);
         // Get the station name for current rinex file 
      string station = roh.markerName;
         // Get the receiver type  
      string RecType = roh.recType;

      string recTypeFile("recType.list");
      RecTypeDataReader recTypeData;
      recTypeData.open(recTypeFile);
      
      set<string> recCodeSet = recTypeData.getCode(RecType);
      int C1 = recCodeSet.count("C1");
      int P1 = recCodeSet.count("P1");
      int P2 = recCodeSet.count("P2");
      int X2 = recCodeSet.count("X2");

      if (C1)
      {
         usingC1 = true;	  
      }
      Triple antennaPos= roh.antennaPosition;
      cout<<"Processing station : "<<station.substr(0,4)<<endl;

      Position nominalPos( antennaPos[0],antennaPos[1],antennaPos[2] );

         // Create a 'ProcessingList' object where we'll store
         // the processing objects in order
      ProcessingList pList;

         // Convert C1 to P1 
      if(dcbFileListOpt.getCount() )
      {
         // Declare a CC2NONCC object
         CC2NONCC cc2noncc(dcbStore);
         // Read the receiver type file.
         cc2noncc.loadRecTypeFile ("recType.list");

         cc2noncc.setRecType(roh.recType);
         cc2noncc.setCopyC1ToP1(true);

         // Add to processing list
         pList.push_back(cc2noncc);

      }

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
      bool filterCode( true);

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

         // Objects to mark cycle slips
      LICSDetector markCSLI;         // Checks LI cycle slips
      pList.push_back(markCSLI);      // Add to processing list
      MWCSDetector markCSMW;          // Checks Merbourne-Wubbena cycle slips
      pList.push_back(markCSMW);       // Add to processing list

     

         // Object to keep track of satellite arcs
      SatArcMarker2 markArc;
      markArc.setDeleteUnstableSats(false);
      markArc.setUnstablePeriod(151.0);
      pList.push_back(markArc);       // Add to processing list


         // Object to decimate data
      Decimate decimateData(30.0, 1.0, SP3EphList.getInitialTime());
      pList.push_back(decimateData);       // Add to processing list


         // Declare a basic modeler
         // the cut-off elevations is set to 10 deg
         // if you want to change, use function setMinElev().
      BasicModel basic(nominalPos, SP3EphList);
         // If we are going to use P1 instead of C1, we must reconfigure 'basic'
      if ( !usingC1 )
      {
         basic.setDefaultObservable(TypeID::P1);
      }
         // Add to processing list
      pList.push_back(basic);

         // Object to compute weights based on elevation
      ComputeElevWeights elevWeights;
      pList.push_back(elevWeights);       // Add to processing list


         // Object to remove eclipsed satellites
      EclipsedSatFilter eclipsedSV;
      pList.push_back(eclipsedSV);       // Add to processing list

         // Object to compute code combination with minus ionospheric delays
         // for L1/L2 calibration
      ComputeLinear linear2;

         // Read if we should use C1 instead of P1
      if ( usingC1 )
      {
         linear2.addLinear(comb.piCombWithC1);
        
      }
      else
      {
         linear2.addLinear(comb.piCombination);
      }
      pList.push_back(linear2);       // Add to processing list

        // Object to smooth PI 
     PISmoother smoothPI;
     smoothPI.setMaxWindowSize(8);
     pList.push_back(smoothPI);       // Add to processing list

      // Initialize the GetIPP
     GetIPP getIPP(nominalPos);
     pList.push_back(getIPP);       // Add to processing list

         // This is the GNSS data structure that will hold all the
         // GNSS-related information
     gnssRinex gRin;
        // just store the type needed to reduce the memory consumption
        // if using global stations to compute, the memory consumption
	// is very huge
     TypeIDSet typeNeed;
     typeNeed.insert(TypeID::PI);
     typeNeed.insert(TypeID::P1);
     typeNeed.insert(TypeID::C1);
     typeNeed.insert(TypeID::LatIPP);
     typeNeed.insert(TypeID::LonIPP);
     typeNeed.insert(TypeID::ionoMap);
     typeNeed.insert(TypeID::weight);
     
         // Loop over all data epochs
     while(rin >> gRin)
     {

            // Store current epoch
        CommonTime time(gRin.header.epoch);
            // Store source
        SourceID source(gRin.header.source);
        if (usingC1)
        {
           C1P2RecSet.insert(source); 	   
        }
        else
        {
           P1P2RecSet.insert(source);	   
        }

        try
        {
               // Let's process data. Thanks to 'ProcessingList' this is
               // very simple and compact: Just one line of code!!!.
            gRin >> pList;
	  	    // add gRin into gnssDataMap
     	    gRin.keepOnlyTypeID(typeNeed);

            gdsMap.addGnssRinex(gRin); 
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
       
      }  // End of 'while(rin >> gRin)'


         // Close current Rinex observation stream
      rin.close();
      ++rnxit;

   }  // end of 'while (...)'

    cout<<"Preprocessing done ! Start to estimate DCBs ..."<<endl;
     // initialize the class 
    SolverIonoDCB2 ionoDCBSolver2(maxOrder);

  
    outit = outputFileListVec.begin();
   
    string outputFileName;

       // Let's open the output file
    if( outputFileListOpt.getCount() )
    {
       outputFileName = (*outit);
    }
    else
    {
       outputFileName = string("IonoDCB.out");
    }

    ofstream outfile;
    outfile.open( outputFileName.c_str(), ios::out );

    CommonTime epochFirst( gdsMap.begin()->first );
    CommonTime epochLast( (--gdsMap.end())->first );
   

    if (intervalOpt.getCount())
    {
       int count = 24/interval; 
       double intervals = interval * 3600.0;  
 
      for (int i = 0;i<count;i++) 
      {
        CommonTime epoch1(epochFirst + intervals*i);
        CommonTime epoch2(epochFirst + intervals*(i+1)-30.0);
        if (epoch2 > epochLast)
        {
           epoch2 = epochLast;
        }
       
       gnssDataMap gData(gdsMap.getDataFromTimeSpan(epoch1,epoch2));
       
       ionoDCBSolver2.Process(gData,P1P2RecSet,C1P2RecSet,interval);
       printSolution(outfile,interval,maxOrder,ionoDCBSolver2,
                     epoch1,epoch2,gData);     
     }    
   } 
   else // using the data of one day to estimate DCBs
   {
     ionoDCBSolver2.Process(gdsMap,P1P2RecSet,C1P2RecSet,24);
   
     printSolution(outfile,24,maxOrder,ionoDCBSolver2,
                  epochFirst,epochLast,gdsMap); 
   } 
   
   cout<<"Esitmating DCBs done ! Results have been written into "
       <<outputFileName.c_str()<<endl;

      // At last , clear the SP3 object
   SP3EphList.clear();

}  // End of 'IonoDCB::process()'



   // Main function
int main(int argc, char* argv[])
{
   try
   {

      IonoDCB program(argv[0]);

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
