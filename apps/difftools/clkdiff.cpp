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
//  Copyright 2004
//
//  Shoujian Zhang, Wuhan Uniersity, 2015 
//
//  Revision
//
//  2010/05/01       create this routine for clock product statistics computation
//  2015/10/06       re-desgin the whole class for flexible usage and more 
//                   information
//  2015/10/08       increase the verbose level
//
//============================================================================


#include "RinexClockData.hpp"
#include "RinexClockHeader.hpp"
#include "RinexClockStream.hpp"
#include "RinexClockFilterOperators.hpp"
#include "RinexSatID.hpp"
#include "DataStructures.hpp"

#include "DiffFrame.hpp"
#include "YDSTime.hpp"
#include "Stats.hpp"

using namespace std;
using namespace gpstk;
using namespace StringUtils;

class clkdiff : public DiffFrame
{
public:
   clkdiff(char* arg0)
      : DiffFrame(arg0, std::string("RINEX clock")),
        refSatOption('r', "SatID", "Reference satellite to" 
                                   "compute the clock difference", true ),
        thresholdOption('x', "Threshold", "threshold to exclude the clk difference")
   {}

protected:
   virtual void process();

   gpstk::CommandOptionWithAnyArg refSatOption;
   gpstk::CommandOptionWithAnyArg thresholdOption;

};

void clkdiff::process()
{
   try
   {
      
         // Get reference satellite from command line
      RinexSatID refSat(refSatOption.getValue().front());

         // default = 10
      double threshold(10);
      if(thresholdOption.getCount())
      {
         threshold = StringUtils::asInt(thresholdOption.getValue().front());
      }

      RinexClockHeader header1, header2;
      RinexClockStream rcs1( inputFileOption.getValue()[0] ), 
                       rcs2( inputFileOption.getValue()[1] );

      rcs1 >> header1;
      rcs2 >> header2;

      if (header1.satList.size() != header2.satList.size())
      {
         cout << "The two files have a different number of satellite." << endl;
         cout << "The first file has ";
         vector<RinexSatID> types1 = header1.satList;
         vector<RinexSatID>::iterator i = types1.begin();
         while (i != types1.end())
	      {
            cout << (*i) << ' ';
            i++;
	      }
         cout << endl;

         cout << "The second file has ";
         vector<RinexSatID> types2 = header2.satList;
         vector<RinexSatID>::iterator j = types2.begin();
         while (j != types2.end())
	      {
            cout << (*j) << ' ';
            j++;
	      }
         cout << endl;
      }

      std::vector<RinexSatID> satList;
      set_intersection(header1.satList.begin(), header1.satList.end(),
                       header2.satList.begin(), header2.satList.end(), 
                       inserter(satList, satList.begin()));



         // Now, let's store the two files' clock data into map
         // which is more efficient for comparing
      epochSatValueMap clkMap1, clkMap2;
         
      //
      // Now, read clock data from two clock file streams
      //
         // Rinex clock data 
      RinexClockData data1, data2;

         // Read clock data
      satValueMap svMap;
      CommonTime lastTime, currTime;
      bool firstEpoch(true);
      while(rcs1>>data1)
      {
            // only access data at the given time span!
         if( data1.time < startTime || 
             data1.time > endTime )
         {
            continue;
         }

            // Only store the satellite clock
         if( (data1.sat).isValid() )
         {
            currTime = data1.time;

            if(currTime > lastTime)
            {
               if(!firstEpoch)
               {
                  clkMap1[lastTime] = svMap;
                  svMap.clear();
               }
               else
               {
                  firstEpoch = false;
               }
            }

            svMap[data1.sat] = data1.bias;
            lastTime = currTime;
         }
      }

         // The last epoch
      clkMap1[lastTime] = svMap;

         // Read data from file2
      satValueMap svMap2;
      CommonTime lastTime2, currTime2;
      bool firstEpoch2(true);
      while (rcs2>>data2)
      {
            // only access data at the given time span!
         if( data2.time < startTime || 
             data2.time > endTime )
         {
            continue;
         }

            // Only store the satellite clock
         if( (data2.sat).isValid() )
         {
            currTime2 = data2.time;
            if(currTime2 > lastTime2)
            {
               if(!firstEpoch2)
               {
                  clkMap2[lastTime2] = svMap2;
                  svMap2.clear();
               }
               else
               {
                  firstEpoch2 = false;
               }
            }
            svMap2[data2.sat] = data2.bias;
            lastTime2 = currTime2;
         }
      }

         // The last epoch
      clkMap2[lastTime2] = svMap2;


         // Now, let's give the statistical information
      epochSatValueMap clkDiffMap; 
      std::map< RinexSatID, Stats<double> > satStatsMap; 
      Stats<double> allStats;

      double refClk1, refClk2;

         // Now, Let's loop the clkMap1
      for( epochSatValueMap::const_iterator cmIter=clkMap1.begin();
           cmIter != clkMap1.end();
           ++cmIter )
      {
            // current time
         CommonTime currTime( (*cmIter).first );

            // Find currTime of clkMap1 in 'clkMap2'
         epochSatValueMap::iterator cmIter2 = clkMap2.find(currTime);

            // If found
         if( cmIter2 != clkMap2.end() )
         {

            satValueMap tempSvMap;

               // Initialize the 'tempSvMap'
            for(vector<RinexSatID>::iterator itSat = satList.begin();
                itSat != satList.end();
                ++itSat)
            {
               tempSvMap[(*itSat)] = 0.0;
            }

               // Firstly, Let's find the reference satellite from both file data
              
               // Find current satellite in 'clkMap2' 
            satValueMap::const_iterator refIter1 = (*cmIter).second.find( refSat ) ;
            satValueMap::const_iterator refIter2 = (*cmIter2).second.find( refSat ) ;

            if( refIter1 != (*cmIter).second.end() && 
                refIter2 != (*cmIter2).second.end() )
            {
               refClk1 = (*refIter1).second;
               refClk2 = (*refIter2).second;
            }
            else
            {
               cerr << "Reference satellite can't be found in clock files." 
                    << "at time: " << currTime << endl;

               exit(1);
            }
            

            for( satValueMap::const_iterator svIter = (*cmIter).second.begin();
                 svIter != (*cmIter).second.end();
                 ++svIter )
            {
                  // curr satellite in 'clkMap1'
               RinexSatID currSat( (*svIter).first );

                  // jump the reference satellite
               if(currSat != refSat)
               {
                     // Find current satellite in 'clkMap2' 
                  satValueMap::const_iterator svIter2( (*cmIter2).second.find( currSat) );
                  if( svIter2 != (*cmIter2).second.end() )
                  {
                      double currClk1, currClk2;
                      currClk1 = (*svIter).second;
                      currClk2 = (*svIter2).second;

                      double ddClk;
                      ddClk = ( currClk1 - currClk2 ) - ( refClk1 - refClk2 );

                         // Change the unit from second to nano-second
                      ddClk = ddClk*1.0e9;


                      if(std::abs(ddClk)<threshold)
                      {
                            // Insert the Stats class
                         satStatsMap[currSat].Add(ddClk); 
                         allStats.Add(ddClk);
                      }

                         // Insert ddclk into 'clkDiffMap'
                      tempSvMap[currSat] = ddClk;

                  }

               } // end of 'if(currSat)'

            } // End of 'for(satValueMap)' 

               // Insert data
            clkDiffMap[currTime] = tempSvMap;

         }  // End of 'if'
      }


      if(verboseLevel)
      {
            // Firstly, print out the header 
         vector<RinexSatID>::iterator isat = satList.begin();
         cout << "# Clock difference for every satellite " << endl;
         cout << "# Reference satellite is : " << refSat << endl;
         cout << "# ";
         while (isat != satList.end())
	      {
            cout << (*isat) << ' ';
            isat++;
	      }
         cout << endl;

         for( epochSatValueMap::const_iterator cmIter=clkDiffMap.begin();
              cmIter != clkDiffMap.end();
              ++cmIter )
         {
               // current time
            CommonTime currTime( (*cmIter).first );

               // Convert to 'yds'
            YDSTime yds( static_cast<YDSTime>(currTime) );

            cout << setw(4)  << yds.year << " "
                 << setw(3)  << yds.doy  << " "
                 << setw(9) << fixed << setprecision(3) << yds.sod  << " ";

            for( satValueMap::const_iterator svIter = (*cmIter).second.begin();
                 svIter != (*cmIter).second.end();
                 ++svIter )
            {
                  // curr satellite in 'clkMap1'
               RinexSatID currSat( (*svIter).first );

               cout << setw(6) << fixed << setprecision(2) << (*svIter).second << " " ; 

            }

            cout << endl;
         }

      }


      cout << "# statistics for each satellite " << endl;
      cout << "# PRN     mean       std (unit:ns)" << endl;
      cout.setf(ios::right);  

         // Now, Let's print out the statistics information
      for( std::map< RinexSatID, Stats<double> >::iterator itsat=satStatsMap.begin();
           itsat !=satStatsMap.end();
           ++itsat ) 
      {
          double mean = (*itsat).second.Average() ;
          double std = (*itsat).second.StdDev() ;

          cout << (*itsat).first << fixed << setprecision(3) 
               << " "<< setw(10) << mean << " " << setw(10) << std << endl;
      }

      cout << "# statistics for all satellite " << endl;
      double mean = allStats.Average() ;
      double std = allStats.StdDev() ;
      cout << "ALL" << fixed << setprecision(3) << " "
           << setw(10) << mean << " " << setw(10)  << std << endl;

   }
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
      clkdiff m(argv[0]);
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
