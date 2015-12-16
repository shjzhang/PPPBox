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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include <iomanip>
#include <string>
#include <map>

#include "DayTime.hpp"
#include "SP3Stream.hpp"
#include "SP3Data.hpp"
#include "SP3Header.hpp"
#include "SP3EphemerisStore.hpp"
#include "SatID.hpp"

/**
 * @file petest.cpp
 *
 */

using namespace std;
using namespace gpstk;

int main(int argc, char *argv[])
{
   if (argc<2) {
      cout << "Usage: petest <SP3-format files ...>\n";
      return -1;
   }

   try
   {


//    CC2NOCC corr;

//    corr.setDCBReceiverFile();
//    corr.setDCBFile();

//    RinexObsStream rin
//    RinexObsHeader roh;
//    rin >> roh;

//    AntennaType antType = roh.antType;

//    corr.setAntennaType(antType);

//    while(rin>>gRin)
//    {
//       corr.Process(gRin);
//    }
      
   }
   catch (Exception& e)
   {
      cout << e;
      exit(-1);
   }
   catch (...)
   {
      cout << "Caught an unknown exception" << endl;
      exit(-1);
   }

   return 0;
}
