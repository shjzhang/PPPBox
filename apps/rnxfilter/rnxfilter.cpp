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
#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "ProcessingClass.hpp"
#include "SimpleFilter.hpp"

// Project

using namespace std;
using namespace gpstk;


class rnxfilter : public gpstk::BasicFramework
{
public:

   rnxfilter(const std::string& applName,
          const std::string& applDesc) 
      throw();

   ~rnxfilter() 
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
   gpstk::CommandOptionWithAnyArg outFileOption;

};


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
rnxfilter::rnxfilter(const std::string& applName,
               const std::string& applDesc) 
   throw()
   :BasicFramework(applName, applDesc),
    inFileOption('i', "inFile", "Input RINEX File.", true),
    outFileOption('o', "outFile","Output RINEX file.",true)
	{
	}
#pragma clang diagnostic pop

  //*********
  //  initialize
  //*********

bool rnxfilter::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) 
      return false;
   return true;
}


void rnxfilter::process()
{
   try{
	      // Get input files from command line arguments
		string infile = inFileOption.getValue().front();
		string outfile = outFileOption.getValue().front();

		  // Create input observation file stream
		RinexObsStream rin;

		  // Enable exceptions
		rin.exceptions(ios::failbit);
	      // Read and store the input RINEX file.
		rin.open(infile, std::ios::in );
		
		
		  // Let's read the header firstly!!!!
		RinexObsHeader roh;
		rin >> roh;
		
		  // Create output observation file stream
		RinexObsStream out(outfile, ios::out);
		out << roh;

      SimpleFilter filter;
      filter.addFilteredType(TypeID::C1);
      filter.addFilteredType(TypeID::P2);
		
	      // Create a gnssRinex object
		gnssRinex gRin;
		while(rin >> gRin)
		{
         gRin>>filter;
			out << gRin;
		}
        cout << "Finish sorting work! " << endl;
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
      rnxfilter fc("rnxfilter", 
	  "Sort the observations according SatID at every epoch.");
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

