#include <exception>
#include <iostream>
#include <memory>

#include "httpServer.h"
#include "ftp_over_http/ftpCommandProcessor.h"


int main(int argc, char **argv)
{
    // TODO: read port from argv
    using namespace srv;

    try
    {
        HttpServer http_server;
        http_server.set_listening_port(1234);
        http_server.set_command_processor(new FtpCommandProcessor());

        http_server.StartAsync();
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
