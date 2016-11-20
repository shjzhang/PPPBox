#pragma ident "$Id$"

/**
* @file GPSTK_NTRIPSTREAM_CPP
* Class to retrieve real data from Internet using Ntrip protocol.
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
//  Modified
//  2016-1-26 .
//
//============================================================================
/*
#include "NtripStream.hpp"
#include <string>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

namespace gpstk
{

	// assignment constructor
	NtripStream::NtripStream(int _state,int _type,int _respbuffsize,string& _respbuff,
							const string& _user,const string& _passwd,const string& _mntpoint,
							const string& _host,const string& _port,const string& _proxyurl,
							const string& _proxyport,int _numrecnt,int _itvalrecnt,
							const int _verb)
	{
		state = _state;
		type = _type;
		respbuffsize = _respbuffsize;
		respbuff = _respbuff;
		user = _user;
		passwd = _passwd;
		mntpoint = _mntpoint;
		host = _host;
		port = _port;
		proxyurl = _proxyurl;
		proxyport = _proxyport;
		numrecnt = _numrecnt;
		itvalrecnt = _itvalrecnt;
		verb = _verb;
	}

	// simple constructor
	NtripStream::NtripStream(const string& _user,const string& _passwd,const string& _mntpoint,
							 const string& _host,const string& _port)
	{
		state = 0;type = 1;respbuffsize = 0;respbuff = "";verb = 0;
		user = _user;
		passwd = _passwd;
		mntpoint = _mntpoint;
		host = _host;
		port = _port;
		proxyurl = "";proxyport = "";numrecnt = 10;itvalrecnt = 10;
	}
	
	// return a string identifying this class.
   string NtripStream::getClassName() const
   { return "NtripStream"; }

   // print NtripStream
   void NtripStream::dump()
   {
	   cout << "**********NtripStream::dump**********" << endl;
	   cout << "state:\t" << state << endl
		    << "type:\t"  << type  << endl
			<< "respbuffsize:\t" << respbuffsize << endl
			<< "user:\t" << user << endl
			<< "password:\t" << passwd << endl
			<< "mountpoint:\t" << mntpoint << endl
			<< "caster host:\t" << host << endl
			<< "caster port:\t" << port << endl
			<< "proxy URL:\t" << proxyurl << endl
			<< "proxy port:\t" << proxyport << endl
			<< "number of reconnection:\t" << numrecnt << endl
			<< "interval of reconnection:\t" << itvalrecnt << endl
			<< "verbosity level:\t" << verb << endl
			<< "****************************************" << endl;
   }

   // Initialize Ntrip stream environment(cross platform:linux or windows)
   void NtripStream::Initenvironment()
   {
#ifdef WIN32
	WSADATA data; //Store initialization information of windows
	if(verb > 0){std::cout << "NtripStream::Initenvironment" << std::endl;}
	// initialize Winsock
	if(WSAStartup(MAKEWORD(2,0), &data) != 0)
	{
		cerr << "***error:initialize Winsock has something wrong!***" << endl;
	}
#endif
   }

   // get socket error
   int NtripStream::getLastSocketError()  
   {  
#ifdef WIN32 
	   return WSAGetLastError();  
#else 
	   return errno;  
#endif  
   }  

   // Initialize stream,return state :1= ok,-1=error
   int NtripStream::StreamInit()
   {
	   if(verb > 0)
	   {
		   cout << "NtripStream::StreamInit" << endl;
	   }
	   if(state)
	   {
		   cerr << "***error:state != 0***" << endl;
		   return -1;
	   }
	   // cross platform
	   Initenvironment();

	   if(!respbuff.empty())
	   {
		   respbuff = "";
		   respbuffsize = 0;
		   return -1;
	   }
	   return 1;

   }

   // get stream state:different with getState()
   // stream state,bytes received/sent,bitrate received/sent
   void NtripStream::getStreamState(int& _strstate,int& _strbytes,int& _strbps)
   {
	   // get stream state
	   _strstate = getState();
	   if(_strstate == 0){_strbytes = 0;_strbps = 0;}

	   // how to get stream bytes and bps?
	   _strbytes = 0;// ???
	   _strbps = 0;// ???

	   if(verb > 0)
	   {
		   cout << "NtripStream::getStreamState" << endl;
		   cout << "state: " << _strstate << "\t"
			    << "bytes: " << _strbytes << "\t"
				<< "bps: "   << _strbps << endl;
	   }
   }

   // send ntrip client request
   int NtripStream::reqntrip_c()
   {
	   char reqbuff[1024];char *p = reqbuff;
	   
	   if(user != "")
	   {
		   p += snprintf(p,sizeof(reqbuff)-40,
				   		"GET /%s HTTP/1.0\r\n"
						"User-Agent: %s\r\n"
						"Authorization: Basic ",
						mntpoint.c_str(),NTRIP_AGENT);
		   cout << reqbuff << endl;
		   cout << "reqbuffsize:\t" << strlen(reqbuff) << endl;
	   }
	   else
	   
	  
	   {
		   p += snprintf(p,sizeof(reqbuff)-40,
				         "GET /%s HTTP/1.0\r\n"
				         "User-Agent: %s\r\n"*/
						 //"Accept: */*\r\n"
						 /*
						 "Connection: close\r\n"
                        "Authorization: Basic ",
                          mntpoint.c_str(),NTRIP_AGENT);
		  cout << reqbuff << endl;
		  cout << "reqbuffsize:\t" << strlen(reqbuff) << endl;
	   }
	   // sizeof(reqbuff) = 1024

	   if(strlen(reqbuff) > (int)sizeof(reqbuff)-40 or strlen(reqbuff) < 0)
	   {
		   cerr << "***error:request is too long!***" << endl;
	   }

	   // encode user:password
	   char _userpasswd[512];
	   sprintf(_userpasswd,"%s:%s",user.c_str(),passwd.c_str());
	   p += encode64(p,(unsigned char *)_userpasswd,strlen(_userpasswd));
	   cout << reqbuff << endl;
	   cout << "reqbuffsize:\t" << strlen(reqbuff) << endl;
	   // cout << _userpasswd << "\tbase64:\t" << _base64userpasswd << endl;

	   p += snprintf(p, 5, "\r\n\r\n");
	   if(verb > 0) 
	   {
		   cout << reqbuff << endl;
		   cout << "reqbuffsize:\t" << strlen(reqbuff) << endl;
	   }

	   // wait socket connect
	   int connfd = -1;int sockfd = -1;
	   waitNtripcli(sockfd,connfd);
	   cout << "sockfd:\t" << sockfd << "\tconnfd:\t" << connfd << endl;

	   // send reqest to caster
	   if((send(sockfd,reqbuff,strlen(reqbuff),0)) < 0)
	   {
		   cerr << "***error:send socket wrong:\t" << getLastSocketError() << endl;
		   state = 0;
		   close(sockfd);
		   return SOCKET_ERROR;
	   }
	   cout << "send request to caster successfully!" << endl;

	   char respbuff[NTRIP_MAXRSP];int respbuffsize;
	   if((respbuffsize = recv(sockfd,respbuff,sizeof(respbuff),0)) <= 0)
	   {
		   cerr << "***error:send socket wrong:\t" << getLastSocketError() << endl;
		   state = 0;
		   close(sockfd);
		   return SOCKET_ERROR;
	   }
	   cout << "receive response from caster successfully!" << endl;
	   cout << "response:\n" << respbuff << endl;
	   cout << "response buffer size:\t" << respbuffsize << endl;






	   
   }

   // encode base64, user:password
   int NtripStream::encode64(char *_str,const unsigned char *_byte,int _n)
   {
	   // The sequence cann't be changed!
	   static const char table[64]= 
	   {
		   'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
		   'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
		   'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
		   'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
	   };
		 
	   int i,j,k,b;
	   for(i = j = 0;i/8 < _n;)
	   {
		   for(k = b = 0;k < 6;k++,i++)
		   {
			   b <<= 1;
			   if(i/8 < _n)  b|=(_byte[i/8]>>(7-i%8))&0x1;
		   }
		   _str[j++]=table[b];
	   }
	   while (j&0x3) _str[j++]='=';
	   _str[j]='\0';
	   if (verb > 1) cout << "NtripStream::encode64\t" << _str << endl;
	   return j;  // return the encoded bytes

   }

   // wait socket connect, before sending/receiving messages
   int NtripStream::waitNtripcli(int& _sockfd,int& _connfd)
   {
	   if(verb > 0) cout << "NtripStream::waitNtripcli" << "\tstate: " << state << endl;
	   if(state < 0 or state > 2) 
	   {
		   cerr << "***error:state is wrong!***" << endl;
		   return -1;
	   }

	   // closed,then try to create the socket and connect caster
	   if(state == 0)
	   {
		   if(createSocket(_sockfd,_connfd) != 1) return SOCKET_ERROR;
	   }

	   // connected
	   //if(state == 2)
	   //{}

   }

   // create socket for client or server(not caster)
   // _type:0=server ,1=client
   // return : -1 = error, 1 = ok
   int NtripStream::createSocket(int& sockfd,int& connfd)
   {
	   // create socket,TCP
	   struct sockaddr_in servaddr;
	   // NB:MUST PAT ATTENTION TO OPERATION PRIORITY！
	   if((sockfd = socket(AF_INET,SOCK_STREAM,0)) < 0)
	   {
		   cerr << "***error:create socket wrong:\t" << getLastSocketError() << endl;
		   state = 0;
		   return SOCKET_ERROR;
	   }
	   cout << "create socket successfully: " << sockfd << endl;
	   
	   // set socket option(???)
	   // setSocketOpt();

	   // gethostbyname:host address -> ip address
	   struct hostent *Host;
	   if(!(Host = gethostbyname(host.c_str())))
	   {
		   // gethostbyname: no errno(should not use getLastSocketError())! 
		   cerr << "***error:gethostname wrong:\t" << hstrerror(h_errno) << endl;
		   close(sockfd);
		   state = 0;
		   return SOCKET_ERROR;
	   }

	   // bzero() replace of memset() to avoid wrong
	   bzero(&servaddr,sizeof(servaddr));
	   servaddr.sin_family = AF_INET;
       //memcpy(&servaddr.sin_addr.s_addr,Host->h_addr,Host->h_length);
	   bcopy(Host->h_addr,&servaddr.sin_addr.s_addr,Host->h_length);
	   servaddr.sin_port = htons(atoi(port.c_str()));

	   if(verb > 0)
	   {
		   cout << "port: " << servaddr.sin_port << "\taddress: " 
			    << servaddr.sin_addr.s_addr << endl;
		   printf("address:%s\n",inet_ntoa(servaddr.sin_addr));
	   }

	   state = 1;
	   if(verb > 0) cout << "NtripStream::createSocket" << "\tstate: " << state<< endl;
	   // NB:MUST PAT ATTENTION TO OPERATION PRIORITY！
	   // -1 = error ,0 = ok
	   if((connfd = connect(sockfd,(struct sockaddr *) &servaddr,sizeof(servaddr))) < 0)
	   {
		   cerr << "***error:connect wrong:\t" << getLastSocketError() << endl;
		   state = 0;
		   close(sockfd);// must close
		   return SOCKET_ERROR;
	   }

	   state = 2;
	   if(verb > 0) 
	   {
		   cout << "connect caster successfully: " << connfd << endl;
		   cout << "NtripStream::connectSocket" << "\tstate: " << state<< endl;
	   }
	   return 1;

   }

   // connect caster
   // return : -1 = error, 1 = ok
   int NtripStream::connectSocket(int _sockfd,int &_confd,struct sockaddr_in _addr)
   {
	   // _confd = -1;
	   if(_confd = connect(_sockfd,(struct sockaddr *) &_addr,sizeof(_addr)) < 0)
	   {
		   cerr << "***error:connect wrong:\t" << getLastSocketError() << endl;
		   state = 0;
		   close(_sockfd);
		   return SOCKET_ERROR;
	   }
	   state = 2;
	   if(verb > 0) cout << "NtripStream::connectSocket" << "\tstate: " << state<< endl;
	   return 1;
   }




}  // End of namespace gpstk
*/