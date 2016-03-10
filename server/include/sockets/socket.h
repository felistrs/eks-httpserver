#ifndef SOCKET_H
#define SOCKET_H


#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "sockets.h"


namespace srv {


class Socket
{
public:
    Socket();
    Socket(int descriptor);

    Socket(const Socket& s) = delete;
    Socket& operator =(const Socket s) = delete;

    using buff_t = char;


    void InitForLocalListening(int port);
    void StartListening(int max_connections);

    std::shared_ptr<Socket> Accept();

    std::istream& InStream();

    void Close();


private:
    int _descriptor = 0;

    struct sockaddr_storage _their_addr;
    socklen_t _addr_size;
    addrinfo _hints, *_tmp_res;

    const ssize_t c_read_buffer_sz = 2048; // ???
    std::stringbuf _sbuffer;
    std::vector<buff_t> _read_buffer;

    std::shared_ptr<std::istream> _istream;

    int _port;


    void init();
    void read_buffer();
};


}

#endif // SOCKET_H
