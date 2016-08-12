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
// @file : SSCData.cpp
//
//

#include "SSCStream.hpp"
#include "SSCHeader.hpp"
#include "SSCData.hpp"
#include "StringUtils.hpp"
#include "YDSTime.hpp"
#include "TypeID.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace gpstk
{
	
	void SSCData::reallyGetRecord(FFStream& ffs)
		throw(exception, FFStreamError, StringException)
	{
		SSCStream& strm = dynamic_cast<SSCStream&>(ffs);
		try 
		{
      	strm.formattedGetLine(strm.line, true);
      }
      catch (gpstk::Exception& e)
      {
      	FFStreamError fse("Without data!");
         GPSTK_THROW(fse);
      }
		
		//strm.formattedGetLine(strm.line, true);
		while(1)
		{
			   // read "Antenna Type" part
			if (strm.line == string("+SITE/ANTENNA"))
			{
				   // std::cout << "+SITE/ANTENNA" << strm.line << std::endl;
				antennaTypeFlag = true;
				break;	
			}
	
     	 	   // read "Antenna offset" part
      	else if (strm.line == string("+SITE/ECCENTRICITY"))
      	{
        	   antennaOffsetFlag = true;
				break;
      	}
			
			   // read the "station coordinates " part
			else if (strm.line == string("+SOLUTION/ESTIMATE"))
			{
				stationCoorFlag = true;
				break;
			} 
			
			else { ; }
			
			if (antennaTypeFlag)
			{
				   /// note line
				if (strm.line[0] == '*')	{ break; }
			
				   /// data line
				else if (strm.line[0] == ' ' && strm.line[1] != ' ')
				{
					station = asString(strm.line.substr(1,4));
					antennaType = asString(strm.line.substr(42,20));				
					break;
				}

				   /// end line
				else if (strm.line[0] == '-')
				{
					antennaTypeFlag = false;
					station = "";
					break;
				}
			} // end of 'if(antennaTypeFlag)'
			
			
			if (antennaOffsetFlag)
			{
				   /// note line
				if (strm.line[0] == '*')   { break; }
		
				   /// data line
				else if (strm.line[0] == ' ' && strm.line[1] != ' ')
	       	{
            	station = strm.line.substr(1,4);
            	antennaOffset[0] = asDouble(strm.line.substr(46,8));
					antennaOffset[1] = asDouble(strm.line.substr(55,8));
					antennaOffset[2] = asDouble(strm.line.substr(64,8));
          	   break;
				}

        		/// end line
				else if (strm.line[0] == '-')
       	   {
          	   antennaOffsetFlag = false;
					station = "";
         	   break;
      	   }

      	} // end of 'if(antennaOffsetFlag)'
			
			
			if (stationCoorFlag)
			{
				   /// comment line
				if (strm.line[0] == '*')	 break; 
				
			   	/// data line
				if (strm.line[0] == ' ' && strm.line[5] != ' ')
				{
					   //@param:
					   //{ x : x coordinates;
					   //	 y : y coordinates;
				   	//  z : z coordinates;
					   // vx : x velocity;
					   // vy : y velocity;
					   // vz : z velocity;
					   //}
					
			   		// read reference epoch
					RefEpoch.year = asDouble(strm.line.substr(27,2));
					RefEpoch.doy = asDouble(strm.line.substr(30,3));
					RefEpoch.sod = asDouble(strm.line.substr(34,5));

					   /// STAX
					if (strm.line.substr(7,4) == std::string("STAX"))
					{
							coordinates[0] = asDouble(strm.line.substr(47,21));
							station = strm.line.substr(14,4); 
							break;
					}
					
					   /// STAY
					else if (strm.line.substr(7,4) == std::string("STAY"))
					{
							coordinates[1] = asDouble(strm.line.substr(47,21));
							station = strm.line.substr(14,4); 
							break;
					}

					   /// STAZ
					else if (strm.line.substr(7,4) == std::string("STAZ"))
					{						
            	   coordinates[2] = asDouble(strm.line.substr(47,21));
		
                        /// Save station name
						station = strm.line.substr(14,4); 
					
						break;

					} // end of 'else if STAZ'
               else if (strm.line.substr(7,4) == std::string("VELX"))
               {
                  vel[0] = asDouble(strm.line.substr(47,21));
                  station = strm.line.substr(14,4);
						containVelFlag = true;
						break;
               }

               else if (strm.line.substr(7,4) == std::string("VELY"))
               {
                  vel[1] = asDouble(strm.line.substr(47,21));
                  station = strm.line.substr(14,4);
						break;
               }

               else if (strm.line.substr(7,4) == std::string("VELZ"))
               {
               	vel[2] = asDouble(strm.line.substr(47,21));

                        /// Save station name
                  station = strm.line.substr(14,4);

                  break;

               }
 
					else 
					{
						FFStreamError fse("No data in this line"
												+ strm.line);
						GPSTK_THROW(fse);
						break;
					}

				} // end of data line

				   /// end line
				if (strm.line[0] == '-')	
				{
					stationCoorFlag = false;
					break;
				}
			}

			break;

		} // end of while(1)

	} // end of void SSCData::reallyGet..."
   
	void SSCData::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
	{
		std::cout << "We will finish this part in a later time." << std::endl;
	}
} // end of namespace



   
