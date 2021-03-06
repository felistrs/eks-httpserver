#include <exception>
#include <iostream>
#include <memory>

#include "httpCommandProcessor.h"
#include "../code/httpServer.h"
#include "utils/fileStorageReader.h"
#include "utils/logger.h"
#include "utils/programBreakEventProvider.h"
#include "utils/programArgumentsParser.h"
#include "utils/singletonContainer.h"


// return values
const int BAD_ARGUMENT = -1;
const int EXCEPTION_SOCKET = -2;
const int EXCEPTION_CONNECTION = -3;
const int EXCEPTION_UNSPECIFIED = -4;


int main(int argc, char **argv)
{
    using namespace std;
    using namespace server;

    int app_result = 0;

    // Event registration
    ProgramBreakEventProvider program_break_event_povider;

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
            app_result = BAD_ARGUMENT;
        }
        else {
            std::string serve_path = values[0];

            // Server
            try {
                unique_ptr<FileStorageReader> fs_reader(
                        new FileStorageReader(serve_path));
                unique_ptr<HttpCommandProcessor> command_processor(
                        new HttpCommandProcessor(fs_reader.get()));

                HttpServer http_server(command_processor.get());
                http_server.set_listening_port(port_number);
                http_server.set_max_connections(max_connections);

                program_break_event_povider.RegisterListener(&http_server);

                http_server.Start();
            }
            catch (SocketException &e)
            {
                error(std::string("Socket exception: ") + e.what() + " / " + e._error_str);
                app_result = EXCEPTION_SOCKET;
            }
            catch (ConnectionException &e) {
                error(std::string("Connection exception: ") + e.what());
                app_result = EXCEPTION_CONNECTION;
            }
            catch (std::exception &e) {
                error(string("Server exception: ") + e.what());
                app_result = EXCEPTION_UNSPECIFIED;
            }

            log("END.");
        }
    }

    SingletonContainer::ReleaseAllSingletones();
    SingletonContainer::ReleaseSingletonContainer();

    return app_result;
}
