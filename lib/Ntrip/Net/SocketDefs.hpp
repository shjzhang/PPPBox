//
// SocketDefs.h
//
// Module:  SocketDefs
//
// Include platform-specific header files for sockets.


#ifndef GPSTK_SOCKETDEFS_HPP
#define GPSTK_SOCKETDEFS_HPP


#if defined(_WIN32) || defined(_WIN64)
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    #include <Windows.h>
    #define socket_t              SOCKET
    #define socklen_t             int
    #define ioctl_request_t       int
    #define GPSTK_INVALID_SOCKET  INVALID_SOCKET
    #define GPSTK_EINTR           WSAEINTR
    #define GPSTK_EACCES          WSAEACCES
    #define GPSTK_EFAULT          WSAEFAULT
    #define GPSTK_EINVAL          WSAEINVAL
    #define GPSTK_EMFILE          WSAEMFILE
    #define GPSTK_EAGAIN          WSAEWOULDBLOCK
    #define GPSTK_EWOULDBLOCK     WSAEWOULDBLOCK
    #define GPSTK_EINPROGRESS     WSAEINPROGRESS
    #define GPSTK_EALREADY        WSAEALREADY
    #define GPSTK_ENOTSOCK        WSAENOTSOCK
    #define GPSTK_EDESTADDRREQ    WSAEDESTADDRREQ
    #define GPSTK_EMSGSIZE        WSAEMSGSIZE
    #define GPSTK_EPROTOTYPE      WSAEPROTOTYPE
    #define GPSTK_ENOPROTOOPT     WSAENOPROTOOPT
    #define GPSTK_EPROTONOSUPPORT WSAEPROTONOSUPPORT
    #define GPSTK_ESOCKTNOSUPPORT WSAESOCKTNOSUPPORT
    #define GPSTK_ENOTSUP         WSAEOPNOTSUPP
    #define GPSTK_EPFNOSUPPORT    WSAEPFNOSUPPORT
    #define GPSTK_EAFNOSUPPORT    WSAEAFNOSUPPORT
    #define GPSTK_EADDRINUSE      WSAEADDRINUSE
    #define GPSTK_EADDRNOTAVAIL   WSAEADDRNOTAVAIL
    #define GPSTK_ENETDOWN        WSAENETDOWN
    #define GPSTK_ENETUNREACH     WSAENETUNREACH
    #define GPSTK_ENETRESET       WSAENETRESET
    #define GPSTK_ECONNABORTED    WSAECONNABORTED
    #define GPSTK_ECONNRESET      WSAECONNRESET
    #define GPSTK_ENOBUFS         WSAENOBUFS
    #define GPSTK_EISCONN         WSAEISCONN
    #define GPSTK_ENOTCONN        WSAENOTCONN
    #define GPSTK_ESHUTDOWN       WSAESHUTDOWN
    #define GPSTK_ETIMEDOUT       WSAETIMEDOUT
    #define GPSTK_ECONNREFUSED    WSAECONNREFUSED
    #define GPSTK_EHOSTDOWN       WSAEHOSTDOWN
    #define GPSTK_EHOSTUNREACH    WSAEHOSTUNREACH
    #define GPSTK_ESYSNOTREADY    WSASYSNOTREADY
    #define GPSTK_ENOTINIT        WSANOTINITIALISED
    #define GPSTK_HOST_NOT_FOUND  WSAHOST_NOT_FOUND
    #define GPSTK_TRY_AGAIN       WSATRY_AGAIN
    #define GPSTK_NO_RECOVERY     WSANO_RECOVERY
    #define GPSTK_NO_DATA         WSANO_DATA
#elif defined(unix) || defined(__unix) || defined(__unix__)
    #include <netdb.h>
    #include <netinet/in.h>  
    #include <sys/socket.h>  
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <unistd.h>
    #include <errno.h>
    #include <stdlib.h>
    #include <fcntl.h>
    #include <time.h>
    #include <unistd.h>
    #include <sys/ioctl.h>
    #include <linux/tcp.h>
    #define socket_t              int
    #define socklen_t             socklen_t
    #define fcntl_request_t       int
    #define ioctl_request_t       int
    #define closesocket(s)        ::close(s)
    #define GPSTK_INVALID_SOCKET  -1
    #define GPSTK_EINTR           EINTR
    #define GPSTK_EACCES          EACCES
    #define GPSTK_EFAULT          EFAULT
    #define GPSTK_EINVAL          EINVAL
    #define GPSTK_EMFILE          EMFILE
    #define GPSTK_EAGAIN          EAGAIN
    #define GPSTK_EWOULDBLOCK     EWOULDBLOCK
    #define GPSTK_EINPROGRESS     EINPROGRESS
    #define GPSTK_EALREADY        EALREADY
    #define GPSTK_ENOTSOCK        ENOTSOCK
    #define GPSTK_EDESTADDRREQ    EDESTADDRREQ
    #define GPSTK_EMSGSIZE        EMSGSIZE
    #define GPSTK_EPROTOTYPE      EPROTOTYPE
    #define GPSTK_ENOPROTOOPT     ENOPROTOOPT
    #define GPSTK_EPROTONOSUPPORT EPROTONOSUPPORT
    #if defined(ESOCKTNOSUPPORT)
    #define GPSTK_ESOCKTNOSUPPORT ESOCKTNOSUPPORT
    #else
    #define GPSTK_ESOCKTNOSUPPORT -1
    #endif
    #define GPSTK_ENOTSUP         ENOTSUP
    #define GPSTK_EPFNOSUPPORT    EPFNOSUPPORT
    #define GPSTK_EAFNOSUPPORT    EAFNOSUPPORT
    #define GPSTK_EADDRINUSE      EADDRINUSE
    #define GPSTK_EADDRNOTAVAIL   EADDRNOTAVAIL
    #define GPSTK_ENETDOWN        ENETDOWN
    #define GPSTK_ENETUNREACH     ENETUNREACH
    #define GPSTK_ENETRESET       ENETRESET
    #define GPSTK_ECONNABORTED    ECONNABORTED
    #define GPSTK_ECONNRESET      ECONNRESET
    #define GPSTK_ENOBUFS         ENOBUFS
    #define GPSTK_EISCONN         EISCONN
    #define GPSTK_ENOTCONN        ENOTCONN
    #if defined(ESHUTDOWN)
        #define GPSTK_ESHUTDOWN   ESHUTDOWN
    #else
        #define GPSTK_ESHUTDOWN   -2
    #endif
    #define GPSTK_ETIMEDOUT       ETIMEDOUT
    #define GPSTK_ECONNREFUSED    ECONNREFUSED
    #if defined(EHOSTDOWN)
    #define GPSTK_EHOSTDOWN   EHOSTDOWN
    #else
    #define GPSTK_EHOSTDOWN   -3
    #endif
    #define GPSTK_EHOSTUNREACH    EHOSTUNREACH
    #define GPSTK_ESYSNOTREADY    -4
    #define GPSTK_ENOTINIT        -5
    #define GPSTK_HOST_NOT_FOUND  HOST_NOT_FOUND
    #define GPSTK_TRY_AGAIN       TRY_AGAIN
    #define GPSTK_NO_RECOVERY     NO_RECOVERY
    #define GPSTK_NO_DATA         NO_DATA
#endif




#endif // GPSTK_SOCKETDEFS_HPP
