#include <exception>
#include <iostream>
#include <memory>

#include "ftp_over_http/ftpCommandProcessor.h"
#include "httpServer.h"
#include "utils/fileStorageReader.h"
#include "utils/logger.h"
#include "utils/programArgumentsParser.h"


int main(int argc, char **argv)
{
    using namespace std;
    using namespace server;

    // Arguments
    std::map<std::string, std::string> default_args = {
            { "port", "1234" },
            { "maxconnections", "100" }
    };

    ProgramArgumentsParser parser(argc, argv, default_args);
    parser.Parse();

    if (parser.key_exists("help") || parser.key_exists("h"))
    {
        log("Http server.\n"
            "\n"
            "Synopsis:\n"
            "\tserver SERVE_PATH [OPTIONS]\n"
            "\n"
            "Possible options :\n"
            "--port : specify port fot server\n"
            "--maxconnections : maximum client connections at one time\n"
            "\n"
            "Supports GET method only.\n");
    }
    else
    {
        int port_number = parser.AcquireValueByKey("port", 1234);
        int max_connections = parser.AcquireValueByKey("maxconnections", 100);

        auto values = parser.get_unpaired_arguments();
        if (values.empty())
        {
            error("Serve path is not specified.");
        }
        else {
            std::string serve_path = values[0];

            // Server
            try {
                unique_ptr<FileStorageReader> fs_reader(
                        new FileStorageReader(serve_path));
                unique_ptr<FtpCommandProcessor> command_processor(
                        new FtpCommandProcessor(fs_reader.get()));

                HttpServer http_server(command_processor.get());
                http_server.set_listening_port(port_number);
                http_server.set_max_connections(max_connections);

                http_server.Start();
            }
            catch (SocketException &e) // TODO: here ?
            {
                error("Socket exception: " + e.info + " / " + e._error_str + " / " + e.what());
            }
            catch (ConnectionException &e) {
                error("Connection exception: " + e.info + " / " + e.what());
            }
            catch (std::exception &e) {
                error(string("Server exception: ") + e.what());
            }

            log("END.");
        }
    }
    return 0;
}
