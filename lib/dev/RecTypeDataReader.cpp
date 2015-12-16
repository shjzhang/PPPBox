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
//  Copyright (c), 2015
//
//  Q.Liu, Wuhan University
//
//============================================================================
// 
//  Revision
//
//  2015/12/09
//
//  Throw error if the file doesn't exist!
//
//============================================================================

/**
 * @file RecTypeDataReader.cpp
 * Class to read receiver type from CODE.
 */

#include "RecTypeDataReader.hpp"

using namespace std;

namespace gpstk
{
	  // Method to store load receiver information in this class'
	  // data map
	void RecTypeDataReader::loadData()
		throw( FFStreamError, gpstk::StringUtils::StringException )
	{
			try
			{
				  // a buffer
				string line;
				string recType;
				string code;
				int j=0;
				  // Let's skip 6 lines
				for(int i=0; i<6; i++) formattedGetLine(line, true);

				  // Now, let's read data
				while(1)
				{
					formattedGetLine(line, true);
					if(line.length() < 50) continue;
					
					recType = line.substr(0,20);
					  // Delete recType's last blank space
					recType = recType.erase(recType.find_last_not_of(" ")+1);
					freqNum = StringUtils::asInt(line.substr(23,1));
					
					for(j=0; j<freqNum; j++)
					{
						code = line.substr(29,2);
						recTypeMap[recType].insert(code);
						formattedGetLine(line, true);
					}

				} // End of 'while(1)'

			} // End of try block 
			catch (EndOfFile& e)
			{
				  // We should close this data stream before returning
				(*this).close();

				return;
			}
			catch (...)
			{
				  // We should close this data stream before returning
				(*this).close();

				return;
			}

	} // End of 'RecTypeDataReader::loadData()'

	void RecTypeDataReader::open(const char* fn)
		throw( FileMissingException )
	{
      try
      {
		     // We need to be sure current data stream is closed
		   (*this).close();

		     //Open data stream
		   FFTextStream::open(fn, std::ios::in);
         if( !FFTextStream::is_open() )
         {
            std::cerr << " The file " << fn << " doesn't exist! "
                 << std::endl;
            exit(-1);
         }
		   loadData();
      }
      catch(FileMissingException& e)
      {
         GPSTK_RETHROW(e);
      }

		return;

	} // End of 'RecTypeDataReader::open()'

	void RecTypeDataReader::open(const string& fn)
		throw( FileMissingException )
	{
      try
      {
		     // We need to be sure current data stream is closed
		   (*this).close();

		     //Open data stream
		   FFTextStream::open(fn.c_str(), std::ios::in);
         if( !FFTextStream::is_open() )
         {
            GPSTK_THROW(FileMissingException("RecType File does not exist!"));
         }

		   loadData();
      }
      catch(FileMissingException& e)
      {
         GPSTK_RETHROW(e);
      }

		return;

	} // End of 'RecTypeDataReader::open()'

	set <string> RecTypeDataReader::getCode(const string& receiver)
	{
		return recTypeMap[receiver];
	}

};// End of namespace gpstk
