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
//  Revision:
//		
//		2015/11/05 : Correct the GPS week we print in output file name to be 
//						 in accordance with the solution	time. 
//						 add a message of generating file successfully.
//
//
//==========================================================================
//
// @file : SSC2MSC.cpp
// Convert SSC file to MSC file
//
 
#include "CovFrame.hpp"
#include "BasicFramework.hpp"

#include "SSCStream.hpp"
#include "SSCBase.hpp"
#include "SSCHeader.hpp"
#include "SSCData.hpp"

#include "DataStructures.hpp"
#include "TypeID.hpp"
#include "SourceID.hpp"
#include "YDSTime.hpp"
#include "CommonTime.hpp"
#include "GPSWeekZcount.hpp"
#include "GPSWeek.hpp"
#include "TimeString.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

/** Modified at 2015/11/03 **/
class SSC2MSC : public CovFrame
{
public:

	SSC2MSC(char* arg0)
		: CovFrame(arg0, std::string("SSC file to MSC "))
	{}

protected:

	virtual void process();

};
void SSC2MSC::process()
{
	try
	{
		int i = 0;
		SSCHeader sh;
		SSCData sd;
		
		string filename = inputFileOption.getValue()[0];

		SSCStream ss;
		ss.open( filename.c_str(), ios::in );
		if (!ss.is_open())
		{
			cerr << "No such file or the filename you input is wrong!" << endl
				  << "Please make sure the file exists!" << endl;
			exit(-1);
		}	

	//	try 
	//	{
		//	ss.open( filename.c_str());
	//	}
	//	catch(...)
	//	{
	//		cerr << "Problem opening file "
	//		  << inputFileOption.getValue()[0]
	//	     << endl;
	// 	exit (-1);
	// }

		
		ss >> sh;
		
		   // take out the release year and day of year stored in ssc header
		   // : releaseTime
		int releaseYear, releaseDOY;
		releaseYear = sh.releaseTime.year;
		releaseDOY  = sh.releaseTime.doy ;
		
		   // get GPS week from release time in header:
		CommonTime commonReleTime(sh.releaseTime.convertToCommonTime());

		std::string GPSWeek = printTime(commonReleTime, "%4F");
		//cout << "GPSWeek: " << GPSWeek << endl;
		 
		
		YDSTime RefEpoch, EarliestEffect;
		//cout << "refepoch " << RefEpoch << endl;
		
		typeValueMap tvMap,
						 tvMap2,
						 tvMap3;
		
			// define a map to store station antenna type.
		std::map< std::string, std::string > stationAntypeMap;
		
			// define a map to store station antenna offsets and coordinates
		std::map< std::string, typeValueMap > stationTypeValueMap,
														  stationTypeValueMap2,
														  stationTypeValueMap3;
		
			// read data
		while (ss >> sd)
		{
				// sd.antennaTypeFlag turns 'TRUE' when the loop comes to 
				// the block of antenna type data 
			
			// cout << "atf " << sd.antennaTypeFlag << " " 
			//		<< "aof " << sd.antennaOffsetFlag << " "
			//		<< "scf " << sd.stationCoorFlag << endl;
			
			if (sd.antennaTypeFlag)
			{
					// make sure the station and antenna type have values
				if ( sd.station.length() != 0 &&
					  sd.antennaType.length() != 0)
				{
					stationAntypeMap[sd.station] = sd.antennaType;
			//		cout << "2 Antennatype: "<< sd.antennaType << endl;
				}
			}
		
				// sd.antennaOffsetFlag turns 'TRUE' when the loop comes
				// to the block of antenna offset data 
			if (sd.antennaOffsetFlag)
			{
					// make sure the station has a value
				if ( sd.station.length() != 0)
				{
					tvMap[TypeID::AntOffU] = sd.antennaOffset[0];
					tvMap[TypeID::AntOffN] = sd.antennaOffset[1];
					tvMap[TypeID::AntOffE] = sd.antennaOffset[2];
					stationTypeValueMap[sd.station] = tvMap;
					//cout << "2 ATOFstation: " << sd.station <<endl;
					//cout << "2 U: " << sd.antennaOffset[0] << " "
					//		<< "2 N: " << sd.antennaOffset[1] << " "
					//		<< "2 E: " << sd.antennaOffset[2] << endl;
				}
	
			}
			
				// sd.stationCoorFlag turns 'TRUE' when the loop comes
         	// to block of station coordinates data 
			if (sd.stationCoorFlag)
			{
					// input the reference epoch
				RefEpoch = sd.RefEpoch;
				EarliestEffect = sd.RefEpoch;
            if (RefEpoch.year >= 80 && RefEpoch.year <= 99)
				{
               RefEpoch.year = 1900 + RefEpoch.year;
               EarliestEffect.year = 1900 + EarliestEffect.year;
				}
            else if ( RefEpoch >= 0 && RefEpoch.year <= 20)
            {  
					RefEpoch.year = 2000 + RefEpoch.year;
					EarliestEffect.year = 2000 + EarliestEffect.year;
				}
				else std::cerr << "Invalid RefEpoch!" << std::endl;
				
					// make sure the coordinates have values
				if (sd.coordinates[0] != 0.0 && 
					 sd.coordinates[1] != 0.0 &&
				 	 sd.coordinates[2] != 0.0)
				{
					tvMap2[TypeID::STAX] = sd.coordinates[0];
					tvMap2[TypeID::STAY] = sd.coordinates[1];
					tvMap2[TypeID::STAZ] = sd.coordinates[2];
					stationTypeValueMap2[sd.station] = tvMap2;			
					//cout << "2 station: " << sd.station << endl;
					//cout << "2 X: " << sd.coordinates[0] << " "
					//		<< "2 Y: " << sd.coordinates[1] << " "
					//		<< "2 Z: " << sd.coordinates[2] << endl;
				}
				
				if (sd.containVelFlag)
				{
					tvMap3[TypeID::VELX] = sd.vel[0];
               tvMap3[TypeID::VELY] = sd.vel[1];
               tvMap3[TypeID::VELZ] = sd.vel[2];
					stationTypeValueMap3[sd.station] = tvMap3;

				}
		
			}
		} // end of 'while'
	

	
	
	 	/// Now we take out the data and store them in a MSC file

			// define an ofstream
		ofstream msc;
		
			// we can modify the output file name in this part
		if ( outputFileOption.getCount() > 0 )
		{
			try 
			{
				msc.open( outputFileOption.getValue()[0].c_str(), ios::out );
			}
			
			catch(...)
			{
				cerr << "Problem opening file "
					  << outputFileOption.getValue()[0]
					  << endl;
				exit (-1);
			}
			
			cout << "file '" << outputFileOption.getValue()[0] 
					  << "' is generated!" << endl;
						
		}
		
		else
		{
			if (filename[5] == 'P')
			{
				int GPSWeek_int;
					/// The GPS week we get from the header's release time 
					/// in weekly igs SSC file is always 2 weeks later 
					/// than the solution time, so we need to minus 2 to 
					/// get the correct SOLUTION WEEK!
				GPSWeek_int = atoi(GPSWeek.c_str())-2;
				//itoa(GPSWeek_int, GPSWeek.c_str(), 10);
				char temp[256];
				sprintf(temp, "%d", GPSWeek_int);
				GPSWeek = temp;
				//cout << "GPS week of solution: " << GPSWeek << endl;
				std::string outputFileName("igs" + GPSWeek + ".msc");
				msc.open( outputFileName.c_str(), ios::out );
			}
			else 
			{
				std::string outputFileName("igs" + GPSWeek + ".msc");
				msc.open( outputFileName.c_str(), ios::out );
			}

				/// check out output file is opening, if true, deliver 
				/// a message
			if (msc.is_open())
			{
				cout << "file 'igs" << GPSWeek << ".msc' is generated!" << endl;
			}
			else 
			{
				cerr << "Problem in opening file igs" << GPSWeek << ".msc" << endl;
			}
		}	
		
		/// test if we store the antenna type data in the map

		//for( std::map<std::string, std::string>::iterator t_saIter
		//		 = stationAntypeMap.begin();
      //       t_saIter != stationAntypeMap.end();
      //       ++t_saIter )
		//{
		//   cout << "MapStation: " << (*t_saIter).first << endl;
		//	  cout << "MapAntennaType: " << (*t_saIter).second << endl;
		//}
		

      	// take data out of the map to the output stream
		for( std::map<std::string, std::string>::iterator saIter
				= stationAntypeMap.begin();
				saIter != stationAntypeMap.end();
				++saIter )
		{
				// the current station we process now
			std::string currStation = (*saIter).first;
			//cout << "2 Current Station:" << (*saIter).first << endl;
			 
			//@param:
			//	STAX : station X coordinate
			// STAY : station Y coordinate
			// STAZ : station Z coordinate
			// AntOffU : antenna U offset
			// AntOffN : antenna N offset
			// AntOffE : antenna E offset
			// VELX : station X velocity
         // VELY : station Y velocity 
         // VELY : station Z velocity  
			double STAX, STAY, STAZ,
					 VELX, VELY, VELZ, 
					 AntOffU, AntOffN, AntOffE;
			
				// find the current station in map which stores antenna offsets
			std::map< std::string, typeValueMap >::iterator stvIter
			= stationTypeValueMap.find(currStation);
			if ( stvIter != stationTypeValueMap.end() )
			{
				AntOffU = (*stvIter).second.getValue(TypeID::AntOffU);
				AntOffN = (*stvIter).second.getValue(TypeID::AntOffN);
				AntOffE = (*stvIter).second.getValue(TypeID::AntOffE);
			}

         	// find the current station in map which stores station coordinates
				// and velocities
         std::map< std::string, typeValueMap >::iterator stvIter2
         = stationTypeValueMap2.find(currStation);
         if ( stvIter != stationTypeValueMap2.end() )
         {
            STAX = (*stvIter2).second.getValue(TypeID::STAX);
            STAY = (*stvIter2).second.getValue(TypeID::STAY);
            STAZ = (*stvIter2).second.getValue(TypeID::STAZ);
			}
			
			if (sd.containVelFlag)
			{
				std::map< std::string, typeValueMap >::iterator stvIter3
         	= stationTypeValueMap3.find(currStation);
         	if ( stvIter != stationTypeValueMap3.end() )
         	{
            	VELX = (*stvIter3).second.getValue(TypeID::VELX);
            	VELY = (*stvIter3).second.getValue(TypeID::VELY);
            	VELZ = (*stvIter3).second.getValue(TypeID::VELZ);
				}
			}
		
				/// when there is no velocity data in SSC file, 
				/// we set the velocity "0.0".	
			else 
			{
				VELX = 0.0;
				VELY = 0.0;
				VELZ = 0.0;
			}

				///@param i : indicates the serial number	of station
			i++;
			
				/// export the output
			if (msc.is_open())
			{
					msc 
					<< setw(4) << releaseYear 
					<< setw(3) << releaseDOY << " "
					
					<< setfill('0') << setw(4) << i
					<<	setw(4) << (*saIter).first << "   " 
					
					<< setw(4) << RefEpoch.year << " " 
					<< setw(3) << RefEpoch.doy << " " 
					<< setfill('0') << setw(5) << int(RefEpoch.sod)
					
					<< setw(4) << EarliestEffect.year << " " 
					<< setw(3) << EarliestEffect.doy << " " 
					<< setfill('0') << setw(5)	<< int(EarliestEffect.sod)
					
					<< setfill(' ') << setprecision(3) 
					<< setiosflags(ios::fixed) << setw(12) << STAX 
					<< setfill(' ') << setprecision(3) 
					<< setiosflags(ios::fixed) << setw(12) << STAY 
					<< setfill(' ') << setprecision(3) 
					<< setiosflags(ios::fixed) << setw(12) << STAZ 
					
               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(7) << VELX
               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(7) << VELY
               << setfill(' ') << setprecision(3)
               << setiosflags(ios::fixed) << setw(7) << VELZ

		 			<< setfill(' ') << setprecision(4) << setw(8) << AntOffU 
					<< setfill(' ') << setprecision(4) << setw(8) << AntOffN 
					<< setfill(' ') << setprecision(4) << setw(8) << AntOffE 
					
					<< setw(20) << (*saIter).second 
					<< endl;
			}	
		} // end of 'for...'


	} // end of 'try'

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
} // end of 'void SSC2MSC::process()' 


int main(int argc, char* argv[])
{
	try
	{
		SSC2MSC m(argv[0]);

			if (!m.initialize(argc, argv))
			return 0;

			if (!m.run())
			{
				return 1;
			}
			return 0;
	}
   catch(Exception& e)
   {
   	cout << e << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 1;

}


