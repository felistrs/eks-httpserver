#ifndef SOCKETS__SOCKETS_H
#define SOCKETS__SOCKETS_H


#ifdef WIN32
#include "sockets/windows.h"

//#include <sys/types.h>
//#include <sys/socket.h>

//#ifdef WIN32
//#include <winsock.h>
//#endif
#else

#include "socketUnix.h"
#endif

#include "socketTypes.h"

#endif // SOCKETS__SOCKETS_H
