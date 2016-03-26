#include <exception>
#include <iostream>
#include <memory>

#include "ftp_over_http/ftpCommandProcessor.h"
#include "httpServer.h"
#include "utils/fileStorageReader.h"


int main(int argc, char **argv)
{
    // TODO: read port from argv
    using namespace std;
    using namespace server;

    try
    {
        unique_ptr<FileStorageReader> fs_reader(new FileStorageReader("/")); // TODO: from argv
        unique_ptr<FtpCommandProcessor> command_processor(new FtpCommandProcessor(fs_reader.get()));

        HttpServer http_server(command_processor.get());
        http_server.set_listening_port(1234);

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
