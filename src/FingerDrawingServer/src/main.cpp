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

// macro for printing out
#define LOG(x) std::cout << x << std::endl
#define TIMEOUT 0 // default timeout
#define WAIT_MESSAGE "WAITING_FOR_SECOND_PLAYER"
#define ALL_CONNECTED "ALL_CONNECTED" // all connected message

using namespace cv;
using std::vector;
using std::thread;
using std::string;

// struct for client (each client has points and rocks sockets)
struct Client
{
    Client() = default;

    // constructor
    Client(UniSocket points, UniSocket rocks) : points(std::move(points)), rocks(std::move(rocks))
    {}

    UniSocket points; // points socket
    UniSocket rocks; // rock paper scissors socket
};

typedef std::pair<Client, Client> ClientPair; // typedef for clientpair
typedef std::vector<ClientPair> ClientPairList; // typedef for list of client pair

// handle pair declaration so we can use it before definition
void handlePair(ClientPair pair, bool &exitFlag);

// listens for new client pairs
void listenForClients(UniServerSocket &listenSock, bool &exitFlag)
{
    while (listenSock.valid() && !exitFlag) // while listening sock is valid and exit flag is false
    {
        // tries to get new client
        try
        {
            ClientPair newPair;
            Client cl1;
            Client cl2;
            UniSocket cl1Points = listenSock.accept(); // accepts new client
            UniSocket cl1Rocks = listenSock.accept();
            cl1 = Client(cl1Points, cl1Rocks); // Makes it into client struct object
            LOG("New Connection from: " << cl1Points.ip);
            newPair.first = cl1;

            UniSocket cl2Points = listenSock.accept(); // receives second client pair
            UniSocket cl2Rocks = listenSock.accept();
            cl2 = Client(cl2Points, cl2Rocks); // turns that client into a client struct object as well
            newPair.second = cl2;
            LOG("New Connection from: " << cl2Points.ip);
            cl1Points.send(ALL_CONNECTED); // after both connected send the all_connected message to both
            cl2Points.send(ALL_CONNECTED);
            // start a handle pair thread for the new pair
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

// forwards all messages from one client to the other
void forwardMessages(Client &c, Client &o, bool &exitFlag)
{
    char buf[DEFAULT_BUFFER_LEN] = {0}; // buffer
    std::vector<UniSocket> cS{c.points, c.rocks}; // vector of both of c's sockets
    std::vector<UniSocket> readable; // readable list
    while (!exitFlag) // while exitFlag isn't on
    {
        memset(buf, 0, sizeof(buf)); // zero out buffer
        readable = UniSocket::select(cS, TIMEOUT); // select on points and rocks of c
        for (UniSocket const &sock : readable) // for each of the readable sockets
        {
            try
            {
                sock.recv(buf); // received
                std::string sBuf = buf;
                LOG("RECEIVED: " << buf << " FROM " << sock.ip);
                // forward to the needed sock
                if (c.points == sock)
                    o.points.send(sBuf);
                else
                    o.rocks.send(sBuf);
            }catch(UniSocketException& e)
            {
                if (e.getErrorType() != UniSocketException::TIMED_OUT) // ignore timeouts
                {
                    // not timeout, try to close this pair
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
            readable.clear(); // reset readable list
        }
    }
}

// handles a pair of clients
void handlePair(ClientPair pair, bool &exitFlag)
{
    // forwards messages from client1 to client2
    std::thread th1(forwardMessages, std::ref(pair.first), std::ref(pair.second), std::ref(exitFlag));
    th1.detach();
    // while at the same time forwarding messages from client 2 to client1
    forwardMessages(pair.second, pair.first, exitFlag);
}

int main(int argc, char **argv)
{
    int port = -1;
    bool exitFlag = false;

    LOG("Enter listening port..."); // receives listening port
    std::cin >> port;

    UniServerSocket serverSocket(port, SOMAXCONN, TIMEOUT); // setup listneing socket
    LOG("Listening on port: " << port);
    // start listening for new pairs
    thread listenThread(listenForClients, std::ref(serverSocket), std::ref(exitFlag));
    listenThread.detach();

    // receive commands (only exit and quit)
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

    UniSocket::cleanup(); // disconnect winsock library if on windows
    return 0;
}