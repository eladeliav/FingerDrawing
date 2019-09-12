//
// Created by elade on 9/11/2019.
//
#include <iostream>
#include <UniSockets/Core.hpp>
#include <opencv2/opencv.hpp>
#include <vector>
#include <thread>
#include <string>

#define LOG(x) std::cout << x << std::endl
#define TIMEOUT 0

using namespace cv;
using std::vector;
using std::thread;
using std::string;

template<class T>
void eraseFromVec(vector<T>& vec, T val)
{
    vec.erase(remove(vec.begin(), vec.end(), val), vec.end());
}

void handleSingleClient(UniSocket client, vector<UniSocket>& allClients, bool& exitFlag);

void listenForClients(UniServerSocket& listenSock, vector<UniSocket>& allClients, bool& exitFlag)
{
    while(listenSock.valid() && !exitFlag)
    {
        try
        {
            UniSocket temp = listenSock.accept();
            if(temp.valid())
            {
                LOG("New connection from: " + temp.ip);
                temp.setTimeout(TIMEOUT);
                allClients.push_back(temp);
                thread tempThread(handleSingleClient, std::move(temp), std::ref(allClients), std::ref(exitFlag));
                tempThread.detach();
            }
        }catch(UniSocketException& e)
        {
            if(e.getErrorType() != UniSocketException::TIMED_OUT)
            {
                LOG(e);
                break;
            }
        }
    }
    listenSock.close();
}

void handleSingleClient(UniSocket client, vector<UniSocket>& allClients, bool& exitFlag)
{
    char buf[DEFAULT_BUFFER_LEN] = {0};
    while(client.valid() && !exitFlag)
    {
        try
        {
            int bytesReceived = client.recv(buf);

            if(bytesReceived > 0)
            {
                string message = buf;
                LOG(client.ip << ": " << message);
                UniSocket::broadcastToSet(message, allClients, false, client);
            }
        }catch(UniSocketException& e)
        {
            if(e.getErrorType() != UniSocketException::TIMED_OUT)
            {
                LOG(e);
                eraseFromVec(allClients, client);
                break;
            }
        }

    }
    client.close();
}

int main(int argc, char** argv)
{
    int port = -1;
    bool exitFlag = false;
    vector<UniSocket> allClients;

    LOG("Enter listening port...");
    std::cin >> port;

    UniServerSocket serverSocket(port, SOMAXCONN, TIMEOUT);
    LOG("Listening on port: " << port);
    thread listenThread(listenForClients, std::ref(serverSocket), std::ref(allClients), std::ref(exitFlag));
    listenThread.detach();

    while(!exitFlag)
    {
        LOG("> ");
        string command;
        std::cin >> command;
        if(command == "exit" || command == "quit")
        {
            exitFlag = true;
        }
    }

    UniSocket::cleanup();
    return 0;
}