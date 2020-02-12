//
// Created by elade on 2/12/2020.
//

#ifndef FINGERDRAWING_CONNECTIONMANAGER_HPP
#define FINGERDRAWING_CONNECTIONMANAGER_HPP
#include "UniSockets/Core.hpp"
#include <string>
#include <iostream>
#include <opencv2/core.hpp>

#define WAIT_MESSAGE "WAITING_FOR_SECOND_PLAYER"
#define ALL_CONNECTED "ALL_CONNECTED"
#define TOGGLE_MODE "TOGGLE"

const cv::Scalar RED_SCALAR = cv::Scalar(0, 0, 255);
const cv::Scalar GREEN_SCALAR = cv::Scalar(0, 255, 0);
const cv::Scalar BLUE_SCALAR = cv::Scalar(255, 0, 0);
const cv::Scalar ERASER_SCALAR = cv::Scalar(0, 0, 0);

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

const std::map<HandShape, std::string> SHAPE_TO_STRING = {
        {ROCK, "Rock"},
        {PAPER, "Paper"},
        {SCISSORS, "Scissors"},
        {INVALID, "Unknown hand shape"}
};

const std::map<Color, cv::Scalar> COLOR_TO_SCALAR = {
        {RED, RED_SCALAR},
        {BLUE, BLUE_SCALAR},
        {GREEN, GREEN_SCALAR},
        {ERASER, ERASER_SCALAR}
};

const std::map<Color, std::string> COLOR_TO_STRING = {
        {RED, "RED"},
        {BLUE, "BLUE"},
        {GREEN, "GREEN"},
        {ERASER, "ERASER"}
};

struct DrawPoint
{
    DrawPoint(int x, int y, int size, Color color);
    DrawPoint(bool toggle)
    {
        this->toggle = toggle;
    }
    int x = -1;
    int y = -1;
    int size = -1;
    Color color = ERASER;
    bool toggle = false;
};

class ConnectionManager
{
    UniSocket pointsSock;
    UniSocket rockSock;
    bool pointsConnected = false;
    bool rockConnected = false;
    bool waitingForPlayers = true;
public:
    ConnectionManager(std::string ip, int port);
    ConnectionManager() = default;

    bool tryConnect(std::string ip, int port);
    DrawPoint getPoint();
    void sendPoint(const DrawPoint& p);
    void sendToggle();
    HandShape getHandShape();
    void sendHandShape(const HandShape& s);
    void disconnect();
    bool waiting();
    bool connected();
private:

};


#endif //FINGERDRAWING_CONNECTIONMANAGER_HPP
