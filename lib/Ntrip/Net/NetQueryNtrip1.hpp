#pragma ident "$ID: NetQueryNtrip1.hpp 2016-10-17 $"

/**
* @file GPSTK_NETQUERYNTRIP1_HPP
* Class to define network request based on Ntrip1.0
*/

#ifndef GPSTK_NETQUERYNTRIP1_HPP
#define GPSTK_NETQUERYNTRIP1_HPP

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

#include <mutex>
#include <fstream>
#include "NetQueryBase.hpp"
#include "Base64Encoder.hpp"
#include "SocketLib.hpp"
#include "Exception.hpp"


#define NTRIP_AGENT         "pppbox/"   /* VER_GPSTk */
#define NTRIP_CLI_PORT      2101        /* default ntrip-client connection port */
#define NTRIP_SVR_PORT      80          /* default ntrip-server connection port */
#define NTRIP_MAXRSP        32768       /* max size of ntrip response */
#define NTRIP_MAXSTR        256         /* max length of mountpoint string */
#define NTRIP_RSP_OK_CLI    "ICY 200 OK\r\n" /* ntrip response: client */
#define NTRIP_RSP_OK_SVR    "OK\r\n"    /* ntrip response: server */
#define NTRIP_RSP_SRCTBL    "SOURCETABLE 200 OK\r\n"/* ntrip response: source table*/
#define NTRIP_RSP_TBLEND    "ENDSOURCETABLE"
#define NTRIP_RSP_HTTP      "HTTP/"     /* ntrip response: http */
#define NTRIP_RSP_ERROR     "ERROR"     /* ntrip response: error */

namespace gpstk
{
    NEW_EXCEPTION_CLASS(MountPointNotFound, gpstk::Exception);
    NEW_EXCEPTION_CLASS(BufferOverflowError, gpstk::Exception);
    NEW_EXCEPTION_CLASS(SocketRecvError, gpstk::Exception);
	class NetQueryNtrip1: public NetQueryBase
	{
	public: 

		/**
		 * default constructor
		 */
		NetQueryNtrip1();
	
		/**
		 * destructor(must be virtual)
		 */
		virtual ~NetQueryNtrip1();

		/**
		 * stop 
		 *	
		 */
		virtual void stop();

		/**
         *	pure virtual functions
         *
         */
		virtual void waitForRequestResult(const NetUrl& url, string& outData);

		/**
         *	start the network request
         *	@param _url
         *	@param _gga
         */
        virtual void startRequest(const NetUrl& _url, const string& _gga)
        throw (MountPointNotFound,SocketRecvError);

		/**
         * Wait for reading caster's message after connected succesfully.
         * @param  outData: output data from caster
         * @return the number of received data bytes
         */
		virtual void keepAliveRequest(const NetUrl& url, const string& gga);

        /**
         * Wait for reading caster's message after connected succesfully.
         * @param  outData: output data from caster
         * @return the number of received data bytes
         */
        virtual int waitForReadyRead(unsigned char* outData);

        /**
         * Write the raw data to file
         * @param out: ofstream object to be writen
         */
        virtual void writeRawData(ofstream& out);

        /**
         * return a string identifying this class
         */
		string getClassName() const;

	private:
		/*
		 * realization of nerwork request based on NTRIP 1.0
		 * @param url: 
		 * @param gga: NMEA GGA string
		 * @param sendRequestOnly: false = send and receive, true = only send
		 */
		void startRequestPrivate(const NetUrl &url,const string &gga,
			                     bool sendRequestOnly);

        // send the request
        void sendRequest(const NetUrl &url,const string &gga);

        // test the response
        void testResponse(string &netPath) throw (BufferOverflowError);

        //
		SocketLib *TCPsocket;

        // size of buffer
        int buffersize;

        // response buffer
        unsigned char *buff;

        // the bytes of received data
        int nbyte;

        // mutex
        mutex m_mutex;

	};	// End of class 'NetQueryNtrip1'

		//@}


} // End of namespace gpstk

#endif   // GPSTK_NETQUERYNTRIP1_HPP
