#pragma ident "$Id$"

/**
 * @file navcorr2Sp3.cpp
 * Convert the broadcast ephemeris file and SSR correction file to sp3 file.
 * The two given file have to own the same date.
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
//  Copyright (c)
//
//  Q.Liu, Wuhan University, Apr 19, 2017.
//
//============================================================================


#include <iostream>
#include <string>

#include "NavCorrToSP3.hpp"
#include "BasicFramework.hpp"
#include "StringUtils.hpp"

using namespace std;
using namespace gpstk;
using namespace gpstk::StringUtils;

int main(int argc, char* argv[])
{
    string navFileName;
    string corrFileName;
    string antexFile;
    string refPoint;
    double sample;

    // print help
    string PrintHelp =
    "navcorrToSp3 usage:\n"
    " 	-help  : print help{no arguments}\n"
    "	-brdc  : broadcast ephemeris file name{navFileName}\n"
    "	-corr  : SSR correction file name{corrFileName}\n"
    "	-smpl  : Sample of output sp3 file{sample}\n"
    "	-point : Reference point of corr file{refPoint}\n"
    "	-antex : Name of antex file{antexFile}\n"
    "\n";

    if(argc == 1)
    {
        cout << PrintHelp << endl;
        exit(0);
    }

    // read input arguments
    int idx = 0;

    while( ++idx < argc)
    {
        if(strncmp(argv[idx],"-help",5) == 0 && idx < argc)
        {
            cout << PrintHelp << endl;
            exit(0);
        }   // end if -help
        else if(strncmp(argv[idx],"-brdc",5) == 0 && idx + 1 < argc)
        {
            navFileName = argv[idx+1];
            navCorrToSp3->setNavFile(navFileName);
            idx++;
            continue;
        }   // end if -brdc
        else if(strncmp(argv[idx],"-corr",5) == 0 && idx + 1 < argc)
        {
            corrFileName = argv[idx+1];
            navCorrToSp3->setCorrFile(corrFileName);
            idx++;
            continue;
        }   // end if -corr
        else if(strncmp(argv[idx],"-smpl",5) == 0 && idx + 1 < argc)
        {
            sample = asDouble(argv[idx+1]);
            navCorrToSp3->setSample(sample);
            idx++;
            continue;
        }   // end if -smpl
        else if(strncmp(argv[idx],"-point",6) == 0 && idx + 1 < argc)
        {
            refPoint = argv[idx+1];
            if(refPoint == "APC")
            {
                navCorrToSp3->setEphRefPoint(NtripSP3Stream::APC);
            }
            idx++;
            continue;
        }   // end if -point
        else if(strncmp(argv[idx],"-antex",6) == 0 && idx + 1 < argc)
        {
            antexFile = argv[idx+1];
            navCorrToSp3->setAntexFile(antexFile);
            idx++;
            continue;
        }   // end if -antex
        else
        {
            cout << "Error input arguments!" << endl;
            cout << PrintHelp << endl;
            exit(-1);
        }
        // set another loop to get key name and key value

    } // end while argc

    if(corrFileName.empty() || navFileName.empty())
    {
        cerr << "Corrrection or ephemeris filename is empty !" << endl;
        exit(-1);
    }

    cout << "Start the conversion!" <<endl;
    navCorrToSp3->makeSP3File();
    cout << "Fininsh the conversion!" <<endl;
    return 0;
}
