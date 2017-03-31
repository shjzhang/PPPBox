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

#include "SocketLib.hpp"
#define NTRIP_MAXRSP        32768       /* max size of ntrip response */

// return class name
string SocketLib::getClassName() const
{return "SocketLib";}


// constructor
SocketLib::SocketLib()
{
    socketID = GPSTK_INVALID_SOCKET;
    sockState = UnconnectedState;
    Initenvironment();
}


// initialize the environment
void SocketLib::Initenvironment()
{
#ifdef _WIN32

    // Store initialization information of windows
    WSADATA data; 

    // initialize Winsock(ws2_32.lib is necessary)
    // 2.2£¨latest version£©,2.1,2.0,1.1,1.0
    if(WSAStartup(MAKEWORD(2,2), &data) != 0)
    {
        std::cerr << "Initialize Winsock has something wrong!" 
                  << std::endl;
        exit(-1);
    }

#endif

    // No operation for Linux
}


    // free socket environment source for windows
void SocketLib::freeEnvironment()
{
#ifdef _WIN32

    WSACleanup();

#endif		
    // No operation for Linux
}


// get the error number
int SocketLib::getSocketError()
{
#ifdef _WIN32
   return WSAGetLastError();
#else
   return errno;
#endif
}

// create socket
socket_t SocketLib::Socket(int family, int type, int protocol)
{
    socket_t sockfd = GPSTK_INVALID_SOCKET;

    if ( (sockfd = socket(family, type, protocol)) < 0)
    {
        cout << "create socket failed: " << getSocketError() << endl;

        CloseSocket();

        return -1;			
    }

    // This case is often happened
    if(sockfd == GPSTK_INVALID_SOCKET)  // maybe because of socket environment
    {
        std::cout << "create socket failed: " << getSocketError()
                  << "maybe forget initialize environment" << std::endl;

        CloseSocket();

        return -1;	
        
    }

    socketID = sockfd;
    sockState = HostLookupState;

    return sockfd;
    
}


// close socket
void SocketLib::CloseSocket()
{

    if(closesocket(socketID) < 0)
        cout << "close socket failed" << endl;
#ifdef _WIN32
        freeEnvironment();
#endif
    socketID = GPSTK_INVALID_SOCKET;
    sockState = ClosingState;
}

/*
	int SocketLib::getAddrFromHost(const char * hostname, const char* service,
			                 const struct addrinfo* hintsp,struct addrinfo **result)
	{
		// For client, host name and service must be reset.
		struct addrinfo hints;

		if(hintsp == NULL)
		{
			// clear address structure(although function memset() easyly cause confusion,
			// but function bzero() is not obtainable in windows system)
			memset(&hints,0,sizeof(hints));
			//hints.ai_flags = AI_CANONNAME; // if server,should be changed to AI_PASSIVE
			hints.ai_family = AF_INET;   // protocol independent
			hints.ai_socktype  = SOCK_STREAM; // TCP
		}
		else
		{
			hints = *hintsp; // struct copy
		}
		
		// success = 0
		int ret = getaddrinfo(hostname,service,&hints,result);

		if (ret != 0) 
		{
			fprintf(stderr,"getaddrinfo: &s\n",  gai_strerror(ret));

			freeaddrinfo(*result);

			exit(-1);
		}

		if(Debug)
		{
			// output all the IPv4 IP address
			struct addrinfo *ressave;

			char ipstr[16];
			for(ressave = *result;ressave != NULL;ressave = ressave->ai_next)
			{
				// only interested in IPv4
				inet_ntop(AF_INET,&(((struct sockaddr_in *)(ressave->ai_addr))->sin_addr),ipstr, 16);

				printf("IP address: %s\n", ipstr);
			}

		}

		return ret; // MUST be 0.

	}
*/


// set socket option
int SocketLib::setSocketOption(socket_t sockfd,int rcvbuffsize,int sndbuffsize,
		                       int rcvtimeo,int sndtimeo,int mode)
{

    // set recv buffer size and send buffer size
    if(setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,(const char*)&rcvbuffsize,sizeof(int)) == -1 ||
       setsockopt(sockfd,SOL_SOCKET,SO_SNDBUF,(const char*)&sndbuffsize,sizeof(int)) == -1)
    {
        cout << "set socket buffer size error: " << getSocketError() << endl;

        // CloseSocket();
    }

    // set TCP_NODELAY
    if(setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,(const char *)&mode,sizeof(mode)) == -1)
    {
        cout << "set socket TCP delay error: " << getSocketError() << endl; 
    }

#ifdef _WIN32
		
    if(rcvtimeo > 0)
    {
        rcvtimeo = rcvtimeo*1000; // s->ms

        if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(const char *)&rcvtimeo,sizeof(int)) == -1)
        {
            cout << "set socket receive timeout error" << getSocketError() << endl;

            CloseSocket();

            return -1;
        }
    }

    if(sndtimeo > 0)
    {
        sndtimeo = sndtimeo*1000; // s->ms

        if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(const char *)&sndtimeo,sizeof(int)) == -1)
        {
            cout << "set sock send timeout error" << getSocketError() << endl;

            CloseSocket();

            return -1;
        }
    }
#else
	struct timeval snd;
	struct timeval rcv;

	if(rcvtimeo > 0)
	{
	    rcv.tv_sec = rcvtimeo; // s
        rcv.tv_usec = ((rcvtimeo*1000)%1000)/1e9; // us 

	    if(setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,(const char *)&rcv,sizeof(rcv)) == -1)
	    {
	        cout << "set socket receive timeout error" << getSocketError() << endl;

	        CloseSocket();	

	        return -1;
	    }
	}

	if(sndtimeo > 0)
	{
	    snd.tv_sec = sndtimeo; // s
	    snd.tv_usec = ((sndtimeo*1000)%1000)/1e9; // us 

	    if(setsockopt(sockfd,SOL_SOCKET,SO_SNDTIMEO,(const char *)&snd,sizeof(snd)) == -1)
	    {
	        cout << "set sock send timeout error" << getSocketError() << endl;

	        CloseSocket();	

	        return -1;
	    }
	}

#endif
    return 0;

} // end function SocketLib::setSocketOption()


int SocketLib::setSocketOption(socket_t sockfd,int rcvbuffsize,int sndbuffsize)
{
    return (setSocketOption(sockfd,rcvbuffsize,sndbuffsize,0,0,1));
}


// connect in blocking model(only interested in IPv4)
void SocketLib::connectToHost(const char* host,const char *port)
{
    if(getSocketStatus() == ConnectedState) return;

    // host name(IP address is hard to remember)
    struct hostent *Host;
    
    if( !(Host = gethostbyname(host)) )
    {
        cout << "gethostbyname error for host: "<< host 
             << "\terror number: "<< getSocketError() 
             << endl;

        CloseSocket();

        return;
    }

    char **pptr;

    pptr = Host->h_addr_list;

    char ipstr[32];

    struct sockaddr_in servaddr;

    servaddr.sin_family = AF_INET;

    //servaddr.sin_addr.s_addr = inet_addr( ipstr ); 

    memcpy(&servaddr.sin_addr.s_addr,Host->h_addr_list[0],Host->h_length);

    // network byte order
    servaddr.sin_port = htons(atoi(port));

    // create the socket
    Socket(AF_INET,SOCK_STREAM,0);
    

    setSocketOption(socketID,NTRIP_MAXRSP,NTRIP_MAXRSP,20,20,1);
    //setSocketOption(socketID,NTRIP_MAXRSP,NTRIP_MAXRSP,10,10,0);

    // set non blocking so we can try to connect and it will not wait
    unsigned long sockmodel = 1;

#ifdef _WIN32

    ioctlsocket(socketID, FIONBIO, &sockmodel);
#else
    int flags = fcntl(socketID,F_GETFL,0);
    fcntl(socketID,F_SETFL,flags|O_NONBLOCK);
#endif

    int connfd = connect(socketID,(struct sockaddr *)&servaddr,sizeof(servaddr));	

    // For non-block mode, we can not immediately connect to the server
    // and it will reported the error, unless the server and the client 
    // are both in local.
    // error number windows: WSAEWOULDBLOCK, linux: EINPROGRESS
    if(connfd < 0)
    {
#ifdef _WIN32
        if( getSocketError()== WSAEWOULDBLOCK )
        {
            sockState = ConnectingState;

            cout << "connecting" << endl;
        }
        else
            return;
#else
        if( getSocketError()== EINPROGRESS )
        {
            sockState = ConnectingState;

            cout << "connecting" << endl;
        }
        else
            return;
#endif
    }
    // this case is usually not happened except local 
    // hostand port, but we should handle.
    else if(connfd == 0) 
    {
        sockState = ConnectedState; // both in local
    }

}


// 
bool SocketLib::waitForConnected(int timeout)
{
    // after connectToHost, the socket has been set to NON-BLOCK
    if(sockState == ConnectedState)
    {
        return true;
    }
    else
    {
        fd_set wset; // only interested in write descriptor
        FD_ZERO(&wset);
        FD_SET(socketID, &wset);
        
        struct timeval tm = {0};
        tm.tv_sec = timeout;
        tm.tv_usec = 0;

        int retval = select(socketID + 1, NULL, &wset, NULL, &tm); 

        if(retval <0 ) // error
        {
            cout << "connect error" << endl;
            return false;
        }
        else if(retval == 0) // time out
        {
            cout << "connect time out" << endl;
            return false;
        }

        // success
        if(FD_ISSET(socketID,&wset))
        {
            int err = -1;
            socklen_t len = sizeof(err);

            getsockopt(socketID, SOL_SOCKET, SO_ERROR, (char*)&err, &len);

            if(err != 0)
            {
                cout << "connect error" << endl;
                return false;
            }
            else
            {
                cout << "connect successfully" << endl;
                sockState = ConnectedState;
            }
        }

    }

    // change to blocking model
    unsigned long sockmodel = 0;

#ifdef _WIN32
    ioctlsocket(socketID, FIONBIO, &sockmodel); 

#else
    ioctl(socketID, FIONBIO, &sockmodel);

#endif

    return (sockState == ConnectedState);

}


bool SocketLib::sockWriteable(int timeout)
{
    fd_set wset;
    FD_ZERO(&wset);
    FD_SET(socketID,&wset);

    struct timeval tm;
    tm.tv_sec = timeout;
    tm.tv_usec = 0;

    // select
    return( select(socketID+1,NULL,&wset,NULL,&tm) );
}


bool SocketLib::sockReadable(int timeout)
{
    fd_set rset;
    FD_ZERO(&rset);
    FD_SET(socketID,&rset);

    struct timeval tm;
    tm.tv_sec = timeout;
    tm.tv_usec = 0;

    // select
    return( select(socketID+1,&rset,NULL,NULL,&tm) );
}

int SocketLib::writen(const char*vptr, size_t n)
{
    // number of bytes has not been written into socket send buffer
    size_t nleft; 
    
    // has been written 
    size_t nwritten;

    const char *ptr;

    ptr = vptr;
    nleft = n;

    while(nleft > 0) // stiil has data not to be sent
    {
        // not MSG_WAITALL
        if( (nwritten = send(socketID, ptr, nleft,0)) <= 0 )
        {
            if (nwritten < 0 && getSocketError() == EINTR)
            {
                nwritten = 0;		/* and call write() again */
            }
            else
            	return -1;
        }

        nleft -= nwritten;
        ptr   += nwritten;

    } // end while

    return n;

}


int SocketLib::readn(char *vptr,size_t n)
{
    size_t	nleft;

    size_t	nread;

    char	*ptr;
    
    ptr = vptr;
    nleft = n;
    
    while (nleft > 0) 
    {
        if ( (nread = recv(socketID, ptr, nleft,0)) < 0) 
        {
            if (errno == EINTR)
                nread = 0;		/* and call read() again */
            else
                return -1;
        } 
        else if (nread == 0)
            break;				/* EOF */
        
        nleft -= nread;
        ptr   += nread;

    } // end while

    return(n - nleft);		/* return >= 0 */
}



