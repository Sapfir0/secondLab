#ifndef SERVER_H
#define SERVER_H

#include <fotodatabase.h>
#include <server_command.h>

#include <pipestream.h>

class Server
{
    private:
        fotoDatabase db;
        bool running;
    public:
        Server();
        bool doCommand(ServerCommand command, PipeStream &,PipeStream &);
        int exec();
        void quit();
};

#endif // SERVER_H