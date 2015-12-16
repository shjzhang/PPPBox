//============================================================================
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
//============================================================================
//
// @file : SSCHeader.cpp
//
 
#include "SSCStream.hpp"
#include "SSCHeader.hpp"
#include "YDSTime.hpp"

#define debug false

namespace gpstk
{
	using namespace StringUtils;
	using namespace std;
	

	void SSCHeader::reallyGetRecord(FFStream& ffs) 
		throw(exception, FFStreamError, StringException)
	{
		SSCStream& strm = dynamic_cast<SSCStream&>(ffs);
		
		string line;
		strm.formattedGetLine(line);
		if(debug) std::cout << "SSC Header Line " << line << std::endl;
		
		if (line[0] == '%' && line[1] == '=')
		{
			// Get Release YDSTime
			int rYear = asInt(line.substr(15,2));
			// Release Year is frome  1980 to 2020;
			if (rYear >= 80 && rYear <= 99)  	rYear = 1900 + rYear;
			else if (rYear >= 0 && rYear <= 20)	rYear = 2000 + rYear;
			else std::cerr << "Relesase Year ERROR !" << std::endl;
			
			int rDoy = asInt(line.substr(18,3));
			double rSod = asInt(line.substr(22,5));
			//cout << "release year " << rYear << endl;
			//cout << "release day of year " << rDoy << endl;	
		
			try
			{
				releaseTime = YDSTime(rYear, rDoy, rSod);
			}
			catch (gpstk::Exception& e)
			{
				FFStreamError fe("Invalid release time!");
				GPSTK_THROW(fe);
			}
			
			//cout << "releseTime " << releaseTime << endl;
			// Get BeginningTime
			int bYear = asInt(line.substr(32,2));
         // Begin Year is frome  1980 to 2020;
         if (bYear >= 80 && bYear <= 99)     bYear = 1900 + bYear;
         else if (bYear >= 0 && bYear <= 20) bYear = 2000 + bYear;
         else std::cerr << "Beginning Year ERROR !" << std::endl;

         int bDoy = asInt(line.substr(35,3));
         double bSod = asInt(line.substr(39,5));
			
         try
         {
            beginningTime = YDSTime(bYear, bDoy, bSod);
         }
         catch (gpstk::Exception& e)
         {
            FFStreamError fe("Invalid beginning time!");
            GPSTK_THROW(fe);
         }


         // Get EndTime
         int eYear = asInt(line.substr(45,2));
         // End Year is frome  1980 to 2020;
         if (eYear >= 80 && eYear <= 99)     eYear = 1900 + eYear;
         else if (eYear >= 0 && eYear <= 20) eYear = 2000 + eYear;
         else std::cerr << "End Year ERROR !" << std::endl;

         int eDoy = asInt(line.substr(48,3));
         double eSod = asInt(line.substr(52,5));
		
         try
         {
            endTime = YDSTime(eYear, eDoy, eSod);
         }
         catch (gpstk::Exception& e)
         {
            FFStreamError fe("Invalid end time!");
            GPSTK_THROW(fe);
         }
			
			// save the header, for use later when reading SSCData records
			strm.header = *this;

		} // End of 'if'

	} // End of 'SSCHeader::reallyGetRecord()'


   void SSCHeader::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
	{
		std::cout << "We are about to finish this!" << std::endl;
	}
	
} // End of 'namespace gpstk'
