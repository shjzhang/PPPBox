//==========================================================================
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
//
//  K.M.Zhu, Wuhan University 
//
//==========================================================================
//  Revision
//
//
//==========================================================================
//
// @file : CovFrame.hpp
// 
// 

#include "BasicFramework.hpp"
#include "CommandOption.hpp"

using namespace std;
using namespace gpstk;


class CovFrame : public gpstk::BasicFramework
{
public:

	CovFrame(char* arg0,
				const std::string& type,
				const std::string& message = std::string())
		: gpstk::BasicFramework(arg0, 
											"Covert input " + type + "files. " + message),

		inputFileOption ( 's', "sscfile",
		      " [-s|--sscfile]  Name of input SSC file",
      		         true ),
   		
   	outputFileOption ( 'm', "mscfile",
 " [-m|--mscfile]  Name of output MSC file ( igs+'GPSWeek'+.msc by default )",
						 false )

		{
	      outputFileOption.setMaxCount(1);	
     		inputFileOption.setMaxCount(1);
  		}		
		  

		virtual bool initialize(int argc, char* argv[]) throw()
		{
			if ( !gpstk::BasicFramework::initialize(argc, argv))
				return false;

			if (inputFileOption.getCount() != 1)
			{
				std::cerr << "This program requires one input files." 
							 << std::endl;
				return false;
			}
			
			string ifname = inputFileOption.getValue()[0];

			if( outputFileOption.getCount() > 0 )
			{
			   string ofname = outputFileOption.getValue()[0];
			}

         return true;
		}


protected:
	
	virtual void process() = 0;

	gpstk::CommandOptionWithAnyArg inputFileOption;
	gpstk::CommandOptionWithAnyArg outputFileOption;

};
