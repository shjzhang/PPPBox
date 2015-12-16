// Filename : rnxdiff.cpp
//============================================================================
//
// This file is part of GPSTk, the GPS Toolkit.
//
// The GPSTk is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published
// by the Free Software Foundation; either version 2.1 of the License, or
// any later version.
//
// The GPSTk is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with GPSTk; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
// Copyright 2004
//
// Keming Zhu, Wuhan University, 2015
//
//============================================================================
// 
// Revision
//
// 2015/11/20 
// Create this program
//
// 2015/11/25 
// modify the output format, by shjzhang
//
//============================================================================

#include "RinexObsBase.hpp"
#include "RinexObsStream.hpp"
#include "RinexObsHeader.hpp"
#include "RinexObsData.hpp"
#include "RinexSatID.hpp"
#include "TypeID.hpp"
#include "DataStructures.hpp"
#include "DiffFrame.hpp"

#include <map>
#include <iostream>
#include <fstream>

using namespace std;
using namespace gpstk;
using namespace StringUtils;

class rnxdiff : public DiffFrame
{
public:
	rnxdiff(char* arg0)
		: DiffFrame(arg0, std::string("Rinex Observation "))
	{}


protected:
	virtual void process();
	
};

void rnxdiff::process()
{
	try
	{
		std::string fileName1 = inputFileOption.getValue()[0];
		std::string fileName2 = inputFileOption.getValue()[1];		
	///cout << fileName1 << ' ' << fileName2 << endl;

         // Define a RinexObsStream and RinexObsHeader for two files
		RinexObsStream rin( fileName1.c_str(), std::ios::in);
		RinexObsHeader roh;
		rin.exceptions(ios::failbit);

      RinexObsStream rin2( fileName2.c_str(), std::ios::in);
		RinexObsHeader roh2;
		rin2.exceptions(ios::failbit);


         // Make sure we have opened the file.
      if ( !rin.is_open() )
      {
         cerr << "The file " << fileName1 << " doesn't exist!"
               << endl
               << "Please check your rinex file!" << endl;
         exit(-1);
      }
         // Read the first file.
		try
		{
		   rin >> roh;
		}
		catch(FFStreamError& e)
		{
			cerr << "Cannot read!" << endl;
			rin.close();
			exit(-1);
		}
      
         // The second file
      if ( !rin2.is_open() )
      {
         cerr << "The file " << fileName2 << " doesn't exist!"
               << endl
               << "Please check your rinex file!" << endl;
         exit(-1);
      }
      
		try
		{
		   rin2 >> roh2;
		}
		catch(FFStreamError& e)
		{
			cerr << "Cannot read!" << endl;
			rin2.close();
			exit(1);
		}
	      
         // Make sure the two files are at the same station.
         // If not, throw an exception.

    //cout << roh.markerName << ' ' << roh2.markerName << endl;

      if (roh.markerName != roh2.markerName)
      {
         cerr << "We cannot compare two rinex files with different station!"
            << endl;
         exit(1);
      }


		   // Firslty, read the data from the first rinex file 
      gnssRinex gRin; 
		epochSatTypeValueMap epochDataMap;
		while(rin >> gRin)
		{
			epochDataMap[gRin.header.epoch] = gRin.body;
		} 
      // Finsh reading the first rinex file.

	   	// Now we read the second file

         // Make sure we have opened the file.
      
      gnssRinex gRin2; 
		epochSatTypeValueMap epochDataMap2;
		while(rin2 >> gRin2)
		{
			epochDataMap2[gRin2.header.epoch] = gRin2.body;
		} 
      // Finsh reading the second rinex file.
      
      // Check whether data is read in the epochSatTypeValueMap if needed.
      
		
		   // Compute the difference between two rinex files
     
      epochSatTypeValueMap rnxdiffMap;
      for(epochSatTypeValueMap::iterator estvIter = epochDataMap.begin();
            estvIter != epochDataMap.end();
            ++estvIter)
      {
            // Current time
         CommonTime currTime( (*estvIter).first );

            // Find current time of epochDataMap in epochDataMap2
         epochSatTypeValueMap::iterator estvIter2 
                                          = epochDataMap2.find(currTime);
            // If found
         if( estvIter2 != epochDataMap2.end() )
         {
            satTypeValueMap tempStvMap;

               // Enter the corresponding satTypeValueMap at currTime
            for( satTypeValueMap::const_iterator stvIter 
                  = (*estvIter).second.begin();
                 stvIter != (*estvIter).second.end();
                 ++stvIter)
            {
                  // Current satellite in "epochDataMap"
               RinexSatID currSat( (*stvIter).first );
               satTypeValueMap::const_iterator 
                  stvIter2( (*estvIter2).second.find(currSat) );
               if( stvIter2 != (*estvIter2).second.end() )
               {
                  typeValueMap tempTvMap;
                  for(typeValueMap::const_iterator tvIter 
                      = (*stvIter).second.begin();
                      tvIter != (*stvIter).second.end();
                      ++tvIter)
                  {
                        // Current TypeID in "epochDataMap"
                     TypeID currType( (*tvIter).first );
                     typeValueMap::const_iterator 
                        tvIter2((*stvIter2).second.find(currType));
                        // If found
                     if( tvIter2 != (*stvIter2).second.end() )
                     {
                        tempTvMap[currType] = (*tvIter).second - 
                           (*tvIter2).second;
                     }
                        // If not found, throw an error
                     else
                     {
                        cerr << "The second Rinex file has no " << currType 
                           << " data" << endl;
                     }

                  } // End of 'for typeValueMap'

                  tempStvMap[currSat] = tempTvMap;
               }
                  // If not found the current satellite, throw an exception
               else
               {
                  cerr << "The second Rinex file has no " 
                     << currSat << " data" << endl;
               }
            }
            
            rnxdiffMap[currTime] = tempStvMap;
         }
         else
         {
            cerr << "The second Rinex file has no data at epoch"
               << currTime << endl;
         }
      } // End of 'for(epochSatTypeValueMap::iterator)'
     


         // Print the output
      ofstream out;
      out.open("rnxdiff.txt", ios::out );
      if (!out.is_open()) 
      {
         cerr << "Output file cannot open!" << endl;
         exit(-1);
      }
      else 
      {
            // Loop the rnxdiffMap to take out the values
         for (epochSatTypeValueMap::iterator rdIter = rnxdiffMap.begin();
              rdIter != rnxdiffMap.end();
              ++rdIter)
         {
            CommonTime time( (*rdIter).first );
               // Convert common time to CivilTime to print the epoch
            CivilTime cvt;
            cvt.convertFromCommonTime(time);

            out << cvt.year << ' '
                << cvt.month << ' '
                << cvt.day << ' '
                << setfill(' ') << cvt.hour << ' '
                << setfill(' ') << cvt.minute << ' '
                << setfill(' ') << int(cvt.second) << ' ' 
                << endl;;


            out.clear();

            for(satTypeValueMap::iterator stvdIter
                = (*rdIter).second.begin();
                stvdIter != (*rdIter).second.end();
                ++stvdIter)
            {
               RinexSatID sat( (*stvdIter).first );

                  // If we want a specific type of data
               if (typeOption.getCount() != 0)
               {
                     TypeID type;
                     type = TypeID(ConvertToTypeID(typeOption.getValue()[0]));
                     if( !type.isValid() )
                     {
                        cerr << "No such type of data in RINEX files!" 
                           << endl;
                        exit(-1);
                     }
                     else
                     {
                        double value((*stvdIter).second.getValue(type));
                        out << sat << ' ' 
                        << setiosflags(ios::fixed)
                        << setprecision(3) << value << ' ';
                     }

               } // end of 'if (typeOption.getCount())'

               else
               {
                     // If we need a filter to reject data that equals 0.
                  if (filterOption.getCount() != 0)
                  {
                        // Take out the data.
                     for(typeValueMap::iterator tvdIter 
                         = (*stvdIter).second.begin();
                         tvdIter != (*stvdIter).second.end();
                         ++tvdIter)
                     {
                        if ((*tvdIter).second != 0)
                        {
                           out 
                           << sat << ' '
                           << setfill(' ') << (*tvdIter).first << ' '
                           << setfill(' ') << setprecision(3)
                           << setiosflags(ios::fixed) << (*tvdIter).second
                           << ' ';
                        } // end of if
                        
                     } // end of for()
                        
                  } // end of if (filterOption.getCount())

                     // We don't need the filter.
                  else
                  {
                     out << sat << ' ';

                     // Take out the data.
                     for(typeValueMap::iterator tvdIter 
                         = (*stvdIter).second.begin();
                         tvdIter != (*stvdIter).second.end();
                         ++tvdIter)
                     {
                        out
                        << setfill(' ') << (*tvdIter).first << ' '
                        << setfill(' ') << setprecision(3)
                        << setiosflags(ios::fixed) << (*tvdIter).second
                        << ' ';
                     } // end of for()
                
                  } //  end of else

               } // End of 'if(typeOption...)'

                  // new line
               out << endl;

            } // end of 'for(satTypeValueMap)'

         } // end of 'for(rnxdiffMap::iterator)'

      }  // end of 'if( !out.is_open())'

      cout << "File 'rnxdiff.txt' is generated!" << endl;

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
		cout << "Uknown exception... terminating..." << endl;
	}
} // end of 'void rnxdiff::process()'


int main(int argc, char* argv[])
{
	try
	{
		rnxdiff m(argv[0]);
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




