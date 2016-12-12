#pragma ident "$ID: NetUrl.hpp 2016-10-10 $"

/**
 * @file NetUrl.hpp
 * Define the structure of http url: 
 * <scheme>://<user>:<password>@<host>:<port>/<path>;
 * Reference:
 * Gourley D, Totty B. HTTP: the definitive guide[M]. " O'Reilly Media, Inc.", 2002.
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
//  XY.Cao, Wuhan University, 2016
//
//============================================================================


#ifndef GPSTK_NETURL_HPP
#define GPSTK_NETURL_HPP

#include <string>

using namespace std;

namespace gpstk
{
	class NetUrl
	{
	public:

		// Default constructor
		NetUrl():Scheme("http"),UserName(""),PassWord(""),
            CasterHost(""),CasterPort("80"),ResourcePath( ""){};

		/** 
		 * Constructor
		 * @param _username: user name
		 * @param _password: password
		 * @param _host: caster host
		 * @param _port: caster port
		 */
		NetUrl(const string& _username,
			   const string& _password,
			   const string& _host,
			   const string& _port);

		/** 
		 * Constructor
		 * @param _mntstring: string with special format
		 * for example http://username:password@host:port/mountpointID
		 * or a simple way
		 * //username:password@host:port/mountpointID
		*/
		NetUrl(const string& _mntstring);

		/**
		 * Copy constructor
		 * @param _url: NetUrl
		 */
		NetUrl(const NetUrl& _url);
		
		// destructor
		~NetUrl(){};

		// return a string identifying this class
		string getClassName() const;

		// set
        void setScheme(const string& _Scheme){Scheme = _Scheme;}

        void setUserName(const string& _UserName){UserName = _UserName;}

        void setPassWord(const string& _PassWord){PassWord = _PassWord;}

        void setCasterHost(const string& _CasterHost){CasterHost = _CasterHost;}

        void setCasterPort(const string& _CasterPort){CasterPort = _CasterPort;}

        void setPath(const string& _Path){ResourcePath = _Path;}

		// get
        string getScheme() const{return Scheme;}

        string getUserName() const{return UserName;}

        string getPassWord()const{return PassWord;}

        string getCasterHost()const{return CasterHost;}

        string getCasterPort()const{return CasterPort;}

		// not including "/"
        string getPath()const{return ResourcePath;}

		// overloading operator =
        NetUrl& operator= (const NetUrl& _url);

		// dump all the information of URL
        void dump() const;


	private:

		// scheme: http,ftp,rtsp etc, by default:http.
		string Scheme;

		// user name
		string UserName;

		// password without encoding 
		string PassWord;

		// caster host: ntrip.gnsslab.cn or others
		string CasterHost;

		// caster port: 80 or 2101(not int, by default:80)
		string CasterPort;

		// resource path
		// Normally in GNSS real-time appilication, it refers to mountpointID
		// for example: ABMF0.
		string ResourcePath;


	};	// End of class 'NetUrl'

	//@}


} // End of namespace gpstk

#endif   // GPSTK_NETURL_HPP
