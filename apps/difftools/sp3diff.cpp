//  Filename : sp3diff.cpp


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
//  Copyright 2004
//  
//  Revision
// 
//  2015/10/09 
//============================================================================


#include "FileFilterFrame.hpp"
#include "FileFilterFrameWithHeader.hpp"

#include "SP3Data.hpp"
#include "SP3Base.hpp"
#include "SP3Header.hpp"
#include "SP3Stream.hpp"
#include "SP3SatID.hpp"

#include "DataStructures.hpp"
#include "TypeID.hpp"
#include "DiffFrame.hpp"
#include "YDSTime.hpp"
#include "Stats.hpp"
#include <cstring>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

class sp3diff : public DiffFrame
{
public:
	sp3diff(char* arg0)
		: DiffFrame(arg0, std::string("SP3 Ephemeris")),
		  thresholdOption('x', "Threshold", 
			"threshold to exclude the orbit difference")
	{}



protected:
	virtual void process();	


   	gpstk::CommandOptionWithAnyArg thresholdOption;
  
};

void sp3diff::process()
{

	try
	{
		// default = 100
		double threshold(100);
    	if(thresholdOption.getCount())
    	{
         threshold = StringUtils::asInt(thresholdOption.getValue().front());
    	}
		
		// Read SP3 Data
		SP3Header sh1, sh2;
		string FileName1 = inputFileOption.getValue()[0];
		string FileName2 = inputFileOption.getValue()[1];

		SP3Stream ss1( FileName1.c_str() , std::ios::in ),
				  ss2( FileName2.c_str() , std::ios::in );

		
		ss1 >> sh1;
		ss2 >> sh2;
			
	
		if (sh1.satList.size() != sh2.satList.size())
		{
			cout << "The two files have a different number of satellite." 
				 << endl;
         	cout << "The first file has ";
         	for( std::map<SP3SatID, short>::iterator sIter 
				 = sh1.satList.begin();
				 sIter != sh1.satList.end();
				 ++sIter )
			{
				cout << (*sIter).first << ' ';
			}
         
         	cout << endl;

         	cout << "The second file has ";
			for( std::map<SP3SatID, short>::iterator sIter2 
				 = sh2.satList.begin();
				 sIter2 != sh2.satList.end();
				 ++sIter2 )
			{
				cout << (*sIter2).first << ' ';
			}
			
			cout << endl;
/** Modified. The followings are original codes: 
         	vector<SP3SatID> types2 = sh2.satList;
         	vector<SP3SatID>::iterator j = types2.begin();
         	while (j != types2.end())
            {
            	cout << (*j) << ' ';
            	j++;
            }
         	cout << endl;
**/			
			
		} // end of if(header1.satList...)
		
		//
		// Store the two files' Position/Velocity data into map
		//
		epochSatTypeValueMap ephMap1, ephMap2;
		
		// Read data from two ephemeris file streams
		// SP3 ephemeris data
		SP3Data sd1, sd2;
		

		// Read data from file1
		satTypeValueMap stvMap;
		typeValueMap tvMap;
		CommonTime lastTime, currTime;
		bool firstEpoch(true);
		while (ss1>>sd1)
		{
//			cout << "a0"<< sd1.time << endl;
//			cout << startTime << endl;
//			cout << endTime << endl;
			sd1.time.setTimeSystem(TimeSystem::Any);

			// Only access data at the given time span
			if( sd1.time < startTime ||
				sd1.time > endTime )
			{
				continue;
			}

//			cout << "aa"<< sd1.time << endl;
//			cout << "sat:" << sd1.sat << endl;
//			cout << "xyz:" << sd1.x[0] << endl;
			 
			// Only store the satellite position
			if( (sd1.sat).isValid() )
			{
				currTime = sd1.time;
//				cout << "currTime" << currTime << endl;
				
				if(currTime > lastTime)
				{
//					cout << "ab" << endl;
					if(!firstEpoch)
					{
						ephMap1[lastTime] = stvMap;
						stvMap.clear();
					}
					else
					{
						firstEpoch = false;
					}
				}
				
				// @para 'sd1.x[]' : The position/velocity of satellite
				//			   in the directions of XYZ
				tvMap[TypeID::satX] = sd1.x[0];
				tvMap[TypeID::satY] = sd1.x[1];
				tvMap[TypeID::satZ] = sd1.x[2];
				stvMap[sd1.sat] = tvMap;
				lastTime = currTime;
//				cout << "ac" << lastTime << endl;
			}
		}

//		cout << "bb"<< lastTime <<endl;
		
		// The last epoch
		ephMap1[lastTime] = stvMap;

		// Read data from file2
		satTypeValueMap stvMap2;
		typeValueMap tvMap2;
      	CommonTime lastTime2, currTime2;
     	bool firstEpoch2(true);
      	while (ss2>>sd2)
      	{
            sd2.time.setTimeSystem(TimeSystem::Any);
			// only access data at the given time span
        	if( sd2.time < startTime ||
            	sd2.time > endTime )
         	{
            	continue;
         	}

            // Only store the satellite clock
         	if( (sd2.sat).isValid() )
         	{
            	currTime2 = sd2.time;
            	if(currTime2 > lastTime2)
            	{
               		if(!firstEpoch2)
               		{
                  		ephMap2[lastTime2] = stvMap2;
                  		stvMap2.clear();
               		}	
               		else
               		{
                  		firstEpoch2 = false;
               		}	
            	}
				
				// @para 'sd2.x[]' : The position/velocity of satellite
                //             in the directions of XYZ
				tvMap2[TypeID::satX] = sd2.x[0];
				tvMap2[TypeID::satY] = sd2.x[1];
				tvMap2[TypeID::satZ] = sd2.x[2];

            	stvMap2[sd2.sat] = tvMap2;
            	lastTime2 = currTime2;
         	}	
      	}

         // The last epoch
      	ephMap2[lastTime2] = stvMap2;
		
		// Statistical information are given 
		epochSatTypeValueMap ephDiffMap;
	
		std::map< SP3SatID, Stats<double> > satXMap;
		std::map< SP3SatID, Stats<double> > satYMap;
		std::map< SP3SatID, Stats<double> > satZMap;
		//Stats<double> allStats;
		
		Stats<double> allSatXStats;
		Stats<double> allSatYStats;
		Stats<double> allSatZStats;

		// Loop the ephMap1
		for( epochSatTypeValueMap::const_iterator emIter = ephMap1.begin();
		 	 emIter != ephMap1.end();
		 	 ++emIter )
		{
			// Current time
			CommonTime currTime( (*emIter).first );
		
			// Find currTime of ephMap1 in "ephMap2"
			epochSatTypeValueMap::iterator emIter2 = ephMap2.find(currTime);
	
			// If found
			if( emIter2 != ephMap2.end() )
			{	
				satTypeValueMap tempStvMap;

				// Enter the corresponding satValueMap at currTime
				for( satTypeValueMap::const_iterator stvIter 
					 = (*emIter).second.begin();
					 stvIter != (*emIter).second.end();
				 	 ++stvIter )
				{
					// Current satellite in "ephMap1"
					SP3SatID currSat( (*stvIter).first );
					// Find the current satellite in "ephMap2"
					satTypeValueMap::const_iterator 
					stvIter2( (*emIter2).second.find(currSat) );
					
					// If found
					if( stvIter2 != (*emIter2).second.end() )
					{	
						typeValueMap tempTvMap;

						// @para dSatX/Y/Z : the difference between the same 
						//					 satellite's position in two files 
						//					 in X/Y/Z direction.
						double dSatX, dSatY, dSatZ;
						dSatX = (*stvIter).second.getValue(TypeID::satX) - 
								(*stvIter2).second.getValue(TypeID::satX);
						dSatY = (*stvIter).second.getValue(TypeID::satY) -
								(*stvIter2).second.getValue(TypeID::satY);
						dSatZ = (*stvIter).second.getValue(TypeID::satZ) - 
								(*stvIter2).second.getValue(TypeID::satZ);
						
						// Change the unit for km to mm
						dSatX = dSatX*1e6;
						dSatY = dSatY*1e6;
						dSatZ = dSatZ*1e6;
						
						// Store the differences in a typeValueMap
						tempTvMap[TypeID::satX] = dSatX;
						tempTvMap[TypeID::satY] = dSatY;
						tempTvMap[TypeID::satZ] = dSatZ;
						
						//cout << "a" << " " << dSatX;
						
						if( std::abs(dSatX) < threshold &&
							std::abs(dSatY) < threshold &&
							std::abs(dSatZ) < threshold	)
						{
							// Insert the Stats class
							satXMap[currSat].Add(dSatX);
							satYMap[currSat].Add(dSatY);
							satZMap[currSat].Add(dSatZ);
							
							allSatXStats.Add(dSatX);
							allSatYStats.Add(dSatY);
							allSatZStats.Add(dSatZ);
						}
					
						// Insert dSP[3] into "tempSvMap"
						tempStvMap[currSat] = tempTvMap;
										
					} // End of 'if(svIter2)'

				} // End of 'for(satValueMap)'
			
				// Insert data into "ephDiffMap"
				ephDiffMap[currTime] = tempStvMap;

			} // End of 'if(emIter2 != ...)'

		} // End of for



		if(verboseLevel)
		{
			cout << "# Orbit difference for every satellite " << endl;
            cout << "# ";
			if( sh1.satList.size() < sh2.satList.size() || 
			 	sh1.satList.size() == sh2.satList.size() )
			{
				for( std::map<SP3SatID, short>::iterator sIter
                 = sh1.satList.begin();
                 sIter != sh1.satList.end();
                 ++sIter )
            	{
              		cout << (*sIter).first << ' ';
            	}
			}
			else
			{
				for( std::map<SP3SatID, short>::iterator sIter
                 	 = sh2.satList.begin();
                	 sIter != sh2.satList.end();
                	 ++sIter )
            	{
                	cout << (*sIter).first << ' ';
            	}	
			}

			cout << endl;

/*		Modified. 2015/10/15		*/
/*			for( epochSatTypeValueMap::iterator isat = ephDiffMap.begin();
**				 isat != ephDiffMap.end();
**				 ++isat )
**			epochSatTypeValueMap::iterator isat = ephDiffMap.begin();
**			
**				for( satTypeValueMap::iterator sat = isat.second.begin();
**					 sat != isat.second.end();
**					 ++sat )
**				{ 
**					cout << (*sat).first << ' ' ;
**				}
*/			
		
			for( epochSatTypeValueMap::const_iterator emIter 
				 = ephDiffMap.begin();
				 emIter != ephDiffMap.end();
				 ++emIter )
			{
				// Current time
				CommonTime currTime( (*emIter).first );
			
               // Convert to 'yds'
            YDSTime yds( static_cast<YDSTime>(currTime) );

            cout << setw(4)  << yds.year << " "
                 << setw(3)  << yds.doy  << " "
                 << setw(9) << fixed << setprecision(3) << yds.sod  << " ";
			
				for( satTypeValueMap::const_iterator stvIter 
							= (*emIter).second.begin();
					 stvIter != (*emIter).second.end();
					 ++stvIter )
				{
					SP3SatID currSat( (*stvIter).first );
				
					for( typeValueMap::const_iterator tvIter 
									= (*stvIter).second.begin();
						 tvIter != (*stvIter).second.end();
						 ++tvIter )
					{
						cout << fixed << setw(3) << setprecision(0) << (*tvIter).second << " " ;
					}
			
				} // End of 'for( satTypeValueMap... )'

				cout << endl;
			} // End of 'for( epochSatTypeValueMap... )'

		} // End of 'if(verboseLevel)'
		

		cout << "# statistics for each satellite " << endl;
		cout << "# PRN    mean_X    std_X      mean_Y     std_Y     mean_Z      std_Z (unit:mm)" << endl;
		cout.setf(ios::right);

		// Print out the statistics information
		for( std::map< SP3SatID, Stats<double> >::iterator xIter 
			 = satXMap.begin();
			 xIter != satXMap.end();
			 ++xIter )
		{
			SP3SatID currSat( (*xIter).first );
			double meanX = (*xIter).second.Average() ;
			double stdX = (*xIter).second.StdDev() ;
			
			double meanY, meanZ, stdY, stdZ;		

	
			std::map< SP3SatID, Stats<double> >::iterator yIter 
				= satYMap.find(currSat);
			if( yIter != satYMap.end() )
			{
				meanY = (*yIter).second.Average();
				stdY = (*yIter).second.StdDev();
				/** Why "double meanY/stdY = ..." 
				*** turns out error message "meanY/stdY not declared"?
				**/
			}
			
			std::map< SP3SatID, Stats<double> >::iterator zIter 
				= satZMap.find(currSat);
			if( zIter != satZMap.end() )
			{
				meanZ = (*zIter).second.Average();
				stdZ = (*zIter).second.StdDev();
			}
			
			cout << currSat 
              << setw(6) << meanX << " " << setw(6) << stdX << " "
				  << setw(6) << meanY << " " << setw(6) << stdY << " "
				  << setw(6) << meanZ << " " << setw(6) << stdZ << endl;


		} // end of 'for(std::map)'
		
		cout << "# statistics for all satellite " << endl;
		double meanXAll = allSatXStats.Average(); 
		double meanYAll = allSatYStats.Average();
		double meanZAll = allSatZStats.Average();

		double stdXAll = allSatXStats.StdDev();
		double stdYAll = allSatYStats.StdDev();
		double stdZAll = allSatZStats.StdDev();
		
		cout << "ALL" << fixed << setprecision(1) << " "
			 << setw(6) << meanXAll << " " << setw(6) << stdXAll << " "
			 << setw(6) << meanYAll << " " << setw(6) << stdYAll << " "
          << setw(6) << meanZAll << " " << setw(6) << stdZAll << endl;

	} // end of try(...)
	
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


int main(int argc, char* argv[])
{
	try
  	{
    	sp3diff m(argv[0]);
      	if (!m.initialize(argc, argv))
        	return 0;
      	if (!m.run())
        	return 1;

      	return 0;
    }
    catch(Exception& e)
    {
       cout << e << endl;
    }
    catch(exception& e)
    {
       cout << e.what() << endl;
    }
    catch(...)
    {
       cout << "unknown error" << endl;
    }
    return 1;
}








