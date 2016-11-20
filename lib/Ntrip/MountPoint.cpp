#pragma ident "$Id 2016-10-11 $"

/**
* @file GPSTK_MOUNTPOINT_CPP
* Class to define the attribute of each mountpoint.
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

#include "MountPoint.hpp"

#include <string>

	///	gpstk string utility function
#include "StringUtils.hpp"

using namespace gpstk;
using namespace std;
using namespace StringUtils;

namespace gpstk
{
	// return a string identifying this class.
   string MountPoint::getClassName() const
   { return "MountPoint"; }


   	// only initialize url of mountpoint
   MountPoint::MountPoint(const string& _username,const string& _password,
		                  const string& _host,const string& _port)
   {
	   MountPointUrl.setScheme("http");
	   MountPointUrl.setUserName(_username);
	   MountPointUrl.setPassWord(_password);
	   MountPointUrl.setCasterHost(_host);
	   MountPointUrl.setCasterPort(_port);
	   MountPointUrl.setPath("/");
	   Format = "";
	   Country = "";
	   Latitude = 0.0;
	   Longtitude = 0.0;
	   nmeaFlag = 0;
	   NtripVersion = "1";
   }

   // constructor: initialize all
   MountPoint::MountPoint(const string& _username,const string& _password,
	                      const string& _host,const string& _port,
						  const string& _mntstring)
   {
	   MountPointUrl.setScheme("http");
	   MountPointUrl.setUserName(_username);
	   MountPointUrl.setPassWord(_password);
	   MountPointUrl.setCasterHost(_host);
	   MountPointUrl.setCasterPort(_port);
	   DecodeMountPoint(_mntstring);
   }


   // constructor 3
   MountPoint::MountPoint(const NetUrl& _url)
   {
	   MountPointUrl = _url;
	   Format = "";
	   Country = "";
	   Latitude = 0.0;
	   Longtitude = 0.0;
	   nmeaFlag = 0;
	   NtripVersion = "1";
   }


   // constructor 4
   MountPoint::MountPoint(const NetUrl& _url,const string& _mntstring)
   {
	   MountPointUrl = _url;
	   DecodeMountPoint(_mntstring);
   }


   	/// copy constructor
   MountPoint::MountPoint(const MountPoint& _mntpoint)
   {
	   MountPointUrl = _mntpoint.MountPointUrl;
	   Format = _mntpoint.Format;
	   Country = _mntpoint.Country;
	   Latitude = _mntpoint.Latitude;
	   Longtitude = _mntpoint.Longtitude;
	   nmeaFlag = _mntpoint.nmeaFlag;
	   NtripVersion = _mntpoint.NtripVersion;
   }


   	// decode special format string
   void MountPoint::DecodeMountPoint(string _mntstring)
   {
	   // delete blanks 
	   string mntpoint= strip(_mntstring);
	   // cout << mntpoint << endl;
	   // get the MountPointID
	   MountPointUrl.setPath( stripFirstWord(mntpoint));
	   // get the Format
	   Format = stripFirstWord(mntpoint);
	   string temp = stripFirstWord(mntpoint);
	   // get the Country
	   if(isdigit(temp[0]))
	   {
		   Country = "" ;
		   Latitude = asDouble(temp);
	   }
	   else
	   {
		   Country = temp;
		   Latitude = asDouble(stripFirstWord(mntpoint));
	   }
	   // get the Latitude and Longtitude
	   Longtitude = asDouble(stripFirstWord(mntpoint));
	   // get the nmea flag
	   if(stripFirstWord(mntpoint) == "no")
	   {
		   nmeaFlag = 0;
	   }
	   else
	   {
		   nmeaFlag = 1;
	   }
	   // get the NtriVersion
	   NtripVersion = stripFirstWord(mntpoint);
   }


   // overloading operator =
   MountPoint &MountPoint::operator= (const MountPoint& _mountpoint)
   {
	   MountPointUrl = _mountpoint.getMountPointUrl();
	   Format = _mountpoint.Format;
	   Country = _mountpoint.Country;
	   Latitude = _mountpoint.Latitude;
	   Longtitude = _mountpoint.Longtitude;
	   nmeaFlag = _mountpoint.nmeaFlag;
	   NtripVersion = _mountpoint.NtripVersion;
	   return *this;

   }


   // overloading operator ==
   bool MountPoint::operator== (const MountPoint& _mountpoint) const
   {
	   string _left = MountPointUrl.getPath();
	   string _right = _mountpoint.getMountPointID();
	   return(_left == _right);
   }


   // dump all the information of mountpoint
   void MountPoint::dump() const
   {
	   string temp = "";
	   temp = MountPointUrl.getScheme() +"://" + 
		      MountPointUrl.getUserName() + ":" + 
			  MountPointUrl.getPassWord() + "@" + 
			  MountPointUrl.getCasterHost() + ":" + 
			  MountPointUrl.getCasterPort() +
			  MountPointUrl.getPath()
			  ;
	   // http://username:password@host:port/path
	   cout << temp << " " << Format 
		            << " " << Country 
					<< " " << Latitude 
					<< " " << Longtitude 
					<< " " << nmeaFlag 
					<< " " << NtripVersion 
					<< endl;
	   
   }





}  // End of namespace gpstk

