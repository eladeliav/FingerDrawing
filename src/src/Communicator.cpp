//
// Created by Elad Eliav on 2019-09-08.
//

#include "../include/Communicator.hpp"

Communicator::Communicator(ConnectionDetails details)
{
    this->details = details;

    serverSocket = UniServerSocket(this->details.inPort, this->details.maxCon, this->details.timeout);
    std::thread connectThread(&Communicator::tryConnect, this);
    std::thread acceptThread(&Communicator::tryAccept, this);
    connectThread.detach();
    acceptThread.detach();

}

void Communicator::tryAccept()
{
    while(!inSocket.valid())
    {
        try
        {
            inSocket = serverSocket.accept();
        }catch(UniSocketException& e)
        {
            if(e.getErrorType() != UniSocketException::SOCKET_INIT)
                continue;
        }
    }
}

void Communicator::tryConnect()
{
    while(!outSocket.valid())
    {
        try
        {
            outSocket = UniSocket(this->details.ip, this->details.outPort, this->details.timeout);
        }catch(UniSocketException& e)
        {
            if(e.getErrorType() != UniSocketException::SOCKET_INIT)
                continue;
        }
    }

}

DrawPoint Communicator::getPoint()
{
    char buffer[DEFAULT_BUFFER_LEN] = {};
    inSocket.recv(buffer);
    string strP = buffer;
    std::cout << "Received Point: " << strP << std::endl;
    string x, y, size;
    x = strP.substr(strP.find("X:") + 2, strP.rfind('Y'));
    y = strP.substr(strP.rfind("Y:") + 2, strP.rfind("S:"));
    size = strP.substr(strP.rfind("S:") + 2);
    int xI, yI, sizeI;
    try
    {
        xI = std::stoi(x), yI = std::stoi(y), sizeI = std::stoi(size);
    } catch (std::invalid_argument &e)
    {
        std::cout << e.what() << std::endl;
        errorFlag = -1;
        return {cv::Point(-1, -1), -1};
    }


    return {cv::Point(xI, yI), sizeI};
}

void Communicator::sendPoint(const DrawPoint& p)
{
    string strP = "X:" + std::to_string(p.p.x) + "Y:" + std::to_string(p.p.y) + "S:" + std::to_string(p.size);
    try
    {
        outSocket.send(strP);
    }
    catch (UniSocketException &e)
    {
        if (e.getErrorType() != UniSocketException::TIMED_OUT)
        {
            std::cout << "Failed to send point" << std::endl;
            errorFlag = -1;
        }
    }
}

ConnectionDetails::ConnectionDetails(const string &ip, int inPort, int outPort, int maxCon, int timeout) : ip(ip),
                                                                                                           inPort(inPort),
                                                                                                           outPort(outPort),
                                                                                                           maxCon(maxCon),
                                                                                                           timeout(timeout)
{}

DrawPoint::DrawPoint(const cv::Point &p, int size) : p(p), size(size)
{}
