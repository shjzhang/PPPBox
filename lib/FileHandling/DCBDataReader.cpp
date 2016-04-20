#pragma ident "$Id$"

/**
 * @file DCBDataReader.cpp
 * Class to read DCB data from CODE.
 */

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
//  Shoujian Zhang, 2015.
//
//============================================================================
//
//  Revision
//
//  2015/12/09   Throw error if the input file does not exist!
//  2016/04/18   Throw the exception when DCB file doesn't exist, but not exit
//               the program.(Q.Liu)
//============================================================================


#include "DCBDataReader.hpp"

using namespace std;

namespace gpstk
{
   
      // Method to store load ocean tide harmonics data in this class'
      // data map
   void DCBDataReader::loadData()
      throw( FFStreamError, gpstk::StringUtils::StringException )
   {

      try
      {
         allDCB.satDCB.clear();
         allDCB.gpsDCB.clear();
         allDCB.glonassDCB.clear();

            // a buffer
         string line;
         
            // read first line 
         formattedGetLine(line, true);

            // Let's skip 6 lines
         for(int i=0; i<6; i++) formattedGetLine(line, true);
         
         
            // Now, let's read data
         while(1)
         {
            formattedGetLine(line, true);

            if(line.length() < 46) continue;
            
            string sysFlag = line.substr(0,1);
            
            int satPRN = StringUtils::asInt(line.substr(1,2));
            
            string station = StringUtils::strip(line.substr(6,4));
            
            double dcbVal = StringUtils::asDouble(line.substr(26,9));      
            double dcbRMS = StringUtils::asDouble(line.substr(38,9));

#pragma unused(dcbRMS)
             
            if(station.length() < 4)       // this is satellite DCB data
            {

               SatID sat;
               if(sysFlag == "G")
               {
                  sat = SatID(satPRN,SatID::systemGPS);
               }
               else if(sysFlag == "R")
               {
                  sat = SatID(satPRN,SatID::systemGlonass);
               }
               else
               {
                  // Unexpected and we do nothing here

               }
               
               allDCB.satDCB[sat] = dcbVal;

            }
            else                           // this is receiver DCB data
            {
               if(sysFlag == "G")
               {
                  allDCB.gpsDCB[station] = dcbVal;
               }
               else if(sysFlag == "R")
               {
                  allDCB.glonassDCB[station] = dcbVal;
               }
               else
               {
                  // Unexpected and we do nothing here

               }
            }

         }  // End of 'while(1)'

      }  // End of try block
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


   }  // End of 'DCBDataReader::loadData()'



      // Method to open AND load DCB data file. 
   void DCBDataReader::open(const char* fn)
		throw( FileMissingException )
   {

      // We need to be sure current data stream is closed
      (*this).close();

		try
		{
         // Open data stream
         FFTextStream::open(fn, std::ios::in);
         if( !FFTextStream::is_open() )
         {
            FileMissingException fm("The DCB file " + std::string(fn) + 
					                     " doesn't exist!");
            GPSTK_THROW(fm);
         }
		   loadData();
		}
      catch(FileMissingException& e)
      {
         GPSTK_RETHROW(e);
      }

      return;

   }  // End of method 'DCBDataReader::open()'



      // Method to open AND load DCB data file. It doesn't
      // clear data previously loaded.
   void DCBDataReader::open(const string& fn)
		throw( FileMissingException )
   {

      // We need to be sure current data stream is closed
      (*this).close();

		try
		{
           // Open data stream
         FFTextStream::open(fn.c_str(), std::ios::in);
         if( !FFTextStream::is_open() )
         {
            FileMissingException fm("The DCB file " + fn +
							               " doesn't exist!");
            GPSTK_THROW(fm);
         }
		   loadData();
		}
      catch(FileMissingException& e)
      {
         GPSTK_RETHROW(e);
      }

      return;
   }  // End of method 'DCBDataReader::open()'

      // return P1-P2 or P1-C1 depend what you have loaded
   double DCBDataReader::getDCB(const SatID& sat)
   {
      std::map<SatID,double>::iterator it=allDCB.satDCB.find(sat);
      if(it!=allDCB.satDCB.end())
      {
         return allDCB.satDCB[sat];     
      }
		else 
		{ 
			return 0.0;
		}
   }

      // Get DCB data of a satellite
      // return P1-P2 or P1-C1 depend what you have loaded
   double DCBDataReader::getDCB(const int& prn,
      const SatID::SatelliteSystem& system)
   {
      SatID sat(prn,system);
      std::map<SatID,double>::iterator it=allDCB.satDCB.find(sat);
      if(it!=allDCB.satDCB.end())
      {
         return allDCB.satDCB[sat];     
      }
		else 
		{ 
			return 0.0;
		}
   }

      // Get DCB data of aReceiver
      // it return P1-P2 
   double DCBDataReader::getDCB(const string& station,
      const SatID::SatelliteSystem& system)
   {

      if(system == SatID::systemGPS)
      {
         return allDCB.gpsDCB[station];
      }
      else if(system == SatID::systemGlonass)
      {
         return allDCB.glonassDCB[station];
      }
      else 
      {
            // Unexpected and return 0
         return 0.0;
      }

   }  // End of 'double DCBDataReader::getDCB(const string& station...'



}  // End of namespace gpstk

