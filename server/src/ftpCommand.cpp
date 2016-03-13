#ifndef FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
#define FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H

#include "ftp_over_http/ftpCommand.h"

#include "utils/logger.h"


namespace srv {

FtpCommand::FtpCommand(const HttpRequest &req)
{
    _command = req.path;
}

void FtpCommand::run()
{
    log("Command : " + _command);
}


}

#endif // FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
