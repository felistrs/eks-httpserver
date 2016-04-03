#include <exception>
#include <iostream>
#include <memory>

#include "ftp_over_http/ftpCommandProcessor.h"
#include "httpServer.h"
#include "utils/fileStorageReader.h"
#include "utils/logger.h"


int main(int argc, char **argv)
{
    // TODO: read port from argv
    using namespace std;
    using namespace server;

    try
    {
        unique_ptr<FileStorageReader> fs_reader(
                    new FileStorageReader("/")); // TODO: from argv
        unique_ptr<FtpCommandProcessor> command_processor(
                    new FtpCommandProcessor(fs_reader.get()));

        HttpServer http_server(command_processor.get());
        http_server.set_listening_port(1234);

        http_server.Start();
    }
    catch (SocketException& e) // TODO: here ?
    {
        error("Socket exception: " + e.info + " / " + e._error_str + " / " + e.what() );
    }
    catch (ConnectionException& e)
    {
        error("Connection exception: " + e.info + " / " + e.what() );
    }
    catch (std::exception& e)
    {
        error(string("Server exception: ") + e.what() );
    }

    log("END.");
    return 0;
}
