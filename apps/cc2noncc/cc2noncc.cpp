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
//  Copyright (c)
//
//  Q.Liu, Wuhan University, 2015.11.19
//
//============================================================================

// System
#include <iostream>
#include <string>

// Library
#include "BasicFramework.hpp"
#include "YDSTime.hpp"
#include "CommonTime.hpp"
#include "GNSSconstants.hpp"

#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RecTypeDataReader.hpp"
#include "DCBDataReader.hpp"
#include "CC2NONCC.hpp"
#include "ProcessingClass.hpp"
#include "SimpleFilter.hpp"

// Project

using namespace std;
using namespace gpstk;


class cc2noncc : public gpstk::BasicFramework
{
public:

   cc2noncc(const std::string& applName,
          const std::string& applDesc) 
      throw();

   ~cc2noncc() 
   {}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   virtual bool initialize(int argc, char *argv[]) 
      throw();
#pragma clang diagnostic pop

protected:

   virtual void process();

      // required
   gpstk::CommandOptionWithAnyArg inFileOption;
   gpstk::CommandOptionWithAnyArg recTypeFileOption;
   gpstk::CommandOptionWithAnyArg DCBFileOption;
   gpstk::CommandOptionWithAnyArg outFileOption;

};


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
cc2noncc::cc2noncc(const std::string& applName,
               const std::string& applDesc) 
   throw()
   :BasicFramework(applName, applDesc),
    inFileOption('i', "inFile", "Input RINEX File.", true),
    recTypeFileOption('r',"recTypeFile","Receiver type file.", true),
    DCBFileOption('D',"DCBFile", "DCB_P1C1 file.", true),
    outFileOption('o', "outFile","Output RINEX file.",true)
	{
	}
#pragma clang diagnostic pop

  //*********
  //  initialize
  //*********

bool cc2noncc::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) 
      return false;
   return true;
}


void cc2noncc::process()
{
   try{
	      // Get input files from command line arguments
		string infile = inFileOption.getValue().front();
		string recTypeFile = recTypeFileOption.getValue().front();
		string DCBfile = DCBFileOption.getValue().front();
		string outfile = outFileOption.getValue().front();

        cout << "Start processing " << infile << endl;
		  // Create input observation file stream
		RinexObsStream rin;

		  // Enable exceptions
		rin.exceptions(ios::failbit);
	      // Read and store the input RINEX file.
		rin.open(infile, std::ios::in );
		
		
		  // Let's read the header firstly!!!!
		RinexObsHeader roh;
		rin >> roh;
		  // Read the receiver from RINEX file's header section.
		string recType;  
		recType = roh.recType;
		
		CC2NONCC cc2noncc;
		cc2noncc.setRecType(recType);
		cc2noncc.setDCBFile(DCBfile);
		cc2noncc.setRecTypeFile(recTypeFile);

      SimpleFilter filter;
      filter.addFilteredType(TypeID::C1);
      filter.addFilteredType(TypeID::P2);
		
		  // Create output observation file stream
		RinexObsStream out(outfile, ios::out);
		out << roh;
		
	      // Create a gnssRinex object
		gnssRinex gRin;
		while(rin >> gRin)
		{
			gRin >> filter;
			gRin >> cc2noncc;
			out << gRin;
		}
        cout << "Finish cc2noncc! " << endl;
	} // end of try{} 
   catch(InvalidRequest& e)
   {
      cout << e << endl;
   }
   catch(Exception& e)
   {
      cout << e << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl
           << endl
           << "Terminating.." << endl;
   }
   catch(...)
   {
      cout << "Unknown exception... terminating..." << endl;
   }

}

int main( int argc, char*argv[] )
{
   try
   {
      cc2noncc fc("cc2noncc", 
	  "Convert the cc(uses cross-correlation tenique) to noncc.");
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

