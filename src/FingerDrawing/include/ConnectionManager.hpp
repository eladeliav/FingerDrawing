//
// Created by elade on 2/12/2020.
//

#ifndef FINGERDRAWING_CONNECTIONMANAGER_HPP
#define FINGERDRAWING_CONNECTIONMANAGER_HPP

#include "UniSockets/Core.hpp"
#include <string>
#include <iostream>
#include <opencv2/core.hpp>

// Constants for networking
#define WAIT_MESSAGE "WAITING_FOR_SECOND_PLAYER"
#define ALL_CONNECTED "ALL_CONNECTED"
#define TOGGLE_MODE "TOGGLE"

// Constants for colors
const cv::Scalar RED_SCALAR = cv::Scalar(0, 0, 255);
const cv::Scalar GREEN_SCALAR = cv::Scalar(0, 255, 0);
const cv::Scalar BLUE_SCALAR = cv::Scalar(255, 0, 0);
const cv::Scalar ERASER_SCALAR = cv::Scalar(0, 0, 0);

// enums for different colors and handshapes
enum Color
{
    RED,
    BLUE,
    GREEN,
    ERASER
};

enum HandShape
{
    ROCK,
    PAPER,
    SCISSORS,
    INVALID
};

// Maps to connect between values and enums
const std::map<HandShape, std::string> SHAPE_TO_STRING = {
        {ROCK,     "Rock"},
        {PAPER,    "Paper"},
        {SCISSORS, "Scissors"},
        {INVALID,  "Unknown hand shape"}
};

const std::map<Color, cv::Scalar> COLOR_TO_SCALAR = {
        {RED,    RED_SCALAR},
        {BLUE,   BLUE_SCALAR},
        {GREEN,  GREEN_SCALAR},
        {ERASER, ERASER_SCALAR}
};

const std::map<Color, std::string> COLOR_TO_STRING = {
        {RED,    "RED"},
        {BLUE,   "BLUE"},
        {GREEN,  "GREEN"},
        {ERASER, "ERASER"}
};

// Struct to describe a drawing point
struct DrawPoint
{
    // Constructors
    DrawPoint(int x, int y, int size, Color color);

    // Constructor for toggling drawing mode
    explicit DrawPoint(bool _toggle) : toggle(_toggle)
    {
    }

    int x = -1; // x of point
    int y = -1; // y of point
    int size = -1; // size of point
    Color color = ERASER; // Color of point
    bool toggle = false; // whether or not this point is a toggle for drawing mode.
};

class ConnectionManager
{
    UniSocket pointsSock; // Socket for points
    UniSocket rockSock; // Socket for hand shapes
    bool pointsConnected = false; // keeping track of what is connected
    bool rockConnected = false;
    bool waitingForPlayers = true;
public:
    // constructors
    ConnectionManager(std::string ip, int port);

    ConnectionManager() = default;

    // Function that initiates a connection with given ip/port
    bool tryConnect(std::string ip, int port);

    // Gets next point from points sock
    DrawPoint getPoint();

    // Sends give point to points sock
    void sendPoint(const DrawPoint &p);

    // Sends a toggle point
    void sendToggle();

    // Gets next hand shape from shapes sock
    HandShape getHandShape();

    // Sends a given hand shape to the shapes sock
    void sendHandShape(const HandShape &s);

    // disconnects
    void disconnect();

    // checks if waiting for other player
    bool waiting();

    // checks if connected
    bool connected();

private:

};


#endif //FINGERDRAWING_CONNECTIONMANAGER_HPP
