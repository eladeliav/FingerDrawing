//
// Created by elad on 23/08/2019.
//
#include <iostream>
#include <string>
#include "../include/DrawingCam.hpp"

using std::string;

#define DEF_IP "172.16.1.127"
#define DEFAULT_PORT 1234

int main(int argc, char **argv)
{
    string ip = DEF_IP;
    int port = DEFAULT_PORT;
    string answer;
    userInput:
    std::cout << "Use default settings?[y]es/[n]o" << std::endl;
    std::cin >> answer;
    if(answer == "n")
    {
        std::cout << "ip: " << std::endl;
        std::cin >> ip;
        std::cout << "port: " << std::endl;
        std::cin >> port;
    }
    else if(answer != "y")
    {
        std::cout << "Invalid input..." << std::endl;
        goto userInput;
    }
    DrawingCam cam(0, ip, port);
    cam.start();
    return 0;
}
