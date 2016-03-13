#ifndef FTP_OVER_HTTP_FTP_COMMAND_H
#define FTP_OVER_HTTP_FTP_COMMAND_H

#include <string>

#include "command.h"
#include "httpServer.h"


namespace srv {


class FtpCommand : public Command
{
public:
    FtpCommand(const HttpRequest& req);
    virtual ~FtpCommand() {}

    virtual void run() override;

private:
    std::string _command;
};


}

#endif // FTP_OVER_HTTP_FTP_COMMAND_H
