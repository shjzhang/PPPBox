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
		std::string lastLine;
//		std::string lastLine = "***DefaultLine****";
//		cout << "defaultline: " << lastLine << endl;
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
			//cout << "currentLine" << strm.line << endl;

			// read GPS Week
		/*	if (strm.line.substr(1,6) == string("OUTPUT")) 
			{
				
				// std::cout << "OUTPUT LINE" << strm.line << std::endl;
				if (strm.line.length() < 55)
				{
					GPSWeek = asString(strm.line.substr(47,4));
					break;
				}
				
				else
				{
					GPSWeek = asString(strm.line.substr(69,4));
					break;
				}
				//	cout << "GPSWeek" << GPSWeek << endl;
			} 
		*/
		
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
			
			// 
			if (antennaTypeFlag)
			{
				/// note line
				if (strm.line[0] == '*')	{ break; }
			
				/// data line
				else if (strm.line[0] == ' ' && strm.line[1] != ' ')
				{
					station = asString(strm.line.substr(1,4));
					//cout << "SSCData::ATstation: " << station << endl;
					antennaType = asString(strm.line.substr(42,20));				
					//cout << "SSCData::antennaType: "	<< antennaType << endl;
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
				 	//cout << "1 ATOFstation: " << station << endl;
					//cout << "1 U: " << antennaOffset[0] << " "
					//		<< "1 N: " << antennaOffset[1] << " " 
					//		<< "1 E: " << antennaOffset[2] << endl;
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
				if (strm.line[0] == '*')	{ break; }
				
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
					double x, y, z, vx, vy, vz;
					
					// read reference epoch
					RefEpoch.year = asDouble(strm.line.substr(27,2));
					RefEpoch.doy = asDouble(strm.line.substr(30,3));
					RefEpoch.sod = asDouble(strm.line.substr(34,5));

					/// read STAX
					if (strm.line.substr(7,4) == std::string("STAX"))
					{
					//	cout << "1 X lastLine: " << lastLine << endl;
					//	if (strm.line.substr(14,4) != lastLine.substr(14,4))
					//	{
							x = asDouble(strm.line.substr(47,21));
					//		cout << "1 STAX: " << coordinates[0] << endl;
					//		lastLine = strm.line; 
							break;
					//	}
					//	else
					//	{
					//		coordinates[0] = 0.0;
					//		FFStreamError err("STAX error in station:"
					//								+ strm.line.substr(14,4) + "!");
					//		GPSTK_THROW(err);
					//		break;
					//	}
					//
					}
					
					/// STAY
					else if (strm.line.substr(7,4) == std::string("STAY"))
					{
					//	cout << "lastLine: " << lastLine << endl;
					//	if (strm.line.substr(14,4) == lastLine.substr(14,4))
					//	{
							
							y = asDouble(strm.line.substr(47,21));
					//		cout << "1 STAY: " << coordinates[1] << endl;
					//		lastLine = strm.line;
							break;
					//	}
					//	else
					//	{
					//		coordinates[1] = 0.0;
					//		FFStreamError err("STAY error in station: " 
					//								+ strm.line.substr(14,4) + "!");
					//		GPSTK_THROW(err);
					//		break; 
					//	}
				
					}

					/// STAZ
					else if (strm.line.substr(7,4) == std::string("STAZ"))
					{						
            	//	if (strm.line.substr(14,4) == lastLine.substr(14,4))
            	// {
            	      z = asDouble(strm.line.substr(47,21));
					//		cout << "1 STAZ: " << coordinates[2] << endl;
					//		lastLine = strm.line;
            	// }
             	// else 
               // {
					//		coordinates[2] = 0.0;
               //    FFStreamError err("STAZ error in station: "
					//								+ strm.line.substr(14,4));
					//		GPSTK_THROW(err);
               // }
		
						/// if there is no error in STAX,STAY,STAZ,
						/// read the stationID at the line which 
						/// contains STAZ.
                  if ( x != 0.0 && y != 0.0
								&& z != 0.0)
						{
							station = strm.line.substr(14,4); 
					
							coordinates[0] = x;
							coordinates[1] = y;
							coordinates[2] = z;			
					//		cout << "1 coor_station: " << station << endl;
							break;
						}
						else 
						{ 
							FFStreamError fse("no stationID read in this station: "
													+ strm.line.substr(14,4));
							GPSTK_THROW(fse);
							break; 
						}

					} // end of 'else if STAZ'
					
					
               else if (strm.line.substr(7,4) == std::string("VELX"))
               {
                  vx = asDouble(strm.line.substr(47,21));
					//cout << setiosflags(ios::fixed) << "test_vx: "<< vx << endl; 
						containVelFlag = true;
						break;
               }

               else if (strm.line.substr(7,4) == std::string("VELY"))
               {
                  vy = asDouble(strm.line.substr(47,21));
						break;
               }

               else if (strm.line.substr(7,4) == std::string("VELZ"))
               {
               	vz = asDouble(strm.line.substr(47,21));

                  if ( vx != 0.0 && vy != 0.0
                        && vz != 0.0)
                  {
                     station = strm.line.substr(14,4);
	
                     vel[0] = vx;
                     vel[1] = vy;
                     vel[2] = vz;		
               //    cout << "1 coor_station: " << station << endl;
                     break;
                  }
                  else
                  {
                     FFStreamError fse("no stationID read in this station: "
                                       + strm.line.substr(14,4));
                     GPSTK_THROW(fse);
                     break;
                  }

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
			//cout << "empty line! PASS!" << endl;
			break;
		} // end of while(1)

	} // end of void SSCData::reallyGet..."
   
	void SSCData::reallyPutRecord(FFStream& ffs) const
      throw(exception, FFStreamError, StringException)
	{
		std::cout << "We will finish this part in a later time." << std::endl;
	}
} // end of namespace



   
