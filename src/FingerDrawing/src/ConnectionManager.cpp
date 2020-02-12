//
// Created by elade on 2/12/2020.
//

#include "include/ConnectionManager.hpp"

ConnectionManager::ConnectionManager(std::string ip, int port)
{
    try
    {
        this->pointsSock = UniSocket(ip, port);
        if(this->pointsSock.valid())
            pointsConnected = true;
        this->rockSock = UniSocket(ip, port);
        if(this->rockSock.valid())
            rockConnected = true;
    }catch(UniSocketException& e)
    {
        std::cout << e << std::endl;
        std::cout << "Staying in offline mode" << std::endl;
    }
}

bool ConnectionManager::tryConnect(std::string ip, int port)
{
    try
    {
        this->pointsSock = UniSocket(ip, port);
        if(this->pointsSock.valid())
            pointsConnected = true;
        this->rockSock = UniSocket(ip, port);
        if(this->rockSock.valid())
            rockConnected = true;
    }catch(UniSocketException& e)
    {
        disconnect();
        std::cout << e << std::endl;
        std::cout << "Staying in offline mode" << std::endl;
    }

    return pointsConnected && rockConnected;
}

DrawPoint ConnectionManager::getPoint()
{
    static char buffer[DEFAULT_BUFFER_LEN] = {0};
    memset(buffer, 0, sizeof(buffer));
    try
    {
        int bytesReceived = pointsSock.recv(buffer);
        if(bytesReceived > 0)
        {
            std::string xS, yS, sS, cS;
            std::string msg = buffer;
            std::cout << "RECEIVED POINT: " << msg << std::endl;

            if(waitingForPlayers && msg == ALL_CONNECTED)
            {
                waitingForPlayers = false;
                std::cout << "NO LONGER WAITING FOR PLAYERS" << std::endl;
                return getPoint();
            }

            int ci = msg.find("X:") + 2;
            xS = msg.substr(ci, msg.find("Y:") - ci);
            ci = msg.find("Y:") + 2;
            yS = msg.substr(ci, msg.rfind("S:") - ci);
            ci = msg.find("S:") + 2;
            sS = msg.substr(ci, msg.rfind("C:") - ci);
            ci = msg.find("C:") + 2;
            cS = msg.substr(ci, msg.rfind("END") - ci);
            int x, y, s;
            try
            {
                x = std::stoi(xS);
                y = std::stoi(yS);
                s = std::stoi(sS);
                Color newColor = BLUE;
                for(const auto& it : COLOR_TO_STRING)
                    if(it.second == cS)
                        newColor = it.first;
                return {x, y, s, newColor};
            }catch(std::invalid_argument& e)
            {
                std::cout << e.what() << std::endl;
            }
        }
    }catch(UniSocketException& e)
    {
        if(e.getErrorType() != UniSocketException::TIMED_OUT)
        {
            std::cout << e << std::endl;
            disconnect();
        }
    }
    return {0, 0, 0, BLUE};
}

void ConnectionManager::sendPoint(const DrawPoint &p)
{
    std::string msg = "X:" + std::to_string(p.x) + "Y:" + std::to_string(p.y) + "S:" + std::to_string(p.size) + "C:" + COLOR_TO_STRING.at(p.color) + "END";
    try
    {
        pointsSock.send(msg);
    }
    catch(UniSocketException& e)
    {
        std::cout << e << std::endl;
        disconnect();
    }
}

HandShape ConnectionManager::getHandShape()
{
    static char buffer[DEFAULT_BUFFER_LEN] = {0};
    memset(buffer, 0, sizeof(buffer));
    try
    {
        int bytesReceived = rockSock.recv(buffer);
        if(bytesReceived > 0)
        {
            HandShape handShape = INVALID;
            for(const auto& p : SHAPE_TO_STRING)
            {
                if(p.second == buffer)
                    handShape = p.first;
            }
            return handShape;
        }
    }catch(UniSocketException& e)
    {
        if(e.getErrorType() != UniSocketException::TIMED_OUT)
        {
            std::cout << e << std::endl;
            disconnect();
        }
    }
    return INVALID;
}

void ConnectionManager::sendHandShape(const HandShape &s)
{
    try
    {
        rockSock.send(SHAPE_TO_STRING.at(s));
    }
    catch(UniSocketException& e)
    {
        std::cout << e << std::endl;
        disconnect();
    }
}

void ConnectionManager::disconnect()
{
    try
    {
        rockSock.close();
        pointsSock.close();
        pointsConnected = false;
        rockConnected = false;
    }catch(UniSocketException& e)
    {
        std::cout << e << std::endl;
    }
}

DrawPoint::DrawPoint(int x, int y, int size, Color color) : x(x), y(y), size(size), color(color)
{}

bool ConnectionManager::waiting()
{
    return this->waitingForPlayers;
}

void ConnectionManager::sendToggle()
{
    this->rockSock.send(TOGGLE_MODE);
}

bool ConnectionManager::connected()
{
    return pointsConnected && rockConnected;
}
