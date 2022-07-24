#include "Interface/NetAPI.h"

#include "Utils/Log.h"

#include <vector>
#include <string>
#include <thread>

int main(int argc, char* argv[])
{
    std::string msg;
    std::thread recvThread;

    auto serverList = Client::ParseCommandLineArgs(argc, argv);
    auto client = Client::Create();

    client->SetAvailableServers(serverList);

    Logger::Log("Available Servers. Choose based on index:");
        
    for (auto i = 0; i < serverList.size(); i++)
        Logger::Log(i, ") Server: ", serverList[i].ToString());

    std::getline(std::cin, msg);

    client->Connect(serverList[std::stoi(msg)]);

    recvThread = std::thread([&]() { client->Receive(); });
    recvThread.detach();

    while (true)
    {
        Logger::Log("Enter a message to be sent or Q to quit: ");
        std::getline(std::cin, msg);

        if (msg.compare("Q") == 0)
        {
            client->Shutdown();
            break;
        }

        client->Send(msg);
    }

    if (recvThread.joinable())
        recvThread.join();
}
