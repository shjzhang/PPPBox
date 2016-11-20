#pragma ident "$ID: NtripStream.hpp 2016-03-25 $"

#ifndef GPSTK_NTRIPSTREAM_HPP
#define GPSTK_NTRIPSTREAM_HPP

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
//  Revision
//
//  2016/03/
//  First version, just for test
//
//============================================================================
/*
#include <string>
#include <iostream>
// platform:windows+linux
#ifdef WIN32
#include <winsock.h>
#include <windows.h>
typedef SOCKET int;
#else
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <errno.h>
#include <arpa/inet.h>
#include <unistd.h>
#define closesocket(sock) close(sock)
#define ALARMTIME (2*60)
#define socket_t int
#endif

using namespace std;
#define NTRIP_AGENT				"NTRIP whu_sgg1.0"
#define NTRIP_MAXRSP			32768
#define NTRIP_RSP_OK_CLI		"ICY 200 OK\r\n"
#define NTRIP_RSP_OK_SVR		"OK	\r\n"
#define NTRIP_RSP_SRC_TAB		"SOURCETAB 200 OK\r\n"
#define NTRIP_RSP_SRC_TAB_END	"ENDSOURCETABLE"
#define SOCKET_ERROR  (-1)

namespace gpstk
{
	class NtripStream
	{
	public:

		// default constructor
		NtripStream(){state = 0,type =1,respbuffsize = 0,respbuff = "",
					   user = "",passwd = "",mntpoint = "",
					   host = "",port = "",proxyurl = "",
					   proxyport = "",numrecnt = 10,itvalrecnt = 10,
					   verb = 0;};
		
		// simple constructor
		NtripStream(const string& _user,const string& _passwd,const string& _mntpoint,
				    const string& _host,const string& _port);

		// assignment constructor
		NtripStream(int _state,int _type,int _respbuffsize,string& _respbuff,
					const string& _user,const string& _passwd,const string& _mntpoint,
					const string& _host,const string& _port,const string& _proxyurl,
					const string& _proxyport,int _numrecnt,int _itvalrecnt,
					const int _verb);
	
		// using stream path to decode
		// NtripStream(string& _streampath);
		
		// destructor
		~NtripStream(){};

		// return a string identifying this class
		string getClassName() const;

		// get stream state(NB cann't be setted)
		int getState(){return state;};

		// get stream type
		int getType(){return type;};

		// get response buffer size
		int getRespbuffsize(){return respbuffsize;};

		//get response buffer(for test)
		string getRespbuff(){return respbuff;};

		// get user name
		string getUser(){return user;};

		// get requested mountpoint 
		string getMntpoint(){return mntpoint;};

		//get stream path
		void getStreamPath()
		{
			std::cout << host <<  ":" << port << std::endl;
		};

		// get proxy url and port
		void getProxy()
		{
			std::cout << proxyurl << ":" << proxyport << std::endl;
		};
		
		// set stream type
		void setType(int _type){type = _type;};

		// set user name
		void setUser(int _user){user = _user;};

		// set password
		void setPasswd(string& _passwd){passwd = _passwd;};

		// set mountpoint
		void setMntpoint(string& _mntpoint){mntpoint = _mntpoint;};

		// set caster host
		void setHost(string& _host){host = _host;};

		// set port:2101 or 80
		void setPort(string& _port){port = _port;};

		// set proxy url and port
		void setProxy(string& _proxyurl,string& _proxyport)
		{proxyurl = _proxyurl; proxyport = _proxyport;};

		// set munber of reconnection
		void setNumRecnt(int _numrecnt){numrecnt = _numrecnt;};

		// set interval of reconnection
		void setItvalRecnt(int _itvalrecnt){itvalrecnt = _itvalrecnt;};

		// set verbosity level
		void setVerb(int _verb){verb = _verb;};

		// print NtripStream
		void dump();

		// Initialize Ntrip stream environment(cross platform:linux or windows)
		void Initenvironment();

		// Initialize stream,return state :1= ok,-1=error 
		int StreamInit();

		// get stream state:different with getState()
		// stream state,bytes received/sent,bitrate received/sent
		void getStreamState(int& _strstate,int& _strbytes,int& _strbps);

		// send ntrip client request
		int reqntrip_c();

		// encode base64, user:password
		// return the encoded bytes //
		int encode64(char *_str,const unsigned char *_byte,int _n);

		// wait socket connect,before sending/receiving messages
		// return :-1 = error,1 = ok
		int waitNtripcli(int& _sockfd,int& _connfd);

		// create socket for client or server(not caster) 
		// return : -1 = error, 1 = ok
		int createSocket(int& _sockfd,int& _connfd);

		// get socket error
		int getLastSocketError();


	private:

		// ntrip state, 0=closed(by default),1=waited, 2=connected
		int state;

		// ntrip type, 0=server, 1=client(by default)
		int type;

		// response buffer size(by default 0)
		int respbuffsize;

		// response buffer
		string respbuff;

		// user name
		string user;

		// user password
		string passwd;

		// mountpoint
		string mntpoint;

		// host
		string host;

		// port(80 or 2101)
		string port;

		// url of proxy
		string proxyurl;

		// port of proxy
		string proxyport;

		// reconnection settings, number of reconnection:10 times by default
		int numrecnt;

		// reconnection settings, time interval of reconnection: 10s by default
		int itvalrecnt;;

		// verbosity level(by default 0)
		int verb;

	};	// End of class 'NtripStream'

		//@}


} // End of namespace gpstk
*/
#endif   // GPSTK_NTRIPSTREAM_HPP
