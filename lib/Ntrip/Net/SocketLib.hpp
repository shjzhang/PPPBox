#pragma ident "$ID: SocketLib.hpp 2016-10-17 $"

/**
 * @file SocketLib.hpp
 * Lib for TCP/IP socket, supporting LINUX+WINDOWS
 * Referece: 
 * (1)Stevens W R, Fenner B, Rudoff A M. UNIX Network Programming: 
 *    The Sockets Networking API[M]. Addison-Wesley Professional, 2004.
 * (2)Jones A, Ohlund J. Network Programming for Microsoft Windows[M]. 
 *    Microsoft Press, 1999.
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


#ifndef GPSTK_SOCKETLIB_HPP
#define GPSTK_SOCKETLIB_HPP

#include<iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SocketDefs.hpp"

using namespace std;

class SocketLib
{
public:

    /**
     * This enumeration dedicates the status of socket
     * referecen to QAbstractSocket
     */
    enum SocketStatus
    {
        // the socket is not connected
        UnconnectedState = 0,

        // the socket is performing a host name lookup
        HostLookupState,

        // the socket has started establishing a connection
        ConnectingState,

        // a connection is estalished
        ConnectedState,

        // the socket is bound to an address and port(only server)
        BoundState,

        // the socket is abount to close(data may still be waiting to be written)
        ClosingState = 6,

    };

    // return a string identifying this class
    string getClassName() const;

    /**
     * default constructor(state = UnconnectedState)
     */
    SocketLib();

    /**
     * destructor
     */
    ~SocketLib(){};

    //Initialize system environment for windows
    void Initenvironment();

    // free socket source for windows
    void freeEnvironment();

    /**
     * Get the socket error.Be careful to use this function because
     * Not ALL API functions can return the error identification
     * @return special error number( > 0 )
     */
    int getSocketError();

    /**
     * create TCP socket
     * @param family:    AF_INET
     * @param type  :    SOCK_STREAM or SOCK_DGRAM
     * @param protocol:  0
     * @return: -1 = error,or created socket descriptor
     */
    socket_t Socket(int family, int type, int protocol);

    /**
     * close socket
     */
    void CloseSocket();


    void abort();



    /**
     * host name->IP(Protocol independent)
     * In general, we usually know the host name, rather than the
     * IP addrress.Therefore, use the function getaddrinfo() to get
     * the IP address structure.
     * @param hostname: host name or IP(dotted decimal notation)
     * @param service: service or port(decimalist)
     * @param hints: struct addrinfo
     * @param result: struct addrinfo
     * @return int, MUST be 0, or will be exit
     */
    int getAddrFromHost(const char * hostname, const char* service,
                         const struct addrinfo* hints,struct addrinfo **result);

    /**
     * set socket option,including SO_RCVTIMEO/SO_SNDTIMEO/
     * SO_RCVBUF/SO_SNDBUF/TCP_NODELAY, not including connect
     * timeout.
     * @param sockfd: socket descriptor
     * @param rcvbuffsize: receive buffer size
     * @param rcvtimeo: receive timeout
     * @param sndbuffsize: send buffer size
     * @param sndtimeo: send timeout
     * @param mode: TCP_NODELAY
     * ALL time units: s
     */
    int setSocketOption(socket_t sockfd,int rcvbuffsize,int sndbuffsize,
                        int rcvtimeo,int sndtimeo,int mode);

    // only set SO_RCVBUF/SO_SNDBUF
    int setSocketOption(socket_t sockfd,int rcvbuffsize,int sndbuffsize);



    /**
     * connect to caster in block model
     * @param host: host name
     * @param port: port number
     * @return
     */
    void connectToHost(const char* host,const char * port);

    // wait until timeout,unit in s
    bool waitForConnected(int timeout);

    // whether readable,unit in s
    bool sockReadable(int timeout);

    // whether writeable,unit in s
    bool sockWriteable(int timeout);


    /**
     * write into socket send buffer
     * @param buff: string to be sent
     * @param nbytes: number of bytes(calling length())
     * @return: -1 = error
     * @return: the number of bytes really be written into buffer
     */
    int writen(const char *buff,size_t nbytes);

    // read
    int readn(char *buff,size_t nbytes);


    // get the socket descriptor
    socket_t getSocketID(){return socketID;}

    // get the socket status
    SocketStatus getSocketStatus(){return sockState;}

private:

    // socket descriptor
    socket_t socketID;

    // socket state
    SocketStatus sockState;

};	// End of class 'SocketLib'

	//@}



#endif   // GPSTK_SOCKETLIB_HPP
