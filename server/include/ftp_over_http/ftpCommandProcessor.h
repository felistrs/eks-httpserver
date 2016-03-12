#ifndef FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
#define FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H

#include "commandProcessor.h"


namespace srv {


class FtpCommandProcessor : public CommandProcessor
{
public:
    FtpCommandProcessor() {}
    virtual ~FtpCommandProcessor() {}

};


}

#endif // FTP_OVER_HTTP_FTP_COMMAND_PROCESSOR_H
