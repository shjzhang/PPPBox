#pragma ident "$Id 2016-10-11 $"

/**
 * @file GPSTK_MOUNTPOINT_HPP
 * Class to define the attribute of each mountpoint.
 * Reference:
 * Gebhard H, Weber G. Ntrip, Version 1.0, Design¡ªProtocol¡ªSoftware, Part I&II[J].
 * Frank-furt, Germany, Published by BKG, 2003.
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
//  XY.Cao, Wuhan Uniersity, 2016 
//
//============================================================================
//  
//  Define the structure of each mountpoint(added other attributes if you want):
//  url;format;country;latitude;longtitude;nmeabool;ntripversion.
//  Each mountpoint contains all necessary message used for network request!
//
//============================================================================

#ifndef GPSTK_MOUNTPOINT_HPP
#define GPSTK_MOUNTPOINT_HPP

#include <string>
#include <map>
#include "NetUrl.hpp"

using namespace std;

namespace gpstk
{
	class MountPoint
	{
	public:

		// default constructor
        MountPoint():Format(""),Country(""),Latitude(0.0),
                    Longitude(0.0),nmeaFlag(0),NtripVersion("1"){;}

		/** 
		 * Constructor:only initialize url of mountpoint
		 * @param _username: user name
		 * @param _password: password
		 * @param _host: caster host
		 * @param _port: caster port
		 */		
		MountPoint(const string& _username,
			       const string& _password,
				   const string& _host,
				   const string& _port);

		/** 
		 * Constructor:initialize all
		 * @param _username: user name
		 * @param _password: password
		 * @param _host: caster host
		 * @param _port: caster port
		 * @param _mntstring:formatted string from conf file
		 * for example: ABMF0 RTCM_3.1 16.27 -61.52 no 1
		 * (in order to be compatible with BNC)
		 */	
		MountPoint(const string& _username,
			       const string& _password,
				   const string& _host,
				   const string& _port,
				   const string& _mntstring);

		// constructor 3
		MountPoint(const NetUrl& _url);

		// constructor 4
		MountPoint(const NetUrl& _url,const string& _mntstring);

		// copy constructor
		MountPoint(const MountPoint& _mntpoint);

		// decode special format string
		// for example: ABMF0 RTCM_3.1 16.27 -61.52 no 1
		void DecodeMountPoint(string _mntstring);
		
		/// destructor
        ~MountPoint(){;}

		// return a string identifying this class
		string getClassName() const;

		// set MountPortUrl
		void setMountPointUrl(const NetUrl& _url)
        {MountPointUrl = _url;}

		void setScheme(const string& _scheme)
        {MountPointUrl.setScheme(_scheme);}

		void setUserName(const string& _UserName)
        {MountPointUrl.setUserName(_UserName);}

		void setPassWord(const string& _PassWord)
        {MountPointUrl.setPassWord(_PassWord);}

		void setCasterHost(const string& _CasterHost)
        {MountPointUrl.setCasterHost(_CasterHost);}

		void setCasterPort(const string& _CasterPort)
        {MountPointUrl.setCasterPort(_CasterPort);}

		void setFormat(const string& format)
        {Format = format;}

		void setCountry(const string& country)
        {Country = country;}

		void setLatitude(const double lat)
        {Latitude = lat;}

        void setLongitude(const double lon)
        {Longitude = lon;}

		void setNmeaFlag(int flag)
        {nmeaFlag = flag;}

		void setNtripVersion(const string& _version)
        {NtripVersion = _version;}

		// get
        NetUrl getMountPointUrl() const{return MountPointUrl;}

        string getUserName() const{return MountPointUrl.getUserName();}

        string getPassWord() const{return MountPointUrl.getPassWord();}
		
        string getCasterHost() const{return MountPointUrl.getCasterHost();}
		
        string getCasterPort() const{return MountPointUrl.getCasterPort();}
		
        string getMountPointID() const{return MountPointUrl.getPath();}

		// RTCM_3,RTCM_2(return upper case letters)
        string getFormat() const{return Format;}

        double getLatitude() const {return Latitude;}

        double getLongitude() const{return Longitude;}

        int getnmeaFlag() const{return nmeaFlag;}

        string getNtripVersion() const{return NtripVersion;}

		// overloading operator =
		MountPoint& operator= (const MountPoint& _mountpoint);

		// overloading operator ==
		bool operator== (const MountPoint& _mountpoint) const;

		// dump all the information of MountPoint
		void dump() const;


	private:

		// //user:password@casterhost:port/mountpointID
		NetUrl MountPointUrl;

		// data format:RTCM_2,RTCM_3,RAW,etc
		string Format;

		// three character country code in ISO 3166
		string Country;

		// aprroximate position in case of nmea = 1
		double Latitude;
        double Longitude;

		// necessity for client to send NMEA message with
		// approximate position to caster
		// 1 = client must send NMEA GGA message
		// 0 = client must not send NMEA message
		int nmeaFlag;

		/// 1,2,2s etc
		string NtripVersion;

	};	// End of class 'MountPoint'

	//@}


} // End of namespace gpstk

#endif   // GPSTK_MOUNTPOINT_HPP
