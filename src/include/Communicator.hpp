//
// Created by Elad Eliav on 2019-09-08.
//

#ifndef FINGERDRAWING_COMMUNICATOR_HPP
#define FINGERDRAWING_COMMUNICATOR_HPP

#include <opencv2/opencv.hpp>
#include <UniSockets/Core.hpp>
#include <string>
#include <thread>

using std::string;

struct ConnectionDetails
{
    string ip;
    int inPort;
    int outPort;
    int maxCon;
    int timeout;

    ConnectionDetails()
    {

    };

    ConnectionDetails(const string &ip, int inPort, int outPort, int maxCon, int timeout);
};

struct DrawPoint
{
    cv::Point p = cv::Point(-1, -1);
    int size = -1;
    DrawPoint() {}
    DrawPoint(const cv::Point &p, int size);
};

class Communicator
{
private:
    ConnectionDetails details;
    UniSocket outSocket;
    UniSocket inSocket;
    UniServerSocket serverSocket;

    void tryConnect();
    void tryAccept();
public:
    Communicator(ConnectionDetails details);

    DrawPoint getPoint();
    void sendPoint(const DrawPoint& p);

    int errorFlag = 0;
};


#endif //FINGERDRAWING_COMMUNICATOR_HPP
