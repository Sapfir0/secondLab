#include <QCoreApplication>

#include <config_pipe_naming.h>
#include <fotodatabase.h>
#include <pipestream.h>
#include <serverworker.h>
#include <thread.h>

#include <QDebug>
#include <iostream>

#include <map>

using namespace std;

int main(int argc, char *argv[])
{
    fotoDatabase db;
    map<clientID_t, ServerWorker> workers;
    map<clientID_t, std::shared_ptr<PipeStream>> pipes;
    bool running = true;

    Thread connectLoop;
    connectLoop.start( [&](){
        clientID_t clientID = 0;
        while (running) {
            qDebug() << "sleep worker";
            PipeStream connectPipe(ConnectPipeName, WinApiHelper::create | WinApiHelper::out);
            connectPipe.waitingClient();
            clientID++;
            pipes.insert(make_pair(clientID, make_shared<PipeStream>(serverSignalsOutputPipeName+QString::number(clientID), WinApiHelper::create | WinApiHelper::out)));
            workers.emplace(make_pair(clientID, ServerWorker(pipes, db, clientID)));
            auto& swork = workers[clientID];
            Thread worker;
            worker.start([&swork](){
                auto resCode = swork.exec();
                qDebug() << "server worker end with code " << resCode;
            });
            qDebug() << "client id: " << clientID;
            Thread::sleep(100);
            connectPipe << clientID;
            connectPipe.close();
        }
    });

    // ждемс
     cout << "To continue the program, press any key..." << endl;
     cin.get();

    // press F to pay respect
    running = false;
    if (connectLoop.running()) {
        connectLoop.quit(); // в бан
    }
    for (auto& item: workers) {
        item.second.quit();
    }
    for (auto item: pipes)
          *item.second << ClientCommand::end_connection;
    return 0;
}
