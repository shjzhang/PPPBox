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
//  Copyright (c)
//
//  XY.Cao, Wuhan University, 2016.
//
//============================================================================
// Modification
//
// 2016.11.20  Q.Liu  Add the function outputting the reveived binary data
//                    to file.
//============================================================================

// Using Ntrip to retrive and decode real-time streams from IGS/EUREF
// The preliminary test just considers the only format rtcm3 and outputs the
// retrived streams to the screen!

#include <iostream>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <thread>

    /// Class to define the version of NtripTool
#include "NtripToolVersion.hpp"

    /// Class to read configuration files
#include "ConfDataReader.hpp"

    /// Class to define the structure of URL
#include "NetUrl.hpp"

    /// Class to define the structure of mountpoint
#include "MountPoint.hpp"

    ///	Class to store mountpoints
#include "ReadMountPoints.hpp"

// Class to handle socket
#include "SocketLib.hpp"

#include "NetQueryBase.hpp"

#include "NetQueryNtrip1.hpp"

#include "SourceTableReader.hpp"

    /// Class to hand c++ string
#include "StringUtils.hpp"

#include "YDSTime.hpp"

#include "NtripTask.h"

#include "ThreadPool.h"

#include "FileSpec.hpp"

#include "SignalCenter.hpp"


using namespace gpstk;
using namespace StringUtils;
using namespace std;

// max size of ntrip response
#define NTRIP_MAXRSP   32768


///////////////////////////////////////////////////////////////////////////////

void catch_signal(int)
{
    cout << "Program Interrupted by Ctrl-C" << endl;
    exit(1);
}


////////////////////////////////////////////////////////////////////////////////

// main program
int main( int argc, char* argv[] )
{
    signal(SIGINT, catch_signal);

    // configure file name,by default named "NtripTool.conf"
    string ConfFileName;

    // raw file name(post process)
    string RawFileName;

    // print help
    string PrintHelp =
    "NtripTool_" NTRIPTOOLPGMNAME NTRIPTOOLVERSION "_" NTRIPTOOLOS " usage:\n"
    " 	-help: print help{no arguments}\n"
    "	-conf: configure file name{ConfFileName}\n"
    "	-file: raw file name{RawFileName}\n"
    "\n";

    /// just for test:output
    /// 0 = no output; 1 = necessary parameters
    /// 2 = parameters of medium process
    /// 3 = input parameters and output parameters
    /// 5 = output all
    int verb = 5;

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

        else if(strncmp(argv[idx],"-conf",5) == 0 && idx + 1 < argc)
        {
            ConfFileName = argv[idx+1];
            idx++;
            continue;
        }   // end if -conf

        else if(strncmp(argv[idx],"-file",5) == 0 && idx + 1 < argc)
        {
            RawFileName = argv[idx+1];
            idx++;
            continue;
        }   // end if -file
        else
        {
            cout << "Error input arguments!" << endl;
            cout << PrintHelp << endl;
            exit(-1);
        }

        // set another loop to get key name and key value

    } // end while argc

    if (verb > 2)
    {
        cout << "ConfFileName: " << ConfFileName << endl;
        cout << "RawFileName: "  << RawFileName  << endl;
    }

    ///////////////// first ,read conf file/////////////

    // open and parse configuration file
    ConfDataReader ntripConf;

    //	check whether the user has provided a configuration file
    if(! ConfFileName.empty())
    {
        // enable exceptions
        ntripConf.exceptions(ios::failbit);
        try
        {
            ntripConf.open(ConfFileName);
        }
        catch(...)
        {
            cerr << "Error: opening file "
                 << ConfFileName << endl;
            cerr << "Maybe configuration doesn't exsit or you don't have "
                 << "proper read permissions."
                 << endl;
            exit(-1);
        }    // end try-catch
    }   // if-else
    else
    {
        try
        {
            //	try to open the default configuration file
            ntripConf.open("NtripTool.conf");
        }
        catch(...)
        {
            cerr << "***Error: opening default configuration file NtripTool.conf"
                 << endl;
            cerr << "Maybe configuration doesn't exsit or you don't have "
                 << "proper read permissions."
                 << endl;
            exit(-1);
        }   // end try-catch
    }    //	 end if-else

    // read configuration file
    // whether get sourcetable from network
    bool getSourceTable( ntripConf.getValueAsBoolean("GetSourceTableFromNet") );

    // whether output raw data to file
    bool outputRaw(ntripConf.getValueAsBoolean("OutputRawToFile"));

    // loop all the casters
    ReadMountPoints ReadmntPoints;
    string casterNum;
    while( (casterNum=ntripConf.getEachSection()) != "")
    {
        if( casterNum == "DEFAULT" )
        {
            continue;
        }

        string UserName = ntripConf.getValue("UserName",casterNum);
        string PassWord = ntripConf.getValue("PassWord",casterNum);

        // whether to use proxy or not
        string CasterHost = ntripConf.getValue("ProxyHost",casterNum);
        string CasterPort = ntripConf.getValue("ProxyPort",casterNum);

        if(CasterHost.empty())
        {
            CasterHost = ntripConf.getValue("CasterHost",casterNum);
            CasterPort = ntripConf.getValue("CasterPort",casterNum);
        }

        // mountpoint or mountpoint list(sepeated by '/')
        string ConfMountPoints = ntripConf.getValue("MountPoints",casterNum);


        if(ConfMountPoints.empty())
        {
            cout << "Warning: no mountpoints!" << endl;
            exit(0);
        }

        if(verb > 1)
        {
            cout << "Caster num:\t" << casterNum << endl;
            cout << "UserName = \t" << UserName << endl;
            cout << "PassWord = \t" << PassWord << endl;
            cout << "CasterHost = \t" << CasterHost << endl;
            cout << "CasterPort = \t" << CasterPort << endl;
            cout << "MountPoints = \t" << ConfMountPoints << endl;
        }

        ///////////////// second ,read sourcetable file/////////////

        // read SourceTable.txt and try to find the mountpoint
        SourceTableReader srcTableReader;

        // local path of SourceTable.txt
        string SourceTablePath = ntripConf.getValue("SourceTable",casterNum);

        if(getSourceTable)
        {
            if(SourceTablePath.empty())
            {
                cout << "SourceTable.txt does not exist in local path, "
                     << "please get it from network!"
                     << endl;
            }
            else
            {
                thread tempThread([&] {
                       srcTableReader.updateFromNet(SourceTablePath, CasterHost,CasterPort);});
                tempThread.join();
                srcTableReader.open(SourceTablePath);
            }
        }
        else
        {
            // get from network
            if(SourceTablePath.empty())
            {
                cout << "SourceTable.txt does not exist in local path, "
                     << "please get it from network!"
                     << endl;
                // not exit

                // TD:get form network and uplate local sourcetable file
            }
            else
            {
                // read SourceTable.txt
                //cout << "get sourcetable from local path." << endl;
                srcTableReader.open(SourceTablePath);
            }
        }
        ///////////////// third ,preparation for multi-thread/////////////

        // the url of caster
        NetUrl mntUrl(UserName,PassWord,CasterHost,CasterPort);

        string mountpointID;

        while(!ConfMountPoints.empty())
        {
            // every mountpoint seperated by "/"
            mountpointID = stripTrailing(stripFirstWord(ConfMountPoints,'/'));

            mntUrl.setPath(mountpointID);
            // get from sourcetable
            if(srcTableReader.haveStream(mountpointID))
            {
                SourceTableReader::mountpointSTR stream =
                    srcTableReader.getStream(mountpointID);
                // add the new mountpoint to the map
                ReadmntPoints.addMountPoint(mountpointID,mntUrl,stream);
            }
            else
            {
                continue;
            }
        }

        if( verb >2 ) ReadmntPoints.dump();

    } // end of "loop all the casters"

    //////////////////////// thread ////////////////////////////////////

    ThreadPool* pTP = ThreadPool::create(15);

    pTP->onStart();

    // get the map
    map<string,MountPoint> mntPointsMap = ReadmntPoints.getMountPointMap();


    // Loop through MountPointsMap
    map<string,MountPoint>::iterator itmnt;

    for(itmnt = mntPointsMap.begin();itmnt != mntPointsMap.end();++itmnt)
    {
        if(verb > 1)
        {
            cout << "Start for " << (*itmnt).first << endl;
        }

        MountPoint pt = (itmnt->second);

        NtripTask* pTask = new NtripTask;
        pTask->setMountPoint(pt);
        SystemTime dateTime;
        CommonTime comTime(dateTime);
        std::string utcTime = CivilTime(comTime).printf("%Y%02m%02d%02H");
        string filename = pt.getMountPointID() + "_" + utcTime +".out";
        pTask->setRawOutFile(filename);
        pTask->setRawOutOpt(outputRaw);
        pTP->pushTask(pTask);
    } // end for

    string pppConf = ntripConf.getValue( "pppConf", "DEFAULT");
    SIG_CENTER->setPPPConfFile(pppConf);
    SIG_CENTER->startPPP();

    char key = ' ';

    while( key != 'q' )
    {
        scanf("%c", &key);
    }

    return 0;
}	/// end main()

