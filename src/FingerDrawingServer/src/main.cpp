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

typedef std::pair<UniSocket, UniSocket> ClientPair;
typedef std::vector<ClientPair> ClientPairList;

template<class T>
void eraseFromVec(vector<T> &vec, T val)
{
    vec.erase(remove(vec.begin(), vec.end(), val), vec.end());
}

void handlePair(ClientPair pair, bool &exitFlag);

void listenForClients(UniServerSocket &listenSock, bool &exitFlag)
{
    while (listenSock.valid() && !exitFlag)
    {
        try
        {
            ClientPair newPair;
            UniSocket temp = listenSock.accept();
            if (temp.valid())
            {
                LOG("New connection from: " + temp.ip);
                temp.setTimeout(TIMEOUT);
                newPair.first = temp;
                //thread tempThread(handlePair, std::move(temp), std::ref(allClients), std::ref(exitFlag));
                //tempThread.detach();
            } else
                continue;
            UniSocket temp2 = listenSock.accept();
            if (temp.valid())
            {
                LOG("New connection from: " + temp2.ip);
                LOG("Making new Pair");
                temp2.setTimeout(TIMEOUT);
                newPair.second = temp2;
                thread tempThread(handlePair, std::move(newPair), std::ref(exitFlag));
                tempThread.detach();
            }
        } catch (UniSocketException &e)
        {
            if (e.getErrorType() != UniSocketException::TIMED_OUT)
            {
                LOG(e);
                break;
            }
        }
    }
    listenSock.close();
}

void handlePair(ClientPair pair, bool &exitFlag)
{
    while (pair.first.valid() && pair.second.valid() && !exitFlag)
    {
        std::thread firstC([=]()
        {
            char buf[DEFAULT_BUFFER_LEN] = {0};
            try
            {
                int bytesReceived = pair.first.recv(buf);
                if(bytesReceived > 0)
                {
                    string msg = buf;
                    //LOG()
                    pair.second.send(msg);
                }
            }
            catch(UniSocketException& e)
            {
                if(e.getErrorType() != UniSocketException::TIMED_OUT)
                {
                    LOG(e);
                    return;
                }
            }
        });
        firstC.join();
        char buf[DEFAULT_BUFFER_LEN] = {0};
        try
        {
            int bytesReceived = pair.second.recv(buf);
            if(bytesReceived > 0)
            {
                string msg = buf;
                //LOG()
                pair.first.send(msg);
            }
        }
        catch(UniSocketException& e)
        {
            if(e.getErrorType() != UniSocketException::TIMED_OUT)
            {
                LOG(e);
                return;
            }
        }
//        try
//        {
//            int bytesReceived = client.recv(buf);
//
//            if(bytesReceived > 0)
//            {
//                string message = buf;
//                LOG(client.ip << ": " << message);
//                UniSocket::broadcastToSet(message, allClients, false, client);
//            }
//        }catch(UniSocketException& e)
//        {
//            if(e.getErrorType() != UniSocketException::TIMED_OUT)
//            {
//                LOG(e);
//                eraseFromVec(allClients, client);
//                break;
//            }
//        }

    }
    pair.first.close();
    pair.second.close();
}

int main(int argc, char **argv)
{
    int port = -1;
    bool exitFlag = false;

    LOG("Enter listening port...");
    std::cin >> port;

    UniServerSocket serverSocket(port, SOMAXCONN, TIMEOUT);
    LOG("Listening on port: " << port);
    thread listenThread(listenForClients, std::ref(serverSocket), std::ref(exitFlag));
    listenThread.detach();

    while (!exitFlag)
    {
        LOG("> ");
        string command;
        std::cin >> command;
        if (command == "exit" || command == "quit")
        {
            exitFlag = true;
        }
    }

    UniSocket::cleanup();
    return 0;
}