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

#include "NetQueryNtrip1.hpp"
#include "NtripToolVersion.hpp"
#include "StringUtils.hpp"
#include <iostream>

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
       ntripStatus = wait;
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
		   TCPsocket->CloseSocket();;
	   }
	   status = finished;

   }

   // send request and read the received data
   void NetQueryNtrip1::waitForRequestResult(const NetUrl& url, string& outData)
   {

   }

   // connect to caster and send the network request
   void NetQueryNtrip1::startRequest(const NetUrl& url, const string& gga)
        throw (MountPointNotFound,SocketRecvError)
   {
       while(ntripStatus != response_ok && ntripStatus != srctbl_received)
       {
           if(ntripStatus <0 )
           {
               return;
           }
           else if(ntripStatus == wait)
           {
               sendRequest(url,gga);
           }
           else if(ntripStatus == requested)
           {
               char *p = (char*)buff;
               int n = recv(TCPsocket->getSocketID(),(char*)buff,buffersize-1,0);
               if(n < 0)
               {
                   SocketRecvError e(getClassName() +": Socket receive error!");
                   GPSTK_THROW(e);
               }

               nbyte = n;
               buff[nbyte] = '\0';

               // wait response
               try
               {
                   testResponse();
               }
               catch(BufferOverflowError &e)
               {
                   GPSTK_THROW(e);
               }
           }

       }
       if(ntripStatus == response_ok)
       {
           int n;
           // judge whether receive rightly
           if((n = recv(TCPsocket->getSocketID(),(char*)buff,buffersize-1,0))<=0)
           {
               cout << "socket error occurs:\t" << TCPsocket->getSocketError() << endl;
               delete TCPsocket;
               TCPsocket  = 0;
               status = error;
               ntripStatus = wait;
               return;
           }
           nbyte = n;
       }

       else if(ntripStatus == srctbl_received )
       {
           if(url.getPath().size()>0)
           {
               ntripStatus = wait;
               MountPointNotFound e(getClassName() + ": The mountpoint '"+url.getPath()+
                    "' is not found in the caster");
               GPSTK_THROW(e);
           }
       }
   }

   // send the nmea order when we have connected to a caster
   void NetQueryNtrip1::keepAliveRequest(const NetUrl& url, const string& gga)
   {

   }

   // read the received data
   int NetQueryNtrip1::waitForReadyRead(const char* outData)
   {
        outData = (const char*)buff;
        return nbyte;
   }


   void NetQueryNtrip1::sendRequest(const NetUrl &url, const string &gga)
   {
       // change the status of request
       status = running;

       string caster = url.getCasterHost();
       string port = url.getCasterPort();

       // connect to caster
       delete TCPsocket;
       TCPsocket = new SocketLib();
       TCPsocket->connectToHost(caster.c_str(),port.c_str());

       // judge whether timeout
       if( !TCPsocket->waitForConnected(timeOut) )
       {
           // if time out
           cout << "connect time out:\t" << TCPsocket->getSocketError() << endl;
           delete TCPsocket;
           TCPsocket  = 0;
           status = error;
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
               delete TCPsocket;
               TCPsocket = 0;
               status = error;
               return;

           }
       }
       else
       {
           // write time out
           cout << "write time out" << endl;
           delete TCPsocket;
           TCPsocket = 0;
           status  = error;
           return;
       }
       status = running;
       ntripStatus = requested;
   }

   void NetQueryNtrip1::testResponse()
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
           ntripStatus = response_ok;
       }

       /* receive source table */
       else if((p=strstr((char*)buff,NTRIP_RSP_SRCTBL)))
       {
           nbyte = 0;
           buff[0] = '\0';
           ntripStatus = srctbl_received;
           TCPsocket->CloseSocket();
       }

       /* http response */
       else if((p=strstr((char *)buff,NTRIP_RSP_HTTP)))
       {
           if ((q=strchr(p,'\r'))) *q='\0';
           else buff[128]='\0';

           nbyte = 0;
           buff[0]='\0';
           ntripStatus = wait;
           TCPsocket->CloseSocket();
       }

       /* buffer overflow */
       else if (nbyte >= NTRIP_MAXRSP)
       {
            printf("response overflow");
            nbyte=0;
            buff[0]='\0';
            ntripStatus = wait;
            TCPsocket->CloseSocket();

            BufferOverflowError e(getClassName() + ": Response buffer overflow!");
            GPSTK_THROW(e);
       }
       else
       {
           nbyte=0;
           buff[0]='\0';
           ntripStatus = wait;
       }
   }

   // write the recived raw data
   void NetQueryNtrip1::writeRawData(ofstream& out)
   {
       if(ntripStatus==response_ok)
       {
           lock_guard<mutex> guard(m_mutex);
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






	   /*
	   /// Send for TCP
	   if((SocketSend(socketfd,reqStr.c_str(),reqStr.size(),0,timeOut)) < 0)
	   {
		   cerr << "error:send socket wrong:\t" << getLastSocketError() << endl;
		   status = error;
		   SocketClose(socketfd);
		   return;
	   }

	   // received caster response
	   char receiveBuff[NTRIP_MAXRSP]={0};
	   // the total number of bytes received from caster
	   int totalbytes = 0;
	   // the number of bytes received from caster at once time
	   int numbytes=0;
	   // start time
	   int starttime = time(0);

	   /* old version
	   *	This method maybe cause something wrong because of the limited buffer size.
	   *	Copy from the receive buffer to the socket buffer could not be finished at one time.
	   if( (numbytes = SocketRecv(socketfd,(char *)receiveBuff,NTRIP_MAXRSP-1,0,timeOut)) < 0)
	   {
		   cerr << "error:receive socket wrong:\t" << getLastSocketError() << endl;
		   status = error;
		   SocketClos444444444444socketfd);
		   return;
	   }
	   */
	   /*
	   // the counter
	   int icounter = 0;
	   bool writeSourcetable = false;

	   while( (numbytes = SocketRecv(socketfd, (char*)(receiveBuff + numbytes), NTRIP_MAXRSP-1, 0, timeOut)) > 0 )
	   {
		   cout << "icounter = " << icounter << " , numbytes = " << numbytes << endl;
		   icounter++;
		   totalbytes += numbytes;
		   cout << "totalbytes = " << totalbytes << endl;
		   
		   bool proxyResponse = false;
		   // the response list from caster
		   vector<string> response;

		   while(true)
		   {
			   string buff = receiveBuff;
			   string line = stripFirstWord(buff,'\n') + "\n";
			   
			   /// whether proxy
			   if( numbytes > 17 &&
                   !strstr(receiveBuff, "ICY 200 OK")  &&
                   (!strncmp(receiveBuff, "HTTP/1.1 200 OK\r\n", 17) ||
                   !strncmp(receiveBuff, "HTTP/1.0 200 OK\r\n", 17)) )
			   {
				   // proxy
				   proxyResponse = true;
			   }

			   if (!proxyResponse && !stripTrailing(line).empty()) 
			   {
				   response.push_back(line);
			   }

			   if (stripTrailing(line).empty()) 
			   {
				   if (proxyResponse) 
				   {
					   proxyResponse = false;
				   }
				   else 
				   {
					   break;
				   }
			   }

			   // invalid password or no password
			   if ( strstr(receiveBuff,"Unauthorized") )
			   {
				   /// output the unauthorized message according HTTP PROTOCOL

				   for(int k = 0; k < numbytes; ++k)
                   {
					   ///fprintf(stderr, "%c", isprint(receiveBuff[k]) ? receiveBuff[k] : '.');
					   fprintf(stderr, "%c", receiveBuff[k]);
                   }
				   break;
			   }

			   // invalid mountpoin
			   if( strstr(receiveBuff,"SOURCETABLE") )
			   {
				   // write to a file: SourceTable
				   ofstream sourcetable;
				   sourcetable.open("SourceTable",ofstream::out);
				   if(!sourcetable)
				   {
					   // If file doesn't exist, issue a warning
					   cerr << "error:sourcetable" << endl;
					   exit(-1);
				   }
				   for(int k = 0; k < totalbytes; ++k)
                   {
					   sourcetable << receiveBuff[k];
                   }
				   // close SourceTable
				   sourcetable.close();
				   break;
			   }

			   if ( !proxyResponse &&
				   ( line.find("200 OK") != std::string::npos ) &&
				   ( line.find("SOURCETABLE") == std::string::npos ))
			   {
				   /// ICY 200 OK,then clear response list
				   response.clear();
				   
			   }

		   }	// end while(true)

	   }	// end while


	   status = running;
	   */
   





}  // End of namespace gpstk

