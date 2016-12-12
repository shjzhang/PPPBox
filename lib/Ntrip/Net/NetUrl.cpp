#pragma ident "$Id 2016-10-10 $"

/**
* @file GPSTK_NETURL_CPP
* Class to define the structure of url
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

#include "NetUrl.hpp"
#include <iostream>
#include <string>

	///	gpstk string utility function
#include "StringUtils.hpp"

using namespace gpstk;
using namespace std;
using namespace StringUtils;

namespace gpstk
{
	// return a string identifying this class.
   string NetUrl::getClassName() const
   { return "NetUrl"; }


   // constructor1
   NetUrl::NetUrl(const string& _username,
	              const string& _password,
				  const string& _host,
				  const string& _port)
   {
	   Scheme = "http";
	   UserName = _username;
	   PassWord = _password;
	   CasterHost = _host;
	   CasterPort = _port;
       ResourcePath = "";
   }

   
   // constructor: using the string to initialize the url
   NetUrl::NetUrl(const string& _urlstring)
   {
	   string tempurl = strip(_urlstring);
	   // tempurl will be changed 
	   if(tempurl[0] != '/')
	   {
		   // http://username:password@host:port/mountpointID;
		   Scheme = stripFirstWord(tempurl,':');
		   UserName = stripFirstWord(tempurl,':');
		   UserName = UserName.erase(0,2);
		   PassWord = stripFirstWord(tempurl,'@');
		   CasterHost = stripFirstWord(tempurl,':');
		   CasterPort = stripFirstWord(tempurl,'/');
		   ResourcePath = tempurl;
	   }
	   else
	   {
		   // //username:password@host:port/mountpointID
		   Scheme = "";
		   UserName = stripFirstWord(tempurl,':');
		   UserName = UserName.erase(0,2);
		   PassWord = stripFirstWord(tempurl,'@');
		   CasterHost = stripFirstWord(tempurl,':');
		   CasterPort = stripFirstWord(tempurl,'/');
		   ResourcePath = tempurl;
	   }

   }


   	// copy constructor
   NetUrl::NetUrl(const NetUrl& _url)
   {
	   Scheme = _url.Scheme;
	   UserName = _url.UserName;
	   PassWord = _url.PassWord;
	   CasterHost = _url.CasterHost;
	   CasterPort = _url.CasterPort;
	   ResourcePath = _url.ResourcePath;
   }


   // overloading operator =
   NetUrl &NetUrl::operator= (const NetUrl& _url)
   {
	   Scheme = _url.Scheme;
	   UserName = _url.UserName;
	   PassWord = _url.PassWord;
	   CasterHost = _url.CasterHost;
	   CasterPort = _url.CasterPort;
	   ResourcePath = _url.ResourcePath;
       // A = B = C;
	   return (*this);
   }


   // dump the absolute url of one mountpoint
   void NetUrl::dump() const
   {
	   string temp = "";
	   temp = Scheme + "://" + UserName + ":" + PassWord +"@"
		   + CasterHost + ":" + CasterPort + ResourcePath;
	   cout << temp << endl;	   
   }


}  // End of namespace gpstk

