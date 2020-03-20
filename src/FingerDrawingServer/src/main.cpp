//
// Created by elade on 9/11/2019.
//
#include <iostream>
#include <UniSockets/Core.hpp>
#include <opencv2/opencv.hpp>
#include <utility>
#include <vector>
#include <thread>
#include <string>

#define LOG(x) std::cout << x << std::endl
#define TIMEOUT 0
#define WAIT_MESSAGE "WAITING_FOR_SECOND_PLAYER"
#define ALL_CONNECTED "ALL_CONNECTED"

using namespace cv;
using std::vector;
using std::thread;
using std::string;

struct SockWrap
{
    UniSocket sock;
    bool points = true;
};

struct Client
{
    Client() = default;

    Client(UniSocket points, UniSocket rocks) : points(std::move(points)), rocks(std::move(rocks))
    {}

    UniSocket points;
    UniSocket rocks;
};

typedef std::pair<Client, Client> ClientPair;
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
            Client cl1;
            Client cl2;
            UniSocket cl1Points = listenSock.accept();
            UniSocket cl1Rocks = listenSock.accept();
            cl1 = Client(cl1Points, cl1Rocks);
            LOG("New Connection from: " << cl1Points.ip);
            newPair.first = cl1;

            UniSocket cl2Points = listenSock.accept();
            UniSocket cl2Rocks = listenSock.accept();
            cl2 = Client(cl2Points, cl2Rocks);
            newPair.second = cl2;
            LOG("New Connection from: " << cl2Points.ip);
            cl1Points.send(ALL_CONNECTED);
            cl2Points.send(ALL_CONNECTED);
            thread tempThread(handlePair, std::move(newPair), std::ref(exitFlag));
            tempThread.detach();
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

void forwardMessages(Client &c, Client &o, bool &exitFlag)
{
    char buf[DEFAULT_BUFFER_LEN] = {0};
    std::vector<UniSocket> cS{c.points, c.rocks};
    std::vector<UniSocket> readable;
    while (!exitFlag)
    {
        memset(buf, 0, sizeof(buf));
        readable = UniSocket::select(cS, TIMEOUT);
        for (UniSocket const &sock : readable)
        {
            try
            {
                sock.recv(buf);
                std::string sBuf = buf;
                LOG("RECEIVED: " << buf << " FROM " << sock.ip);
                if (c.points == sock)
                    o.points.send(sBuf);
                else
                    o.rocks.send(sBuf);
            }catch(UniSocketException& e)
            {
                if (e.getErrorType() != UniSocketException::TIMED_OUT)
                {
                    LOG(e);
                    try
                    {
                        c.points.close();
                        c.rocks.close();
                        o.points.close();
                        o.rocks.close();
                    }
                    catch(UniSocketException& e)
                    {

                    }
                    std::cout << "Closing Pair" << std::endl;
                    return;
                }
            }
            readable.clear();
        }
    }
}

void handlePair(ClientPair pair, bool &exitFlag)
{
    std::thread th1(forwardMessages, std::ref(pair.first), std::ref(pair.second), std::ref(exitFlag));
    th1.detach();
    forwardMessages(pair.second, pair.first, exitFlag);
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