//
// Created by elade on 2/12/2020.
//

#include "include/ConnectionManager.hpp"

// Constructor that tries to connect to given ip and port
ConnectionManager::ConnectionManager(std::string ip, int port)
{
    try
    {
        // attempts to connect both points stream and rock stream
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

// Attempts to connect to given ip and port
bool ConnectionManager::tryConnect(std::string ip, int port)
{
    try
    {
        // attempts to connect both points stream and rock stream
        this->pointsSock = UniSocket(ip, port);
        if(this->pointsSock.valid())
            pointsConnected = true;
        this->rockSock = UniSocket(ip, port);
        if(this->rockSock.valid())
            rockConnected = true;
    }catch(UniSocketException& e)
    {
        disconnect(); // disconnects if failed
        std::cout << e << std::endl;
        std::cout << "Staying in offline mode" << std::endl;
    }

    return pointsConnected && rockConnected; // returns if succeeded
}

// gets next point from the points socket stream
DrawPoint ConnectionManager::getPoint()
{
    static char buffer[DEFAULT_BUFFER_LEN] = {0}; // buffer
    memset(buffer, 0, sizeof(buffer)); // zeroing out buffer
    try
    {
        // receives from points socket stream
        int bytesReceived = pointsSock.recv(buffer);
        if(bytesReceived > 0) // received more than 0 bytes
        {
            // prints out
            std::string xS, yS, sS, cS; // vars for x and y coords, size and color
            std::string msg = buffer;
            std::cout << "RECEIVED POINT: " << msg << std::endl;

            // if still waiting for players and received the ALL_CONNECTED message
            if(waitingForPlayers && msg == ALL_CONNECTED)
            {
                // no longer waiting for players
                waitingForPlayers = false;
                std::cout << "NO LONGER WAITING FOR PLAYERS" << std::endl;
                // redo function
                return getPoint();
            }

            // if message toggles drawing mode, return it
            if(msg == TOGGLE_MODE)
                return DrawPoint(true);

            // extracts information from message according to delimiters
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
                x = std::stoi(xS); // converts to integer x, y and size
                y = std::stoi(yS);
                s = std::stoi(sS);
                Color newColor = BLUE; // converts size to enum of Color
                for(const auto& it : COLOR_TO_STRING)
                    if(it.second == cS)
                        newColor = it.first;
                return {x, y, s, newColor};
            }catch(std::invalid_argument& e)
            {
                std::cout << e.what() << std::endl;
            }
        } else // received <= 0 bytes, disconnect
            disconnect();
    }catch(UniSocketException& e)
    {
        if(e.getErrorType() != UniSocketException::TIMED_OUT) // ignore time outs
        {
            std::cout << e << std::endl;
            disconnect();
        }
    }
    return {0, 0, 0, BLUE}; // default point
}

// sends given point to server
void ConnectionManager::sendPoint(const DrawPoint &p)
{
    // constructs message according to protocol
    std::string msg = "X:" + std::to_string(p.x) + "Y:" + std::to_string(p.y) + "S:" + std::to_string(p.size) + "C:" + COLOR_TO_STRING.at(p.color) + "END";
    try
    {
        // sends
        pointsSock.send(msg);
    }
    catch(UniSocketException& e)
    {
        std::cout << e << std::endl; // disconnects if failed
        disconnect();
    }
}

// Receives next handshape from server
HandShape ConnectionManager::getHandShape()
{
    static char buffer[DEFAULT_BUFFER_LEN] = {0}; // buffer
    memset(buffer, 0, sizeof(buffer)); // zeroing out buffer
    try
    {
        int bytesReceived = rockSock.recv(buffer); // receives from server
        if(bytesReceived > 0) // received valid amount of bytes
        {
            HandShape handShape = INVALID; // check what kind of shape received
            for(const auto& p : SHAPE_TO_STRING)
            {
                if(p.second == buffer)
                    handShape = p.first;
            }
            return handShape; // return it
        } else
            disconnect(); // disconnect if error
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

// sends given hand shape to server
void ConnectionManager::sendHandShape(const HandShape &s)
{
    try
    {
        // tries to send
        rockSock.send(SHAPE_TO_STRING.at(s));
    }
    catch(UniSocketException& e)
    {
        // disconnects if failed
        std::cout << e << std::endl;
        disconnect();
    }
}

// disconnects and goes to offline mode
void ConnectionManager::disconnect()
{
    try
    {
        waitingForPlayers = true;
        rockSock.close();
        pointsSock.close();
        pointsConnected = false;
        rockConnected = false;
    }catch(UniSocketException& e)
    {
        std::cout << e << std::endl;
    }
}

// cosntructor for draw point struct
DrawPoint::DrawPoint(int x, int y, int size, Color color) : x(x), y(y), size(size), color(color)
{}

// getter for waiting for players
bool ConnectionManager::waiting()
{
    return this->waitingForPlayers;
}

// sends toggle message
void ConnectionManager::sendToggle()
{
    this->pointsSock.send(TOGGLE_MODE);
}

// checks if connected to server
bool ConnectionManager::connected()
{
    return pointsConnected && rockConnected;
}
