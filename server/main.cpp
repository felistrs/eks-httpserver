#include "httpServer.h"

#include <iostream>
#include <exception>


// TODO: codestyle !!!


int main(int argc, char **argv)
{
    // TODO: read port from argv

    try
    {
        HttpServer http_server;
        http_server.set_listening_port(1234);

        http_server.start(); // TODO: add start_async
    }
    catch (SocketException& e) // TODO: here ?
    {
        std::cerr << "Socket exception: " << e.info << "/" << e.what() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Server exception: " << e.what() << std::endl;
    }

    std::cout << "END." << std::endl;

    return 0;
}
