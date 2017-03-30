#pragma ident "$ID: NetQueryBase.hpp 2016-10-11 $"

/**
* @file GPSTK_NETQUERYBASE_HPP
* Base class to define network communication,
* supporting TCP/UDP/RSTP/NTRIP1/NTRIP2/NTRIP2s.
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

#ifndef GPSTK_NETQUERYBASE_HPP
#define GPSTK_NETQUERYBASE_HPP

#include <string>
#include "NetUrl.hpp"

namespace gpstk
{
	class NetQueryBase
	{
	public:

		/** 
		 * This enumeration dedicates the status of network communication
         * init = 0; connected = 1; dataReceiveable = 2; finished = 3; error = -1
		 */ 
		enum queryStatus
		{
            init,                   // Initialize state
            connected,              // have connected to the server
            dataReceiveable,        // can receive the data
            finished,               // finished the work
            error = -1              // errors occur
		};

		/**
		 * default constructor
		 */
        NetQueryBase(){;}
	
		/**
		 * destructor(must be virtual)
		 */
        virtual ~NetQueryBase(){;}

		/**
		 * pure virtual functions
		 * 
		 * Stop network connections and the status = finished
		 */
		virtual void stop() = 0;

		/**
		 * pure virtual functions
		 *	
		 */
		virtual void waitForRequestResult(const NetUrl& url, string& outData) = 0;

		/**
		 * pure virtual functions
		 *	
		 * Initiate network connection. If connection is successful, then 
		 * status = running.
		 * @param url: URL for network request
		 * @param gga: NMEA GGA, usually empty 
		 */
		virtual void startRequest(const NetUrl& url, const string& gga, unsigned char* buff) = 0;

		/**
		 * pure virtual functions
		 *	
		 */
		virtual void keepAliveRequest(const NetUrl& url, const string& gga) = 0;

  
		/**	
		 * send NMEA-GGA message to caster
		 *	@param gga		NMEA-GGA message 
		 */
		void sendNMEA(const string& gga) 
		{
			keepAliveRequest(url, gga);
		}


        /**
         * Write the raw data to file
         * @param out: ofstream object to be writen
         */
        virtual void writeRawData(ofstream& out, unsigned char* buff) =0;

		/**
		 *	get the status of network request
		 */
		queryStatus getStatus() const { return status; }

		/**
         * return length of received buffer
         */
		virtual int getBuffLength() = 0;

	protected:

		// init, running, finished, error
		queryStatus status;

		// time-out threshold, unit in s.
		int timeOut;

		// URL for network request
		NetUrl url;

	};	// End of class 'NetQueryBase'

	//@}


} // End of namespace gpstk

#endif   // GPSTK_NETQUERYBASE_HPP
