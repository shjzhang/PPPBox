#pragma ident "$Id: NetQueryNtrip1.cpp $"

/**
* @file GPSTK_NETQUERYNTRIP1_CPP
* Class to define network request based on Ntrip1.0
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
// Modification
//
// 2016.11.20  Q.Liu  Add the function outputting the reveived binary data
//                    to file.
//============================================================================

#include <iostream>

#include "NetQueryNtrip1.hpp"
#include "NtripToolVersion.hpp"
#include "StringUtils.hpp"


using namespace vdraw;
using namespace gpstk;
using namespace StringUtils;

// max size of ntrip response
#define NTRIP_MAXRSP   32768

namespace gpstk
{

	// return a string identifying this class.
   string NetQueryNtrip1::getClassName() const
   { return "NetQueryNtrip1"; }


   // constructor
   NetQueryNtrip1::NetQueryNtrip1()
   {
	   TCPsocket = 0;
       timeOut = 20; // by default: 20s
       status = init;
       buffersize = 4096;
       buff = (unsigned char *)malloc(buffersize);
   }


   NetQueryNtrip1::~NetQueryNtrip1()
   {
       free(buff);
	   delete TCPsocket;
   }

   // stop the socket
   void NetQueryNtrip1::stop()
   {
	   if(TCPsocket)
	   {
		   //TCPsocket->abort();
           cout << "Here stop the netQuery! ";
		   TCPsocket->CloseSocket();;
	   }
	   status = finished;

   }

   // send request and read the received data
   void NetQueryNtrip1::waitForRequestResult(const NetUrl& url, string& outData)
   {

   }


   // connect to caster and send the network request
   void NetQueryNtrip1::startRequest(const NetUrl& url, const string& gga, unsigned char* buff)
        throw (MountPointNotFound,SocketRecvError)
   {
       int n, err;
       if(status == error )
       {
           return;
       }
       else if(status == init)
       {
           sendRequest(url,gga);
       }
       else if(status == connected)
       {
       }

       n = recv(TCPsocket->getSocketID(),(char*)buff,buffersize-1,0);
       if(n < 0)
       {
           err = TCPsocket->getSocketError();
           status = init;
           cout << "Socket receive error! Reconnect now." << endl;
           SocketRecvError e(getClassName() +": Socket receive error!");
		   return;
           //GPSTK_THROW(e);
       }
       nbyte = n;

       // wait response
       if(status != dataReceiveable)
       {
           try
           {
               string netPath = url.getPath();
               testResponse(netPath, buff);
           }
           catch(BufferOverflowError &e)
           {
               GPSTK_THROW(e);
           }
           catch(MountPointNotFound &e)
           {
               GPSTK_THROW(e);
           }
       }
       return;
   }

   // send the nmea order when we have connected to a caster
   void NetQueryNtrip1::keepAliveRequest(const NetUrl& url, const string& gga)
   {

   }


   int NetQueryNtrip1::getBuffLength()
   {
	   return nbyte;
   }


   void NetQueryNtrip1::sendRequest(const NetUrl &url, const string &gga)
   {

       string caster = url.getCasterHost();
       string port = url.getCasterPort();

       // connect to caster
       TCPsocket = new SocketLib();
       cout << "Connect to " << url.getCasterHost() << ", " << url.getPath() << endl;
       TCPsocket->connectToHost(caster.c_str(),port.c_str());

       // judge whether timeout
       if( !TCPsocket->waitForConnected(timeOut) )
       {
           // if time out, reconnect
           cout << "For host " << url.getCasterHost() << ", " << url.getPath() << ", "
                << "connect time out." << endl;
           status = init;

           return;
       }

       // send request
       string uName = url.getUserName();
       string passW = url.getPassWord();

       string userAndPwd;
       userAndPwd = uName +":" + passW;

       // encode
       userAndPwd = "Authorization: Basic "
                    + Base64Encoder::encode(userAndPwd)
                    + "\r\n";

       string tempMountpoint = url.getPath(); // mountpoint ID

       // request
       string reqStr;

       reqStr = "GET /" + tempMountpoint + " HTTP/1.0\r\n"
             + "User-Agent: NTRIP " + NTRIPTOOLPGMNAME + "/" + NTRIPTOOLVERSION
             +"(" NTRIPTOOLOS ")\r\n"
             + "Host: " + caster + "\r\n"
             + userAndPwd + "\r\n";

       // NMEA GGA string to handle VRS stream
       if(!gga.empty())
       {
           reqStr += gga + "\r\n";
       }


       // request is written into socket send buffer
       if(TCPsocket->sockWriteable(timeOut))
       {
           int writebytes = TCPsocket->writen(reqStr.c_str(),reqStr.length());

           if(writebytes == -1)
           {
               cout << "send error:\t" << TCPsocket->getSocketError() << endl;
               TCPsocket->CloseSocket();
               delete TCPsocket;
               status = error;
               return;
           }
       }
       else
       {
           // write time out
           cout << "write time out" << endl;
           TCPsocket->CloseSocket();
           delete TCPsocket;
           status  = error;
           return;
       }
       status = connected;
   }

   void NetQueryNtrip1::testResponse(string &netPath, unsigned char* buff)
          throw (BufferOverflowError)
   {
       int i;
       char *p, *q;

       /* OK */
       if((p=strstr((char *)buff, NTRIP_RSP_OK_CLI)))
       {
           q = (char *)buff;
           p += strlen(NTRIP_RSP_OK_CLI);
           nbyte -= p-q;
           for(i=0; i<nbyte; ++i)
           {
               *q++ = *p++;
           }
           status = dataReceiveable;
       }

       /* receive source table */
       else if((p=strstr((char*)buff,NTRIP_RSP_SRCTBL)))
       {

           if(netPath == "")
           {
               status = dataReceiveable;
           }
           else
           {
               nbyte = 0;
               buff[0] = '\0';
               status = error;
               MountPointNotFound e(getClassName() + ": The mountpoint '"+ netPath +
                    "' is not found in the caster");
               GPSTK_THROW(e);
           }
       }

       /* http response */
       else if((p=strstr((char *)buff,NTRIP_RSP_HTTP)))
       {
           if ((q=strchr(p,'\r'))) *q='\0';
           else buff[128]='\0';

           nbyte = 0;
           buff[0]='\0';
           status = init;
           cout << "Here3 stop the netQuery! ";
           TCPsocket->CloseSocket();
       }

       /* buffer overflow */
       else if (nbyte >= NTRIP_MAXRSP)
       {
            printf("response overflow");
            nbyte=0;
            buff[0]='\0';
            status = init;
            cout << "Here4 stop the netQuery! ";
            TCPsocket->CloseSocket();

            BufferOverflowError e(getClassName() + ": Response buffer overflow!");
            GPSTK_THROW(e);
       }
       else
       {
           nbyte=0;
           buff[0]='\0';
           status = init;
       }
   }

   // write the recived raw data
   void NetQueryNtrip1::writeRawData(ofstream& out, unsigned char* buff)
   {
       if(status == dataReceiveable)
       {
           out.write((const char*)buff,nbyte);
           out.flush();
       }
       else
       {
           return;
       }
   }

   // The realization of the network request based on Ntrip 1.0 version
   void NetQueryNtrip1::startRequestPrivate(const NetUrl& url, 
							                const string& gga, 
							                bool sendRequestOnly)
   {
       // break up the origin function from XY.Cao
   }


}  // End of namespace gpstk

