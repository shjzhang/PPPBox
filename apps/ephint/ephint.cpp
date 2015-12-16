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
//  Copyright 
//  =========
//
//  Shoujian Zhgang, Wuhan University, 2015.8.10
//
//============================================================================

// System
#include <stdio.h>
#include <iostream>

// Library
#include "BasicFramework.hpp"
#include "YDSTime.hpp"
#include "CommonTime.hpp"
#include "GNSSconstants.hpp"
// Class to store satellite precise navigation data
#include "SP3EphemerisStore.hpp"
#include "AntexReader.hpp"
#include "ComputeSatPCOffset.hpp"
#include "Triple.hpp"
#include "Xvt.hpp"
#include "SatID.hpp"

// Project

using namespace std;
using namespace gpstk;


class ephint : public gpstk::BasicFramework
{
public:

   ephint(const std::string& applName,
          const std::string& applDesc) 
      throw();

   ~ephint() 
   {}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Woverloaded-virtual"
   virtual bool initialize(int argc, char *argv[]) 
      throw();
#pragma clang diagnostic pop

protected:

   virtual void process();

      // required
   gpstk::CommandOptionWithAnyArg yearOption;
   gpstk::CommandOptionWithAnyArg DOYOption;
   gpstk::CommandOptionWithAnyArg IntvOption;
   gpstk::CommandOptionWithAnyArg ephFileOption;
   gpstk::CommandOptionWithAnyArg clkFileOption;
   gpstk::CommandOptionWithAnyArg atxFileOption;

      // optional
   gpstk::CommandOptionWithAnyArg prnOption;

};


#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreorder"
ephint::ephint(const std::string& applName,
               const std::string& applDesc) 
   throw()
   :BasicFramework(applName, applDesc),
    yearOption('y', "year", "Year of interest.", true),
    DOYOption('j',"doy","Day of year.", true),
    IntvOption('i',"interval", "interpolation interval", true),
    ephFileOption('e', "eph","input sp3 file name",true),
    clkFileOption('c', "clk","input clk file name",true),
    atxFileOption('x', "atx","input antex file name",true),
    prnOption('s', "PRN","satellite PRN number to be listed",false)
{
   yearOption.setMaxCount(1);
   DOYOption.setMaxCount(1);
}
#pragma clang diagnostic pop

  //*********
  //  
  //  initialize
  //
  //*********

bool ephint::initialize(int argc, char *argv[])
   throw()
{
   if (!BasicFramework::initialize(argc, argv)) 
      return false;
   return true;
}


void ephint::process()
{
      // Get day of interest from command line arguments
      // Arrange so this'll work with either 2-digit or 4-digit year numbers
   int year = StringUtils::asInt( yearOption.getValue().front() );
   if (year>=0 && year<=70) year += 2000;
   if (year<100) year += 1900;

   int DOY = StringUtils::asInt( DOYOption.getValue().front() );
   int interval = StringUtils::asInt( IntvOption.getValue().front() );

      /***
       * Now, Let's read eph file
       */

      // Declare a "SP3EphemerisStore" object to handle precise ephemeris
   SP3EphemerisStore ephData;
   ephData.rejectBadPositions(true);
   ephData.rejectBadClocks(true);

   for (size_t i=0; i < ephFileOption.getCount(); i++)
   {
      string sp3File( ephFileOption.getValue()[i] );
      try
      {
         ephData.loadFile( sp3File );
      }
      catch (...)
      {
            // If file doesn't exist, issue a warning
         cerr << "SP3 file '" << sp3File << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         exit(-1);
      }
   }

      /***
       * Now, Let's read clk file
       */
   for (size_t i=0; i < clkFileOption.getCount(); i++)
   {
      string clkFile( clkFileOption.getValue()[i] );
      try
      {
         ephData.loadRinexClockFile(clkFile);
      }
      catch (...)
      {
            // If file doesn't exist, issue a warning
         cerr << "clk file '" << clkFile << "' doesn't exist or you don't "
              << "have permission to read it. Skipping it." << endl;

         exit(-1);
      }
   }

   if (ephData.size() == 0 )
   {
      cout << "Didn't get any ephemeris data from the eph files. "
           << "Exiting." << endl;
      exit(-1);
   }

      /***
       * Now, Let's read antex file
       */
   AntexReader antexReader;
   string atxFile = atxFileOption.getValue()[0];

   antexReader.open( atxFile );

   ComputeSatPCOffset svPcenter;
   svPcenter.setAntexReader(antexReader);
      
      // Now, Let's read the atx file 
   double sec(0.0);
   while(sec<SEC_PER_DAY)
   {
  
      YDSTime time( year, DOY, sec, TimeSystem::GPS);
      CommonTime epoch( time.convertToCommonTime() );
     
         // Now, let's insert satellite into gRin;
      for(int PRN=1;PRN<=32;PRN++)
      {
         SatID sat(PRN, SatID::systemGPS);
  
         try
         {
            Xvt svPosVel;
            svPosVel = ephData.getXvt(sat, epoch );
  
            Triple svPos(0.0, 0.0, 0.0);
            svPos[0] = svPosVel.x.theArray[0];
            svPos[1] = svPosVel.x.theArray[1];
            svPos[2] = svPosVel.x.theArray[2];
  
            SunPosition sunPosition;
            Triple sunPos(sunPosition.getPosition(epoch));
  
            Triple svAtx = svPcenter.getSatPCOffset(sat, epoch, svPos, sunPos);
  
            cout << setw(4) << static_cast<YDSTime>(epoch).year << " "
                 << setw(4) << static_cast<YDSTime>(epoch).doy  << " "
                 << setw(6) << setprecision(1) << static_cast<YDSTime>(epoch).sod  << " "
                 << sat << " "
                 << setprecision(3) << fixed << setw(14) << svPosVel.x[0] << " " 
                 << setprecision(3) << fixed << setw(14) << svPosVel.x[1] << " "
                 << setprecision(3) << fixed << setw(14) << svPosVel.x[2] << " "
                 << setprecision(3) << fixed << setw(12) << svPosVel.v[0] << " "
                 << setprecision(3) << fixed << setw(12) << svPosVel.v[1] << " "
                 << setprecision(3) << fixed << setw(12) << svPosVel.v[2] << " "
                 << setprecision(12) << setw(15) << svPosVel.clkbias << " "
                 << setprecision(3) << fixed << setw(8) << svAtx[0] << " "
                 << setprecision(3) << fixed << setw(8) << svAtx[1] << " "
                 << setprecision(3) << fixed << setw(8) << svAtx[2] << " "
                 << endl;

//          cout << epoch << " " 
//               << sat << " "
//               << svPosVel << " "
//               << svAtx << " "
//               << endl;
         }
         catch(InvalidRequest& ir)
         {
            continue;
         }
      }
      
         // increment with interval
      sec+=interval;
   }

}


int main( int argc, char*argv[] )
{
   try
   {
      ephint fc("ephint", "Interpolate the eph,clk and corrections");
      if (!fc.initialize(argc, argv)) return(false);
      fc.run();
   }
   catch(gpstk::Exception& exc)
   {
      cout << exc << endl;
      return 1;
   }
   catch(...)
   {
      cout << "Caught an unnamed exception. Exiting." << endl;
      return 1;
   }
   return 0;
}

