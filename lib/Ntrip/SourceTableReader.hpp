#pragma ident "$Id 2016-11-14 $"
/**
 * @file SourceTableReader.cpp
 * Class to read and store the sourcetable from
 * an existing file
 */

#ifndef SOURCETABLEREADER_HPP
#define SOURCETABLEREADER_HPP

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
//  Q.Liu,XY.CAO, Wuhan Uniersity, 2016.
//
//============================================================================
// Modification
//
// 2016.12.12  Q.Liu  Add the function that update the source table file from
//                    network.
//============================================================================

#include <string>
#include <vector>
#include <map>

#include "Exception.hpp"
#include "StringUtils.hpp"
#include "FFTextStream.hpp"
#include "MountPoint.hpp"
#include "NetUrl.hpp"
#include "NetQueryNtrip1.hpp"


#define MAXSRCTBL   512000              // max source table size (bytes)
#define ENDSRCTBL	"ENDSOURCETABLE"	// end marker of table
#define NTRIP_CYCLE 50                  // processing cycle (ms)
#define NTRIP_TIMEOUT 10000             // response timeout (ms)

namespace gpstk
{
	
	class SourceTableReader : public FFTextStream
	{
	
	
	public:


        typedef std::string MountPointID;

		// only interested in type STR
		struct mountpointSTR
		{
			// source identifier, e.g. BRDC_APC_ITRF for correction stream
			string identifier;

			// data fomat,e.g. RTCM 3.0
			string format;

			// RTCM message types, updata rate(in seconds)
			string format_details;

			// data stream contais carrier, 0 or 1 or 2
			int carrierNo;

			// navigation system
			string navsystem;

			string network;

			string country;

            double latitude,longitude;

			// 1 = MUST send NMEA GGA
			int nmeaFlag;

			// stream generated from single base(0) or network(1)
			int solution;

			// hard- or software generating data stream
			string generator;

			string compression;

			// access protection for this paticular data stream
			// N = none, B = basic, D = digest
			string authentication;

			// user fee for receiving paticular stream
			// N = no user fee, Y = usage is charged
			string fee;

			// bitrate of data stream,bit/s
			int bitrate;

			string misc;

		};

        // Default constructor
        SourceTableReader();

        // Method to open AND load sourcetable file.
        void open(const char* fn)throw( FileMissingException );

        // Method to open AND load sourcetable file.
        void open(const std::string& fn)throw( FileMissingException );

        // Destructor
        virtual ~SourceTableReader();

		// dump all stream in sourcetable
		void dumpSourceTable();

		// dump thr interested stream in sourcetable
        void dumpStream(string &mountpoint);

		// whether contains the designated mountpoint 
        bool haveStream(string &mountpoint);

		// get the designated mountpoint 
        mountpointSTR getStream(string &mountpoint);

		// count the number of rtcm format
		// format: RTCM 2.0/RTCM 2.3/RTCM 3.0/RTCM3.1/RTCM 3.2 OR
		// espective lower case
        int RTCMTypeNumber(string &format);

        // update the source table file from networks
        void updateFromNet(string &file, NetUrl &url);

        // update the source table file from networks
        void updateFromNet(string &file, string &host, string &port);

    private:

        // map to store all the table contents
        std::map<MountPointID,mountpointSTR> mountPointMap;

		// content length in sourcetable
		int tableLength;

        // Method to store sourcetable data in this class' data map
        virtual void loadData()
           throw( FFStreamError, gpstk::StringUtils::StringException );

    }; // End of class SourceTableReader

}  // End of namespace gpstk

#endif // SOURCETABLEREADER_HPP
