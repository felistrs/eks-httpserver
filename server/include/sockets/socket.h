#ifndef SOCKET_H
#define SOCKET_H


#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "sockets.h"


namespace srv {


using socket_t = int;


class Socket
{
public:
    Socket();
    Socket(int descriptor);

    Socket(const Socket& s) = delete;
    Socket& operator =(const Socket s) = delete;


    static socket_t SocketForLocalListening(int port);
    static void StartListening(socket_t descriptor, int max_connections);
    static std::shared_ptr<Socket> Accept(socket_t descriptor);

    std::istream& InStream();

    void Close();
    static void Close(socket_t descriptor);


private:
    void init();
    void read_buffer();


    socket_t _descriptor = 0;

    const ssize_t c_read_buffer_sz = 2048; // ???
    std::stringbuf _sbuffer;
    std::vector<char> _read_buffer;

    std::shared_ptr<std::istream> _istream;

};


}

#endif // SOCKET_H
