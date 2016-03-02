#ifndef SOCKETS_H
#define SOCKETS_H


#ifdef WIN32
#include "sockets/windows.h"

//#include <sys/types.h>
//#include <sys/socket.h>

//#ifdef WIN32
//#include <winsock.h>
//#endif
#else

#include "sockets_unix.h"
#endif

#include "socket_utils.h"

#endif // SOCKETS_H
